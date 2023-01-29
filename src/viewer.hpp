#pragma once
#include <pch.h>
#include <utils/colourmap.hpp>

#include <mesh.hpp>
#include <vlm.hpp>

using rl::Vector3;

class Viewer
{
private:
	Vlm* vlm;
	MultiMesh* mesh;
	const std::vector<std::array<Vector3, 2>> meshLines;

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

	int screenWidth{ 800 };
	int screenHeight{ 500 };
	const bool showCp;
	const bool showNormals;

	utils::colourMap::CmType cmType = utils::colourMap::CmType::Coolwarm;


	const Vector3 x_axis{ 1,0,0 };
	const float axis_rotation{ (float)(- nc::constants::pi / 2) };

	void startWindow();

	void drawMesh();
	void drawL();
	void drawD();
	void drawWind();
	void drawWake();


public:
	std::atomic<bool> running;

	Viewer(Vlm* vlm, bool showCp = true, bool showNormals = false)
		: running{ true }
		, vlm { vlm }
		, mesh{ vlm->getPlane()->mesh}
		, showCp{ showCp }
		, showNormals{ showNormals }
		, meshLines{ mesh->getRlLines() }
		, panelData{ mesh }
	{
		
	};

	void startWindowThreadDetached();
	void startWindowThreadJoined();
};