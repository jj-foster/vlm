#include <pch.h>

#include <vlm.hpp>

Vlm::Vlm(Plane* plane)
	: plane{ plane }
{

}

// Calculates induced velocity on a point due to a line vortex element.
std::array<double,3> Vlm::lineVortex(
	double x, double y, double z, double x1, double y1, double z1,
	double x2, double y2, double z2, double gamma, double R
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
		(gamma / (4 * nc::constants::pi * r1x2_mod2)) * 
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
	double xD, double yD, double zD, double gamma, double R
)
{
	// Induced velocities by each line vortex in the trailing vortex
	std::array<double, 3> q1{ lineVortex(x,y,z,xA,yA,zA,xB,yB,zB,gamma,R) };
	std::array<double, 3> q2{ lineVortex(x,y,z,xB,yB,zB,xC,yC,zC,gamma,R) };
	std::array<double, 3> q3{ lineVortex(x,y,z,xC,yC,zC,xD,yD,zD,gamma,R) };

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

/// <summary>
/// Solves vortex strength at each collocation point on the mesh:
///		[a_ij][gamma_i] = -V_inf . n_i
///			where a_ij = (u,v,w)_ij . n_i
/// 
/// See 'Low Speed Aerodynamics...' - Katz & Plotkin for more detail.
/// </summary>
/// <param name="Qinf">Absolute freestream velocity</param>
/// <param name="alpha">Angle of attack (deg)</param>
/// <param name="beta">Angle of slideslip (deg)</param>
void Vlm::runHorseshoe(double Qinf, double alpha, double beta, double atmosphereDensity)
{
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

	int N{ plane->mesh->nPanels };
	nc::NdArray<double> a = nc::zeros<double>(N, N);
	nc::NdArray<double> b = nc::zeros<double>(N, N);
	nc::NdArray<double> RHS = nc::zeros<double>(N, 1);

	// Collocation point loop
	int i{ 0 };
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
	}

	nc::NdArray<double> gamma{ nc::linalg::solve(a,RHS) };
	nc::NdArray<double> w_ind{ nc::matmul(b,gamma) };

	// Aero force computation
	int k{ 0 };
	for (Panel& p : *plane->mesh)
	{
		p.gamma = gamma(k, 0);
		p.w_ind = w_ind(k, 0);

		p.dL = rho * Qinf * gamma(k, 0) * p.dy;
		p.dDi = -rho * w_ind(k, 0) * gamma(k, 0) * p.dy;
	}

}

double Vlm::getCL()
{
	double L{ 0 };
	for (Panel& p : *plane->mesh) { L += p.dL; }

	double CL{ L / (0.5 * rho * plane->S_ref * std::pow(Qinf,2)) };

	return CL;
}

double Vlm::getCDi()
{
	double Di{ 0 };
	for (Panel& p : *plane->mesh) { Di += p.dDi; }

	double CDi{ Di / (0.5 * rho * plane->S_ref * std::pow(Qinf,2)) };

	return CDi;
}