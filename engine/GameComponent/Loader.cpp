#include <iostream>
#include "Loader.h"

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