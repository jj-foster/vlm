#include <pch.h>

#include <viewer.hpp>
#include <mesh.hpp>
#include <utils/algorithms.hpp>
#include <utils/colourmap.hpp>

using rl::Vector3;

void Viewer::startWindow()
{
	rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
	rl::InitWindow(screenWidth, screenHeight, "window title");
	rl::Font font{ rl::LoadFont("resources/fonts/Segoe UI.ttf") };

	// Define camera to look into 3D scene
	rl::Camera3D camera{ 0 };
	camera.position = Vector3{ -3.0f,3.0f,-3.0f };
	camera.target = Vector3{ 0.0f,0.0f,0.0f };
	camera.up = Vector3{ 0.0f,1.0f,0.0f };
	camera.fovy = 45.0f;
	camera.projection = rl::CAMERA_PERSPECTIVE;

	Vector3 cubePosition{ 0.0f, 0.0f, 0.0f };

	rl::SetCameraMode(camera, rl::CAMERA_FREE);
	rl::SetCameraPanControl(rl::MOUSE_BUTTON_LEFT);
	rl::SetCameraAltControl(rl::KEY_LEFT_ALT);

	rl::SetTargetFPS(60);

	// Axes parameters
	Vector3 origin{ 0.0f,0.0f,0.0f };
	float ax_scale{ 0.3f };
	float distCamera{ 0 };
	Vector3 distCameraVec{ 0,0,0 };

	Vector3 ax_x{ 1.0f };
	Vector3 ax_y{ 1.0f };
	Vector3 ax_z{ 1.0f };

	// GUI controls initialisation
	int dropdownBox0Active = 0;
	bool dropdown0EditMode = false;

	// Main game loop
	while (running.load() && !rl::WindowShouldClose())
	{
		// Update camera
		rl::UpdateCamera(&camera);
		if (rl::IsMouseButtonDown(rl::MOUSE_BUTTON_MIDDLE)) 
			{ camera.target = Vector3{ 0.0f,0.0f,0.0f }; }

		// General updates
		rl::SetWindowTitle(
			(std::to_string(rl::GetFPS())+" fps").c_str()
		);

		// Cycle through aero maps
		if (rl::IsKeyPressed(rl::KEY_RIGHT)) {
			switch (aeroVis)
			{
			case AeroVis::mesh: aeroVis = AeroVis::lift; break;
			case AeroVis::lift: aeroVis=AeroVis::drag; break;
			case AeroVis::drag: aeroVis = AeroVis::downwash; break;
			case AeroVis::downwash: aeroVis = AeroVis::mesh; break;
			default: break;
			}
		}
		if (rl::IsKeyPressed(rl::KEY_LEFT)) {
			switch (aeroVis)
			{
			case AeroVis::mesh: aeroVis = AeroVis::downwash; break;
			case AeroVis::lift: aeroVis = AeroVis::mesh; break;
			case AeroVis::drag: aeroVis = AeroVis::lift; break;
			case AeroVis::downwash: aeroVis = AeroVis::drag; break;
			default: break;
			}
		}

		screenWidth = rl::GetScreenWidth();
		screenHeight = rl::GetScreenHeight();

		rl::BeginDrawing();
		{
			rl::ClearBackground(rl::BLACK);

			rl::BeginMode3D(camera);
			{
				//std::lock_guard<std::mutex> lock(meshMutex);
				if (mesh != nullptr)
				{
					drawMesh();

					switch (aeroVis)
					{
					case AeroVis::lift: drawL(); break;
					case AeroVis::drag: drawD(); break;
					case AeroVis::downwash: drawWind(); break;
					default: break;
					}

					// Draw axes
					distCamera = rl::Vector3Length(
						rl::Vector3Subtract(camera.position, origin)) * ax_scale;
					distCameraVec = { distCamera,distCamera,distCamera };

					ax_x = rl::Vector3Multiply(Vector3{ 1,0,0 }, distCameraVec);
					ax_y = rl::Vector3Multiply(Vector3{ 0,1,0 }, distCameraVec);
					ax_z = rl::Vector3Multiply(Vector3{ 0,0,-1 }, distCameraVec);
					rl::DrawLine3D(origin, ax_x, rl::PINK);
					rl::DrawLine3D(origin, ax_y, rl::LIME);
					rl::DrawLine3D(origin, ax_z, rl::SKYBLUE);

				}
			}
			rl::EndMode3D();


			// GUI
			std::string type_str;
			switch (aeroVis)
			{
			case AeroVis::mesh: type_str = "Mesh"; break;
			case AeroVis::lift: type_str = "Lift"; break;
			case AeroVis::drag: type_str = "Drag"; break;
			case AeroVis::downwash: type_str = "Downwash"; break;
			}
			rl::DrawText(type_str.c_str(), 10, 10, 25, rl::WHITE);

			rl::DrawText(	// Reference area
				("Sw: " + std::to_string(vlm->getPlane()->S_ref)).c_str(),
				10, screenHeight - 100, 18, rl::WHITE
			);
			rl::DrawText(	// Reference span
				("b: " + std::to_string(vlm->getPlane()->b_ref)).c_str(),
				10, screenHeight - 80, 18, rl::WHITE
			);
			rl::DrawText(	// Reference chord
				("MAC: " + std::to_string(vlm->getPlane()->c_ref)).c_str(),
				10, screenHeight - 60, 18, rl::WHITE
			);
			rl::DrawText(	// CL
				("CL: " + std::to_string(vlm->CL)).c_str(),
				10, screenHeight - 40, 18, rl::SKYBLUE
			);
			rl::DrawText(	// CD
				("CD: " + std::to_string(vlm->CDi)).c_str(),
				10, screenHeight - 20, 18, rl::SKYBLUE
			);

			// Colourbar
			if (aeroVis != AeroVis::mesh) {
				rl::DrawRectangleLines(
					screenWidth - 60, 60, 30, screenHeight - 120, rl::WHITE);

				float x{ 0 };
				float y_pos{ 0 };
				float dy{ ((float)screenHeight - 121) / 21 };
				for (int i{ 0 }; i != 21; i++)
				{
					x = (float)i / 21;
					y_pos = 61 + x * (screenHeight - 121);

					utils::colourMap::Colour colour =
						utils::colourMap::getColour(1 - x, cmType);
					rl::Color cb_i{
						(unsigned char)(colour.r() * 255),
						(unsigned char)(colour.g() * 255),
						(unsigned char)(colour.b() * 255),
						(unsigned char)(255)
					};

					rl::DrawRectangle(
						screenWidth - 59, y_pos, 28, dy, cb_i
					);
				}

				// Text
				double panel_max{ 0 };
				double panel_min{ 0 };
				switch (aeroVis) {
				case AeroVis::lift:
					panel_max = panelData.Lmax;
					panel_min = panelData.Lmin;
					break;
				case AeroVis::drag:
					panel_max = panelData.Dmax;
					panel_min = panelData.Dmin;
					break;
				case AeroVis::downwash:
					panel_max = panelData.wmax;
					panel_min = panelData.wmin;
					break;
				default:
					break;
				}

				rl::DrawText(
					std::to_string(panel_max).c_str(),
					screenWidth - 80, 40, 15, rl::WHITE);
				rl::DrawText(
					std::to_string(panel_min).c_str(),
					screenWidth - 80, screenHeight-55, 15, rl::WHITE);

			}

		}
		rl::EndDrawing();
	}

	rl::CloseWindow();
}

