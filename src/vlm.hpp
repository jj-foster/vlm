#pragma once

#include <pch.h>

#include <mesh.hpp>
#include <plane.hpp>

class Vlm
{
private:
	Plane* plane;
	double R{ 1e-10 };
	double rho{ 0 };
	double Qinf{ 0 };


	std::array<double, 3> lineVortex(
		double x, double y, double z, double x1, double y1, double z1,
		double x2, double y2, double z2, double vorticity, double R
	);

	std::array<std::array<double, 3>, 2> horseshoeVortex(
		double x, double y, double z, double xA, double yA, double zA,
		double xB, double yB, double zB, double xC, double yC, double zC,
		double xD, double yD, double zD, double vorticity, double R
	);

	std::array<std::array < double, 3>, 2> ringVortex(
		double x, double y, double z, double xA, double yA, double zA,
		double xB, double yB, double zB, double xC, double yC, double zC,
		double xD, double yD, double zD, double vorticity, double R
	);

public:
	double CL{ 0 };
	double CDi{ 0 };

	enum class SolverType {
		lifting_line, lifting_surface
	};

	SolverType solverType;

	Vlm(Plane* plane);

	void runLiftingLine(double Qinf, double alpha, double beta, double atmosphereDensity);
	void runLiftingSurface(
		double Qinf, double alpha, double beta, double atmosphereDensity, int wakeIterations);

	const Plane* getPlane() { return plane; }

};