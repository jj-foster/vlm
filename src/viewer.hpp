#pragma once
#include <pch.h>

#include <mesh.hpp>

class Viewer
{
private:
	const int screenWidth{ 800 };
	const int screenHeight{ 500 };
	const bool showCp;
	const bool showNormals;

	const rl::Vector3 x_axis{ 1,0,0 };
	const float axis_rotation{ (double)(- nc::constants::pi / 2) };

	MultiMesh* currentMesh;
	const std::vector<std::array<rl::Vector3, 2>> currentMeshLines;

	void startWindow();
	void drawMesh(MultiMesh* mesh, rl::Camera camera);

public:
	std::atomic<bool> running;

	Viewer(MultiMesh* mesh, bool showCp = true, bool showNormals = false)
		: running{ true }
		, currentMesh{ mesh }
		, showCp{ showCp }
		, showNormals{ showNormals }
		, currentMeshLines{ currentMesh->getRlLines() }
	{
	};

	void startWindowThreadDetached();
	void startWindowThreadJoined();
	void setCurrentMesh(MultiMesh* mesh);
};