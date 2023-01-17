#pragma once
#include <pch.h>

#include <mesh.hpp>
#include <utils/colourmap.hpp>

using rl::Vector3;

class Viewer
{
private:
	int screenWidth{ 800 };
	int screenHeight{ 500 };
	const bool showCp;
	const bool showNormals;

	utils::colourMap::CmType cmType = utils::colourMap::CmType::Jet;

	enum class AeroVis {
		mesh, lift, drag, downwash
	};

	AeroVis aeroVis{ AeroVis::lift };

	struct PanelData
	{
		double Lmax;
		double Lmin;
		double Dmax;
		double Dmin;
		double wmax;
		double wmin;

		PanelData(MultiMesh* mesh);
	};

	PanelData panelData;

	const Vector3 x_axis{ 1,0,0 };
	const float axis_rotation{ (float)(- nc::constants::pi / 2) };

	MultiMesh* mesh;
	const std::vector<std::array<Vector3, 2>> meshLines;

	void startWindow();

	void drawMesh();
	void drawL();
	void drawD();
	void drawWind();


public:
	std::atomic<bool> running;

	Viewer(MultiMesh* mesh, bool showCp = true, bool showNormals = false)
		: running{ true }
		, mesh{ mesh }
		, showCp{ showCp }
		, showNormals{ showNormals }
		, meshLines{ mesh->getRlLines() }
		, panelData{ mesh }
	{
		
	};

	void startWindowThreadDetached();
	void startWindowThreadJoined();
};