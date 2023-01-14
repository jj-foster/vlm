#pragma once

#include <pch.h>

class Aerofoil
{
private:
	std::string filepath;
	nc::NdArray<double> coords;
	nc::NdArray<double> camber;

	// Utility functions
	std::vector<std::string> split_string_delim(std::string s, char del);
	nc::NdArray<double> flat_plate();
	std::array<nc::NdArray<double>, 2> split_surface();
	nc::NdArray<double> scale_chord(double chord);

	// Method functions
	void read_dat();
	void calc_camber();

public:
	Aerofoil(std::string filepath);

	nc::NdArray<double> get_camber_points(int n, double chord);
};