void Viewer::startWindowThreadJoined()
{
	std::thread windowThread(&Viewer::startWindow, this);
	windowThread.join();
}
void Viewer::startWindowThreadDetached()
{
	std::thread windowThread(&Viewer::startWindow, this);
	windowThread.detach();
}

Viewer::PanelData::PanelData(MultiMesh* mesh)
{
	std::vector<double> Ls(mesh->nPanels);
	std::vector<double> Ds(mesh->nPanels);
	std::vector<double> ws(mesh->nPanels);

	int i{ 0 };
	for (Panel& p : *mesh)
	{
		Ls[i] = p.dL;
		Ds[i] = p.dDi;
		ws[i] = p.w_ind;

		i++;
	}

	Lmax = *std::max_element(Ls.begin(), Ls.end());
	Lmin = *std::min_element(Ls.begin(), Ls.end());
	Dmax = *std::max_element(Ds.begin(), Ds.end());
	Dmin = *std::min_element(Ds.begin(), Ds.end());
	wmax = *std::max_element(ws.begin(), ws.end());
	wmin = *std::min_element(ws.begin(), ws.end());
}

void Viewer::drawMesh()
{	
	for (Panel& p : *mesh)
	{
		Vector3 cpPos_{ (float)p.cp[0], (float)p.cp[1], (float)p.cp[2] };
		Vector3 cpPos{ rl::Vector3RotateByAxisAngle(cpPos_, x_axis, axis_rotation) };

		// Colocation point
		if (showCp)
		{
			float p_dy{ (float)p.dy };

			rl::DrawSphereEx(
				cpPos,
				p_dy / 20.0f,
				5,
				5,
				rl::RED
			);
		}

		// Panel normal vectors
		if (showNormals)
		{
			Vector3 normal_{ (float)p.normal[0], (float)p.normal[1], (float)p.normal[2] };
			Vector3 normal{ rl::Vector3RotateByAxisAngle(normal_, x_axis, axis_rotation) };

			float normalLength{ (float)p.dy * 0.7f };
			Vector3 normalLengthVec{ normalLength, normalLength, normalLength };

			rl::DrawLine3D(
				cpPos,
				rl::Vector3Add(cpPos, rl::Vector3Multiply(normal, normalLengthVec)),
				rl::RED
			);
		}
	}

	// Draw panel lines
	for (const std::array<Vector3, 2>& x : meshLines)
	{
		Vector3 start_{ x[0] };
		Vector3 end_{ x[1] };
		Vector3 start{ rl::Vector3RotateByAxisAngle(start_, x_axis, axis_rotation) };
		Vector3 end{ rl::Vector3RotateByAxisAngle(end_, x_axis, axis_rotation) };
		
		Vector3 startMirror{ start.x, start.y, -start.z};
		Vector3 endMirror{ end.x, end.y, -end.z };

		rl::DrawLine3D(startMirror, endMirror, rl::WHITE);
		if (aeroVis == AeroVis::mesh) {
			rl::DrawLine3D(start, end, rl::WHITE);
		}
		else {
			//rl::DrawLine3D(start, end, rl::BLACK);
		}
		
	}

}

