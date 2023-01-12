#include <pch.h>

#include <mesh.hpp>
#include <panel.hpp>

void Mesh::generate(Wing* wing)
{
    calc_points(wing);
    calc_panels(wing);
}

/// <summary>
/// Calculates coordinates of mesh points. (Flat plate, no camber).
/// </summary>
void Mesh::calc_points(Wing* wing) {
    int N = (wing->m_sum + 1) * (wing->n + 1);
    points = nc::zeros<double>(N, 3);

    int point_count{ 0 };
    for (int i{ 0 }; i <= (wing->n); i++) {
        int s(0);
        for (Section& section : wing->sections) {

            auto dx_P1 = (double)i * section.chords(0, 0) / wing->n;
            auto dx_P2 = (double)i * section.chords(0, 1) / wing->n;

            auto P1 = section.leading_edges.at(0) + nc::NdArray<double> {dx_P1, 0, 0};
            auto P2 = section.leading_edges.at(1) + nc::NdArray<double> {dx_P2, 0, 0};
            auto P1P2 = P2 - P1;

            for (int j{ 0 }; j < (section.m + 1); j++) {
                if (j == 0 && s != 0) {
                    continue;
                }
                else {
                    double t = (double)j / section.m;
                    auto x = P1P2 * t + P1;

                    points(point_count, 0) = x(0, 0);
                    points(point_count, 1) = x(0, 1);
                    points(point_count, 2) = x(0, 2);

                    point_count++;
                }
            }
            s++;
        }
    }
}

/// <summary>
/// Panels are generated spanwise, then chordwise. Sections are ignored.
/// Panel definition:
/// 
/// P1--P2 -> y
/// :   :
/// P4--P3
/// |
/// x
/// 
/// </summary>
void Mesh::calc_panels(Wing* wing) {
    int panel_count{ 0 };

    for (int i{ 0 }; i < (wing->n); i++)
    {
        for (int j{ 0 }; j < (wing->m_sum); j++)
        {
            int n{ j + (wing->m_sum + 1) * i};

            auto P1 = points(n, points.cSlice());
            auto P2 = points(n + 1, points.cSlice());
            auto P3 = points(n + wing->m_sum + 2, points.cSlice());
            auto P4 = points(n + wing->m_sum + 1, points.cSlice());

            Panel panel(P1, P2, P3, P4, panel_count);

            panels.push_back(panel);

            panel_count++;
        }
    }

}

void MultiMesh::draw()
{

}