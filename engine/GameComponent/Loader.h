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
	//座標
	void Positions(const std::string& fileName,std::vector<Vector3>& positions);
	//Transform
	void Transforms(const std::string& fileName, std::vector<Transform>& transform);
	//球(半径、座標)
	void Spheres(const std::string& fileName, std::vector<Sphere>& spheres);
};

