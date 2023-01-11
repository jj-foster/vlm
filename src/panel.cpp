#include <pch.h>

#include <panel.hpp>

Panel::Panel(
    nc::NdArray<double> P1,
    nc::NdArray<double> P2,
    nc::NdArray<double> P3,
    nc::NdArray<double> P4
)
    : P1(P1),
    P2(P2),
    P3(P3),
    P4(P4)
{
    calc_area();
    calc_cp();
    calc_bound_vortex();
    calc_normal();

    dy = nc::norm(P1-P2)[0];
}

/// <summary>
/// Calculates area of panel. Assumes quadrelateral is convex and consts of
/// 2 equally sized triangles.
/// </summary>
void Panel::calc_area() {

    auto P12 = P2-P1;
    auto P23 = P3-P2;
    auto P34 = P4-P3;
    auto P41 = P1-P4;

    S = 0.5*nc::norm(nc::cross(P12,P23)+nc::cross(P34,P41))[0];

}

/// <summary>
/// Calculates panel collocation point. This is the point on the panel on which
/// the velocity induced by a vortex element acts. It lies at 3/4 chord - see
/// Katz & Plotkin for details.
/// </summary>
void Panel::calc_cp() {

    auto c0 = nc::norm(P4-P1)[0];
    auto c1 = nc::norm(P3-P2)[0];

    auto _P1 = P1 + nc::NdArray<double> {c0*0.75,0,0};
    auto _P2 = P2 + nc::NdArray<double> {c1*0.75,0,0};

    auto _P12 = _P2 - _P1;

    cp = 0.5*_P12 + _P1;

}

void Panel::calc_bound_vortex() {
    
    auto c0 = nc::norm(P4-P1)[0];
    auto c1 = nc::norm(P3-P2)[0];

    B = P1 + nc::NdArray<double> {c0*0.25,0,0};
    C = P2 + nc::NdArray<double> {c1*0.25,0,0};
}

void Panel::calc_normal() {

    auto P1cp = cp - P1;
    auto P2cp = cp - P2;

    auto n_ = nc::cross(P2cp,P1cp);
    auto n_normalized = n_ / nc::norm(n_)[0];

    n = n_normalized;

}

// int main() {
//     nc::NdArray<double> A {1,2,3};
//     nc::NdArray<double> B {3,4,5};
//     nc::NdArray<double> C {6,7,8};
//     nc::NdArray<double> D {9,10,11};

//     Panel panel1 {A,B,C,D};

//     auto x = nc::zeros<int>(3);
//     x.print();

//     return 0;
// }