#include <iostream>
#include <memory>
#include "src/MshReader.hpp"

using namespace std;

int main()
{
	mshreader::MshReader reader;
	std::shared_ptr<grid::Mesh> mesh = make_shared<grid::Mesh>(*reader.read("attempt2.msh"));
	mesh->process_geometry();
	reader.write(mesh.get(), "attempt2.nebr");

	return 0;
}