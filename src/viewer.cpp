#include <pch.h>

#include <viewer.hpp>
#include <mesh.hpp>

using rl::Vector3;

void Viewer::startWindow()
{
	rl::InitWindow(screenWidth, screenHeight, "window title");
	rl::Font font{ rl::LoadFont("resources/fonts/Segoe UI.ttf") };

	// Define camera to look into 3D scene
	rl::Camera3D camera{ 0 };
	camera.position = Vector3{ 10.0f,10.0f,10.0f };
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
	float dist_camera{ 0 };
	Vector3 dist_camera_vec{ 0,0,0 };

	Vector3 ax_x{ 1.0f };
	Vector3 ax_y{ 1.0f };
	Vector3 ax_z{ 1.0f };

	// Main game loop
	while (running.load() && !rl::WindowShouldClose())
	{
		// Update
		rl::UpdateCamera(&camera);
		if (rl::IsKeyDown('Z')) { camera.target = Vector3{ 0.0f,0.0f,0.0f }; }

		rl::SetWindowTitle(
			(std::to_string(rl::GetFPS())+" fps").c_str()
		);

		rl::BeginDrawing();
		{
			rl::ClearBackground(rl::BLACK);
			rl::BeginMode3D(camera);
			{
				//std::lock_guard<std::mutex> lock(meshMutex);
				if (currentMesh != nullptr)
				{
					drawMesh(currentMesh, camera);

					// Draw axes

					dist_camera = rl::Vector3Length(
						rl::Vector3Subtract(camera.position, origin)) * ax_scale;
					dist_camera_vec = { dist_camera,dist_camera,dist_camera };

					ax_x = rl::Vector3Multiply( Vector3{ 1,0,0 }, dist_camera_vec );
					ax_y = rl::Vector3Multiply( Vector3{ 0,1,0 }, dist_camera_vec );
					ax_z = rl::Vector3Multiply( Vector3{ 0,0,1 }, dist_camera_vec );
					rl::DrawLine3D(origin, ax_x, rl::PINK);
					rl::DrawLine3D(origin, ax_y, rl::LIME);
					rl::DrawLine3D(origin, ax_z, rl::SKYBLUE);


				}
				else
				{
				}
			}
			rl::EndMode3D();

			//// Display number of objects drawn
			//std::string line_count{ std::to_string(currentMeshLines.size()) };
			//rl::DrawTextEx(
			//	font,
			//	(line_count + " lines").c_str(),
			//	rl::Vector2{10,10},
			//	30,
			//	0,
			//	rl::WHITE
			//);

		}
		rl::EndDrawing();
	}

	rl::CloseWindow();
}

void Viewer::startWindowThreadJoined()
{
	// Pause to let raylib startup.
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::thread windowThread(&Viewer::startWindow, this);
	windowThread.join();
}
void Viewer::startWindowThreadDetached()
{
	// Pause to let raylib startup.
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::thread windowThread(&Viewer::startWindow, this);
	windowThread.detach();
}

void Viewer::setCurrentMesh(MultiMesh* mesh)
{
	running = false;
	currentMesh = mesh;
	running = true;
}

void Viewer::drawMesh(MultiMesh* mesh, rl::Camera camera)
{	
	// Colocation point scaling and draw
	if (showCp)
	{
		for (Panel& p : *mesh)
		{
			Vector3 cp_pos_{ (float)p.cp[0], (float)p.cp[1], (float)p.cp[2] };
			Vector3 cp_pos{ rl::Vector3RotateByAxisAngle(cp_pos_, x_axis, axis_rotation) };

			auto dist_camera = rl::Vector3Length(
				rl::Vector3Subtract(camera.position, cp_pos)) * 0.005f;

			rl::DrawSphere(
				cp_pos,
				dist_camera,
				rl::RED
			);
		}
	}

	// Draw panel lines
	for (const std::array<Vector3, 2>& x : currentMeshLines)
	{
		Vector3 start_{ x[0] };
		Vector3 end_{ x[1] };
		Vector3 start{ rl::Vector3RotateByAxisAngle(start_, x_axis, axis_rotation) };
		Vector3 end{ rl::Vector3RotateByAxisAngle(end_, x_axis, axis_rotation) };
		
		Vector3 start_mirror{ start.x, start.y, -start.z};
		Vector3 end_mirror{ end.x, end.y, -end.z };

		rl::DrawLine3D(start, end, rl::WHITE);
		rl::DrawLine3D(start_mirror, end_mirror, rl::WHITE);
	}
}