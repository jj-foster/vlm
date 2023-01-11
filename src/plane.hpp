#pragma once

#include <pch.h>

#include <mesh_generator.hpp>

class Mesh;
class Wing;
class Section;

class Plane {

private:
    Mesh plane_mesh;

    void read_json(std::ifstream& file);
    void appendMesh();

public:

    double S_ref;
    double b_ref;
    double c_ref;

    std::vector<std::unique_ptr<Wing>> wings{};
    int n_wings{ 0 };

    Plane(std::ifstream& file);

    void drawMesh();
};

class Wing {
    std::shared_ptr<Mesh> mesh_;

public:
    int n;
    std::vector<Section> sections{};
    int n_sections{ 0 };
    int m_sum{ 0 };

    Wing(int n) : n{ n } {};

    void generateMesh();

    const std::shared_ptr<Mesh> &getMesh() const
    {
        return mesh_;
    }

};

class Section {

public:

    int m;
    std::array<nc::NdArray<double>, 2> leading_edges;
    nc::NdArray<double> chords;
    std::array<double, 2> incidents;

    Section(
        int m,
        std::array<nc::NdArray<double>, 2> leading_edges,
        nc::NdArray<double> chords,
        std::array<double,2> incidents
    )
        : m{ m },
        leading_edges{ leading_edges },
        chords{ chords },
        incidents{ incidents }
    {};

    void print();

};