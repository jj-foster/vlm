#include <pch.h>

#include <wake.hpp>
#include <vlm.hpp>
#include <mesh.hpp>

Wake::Wake(Wing* wing)
	: y{ wing->m_sum }
{
	int n{ wing->n };

	Mesh* wing_mesh{ wing->getMesh().get()};
	std::vector<Panel> wing_panels{ wing_mesh->getPanels() };

	TE_panels = { wing_panels.end() - n,wing_panels.end() };

	// Get trailing edge points
	for (int i{ 0 }; i != TE_panels.size(); i++) {
		// TE point at root of wing
		if (i == 0) {
			std::array<nc::NdArray<double>, 4> corners{ TE_panels[i].getCorners() };
			points[0].push_back({ corners[3][0],corners[3][1], corners[3][2] });
		}
		
		// Everything else

		std::array<nc::NdArray<double>, 4> corners{ TE_panels[i].getCorners() };
		points[0].push_back({ corners[2][0], corners[2][1], corners[2][2] });

	}

}

void Wake::timeStep(double dt)
{
	// Move wake vertices back by dt and insert new row at wing TE.

	std::vector<std::array<double, 3>> new_row{ points[0] };
	for (auto& row : points)
	{
		for (auto& coord : row)
		{
			coord[0] += dt;
		}
	}
	points.insert(points.begin(), new_row);

	// Insert vorticity strength from wing TE panels.

	std::vector<double> new_vorticities;
	for (auto& panel : TE_panels) {
		new_vorticities.push_back(panel.vorticity);
	}
	vorticities.insert(vorticities.begin(), new_vorticities);

	x++;

}

const std::vector<std::array<rl::Vector3, 2>> Wake::getRlLines() const
{
	std::vector<std::array<Vector3, 2>> lines(4*(points.size()-1)*(points[0].size()-1));

	int i{ 0 };
	for (int r{ 1 }; r != points.size(); r++)
	{
		for (int p{ 1 }; p != points[r].size(); p++)
		{
			float xA{ (float)points[r][p - 1][0] };
			float yA{ (float)points[r][p - 1][1] };
			float zA{ (float)points[r][p - 1][2] };
			
			float xB{ (float)points[r - 1][p - 1][0] };
			float yB{ (float)points[r - 1][p - 1][1] };
			float zB{ (float)points[r - 1][p - 1][2] };
			
			float xC{ (float)points[r - 1][p][0] };
			float yC{ (float)points[r - 1][p][1] };
			float zC{ (float)points[r - 1][p][2] };
			
			float xD{ (float)points[r][p][0] };
			float yD{ (float)points[r][p][1] };
			float zD{ (float)points[r][p][2] };

			lines[i]   = { Vector3{xA,yA,zA},Vector3{xB,yB,zB} };
			lines[i+1] = { Vector3{xB,yB,zB},Vector3{xC,yC,zC} };
			lines[i+2] = { Vector3{xC,yC,zC},Vector3{xD,yD,zD} };
			lines[i+3] = { Vector3{xD,yD,zD},Vector3{xA,yA,zA} };

			i+=4;
		}
	}

	return lines;
}
