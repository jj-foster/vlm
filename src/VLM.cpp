#include <pch.h>

#include <vlm.hpp>

Vlm::Vlm(Plane* plane)
	: plane{ plane }
{

}

// Calculates induced velocity on a point due to a line vortex element.
std::array<double,3> Vlm::lineVortex(
	double x, double y, double z, double x1, double y1, double z1,
	double x2, double y2, double z2, double vorticity, double R
)
{
	std::array<double, 3> inducedVelocity{ 0,0,0 };

	double r1x2_x{ (y - y1) * (z - z2) - (z - z1) * (y - y2) };
	double r1x2_y{ -(x - x1) * (z - z2) - (z - z1) * (x - x2) };
	double r1x2_z{ (x - x1) * (y - y2) - (y - y1) * (x - x2) };

	double r1x2_mod2{ std::pow(r1x2_x,2) + std::pow(r1x2_y,2) + std::pow(r1x2_z,2) };

	double r1_mod{ 
		std::sqrt(std::pow(x - x1,2) + std::pow(y - y1,2) + std::pow(z - z1,2)) };
	double r2_mod{ 
		std::sqrt(std::pow(x - x2,2) + std::pow(y - y2,2) + std::pow(z - z2,2)) };

	// Singularity condition:
	// If point lies on vortex, induced velocities = 0
	if (r1_mod < R || r2_mod < R || r1x2_mod2 < R)
	{
		return inducedVelocity;
	}


	double r0dotr1{ (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1) + (z2 - z1) * (z - z1) };
	double r0dotr2{ (x2 - x1) * (x - x2) + (y2 - y1) * (y - y2) + (z2 - z1) * (z - z2) };

	double K{
		(vorticity / (4 * nc::constants::pi * r1x2_mod2)) * 
		((r0dotr1 / r1_mod) - (r0dotr2 / r2_mod))
	};

	inducedVelocity[0] = K * r1x2_x;
	inducedVelocity[1] = K * r1x2_y;
	inducedVelocity[2] = K * r1x2_z;

	return inducedVelocity;
}

// Calculates velocities by each line vortex in the trailing vortex
std::array<std::array<double, 3>, 2> Vlm::horseshoeVortex(
	double x, double y, double z, double xA, double yA, double zA,
	double xB, double yB, double zB, double xC, double yC, double zC,
	double xD, double yD, double zD, double vorticity, double R
)
{
	// Induced velocities by each line vortex in the trailing vortex
	std::array<double, 3> q1{ lineVortex(x,y,z,xA,yA,zA,xB,yB,zB,vorticity,R) };
	std::array<double, 3> q2{ lineVortex(x,y,z,xB,yB,zB,xC,yC,zC,vorticity,R) };
	std::array<double, 3> q3{ lineVortex(x,y,z,xC,yC,zC,xD,yD,zD,vorticity,R) };

	// Induced velocity by horseshoe vortex on point P
	std::array<double, 3> q{
		q1[0] + q2[0] + q3[0],
		q1[1] + q2[1] + q3[1],
		q1[2] + q2[2] + q3[2]
	};

	// Induced downwash velocity by horseshoe vortex on point P
	std::array<double, 3> q_{
		q1[0] + q3[0],
		q1[1] + q3[1],
		q1[2] + q3[2]
	};

	return { q, q_ };
}

std::array<std::array < double, 3>, 2> Vlm::ringVortex(
	double x, double y, double z, double xA, double yA, double zA,
	double xB, double yB, double zB, double xC, double yC, double zC,
	double xD, double yD, double zD, double vorticity, double R
)
{
	// Induced velocities by each line vortex in the ring
	std::array<double, 3> q1{ lineVortex(x,y,z,xA,yA,zA,xB,yB,zB,vorticity,R) };
	std::array<double, 3> q2{ lineVortex(x,y,z,xB,yB,zB,xC,yC,zC,vorticity,R) };
	std::array<double, 3> q3{ lineVortex(x,y,z,xC,yC,zC,xD,yD,zD,vorticity,R) };
	std::array<double, 3> q4{ lineVortex(x,y,z,xD,yD,zD,xA,yA,zA,vorticity,R) };

	// Induced velocity by ring vortex on point P
	std::array<double, 3> q{
		q1[0] + q2[0] + q3[0] + q4[0],
		q1[1] + q2[1] + q3[1] + q4[1],
		q1[2] + q2[2] + q3[2] + q4[2]
	};

	// Induced downwash velocity by horseshoe vortex on point P
	std::array<double, 3> q_{
		q2[0] + q4[0],
		q2[1] + q4[1],
		q2[2] + q4[2]
	};

	return { q, q_ };
}

