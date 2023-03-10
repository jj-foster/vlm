#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>

using json = nlohmann::json;

Plane::Plane(std::ifstream& f) {

    read_json(f);
    calc_ref();

    // generate wing meshes.
    for (auto& wing : wings) {
        wing->generateMesh();
    }

    // populate wing mesh container.
    std::vector<std::shared_ptr<Mesh>> wing_meshes;
    for (auto& wing : wings) {
        std::shared_ptr<Mesh> mesh = wing->getMesh();
        wing_meshes.push_back(mesh);
    }

    // plane mesh container.
    mesh = new MultiMesh { wing_meshes };
}

void Wing::generateMesh()
{
    mesh_ = std::make_shared<Mesh>();
    mesh_->generate(this);
}

/// <summary>
/// Prints plane info.
/// </summary>
void Section::print() {
    std::cout << "m: " << m << '\n';

    std::cout << "leading edges: ";
    std::cout << leading_edge;
    std::cout << "chords: " << chord << std::endl;
    std::cout << "angles of incident: [" << incident << '\n';
    std::cout << "aerofoil file: " << aerofoil.get()->get_filepath() << '\n';

}

/// <summary>
/// Reads json file defining plane.
/// </summary>
/// 
/// <param name="file"> {std::ifstream}: Input .json filesream.</param>
void Plane::read_json(std::ifstream& file) {

    json j_wings = json::parse(file)["wings"];
    file.close();

    std::vector<std::shared_ptr<Aerofoil>> aerofoils;

    for (auto& j_wing : j_wings) {
        Wing wing(j_wing["#chordwise_panels"]);

        json j_sections = j_wing["sections"];

        for (int i{ 0 }; i != j_sections.size(); i++) {
            auto& j_section = j_sections[i];

            // read chords from json
            double chord{ j_section["chord"] };

            // read leading edge coordinates from json
            std::array<double, 3> le = j_section["leading_edge"];
            nc::NdArray<double> leading_edge{ le };

            // read number of spanwise splits from json
            int m(j_section["#spanwise_panels"]);

            // read angles of incident from json
            double incident = j_section["i_angle"];

            // read aerofoil .dat path from json
            std::string aerofoil_file = j_section["aerofoil"];

            // Generate aerofoil if it doesn't already exist.
            int a_index{ -1 };
            std::shared_ptr<Aerofoil> aerofoil;

            for (auto& a : aerofoils) {
                if (a.get()->get_filepath() == aerofoil_file)
                {
                    aerofoil = a;
                    a_index = 1;
                    break;
                }
            }

            if (a_index == -1) {
                aerofoil = std::make_shared<Aerofoil>(aerofoil_file);
                aerofoils.push_back(aerofoil);
            }

            Section section{
                m,
                leading_edge,
                chord,
                incident,
                aerofoil
            };

            //section.print();
            wing.sections.push_back(section);
            wing.n_sections++;

            // Ignore first section. Mesh is generated from tip to root per section.
            if (i != 0) { wing.m_sum += section.m; }
        }

        std::unique_ptr<Wing> wing_ptr = std::make_unique<Wing>(wing);
        wings.push_back(std::move(wing_ptr));
        n_wings++;

    }
}

void Plane::calc_ref()
{
    Wing* main_wing = wings[0].get();

    double b{ 0 };
    double Sw{ 0 };
    double MAC{ 0 };

    double b_i{ 0 };
    for (int i{ 1 }; i != main_wing->sections.size(); i++)
    {
        Section& s_curr{ main_wing->sections[i] };
        Section& s_prev{ main_wing->sections[i - 1] };

        double c_curr{ s_curr.chord };
        double c_prev{ s_prev.chord };

        b_i = 2 * std::sqrt(
            std::pow(s_curr.leading_edge[1] - s_prev.leading_edge[1], 2)
            + std::pow(s_curr.leading_edge[2] - s_prev.leading_edge[2], 2)
        );
        b += b_i;

        Sw += 0.5 * (c_curr + c_prev) * b_i;
    }

    MAC = Sw / b;

    S_ref = Sw;
    b_ref = b;
    c_ref = MAC;
}