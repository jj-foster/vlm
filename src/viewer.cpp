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
	camera.position = Vector3{ 5.0f,5.0f,5.0f };
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

	// Main game loop
	while (running.load() && !rl::WindowShouldClose())
	{
		// Update camera
		rl::UpdateCamera(&camera);
		if (rl::IsMouseButtonDown(rl::MOUSE_BUTTON_MIDDLE)) 
			{ camera.target = Vector3{ 0.0f,0.0f,0.0f }; }

		/*float camera_pos{ rl::Vector3Length(camera.position) };
		if (rl::IsKeyReleased(rl::KEY_UP)) {
			rl::SetCameraMode(camera, rl::CAMERA_CUSTOM);
			camera.position = Vector3{ 0.0f, camera_pos, 0.0f };
		}
		if (rl::IsKeyReleased(rl::KEY_RIGHT)) {
			rl::SetCameraMode(camera, rl::CAMERA_CUSTOM);
			camera.position = Vector3{ camera_pos, 0.0f, 0.0f };
		}
		if (rl::IsKeyReleased(rl::KEY_DOWN)) {
			rl::SetCameraMode(camera, rl::CAMERA_CUSTOM);
			camera.position = Vector3{ 0.0f, 0.0f, camera_pos };
		}*/
		
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

					distCamera = rl::Vector3Length(
						rl::Vector3Subtract(camera.position, origin)) * ax_scale;
					distCameraVec = { distCamera,distCamera,distCamera };

					ax_x = rl::Vector3Multiply( Vector3{ 1,0,0 }, distCameraVec );
					ax_y = rl::Vector3Multiply( Vector3{ 0,1,0 }, distCameraVec );
					ax_z = rl::Vector3Multiply( Vector3{ 0,0,1 }, distCameraVec );
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
	std::this_thread::sleep_for(std::chrono::milliseconds(0));

	std::thread windowThread(&Viewer::startWindow, this);
	windowThread.join();
}
void Viewer::startWindowThreadDetached()
{
	// Pause to let raylib startup.
	std::this_thread::sleep_for(std::chrono::milliseconds(0));

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
	for (Panel& p : *mesh)
	{
		Vector3 cpPos_{ (float)p.cp[0], (float)p.cp[1], (float)p.cp[2] };
		Vector3 cpPos{ rl::Vector3RotateByAxisAngle(cpPos_, x_axis, axis_rotation) };

		// Colocation point scaling and draw
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

		// Panel normal vector draw
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
	for (const std::array<Vector3, 2>& x : currentMeshLines)
	{
		Vector3 start_{ x[0] };
		Vector3 end_{ x[1] };
		Vector3 start{ rl::Vector3RotateByAxisAngle(start_, x_axis, axis_rotation) };
		Vector3 end{ rl::Vector3RotateByAxisAngle(end_, x_axis, axis_rotation) };
		
		Vector3 startMirror{ start.x, start.y, -start.z};
		Vector3 endMirror{ end.x, end.y, -end.z };

		rl::DrawLine3D(start, end, rl::WHITE);
		rl::DrawLine3D(startMirror, endMirror, rl::WHITE);
	}

	// Draw panel normals
	if (showNormals)
	{
		for (Panel& p : *mesh)
		{
		}
	}
}