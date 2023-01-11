#pragma once

#include <pch.h>

#include <panel.hpp>
#include <plane.hpp>

class Wing;

class Mesh {
private:
    nc::NdArray<double> points;
    std::vector<Panel> panels;

    void calc_points(Wing* wing);
    void calc_panels(Wing* wing);

public:
    Mesh() = default;

    void generate(Wing* wing);
    void append(std::shared_ptr<Mesh> mesh1);

    nc::NdArray<double> const getPoints() const {return points;}
    std::vector<Panel> const getPanels() const { return panels; }

};