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
    for (auto& le : leading_edges) {
        std::cout << le << ' ';
    }
    std::cout << "chords: " << chords << std::endl;
    std::cout << "angles of incident: [" << incidents.at(0)
        << ", " << incidents.at(1) << ']' << '\n';

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
        Wing wing(j_wing["n"]);

        json j_sections = j_wing["section"];

        for (auto& j_section : j_sections) {
            // read chords from json
            std::vector<double> ch = j_section["chords"];
            nc::NdArray<double> chords{ ch };

            // read leading edge coordinates from json
            std::array<std::array<double, 3>, 2> les(j_section["leading_edges"]);

            nc::NdArray<double> le0{ les.at(0) };
            nc::NdArray<double> le1{ les.at(1) };

            // read number of spanwise splits from json
            int m(j_section["m"]);

            // read angles of incident from json
            std::array<double, 2> incidents = j_section["i_angle"];

            Section section(
                m,
                std::array<nc::NdArray<double>, 2> {le0, le1},
                chords,
                incidents
            );

            //section.print();
            wing.sections.push_back(section);
            wing.n_sections++;
            wing.m_sum += section.m;
        }

        std::unique_ptr<Wing> wing_ptr = std::make_unique<Wing>(wing);
        wings.push_back(std::move(wing_ptr));
        n_wings++;

    }
}
