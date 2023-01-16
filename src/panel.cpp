#include <pch.h>

#include <panel.hpp>

Panel::Panel(
    nc::NdArray<double> P1,
    nc::NdArray<double> P2,
    nc::NdArray<double> P3,
    nc::NdArray<double> P4,
    int id
)
    : P1(P1),
    P2(P2),
    P3(P3),
    P4(P4),
    id(id)
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

    area = 0.5*nc::norm(nc::cross(P12,P23)+nc::cross(P34,P41))[0];

}

/// <summary>
/// Calculates panel collocation point. This is the point on the panel on which
/// the velocity induced by a vortex element acts. It lies at 3/4 chord - see
/// Katz & Plotkin for details.
/// </summary>
void Panel::calc_cp() {

    double side_in_norm = nc::norm(P4-P1)[0];
    double side_out_norm = nc::norm(P3-P2)[0];

    nc::NdArray<double> side_in_vec = 0.75 * (P4 - P1);
    nc::NdArray<double> side_out_vec = 0.75 * (P4 - P1);

    auto _P1 = P1 + side_in_vec;
    auto _P2 = P2 + side_out_vec;

    auto _P12 = _P2 - _P1;

    cp = 0.5*_P12 + _P1;

}

void Panel::calc_bound_vortex() {

    double side_in_norm = nc::norm(P4 - P1)[0];
    double side_out_norm = nc::norm(P3 - P2)[0];

    nc::NdArray<double> side_in_vec = 0.25 * (P4 - P1);
    nc::NdArray<double> side_out_vec = 0.25 * (P4 - P1);

    B = P1 + side_in_vec;
    C = P2 + side_in_vec;
}

void Panel::calc_normal() {

    auto P1cp = cp - P1;
    auto P2cp = cp - P2;

    auto n_ = nc::cross(P2cp,P1cp);
    auto n_normalized = n_ / nc::norm(n_)[0];

    normal = n_normalized;

}

void Panel::print()
{   
    std::cout << "PANEL " << id << '\n';
    std::cout << '\t' << "Corners: " << '\n';
    std::cout << '\t' << P1;
    std::cout << '\t' << P2;
    std::cout << '\t' << P3;
    std::cout << '\t' << P4 << '\n';

    std::cout << '\t' << "Area: " << area << '\n';
    std::cout << '\t' << "Co-location point: " << cp;
    std::cout << '\t' << "dy: " << dy << '\n';
    std::cout << '\t' << "Normal: " << normal << '\n';

    if (gamma != 0) { std::cout << '\t' << "gamma: " << gamma << '\n'; }
    if (dL != 0) { std::cout << '\t' << "dL: " << dL << '\n'; }
    if (w_ind != 0) { std::cout << '\t' << "w_ind: " << w_ind << '\n'; }
    if (dDi != 0) { std::cout << '\t' << "dD: " << dDi << '\n'; }

}
