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

    // Loop through chordwise splits
    for (int i{ 0 }; i <= (wing->n); i++) {
        int s(0);

        // Loop through sections i.e. where chord, leading edge coordinates 
        // and # spanwise splits change.
        for (int j{ 1 }; j != wing->sections.size(); j++) {

            Section& section_curr{ wing->sections[j] };
            Section& section_prev{ wing->sections[j - 1] };

            // Increment between spanwise splits down chord.
            auto P1_increment = (double)i * section_prev.chord / wing->n;
            auto P2_increment = (double)i * section_curr.chord / wing->n;

            // Vectors between spanwise splits down chord.
            nc::NdArray<double> P1_inc_vec{
                P1_increment * nc::cos(nc::deg2rad(section_prev.incident)),
                0,
                -P1_increment* nc::sin(nc::deg2rad(section_curr.incident))
            };

            nc::NdArray<double> P2_inc_vec{
                P2_increment * nc::cos(nc::deg2rad(section_prev.incident)),
                0,
                -P2_increment * nc::sin(nc::deg2rad(section_curr.incident))
            };

            // Vector between leading edge locations.
            // Shifts start and end coordiantes of spanwise split down chord.
            auto P1 = section_prev.leading_edge + P1_inc_vec;
            auto P2 = section_curr.leading_edge + P2_inc_vec;
            auto P1P2 = P2 - P1;

            // ... spanwise points.
            for (int k{ 0 }; k < (section_curr.m + 1); k++) {

                // Skips double generating points at connection between sections.
                if (k == 0 && s != 0) {
                    continue;
                }

                else {
                    double t = (double)k / section_curr.m;
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