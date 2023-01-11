#pragma once

//#include <NumCpp/NdArray.hpp>

#include <pch.h>

class Panel {
private:
    nc::NdArray<double> P1;
    nc::NdArray<double> P2;
    nc::NdArray<double> P3;
    nc::NdArray<double> P4;

public:
    double S;
    nc::NdArray<double> cp;
    nc::NdArray<double> B;
    nc::NdArray<double> C;
    nc::NdArray<double> n;

    double dy;
    double gamma;
    double dL;
    double w_ind;
    double dD;

    Panel(
        nc::NdArray<double> P1,
        nc::NdArray<double> P2,
        nc::NdArray<double> P3,
        nc::NdArray<double> P4
    );

    void calc_area();
    void calc_cp();
    void calc_bound_vortex();
    void calc_normal();

};