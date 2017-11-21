#include "MshReader.hpp"

using namespace mshreader;
using namespace std;

MshReader::MshReader() 
{
}
MshReader::~MshReader()
{
}
const grid::Mesh* MshReader::read(const string filename)
{
	grid::Mesh* mesh = new grid::Mesh;
	ifstream msh;
	msh.open(filename.c_str(), ifstream::in);

	// Trash
	do
		msh >> buf;
	while (buf != NODES_BEGIN);
	// Number of nodes
	msh >> mesh->pts_size;
	// Nodes
	msh >> buf;
	while (buf != NODES_END)
	{
		msh >> x; msh >> y;	msh >> z;
		mesh->pts.push_back(point::Point(stod(x, &sz), stod(y, &sz), stod(z, &sz)));
		msh >> buf;
	}
	assert(mesh->pts.size() == mesh->pts_size);

	// Trash
	do
		msh >> buf;
	while (buf != ELEMS_BEGIN);
	// Elements
	msh >> buf;

	while (buf != ELEMS_END)
	{
		auto readElem = [&, this](const elem::EType type, bool isInner)
		{
			bool isFrac = false;
			bool isBorder = false;
			msh >> buf;	msh >> buf;
			if (buf == FRAC_REGION)
				isFrac = true;
			if (buf == BORDER_HEX)
				isBorder = true;
			msh >> buf;

			for (int i = 0; i < elem::num_of_verts(type); i++)
			{
				msh >> vertInds[i];
				vertInds[i]--;
			}

			if (isInner)
				mesh->elems.push_back(elem::Element(type, vertInds));
			else
			{
				if(type == elem::BORDER_QUAD || type == elem::BORDER_TRI)
					if (isFrac)
						frac_elems.push_back(elem::Element(elem::FRAC_QUAD, vertInds));
					else
						border_elems.push_back(elem::Element(type, vertInds));
				else if (type == elem::HEX)
					if (isBorder)
						mesh->elems.push_back(elem::Element(elem::BORDER_HEX, vertInds));
					else
						mesh->elems.push_back(elem::Element(type, vertInds));
			}
		};

		msh >> buf;
		if (buf == TRI_TYPE)		readElem(elem::EType::BORDER_TRI, false);
		else if (buf == QUAD_TYPE)	readElem(elem::EType::BORDER_QUAD, false);
		else if (buf == HEX_TYPE)	readElem(elem::EType::HEX, false);
		else if (buf == PRISM_TYPE)	readElem(elem::EType::PRISM, true);
		else						getline(msh, buf);

		msh >> buf;
	}

	msh.close();

	mesh->inner_size = mesh->elems.size();
	mesh->frac_size = frac_elems.size();
	mesh->border_size = border_elems.size();

	for (const auto& el : border_elems)
		mesh->elems.push_back(el);
	for (auto& el : frac_elems)
	{
		el.nebrs_num = 2;
		mesh->elems.push_back(el);
	}

	for (int i = 0; i < mesh->elems.size(); i++)
		mesh->elems[i].num = i;

	border_elems.clear();
	frac_elems.clear();

	return mesh;
}
void MshReader::write(const grid::Mesh* mesh, const std::string filename)
{
	ofstream nebr;
	nebr.open(filename.c_str(), ofstream::out);

	nebr << NODES_BEGIN << endl << mesh->pts_size << endl;
	int counter = 0;
	for (const auto& pt : mesh->pts)
	{
		nebr << counter++ << "\t" << pt.x << "\t" << pt.y << "\t" << pt.z;
		nebr << "\t" << pt.cells.size();
		for (const auto& idx : pt.cells)
			nebr << "\t" << idx;
		nebr << endl;
	}
	nebr << NODES_END << endl << ELEMS_BEGIN << endl << mesh->elems.size() << endl;
	for (const auto& el : mesh->elems)
	{
		nebr << el.num << "\t" << el.type;
		for (int i = 0; i < el.verts_num; i++)
			nebr << "\t" << el.verts[i];
		for (int i = 0; i < el.nebrs_num; i++)
			nebr << "\t" << el.nebrs[i].id;
		nebr << endl;
	}
	nebr << ELEMS_END;

	nebr.close();
}