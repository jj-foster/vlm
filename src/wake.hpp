#pragma once

#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>

class Wing;

class Wake
{
private:

	int x{ 0 };	// number of ring vorticies in x
	int y{ 0 };	// number of ring vorticies in y

	std::vector<Panel*> TE_panels;
	std::vector<std::array<double, 3>> TE_points;

public:

	Wake(Wing* wing);

	void timeStep(double dt);

	// list of rows of ring vortex corner coordinates. size=(x+1,y+1)
	std::vector<std::vector<std::array<double, 3>>> points{ {} };

	// list of rows of vorticities. size=(x,y)
	std::vector<std::vector<double>> vorticities{ {} };

	const std::vector<std::array<rl::Vector3, 2>> getRlLines() const;
};