void Viewer::drawL()
{
	for (Panel& p : *mesh)
	{
		std::array<nc::NdArray<double>,4> corners = p.getCorners();
		Vector3 P1_{ 
			(float)corners[0][0], (float)corners[0][1], (float)corners[0][2] };
		Vector3 P2_{ 
			(float)corners[1][0], (float)corners[1][1], (float)corners[1][2] };
		Vector3 P3_{ 
			(float)corners[2][0], (float)corners[2][1], (float)corners[2][2] };
		Vector3 P4_{ 
			(float)corners[3][0], (float)corners[3][1], (float)corners[3][2] };
		Vector3 P1{ rl::Vector3RotateByAxisAngle(P1_, x_axis, axis_rotation) };
		Vector3 P2{ rl::Vector3RotateByAxisAngle(P2_, x_axis, axis_rotation) };
		Vector3 P3{ rl::Vector3RotateByAxisAngle(P3_, x_axis, axis_rotation) };
		Vector3 P4{ rl::Vector3RotateByAxisAngle(P4_, x_axis, axis_rotation) };

		double dL = p.dL;
		double dL_ratio = utils::lerp2D(
			dL, panelData.Lmin, panelData.Lmax, (double)0, (double)1);

		utils::colourMap::Colour colour = 
			utils::colourMap::getColour(dL_ratio,cmType);
		rl::Color colour_rl{
			(unsigned char)(colour.r() * 255),
			(unsigned char)(colour.g() * 255),
			(unsigned char)(colour.b() * 255),
			(unsigned char)(255)
		};
		
		rl::DrawTriangle3D(P3, P2, P1, colour_rl);
		rl::DrawTriangle3D(P4, P3, P1, colour_rl);

	}
}

