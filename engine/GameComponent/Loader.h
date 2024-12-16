#pragma once
/**
 * @file Load.h
 * @brief fileから読み込み
 */
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

#include "Mymath.h"

namespace Load
{
	//ロード
	void Positions(const std::string& fileName,std::vector<Vector3>& positions);
	void Transforms(const std::string& fileName, std::vector<Transform>& transform);
	void Spheres(const std::string& fileName, std::vector<Sphere>& spheres);
};

