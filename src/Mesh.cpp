#include "Mesh.hpp"
#include <algorithm>
#include <cstdlib>

using namespace grid;
using namespace std;

Mesh::Mesh()
{
}
Mesh::~Mesh()
{
}
void Mesh::process_geometry()
{
	set_neighbors();
	//int idx = check_neighbors();
	//if (idx >= 0)
	//	exit(-1);
}
void Mesh::set_neighbors()
{
	for (int i = 0; i < inner_size; i++)
	{
		bool b[6] = { false };
		auto& el = elems[i];
		if (el.type == elem::EType::HEX)
			for (const auto& el_nebr : elems)
			{
				if (el_nebr.num != el.num)
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
					if (!b[0]) 
						if (find_in_verts4(el.verts[0], el.verts[1], el.verts[2], el.verts[3])) 
							{ 
							el.nebrs[0].id = el_nebr.num; b[0] = true; };
					if (!b[1]) 
						if (find_in_verts4(el.verts[4], el.verts[5], el.verts[6], el.verts[7])) 
							{ 
							el.nebrs[1].id = el_nebr.num; b[1] = true; };
					if (!b[2]) 
						if (find_in_verts4(el.verts[0], el.verts[1], el.verts[5], el.verts[4])) 
							{ 
							el.nebrs[2].id = el_nebr.num; b[2] = true; };
					if (!b[3]) 
						if (find_in_verts4(el.verts[1], el.verts[2], el.verts[6], el.verts[5])) 
							{ 
							el.nebrs[3].id = el_nebr.num; b[3] = true; };
					if (!b[4]) 
						if (find_in_verts4(el.verts[2], el.verts[3], el.verts[7], el.verts[6])) 
							{ 
							el.nebrs[4].id = el_nebr.num; b[4] = true; };
					if (!b[5]) 
						if (find_in_verts4(el.verts[3], el.verts[0], el.verts[4], el.verts[7])) 
							{ 
							el.nebrs[5].id = el_nebr.num; b[5] = true; };
				}
			}
		else if (el.type == elem::EType::PRISM)
			for (const auto& el_nebr : elems)
			{
				if (el_nebr.num != el.num)
				{
					auto find_in_verts3 = [&](int v1, int v2, int v3) -> bool
					{
						bool b1, b2, b3;
						b1 = b2 = b3 = false;
						for (int j = 0; j < el_nebr.verts_num; j++)
						{
							if (!b1) if (v1 == el_nebr.verts[j]) { b1 = true; continue; }
							if (!b2) if (v2 == el_nebr.verts[j]) { b2 = true; continue; }
							if (!b3) if (v3 == el_nebr.verts[j]) { b3 = true; continue; }
						}
						return b1 * b2 * b3;
					};
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
					if (!b[0]) if (find_in_verts3(el.verts[0], el.verts[1], el.verts[2])) { el.nebrs[0].id = el_nebr.num; b[0] = true; };
					if (!b[1]) if (find_in_verts3(el.verts[3], el.verts[4], el.verts[5])) { el.nebrs[1].id = el_nebr.num; b[1] = true; };
					if (!b[2]) if (find_in_verts4(el.verts[0], el.verts[1], el.verts[4], el.verts[3])) { el.nebrs[2].id = el_nebr.num; b[2] = true; };
					if (!b[3]) if (find_in_verts4(el.verts[1], el.verts[2], el.verts[5], el.verts[4])) { el.nebrs[3].id = el_nebr.num; b[3] = true; };
					if (!b[4]) if (find_in_verts4(el.verts[2], el.verts[0], el.verts[3], el.verts[5])) { el.nebrs[4].id = el_nebr.num; b[4] = true; };
				}
			}

		for (int j = 0; j < el.nebrs_num; j++)
			assert(el.nebrs[j].id >= 0 && el.nebrs[j].id < elems.size());

	}

	bool isFound;
	for (int i = inner_size; i < elems.size(); i++)
	{
		isFound = false;
		auto& el = elems[i];
		for (int j = 0; j < inner_size; j++)
		{
			const auto& el_nebr = elems[j];
			for (int k = 0; k < el_nebr.nebrs_num; k++)
			{
				if (el_nebr.nebrs[k].id == el.num)
				{
					el.nebrs[0].id = el_nebr.num;
					isFound = true;
					break;
				}
			}
			if (isFound) break;
		}
	}
}
int Mesh::check_neighbors() const
{
	int sum;
	for (const auto& el : elems)
		for (int i = 0; i < el.nebrs_num; i++)
		{
			sum = 0;
			const auto& el_nebr = elems[el.nebrs[i].id];
			for (int j = 0; j < el_nebr.nebrs_num; j++)
				if (el_nebr.nebrs[j].id == el.num)
					if (el_nebr.nebrs[j].cent == el.nebrs[i].cent)
						break;
					else
						return i;
		}
	return -1;
}