#pragma once

#include <pch.h>

class Panel {
private:
    nc::NdArray<double> P1;
    nc::NdArray<double> P2;
    nc::NdArray<double> P3;
    nc::NdArray<double> P4;

    int id;

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
        nc::NdArray<double> P4,
        int id
    );

    void calc_area();
    void calc_cp();
    void calc_bound_vortex();
    void calc_normal();

    const std::array<nc::NdArray<double>, 4> getCorners() const {
        return { P1, P2, P3, P4 };
    }

    void print();

};