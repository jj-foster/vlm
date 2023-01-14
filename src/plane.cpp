#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>

using json = nlohmann::json;

Plane::Plane(std::ifstream& f) {

    read_json(f);

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

}

/// <summary>
/// Reads json file defining plane.
/// </summary>
/// 
/// <param name="file"> {std::ifstream}: Input .json filesream.</param>
void Plane::read_json(std::ifstream& file) {
    json j_plane = json::parse(file)["plane"];
    file.close();

    S_ref = j_plane["S_ref"];
    b_ref = j_plane["b_ref"];
    c_ref = j_plane["c_ref"];

    json j_wings(j_plane["wing"]);

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

            Section section(
                m,
                leading_edge,
                chord,
                incident,
                aerofoil_file
            );

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
