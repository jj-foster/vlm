#pragma once

#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>

class Wing;

class Wake
{
private:

	int y{ 0 };	// number of ring vorticies in y
	int x{ 0 };	// number of ring vorticies in x

	// list of rows of ring vortex corner coordinates. size=(x+1,y)
	std::vector<std::vector<std::array<double, 3>>> points{ {} };

	// list of rows of vorticities. size=(x,y)
	std::vector<std::vector<double>> vorticities{ {} };

	std::vector<Panel> TE_panels;

public:

	Wake(Wing* wing);

	void timeStep(double dt);

	const std::vector<std::vector<std::array<double, 3>>> getPoints() const {
		return points;
	}

	const std::vector<std::array<rl::Vector3, 2>> getRlLines() const;
};