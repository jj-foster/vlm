#include <pch.h>

#include <mesh.hpp>
#include <panel.hpp>

using rl::Vector3;

void Mesh::generate(Wing* wing)
{
    calc_points(wing);
    calc_panels(wing);
}

/// <summary>
/// Calculates coordinates of mesh points..
/// </summary>
void Mesh::calc_points(Wing* wing)
{
    // Pre-generate camber coordiantes for each section
    std::vector<nc::NdArray<double>> cambers;
    for (Section& section : wing->sections)
    {
        const nc::NdArray<double>& camber{
                section.aerofoil.get()->get_camber_points( wing->n, section.chord )
        };

        cambers.push_back(camber);
    }

    int N = (wing->m_sum + 1) * (wing->n + 1);
    points = nc::zeros<double>(N, 3);

    int point_count{ 0 };

    // Loop through chordwise splits
    for (int i{ 0 }; i <= wing->n; i++) {
        int s(0);

        // Loop through sections i.e. where chord, leading edge coordinates 
        // and # spanwise splits change.
        for (int j{ 1 }; j != wing->sections.size(); j++) {

            Section& section_curr{ wing->sections[j] };
            Section& section_prev{ wing->sections[j - 1] };

            // Get camber coordinates for each section.
            const nc::NdArray<double>& camber_curr{ cambers[j] };
            const nc::NdArray<double>& camber_prev{ cambers[j - 1] };

            // Increment between spanwise splits down chord.
            auto P1_increment{ camber_prev(i, camber_prev.cSlice()) };
            auto P2_increment{ camber_curr(i, camber_curr.cSlice()) };

            // Rotation about leading edge for incident angle
            double cos_prev{ nc::cos(nc::deg2rad(section_prev.incident)) };
            double sin_prev{ -nc::sin(nc::deg2rad(section_prev.incident)) };
            double cos_curr{ nc::cos(nc::deg2rad(section_curr.incident)) };
            double sin_curr{ -nc::sin(nc::deg2rad(section_curr.incident)) };

            // Vectors between spanwise splits down chord.
            nc::NdArray<double> P1_inc_vec{
                P1_increment[0] * cos_prev - P1_increment[2] * sin_prev,
                0,
                P1_increment[0] * sin_prev + P1_increment[2] * cos_prev
            };

            nc::NdArray<double> P2_inc_vec{
                P2_increment[0] * cos_curr - P2_increment[2] * sin_curr,
                0,
                P2_increment[0] * sin_curr + P2_increment[2] * cos_curr
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

            bool leading_edge{ (i == 0) ? true : false };

            Panel panel(P1, P2, P3, P4, panel_count,leading_edge);

            panels.push_back(panel);

            panel_count++;
        }
    }

}

MultiMesh::MultiMesh(std::vector<std::shared_ptr<Mesh>> meshes)
    : meshes{ meshes }
    , nPanels{sumPanels()}
{

}

const std::vector<std::array<Vector3, 2>> MultiMesh::getRlLines()
{
    const int line_size = nPanels * 4;
    std::vector<std::array<Vector3,2>> lines(line_size);

    int i{ 0 };
    for (auto it = this->begin(); it !=this->end(); it++)
    {
        const Panel& currentPanel = *it;
        std::array<nc::NdArray<double>, 4> corners{ currentPanel.getCorners() };

        Vector3 P1_rl{ (float)corners[0][0],(float)corners[0][1],(float)corners[0][2] };
        Vector3 P2_rl{ (float)corners[1][0],(float)corners[1][1],(float)corners[1][2] };
        Vector3 P3_rl{ (float)corners[2][0],(float)corners[2][1],(float)corners[2][2] };
        Vector3 P4_rl{ (float)corners[3][0],(float)corners[3][1],(float)corners[3][2] };

        lines[i] = { P1_rl, P2_rl };
        lines[i+1] = { P2_rl, P3_rl };
        lines[i+2] = { P3_rl, P4_rl };
        lines[i+3] = { P4_rl, P1_rl };

        i+=4;
    }

    return lines;
}