/// <summary>
/// Lifting line vortex lattice method implimentation. 
/// 
/// Solves vortex strength at each collocation point on the mesh:
///		[a_ij][gamma_i] = -V_inf . n_i
///			where a_ij = (u,v,w)_ij . n_i
/// 
/// See 'Low Speed Aerodynamics...' - Katz & Plotkin for more detail.
/// </summary>
/// <param name="Qinf">Absolute freestream velocity</param>
/// <param name="alpha">Angle of attack (deg)</param>
/// <param name="beta">Angle of slideslip (deg)</param>
void Vlm::runLiftingLine(double Qinf, double alpha, double beta, double atmosphereDensity)
{
	solverType = SolverType::lifting_line;

	this->Qinf = Qinf;
	rho = atmosphereDensity;
	double alpha_rad{ nc::deg2rad(alpha) };
	double beta_rad{ nc::deg2rad(beta) };

	double b_ref{ plane->b_ref };

	const nc::NdArray<double> Qinf_vec{
		Qinf *
		nc::NdArray<double>{
			nc::cos(alpha_rad)* nc::cos(beta_rad),
			-nc::sin(beta_rad),
			nc::sin(alpha_rad)* nc::cos(beta_rad)
		}
	};

	const int N{ plane->mesh->nPanels };
	nc::NdArray<double> a = nc::zeros<double>(N, N);
	nc::NdArray<double> b = nc::zeros<double>(N, N);
	nc::NdArray<double> RHS = nc::zeros<double>(N, 1);

	// Progress bar setup
	indicators::show_console_cursor(false);

	indicators::ProgressBar bar{
		indicators::option::BarWidth{30},
		indicators::option::Start{" ["},
		indicators::option::Fill{"#"},
		indicators::option::Lead{"#"},
		indicators::option::Remainder{"-"},
		indicators::option::End{"]"},
		indicators::option::PrefixText{"Computing influence matrix"},
		indicators::option::ForegroundColor{indicators::Color::white},
		indicators::option::ShowElapsedTime{true},
		indicators::option::ShowRemainingTime{true},
		indicators::option::FontStyles{
			std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
	};

	// Collocation point loop
	int i{ 0 };
	float progress{ 0.0f };
	for (Panel& p0 : *plane->mesh)
	{	
		RHS(0, i) = nc::dot(-Qinf_vec, p0.normal)[0];

		double x{ p0.cp(0,0) };
		double y{ p0.cp(0,1) };
		double z{ p0.cp(0,2) };

		// Vortex element loop
		int j{ 0 };
		for (Panel& p1 : *plane->mesh)
		{
			double xB{ p1.B(0,0) };
			double yB{ p1.B(0,1) };
			double zB{ p1.B(0,2) };

			double xC{ p1.C(0,0) };
			double yC{ p1.C(0,1) };
			double zC{ p1.C(0,2) };

			double xA{ 10 * b_ref };	// downstream trailing vertex
			double xD{ 10 * b_ref };	// "

			double yA{ yB };
			double yD{ yC };

			double zA{ xA * nc::sin(alpha_rad) };
			double zD{ xD * nc::sin(alpha_rad) };

			std::array<std::array<double, 3>, 2> Uind{
				horseshoeVortex(x,y,z,xA,yA,zA,xB,yB,zB,xC,yC,zC,xD,yD,zD,1,R)
			};

			std::array<std::array<double, 3>, 2> Uind_mirror{
				horseshoeVortex(x,-y,z,xA,yA,zA,xB,yB,zB,xC,yC,zC,xD,yD,zD,1,R)
			};

			nc::NdArray<double> q{
				Uind[0][0] + Uind_mirror[0][0],
				Uind[0][1] - Uind_mirror[0][1],
				Uind[0][2] + Uind_mirror[0][2]
			};
			nc::NdArray<double> q_{
				Uind[1][0] + Uind_mirror[1][0],
				Uind[1][1] - Uind_mirror[1][1],
				Uind[1][2] + Uind_mirror[1][2]
			};

			a(i, j) = nc::dot(q, p0.normal)[0];	// influence coefficient matrix
			b(i, j) = nc::dot(q_, p0.normal)[0];// normal component of wake induced downwash

			j++;
		}

		i++;
		if ((float)i/N >= progress) {
			progress += (1.0f / 30.0f);
			bar.set_progress(progress*100);
		}
		
	}
	indicators::show_console_cursor(true);

	std::cout << "Solving influence matrix..." << '\n';
	nc::NdArray<double> vorticity{ nc::linalg::solve(a,RHS) };
	nc::NdArray<double> w_ind{ nc::matmul(b,vorticity) };

	// Aero force computation
	int k{ 0 };
	double L{ 0 };
	double Di{ 0 };
	for (Panel& p : *plane->mesh)
	{
		p.vorticity = vorticity[k];
		p.w_ind = w_ind(k, 0);

		p.dL = rho * this->Qinf * vorticity(k, 0) * p.dy;
		p.dDi = -rho * w_ind(k, 0) * vorticity(k, 0) * p.dy;

		L += p.dL;
		Di += p.dDi;

		k++;
	}

	CL = L / (0.5 * rho * plane->S_ref * std::pow(Qinf, 2));
	CDi = Di / (0.5 * rho * plane->S_ref * std::pow(Qinf, 2));
}

void Vlm::runLiftingSurface(
	double Qinf, double alpha, double beta, double atmosphereDensity, int wakeIterations)
{
	solverType = SolverType::lifting_surface;

	if (wakeIterations < 1) {
		throw std::invalid_argument("Cannot have fewer than 1 wake iterations.");
	}

	this->Qinf = Qinf;
	rho = atmosphereDensity;
	double alpha_rad{ nc::deg2rad(alpha) };
	double beta_rad{ nc::deg2rad(beta) };

	double b_ref{ plane->b_ref };

	const nc::NdArray<double> Qinf_vec{
		Qinf *
		nc::NdArray<double>{
			nc::cos(alpha_rad)* nc::cos(beta_rad),
			-nc::sin(beta_rad),
			nc::sin(alpha_rad)* nc::cos(beta_rad)
		}
	};

	// wing-wing influence coefficients
	const int N{ plane->mesh->nPanels };
	nc::NdArray<double> a_body = nc::zeros<double>(N, N);
	nc::NdArray<double> b = nc::zeros<double>(N, N);
	nc::NdArray<double> RHS = nc::zeros<double>(N, 1);

	// Progress bar setup
	indicators::show_console_cursor(false);

	indicators::ProgressBar bar{
		indicators::option::BarWidth{20},
		indicators::option::Start{" ["},
		indicators::option::Fill{"#"},
		indicators::option::Lead{"#"},
		indicators::option::Remainder{"-"},
		indicators::option::End{"]"},
		indicators::option::PrefixText{"Computing body-body influence matrix"},
		indicators::option::ForegroundColor{indicators::Color::white},
		indicators::option::ShowElapsedTime{true},
		indicators::option::ShowRemainingTime{true},
		indicators::option::FontStyles{
			std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
	};

	// Collocation point loop
	int i{ 0 };
	float progress{ 0.0f };
	for (Panel& p0 : *plane->mesh)
	{
		RHS(0, i) = nc::dot(-Qinf_vec, p0.normal)[0];

		double x{ p0.cp(0,0) };
		double y{ p0.cp(0,1) };
		double z{ p0.cp(0,2) };

		// Vortex element loop
		int j{ 0 };
		for (Panel& p1 : *plane->mesh)
		{
			double xA{ p1.A(0,0) };
			double yA{ p1.A(0,1) };
			double zA{ p1.A(0,2) };

			double xB{ p1.B(0,0) };
			double yB{ p1.B(0,1) };
			double zB{ p1.B(0,2) };

			double xC{ p1.C(0,0) };
			double yC{ p1.C(0,1) };
			double zC{ p1.C(0,2) };

			double xD{ p1.D(0,0) };
			double yD{ p1.D(0,1) };
			double zD{ p1.D(0,2) };

			std::array<std::array<double, 3>, 2> Uind{
				ringVortex(x,y,z,xA,yA,zA,xB,yB,zB,xC,yC,zC,xD,yD,zD,1,R)
			};

			std::array<std::array<double, 3>, 2> Uind_mirror{
				ringVortex(x,-y,z,xA,yA,zA,xB,yB,zB,xC,yC,zC,xD,yD,zD,1,R)
			};

			nc::NdArray<double> q{
				Uind[0][0] + Uind_mirror[0][0],
				Uind[0][1] - Uind_mirror[0][1],
				Uind[0][2] + Uind_mirror[0][2]
			};
			nc::NdArray<double> q_{
				Uind[1][0] + Uind_mirror[1][0],
				Uind[1][1] - Uind_mirror[1][1],
				Uind[1][2] + Uind_mirror[1][2]
			};

			a_body(i, j) = nc::dot(q, p0.normal)[0];	// influence coefficient matrix
			b(i, j) = nc::dot(q_, p0.normal)[0];// normal component of wake induced downwash

			j++;
		}

		i++;
		if ((float)i / N >= progress) {
			progress += (1.0f / 20.0f);
			bar.set_progress(progress * 100);
		}

	}
	indicators::show_console_cursor(true);


	// TIME-STEPPING LOOP
	//		wake influence coefficients
	//		wake relaxation
	//		solve for R

}