#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

#include "Mymath.h"

namespace Load
{
	void Positions(const std::string& fileName,std::vector<Vector3>& positions);
	void Transforms(const std::string& fileName, std::vector<Transform>& transform);

	void Spheres(const std::string& fileName, std::vector<Sphere>& spheres);
};

