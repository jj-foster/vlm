#include <pch.h>

#include <aerofoil.hpp>
#include <utils.hpp>

Aerofoil::Aerofoil(std::string filepath)
	: filepath{ filepath }
{
	read_dat();
	calc_camber();

}

void Aerofoil::read_dat()
{
	// Open file
	std::ifstream file;
	file.open(filepath);

	// Replace with flat plate if file not found
	if (file.fail())
	{
		std::cout << "Aerofoil not found: " << filepath << '\n';
		std::cout << "Using flat plate instead." << "\n\n";

		coords = flat_plate();
	}
	else
	{
		// Read .dat file
		std::vector<std::array<double, 3>> coords_;
		std::string line;
		int line_count{ -1 };

		while (std::getline(file, line))
		{
			line_count++;

			if (line == "") { continue; } // skip black lines - not sure if needed.

			std::vector<std::string> words{ split_string_delim(line,' ') };

			// Remove empty words (filters extra white spaces etc)
			for (int i{ 0 }; i != words.size(); i++) {
				if (words[i] == "") {
					words.erase(words.begin() + i);
					i--;
				}
			}

			// Skip if there are not exactly 2 words in the filtered line
			if (words.size() != 2) { continue; }

			// Try to convert 2 strings into doubles.
			double x, z;
			try {
				x = std::stod(words[0]);
				z = std::stod(words[1]);
			}
			catch (std::invalid_argument err) {
				// continue if word is not double.
				continue;
			}

			coords_.push_back(std::array<double, 3> {x, 0, z});
		}

		// Assign coords to temp coords array.
		if (coords_.size() != 0)
		{
			coords = nc::zeros<double>(coords_.size(), 3);

			for (int i{ 0 }; i != coords_.size(); i++) {
				coords(i, 0) = coords_[i][0];
				coords(i, 1) = coords_[i][1];
				coords(i, 2) = coords_[i][2];

			}
		}
		else // if aerofoil file is empty (likely because deliminated incorrectly).
		{
			std::cout << "Error: Aerofoil file '" << filepath << "' is corrupt." << '\n';
			std::cout << "Coordinates must be defined with a space between:" << "\n\n";

			coords = flat_plate();
		}
	}
	file.close();

}

/// <param name="s">- String to split</param>
/// <param name="del">- Deliminator to split by</param>
/// <returns>Split string</returns>
std::vector<std::string> Aerofoil::split_string_delim(std::string s, char del)
{
	std::stringstream ss{ s };
	std::string word;
	std::vector<std::string> words;
	while (!ss.eof())
	{
		std::getline(ss, word, del);
		words.push_back(word);
	}

	return words;
}

nc::NdArray<double> Aerofoil::flat_plate()
{
	nc::NdArray<double> coords_ = {
		{1, 0, 0},
		{0.5, 0, 0},
		{0, 0, 0},
		{0.5, 0, 0},
		{1, 0, 0},
	};

	return coords_;
}

/// <summary>
/// Splits aerofoil coordinate definition into upper and lower surfaces.
/// Sorts surface points ascending in x.
/// </summary>
/// <returns>{upper, lower}</returns>
std::array<nc::NdArray<double>, 2> Aerofoil::split_surface()
{
	nc::NdArray<double> del{ 0, 0, 0 };
	
	int split_index{ -1 };
	int coords_len_in = coords.shape().rows;

	for (int i{ 0 }; i != coords_len_in; i++)
	{
		nc::NdArray<double> coord = coords(i, coords.cSlice());

		if (
			coord(0, 0) == del(0, 0) &&
			coord(0, 1) == del(0, 1) &&
			coord(0, 2) == del(0, 2)
			)
		{
			split_index = i;
		}
	}

	if (split_index == -1) {
		std::cout << "Error: Aerofoil '" << filepath <<
			"' must contain coordinate at (0, 0).";

		exit(0);
	}
	else
	{
		nc::NdArray<double> upper = coords(
			nc::Slice(0, split_index + 1),
			coords.cSlice()
		);
		nc::NdArray<double> lower = coords(
			nc::Slice(split_index, coords_len_in),
			coords.cSlice()
		);

		return { upper, lower };
	}
}

