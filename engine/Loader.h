#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Mymath.h"
#include <fstream>
#include <sstream>

namespace Load
{
	void Positions(const std::string& fileName,std::vector<Vector3>& positions);

	void Spheres(const std::string& fileName, std::vector<Sphere>& spheres);
};

