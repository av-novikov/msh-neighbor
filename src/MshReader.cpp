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
			msh >> buf;	msh >> buf;	msh >> buf;
			for (int i = 0; i < elem::num_of_verts(type); i++)
			{
				msh >> vertInds[i];
				vertInds[i]--;
			}

			if (isInner)
				mesh->elems.push_back(elem::Element(type, vertInds));
			else
				border_elems.push_back(elem::Element(type, vertInds));
		};

		msh >> buf;
		if (buf == TRI_TYPE)		readElem(elem::EType::BORDER_TRI, false);
		else if (buf == QUAD_TYPE)	readElem(elem::EType::BORDER_QUAD, false);
		else if (buf == HEX_TYPE)	readElem(elem::EType::HEX, true);
		else if (buf == PRISM_TYPE)	readElem(elem::EType::PRISM, true);
		else						getline(msh, buf);

		msh >> buf;
	}

	msh.close();

	// border filtering
	int num_found;
	vector<bool> marked(border_elems.size(), false);
	for (int i = 0; i < border_elems.size(); i++)
	{
		num_found = 0;
		const auto& el = border_elems[i];
		if (el.type == elem::EType::BORDER_QUAD)
			for (const auto& el_nebr : mesh->elems)
			{
					auto find_in_verts4 = [&](int v1, int v2, int v3, int v4) -> bool
					{
						bool b1, b2, b3, b4;
						b1 = b2 = b3 = b4 = false;
						for (int j = 0; j < el_nebr.verts_num; j++)
						{
							if (!b1) if (v1 == el_nebr.verts[j]) { b1 = true; continue; }
							if (!b2) if (v2 == el_nebr.verts[j]) { b2 = true; continue; }
							if (!b3) if (v3 == el_nebr.verts[j]) { b3 = true; continue; }
							if (!b4) if (v4 == el_nebr.verts[j]) { b4 = true; continue; }
						}
						return b1 * b2 * b3 * b4;
					};
					if (find_in_verts4(el.verts[0], el.verts[1], el.verts[2], el.verts[3])) num_found++;
			}
		if (num_found > 1)
			marked[i] = true;
		else if (num_found > 2)
			exit(-1);
	}

	mesh->inner_size = mesh->elems.size();

	for (int i = 0; i < border_elems.size(); i++)
	{
		if(!marked[i])
			mesh->elems.push_back(border_elems[i]);
	}
	mesh->border_size = mesh->elems.size() - mesh->inner_size;

	for (int i = 0; i < mesh->elems.size(); i++)
		mesh->elems[i].num = i;

	return mesh;
}
void MshReader::write(const grid::Mesh* mesh, const std::string filename)
{
	ofstream nebr;
	nebr.open(filename.c_str(), ofstream::out);

	nebr << NODES_BEGIN << endl << mesh->pts_size << endl;
	int counter = 0;
	for (const auto& pt : mesh->pts)
		nebr << counter++ << "\t" << pt.x << "\t" << pt.y << "\t" << pt.z << endl;
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