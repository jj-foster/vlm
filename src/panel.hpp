#pragma once

#include <pch.h>

/// Panel coordinates:
///      P1--P2->y
///      :   :
///      P4--P3
///      |
///      x
/// 
/// Vortex coordinates:
///     B--C->y
///     :  :
///     A--D
///     |
///     x
class Panel {
private:
    nc::NdArray<double> P1;
    nc::NdArray<double> P2;
    nc::NdArray<double> P3;
    nc::NdArray<double> P4;

    int id;

public:
    double area;
    nc::NdArray<double> cp;

    const bool leading_edge;

    // Relative to panel centre:
    nc::NdArray<double> A;  // +x, -y
    nc::NdArray<double> B;  // -x, -y
    nc::NdArray<double> C;  // -x, +y
    nc::NdArray<double> D;  // +x, +y

    // Therefore:
    //  AB = trailing vortex inboard,
    //  BC = bound vortex,
    //  CD = trailing vortex outboard,
    //  DA = starting vortex.

    nc::NdArray<double> normal;

    double dy;              // span
    double vorticity{ 0 };  // induced vorticity
    double dL{ 0 };         // induced lift
    double w_ind{ 0 };      // induced velocity
    double dDi{ 0 };        // induced drag

    Panel(
        nc::NdArray<double> P1,
        nc::NdArray<double> P2,
        nc::NdArray<double> P3,
        nc::NdArray<double> P4,
        int id,
        bool leading_edge
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