/**
 * @file Load.h
 * @brief fileから読み込み
 */
#include "GameComponent/Loader.h"

#include <iostream>

namespace Load {
	void Positions(const std::string& fileName, std::vector<Vector3>& positions)
	{
		std::string directoryPath = "Resources/positions/";
		std::string filePath = directoryPath + fileName;
		std::ifstream file(filePath);

		std::string line;

		if (!file.is_open()) {
			assert(false);
		}

		while (std::getline(file, line)) {
			std::stringstream ss(line);
			char comma;
			float x, y, z;

			if (ss >> x >> comma >> y >> comma >> z) {
				positions.emplace_back(Vector3{ x, y, z });
			}
		}

		file.close();
	}

	void Transforms(const std::string& fileName, std::vector<Transform>& transform)
	{
		std::string directoryPath = "Resources/transforms/";
		std::string filePath = directoryPath + fileName;
		std::ifstream file(filePath);

		std::string line;

		if (!file.is_open()) {
			assert(false);
		}

		while (std::getline(file, line)) {
			std::stringstream ss(line);
			char comma;
			Transform tmp;

			if (ss >> tmp.scale.x >> comma >> tmp.scale.y >> comma >> tmp.scale.z >>
				comma >> tmp.rotate.x >> comma >> tmp.rotate.y >> comma >> tmp.rotate.z >>
				comma >> tmp.translate.x >> comma >>tmp.translate.y >> comma >> tmp.translate.z) {
				
			}

			transform.push_back(tmp);
		}

		file.close();
	}

	void Spheres(const std::string& fileName, std::vector<Sphere>& spheres)
	{
		std::string directoryPath = "Resources/positions/";
		std::string filePath = directoryPath + fileName;
		std::ifstream file(filePath);

		std::string line;

		if (!file.is_open()) {
			assert(false);
		}

		while (std::getline(file, line)) {
			std::stringstream ss(line);
			char comma;
			float x, y, z;
			float radius;

			if (ss >> x >> comma >> y >> comma >> z >> comma >> radius) {
				spheres.emplace_back(Vector3{ x, y, z }, radius);
			}
		}

		file.close();
	}
}