void Viewer::drawD()
{
	for (Panel& p : *mesh)
	{
		std::array<nc::NdArray<double>, 4> corners = p.getCorners();
		Vector3 P1_{
			(float)corners[0][0], (float)corners[0][1], (float)corners[0][2] };
		Vector3 P2_{
			(float)corners[1][0], (float)corners[1][1], (float)corners[1][2] };
		Vector3 P3_{
			(float)corners[2][0], (float)corners[2][1], (float)corners[2][2] };
		Vector3 P4_{
			(float)corners[3][0], (float)corners[3][1], (float)corners[3][2] };
		Vector3 P1{ rl::Vector3RotateByAxisAngle(P1_, x_axis, axis_rotation) };
		Vector3 P2{ rl::Vector3RotateByAxisAngle(P2_, x_axis, axis_rotation) };
		Vector3 P3{ rl::Vector3RotateByAxisAngle(P3_, x_axis, axis_rotation) };
		Vector3 P4{ rl::Vector3RotateByAxisAngle(P4_, x_axis, axis_rotation) };

		double dD = p.dDi;
		double dD_ratio = utils::lerp2D(
			dD, panelData.Dmin, panelData.Dmax, (double)0, (double)1);

		utils::colourMap::Colour colour = 
			utils::colourMap::getColour(dD_ratio, cmType);
		rl::Color colour_rl{
			(unsigned char)(colour.r() * 255),
			(unsigned char)(colour.g() * 255),
			(unsigned char)(colour.b() * 255),
			(unsigned char)(255)
		};

		rl::DrawTriangle3D(P3, P2, P1, colour_rl);
		rl::DrawTriangle3D(P4, P3, P1, colour_rl);

	}
}

void Viewer::drawWind()
{
	for (Panel& p : *mesh)
	{
		std::array<nc::NdArray<double>, 4> corners = p.getCorners();
		Vector3 P1_{
			(float)corners[0][0], (float)corners[0][1], (float)corners[0][2] };
		Vector3 P2_{
			(float)corners[1][0], (float)corners[1][1], (float)corners[1][2] };
		Vector3 P3_{
			(float)corners[2][0], (float)corners[2][1], (float)corners[2][2] };
		Vector3 P4_{
			(float)corners[3][0], (float)corners[3][1], (float)corners[3][2] };
		Vector3 P1{ rl::Vector3RotateByAxisAngle(P1_, x_axis, axis_rotation) };
		Vector3 P2{ rl::Vector3RotateByAxisAngle(P2_, x_axis, axis_rotation) };
		Vector3 P3{ rl::Vector3RotateByAxisAngle(P3_, x_axis, axis_rotation) };
		Vector3 P4{ rl::Vector3RotateByAxisAngle(P4_, x_axis, axis_rotation) };

		double w_ind = p.w_ind;
		double dL_ratio = 1 - utils::lerp2D(
			w_ind, panelData.wmin, panelData.wmax, (double)0, (double)1);

		utils::colourMap::Colour colour =
			utils::colourMap::getColour(dL_ratio, cmType);
		rl::Color colour_rl{
			(unsigned char)(colour.r() * 255),
			(unsigned char)(colour.g() * 255),
			(unsigned char)(colour.b() * 255),
			(unsigned char)(255)
		};

		rl::DrawTriangle3D(P3, P2, P1, colour_rl);
		rl::DrawTriangle3D(P4, P3, P1, colour_rl);

	}
}