void Aerofoil::calc_camber()
{
	// Split into upper and lower surfaces
	std::array<nc::NdArray<double>,2> ul {split_surface()};

	nc::NdArray<double> upper{ ul[0] };
	nc::NdArray<double> lower{ ul[1] };

	if (upper.shape().rows != lower.shape().rows) {
		std::cout << "Error: Uneven point distribution on upper and lower"
			" surfaces of aerofoil '" << filepath << '\n';

		exit(0);
	}

	int n_per_side = upper.shape().rows;

	camber = nc::zeros<double>(n_per_side, 3);

	bool warned{ false };
	for (int i{ 0 }; i != n_per_side; i++)
	{
		double x_up{ upper(n_per_side - i - 1, 0) };
		double x_lo{ lower(i, 0) };

		if (x_up != x_lo) {
			if (!warned) {
				std::cout << "Warning: Assymetric aerofoil: '" << filepath << "'\n";
				std::cout << "Coordinates should be symmetrical in x."
					" Camberline may be skewif." << '\n';

				warned = true;
			}
		}

		double z_up{ upper(n_per_side - i - 1, 2) };
		double z_lo{ lower(i, 2) };

		// Averages upper and lower z coordinate.
		camber(i, 2) = (z_up+z_lo)/2;
		camber(i, 0) = x_up;
	}
}

// Scales camber by new chord length.
nc::NdArray<double> Aerofoil::scale_chord(double chord)
{
	nc::NdArray<double> camber_scaled = camber;

	double scaling_factor{
		chord / (camber(camber.shape().rows - 1, 0) - camber(0, 0))
	};

	nc::NdArray<double> A = camber(0, camber.cSlice());
	for (int i{ 1 }; i != camber.shape().rows; i++)
	{
		nc::NdArray<double> B = camber(i, camber.cSlice());
		auto AB = B - A;

		camber_scaled(i, 0) = AB[0] * scaling_factor;
		camber_scaled(i, 2) = AB[2] * scaling_factor;
	}

	return camber_scaled;
}

/// <summary>
/// Scales and redistributes points on camberline.
/// </summary>
/// <param name="n">: # chordwise points</param>
/// <param name="chord">New chordlength</param>
/// <returns>Camberline coordiantes</returns>
nc::NdArray<double> Aerofoil::get_camber_points(int n, double chord)
{
	nc::NdArray<double> camber_scaled{ scale_chord(chord) };

	// Calculate incrimental line length along camberline.
	std::vector<double> L_is{ 0 };
	double L{ 0 };
	for (int i{ 1 }; i != camber.shape().rows; i++)
	{
		L_is.push_back(
			nc::norm(
				camber_scaled(i, camber_scaled.cSlice())
				- camber_scaled(i - 1, camber_scaled.cSlice())
			)[0] + L
		);
		L += L_is[i] - L ;
	};

	nc::NdArray<double> camber_interp = nc::zeros<double>(n + 1, 3);

	double spacing{ L / (n)};
	double dL{ 0 };
	for (int i{ 0 }; i != n + 1; i++)
	{
		// Search for index where cumulative length would lie.
		int P1_i{ utils::search(L_is,dL,"left") };
		int P0_i{ P1_i - 1 };
		if (P0_i < 0) { P0_i = 0; }

		nc::NdArray<double> P0 { camber_scaled(P0_i, camber_scaled.cSlice()) };
		nc::NdArray<double> P1 { camber_scaled(P1_i, camber_scaled.cSlice()) };
		double L0{ L_is[P0_i] };
		double L1{ L_is[P1_i] };

		// Linear interpolation
		double ratio;
		if (dL != L_is[P0_i]) {
			ratio = (dL - L0) / (L1 - L0);
		}
		else {
			ratio = 0;
		}

		nc::NdArray<double> P01{ P1 - P0 };
		nc::NdArray<double> P_interp{ P0 + P01 * ratio };

		camber_interp(i, 0) = P_interp[0];
		camber_interp(i, 2) = P_interp[2];

		dL += spacing;
	}

	return camber_interp;
}