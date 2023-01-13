#pragma once

#include <pch.h>

#include <mesh.hpp>

class Mesh;
class MultiMesh;
class Wing;
class Section;

class Plane {

private:
    void read_json(std::ifstream& file);

public:

    double S_ref;
    double b_ref;
    double c_ref;

    MultiMesh* mesh;

    std::vector<std::unique_ptr<Wing>> wings{};
    int n_wings{ 0 };

    Plane(std::ifstream& file);

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

    std::shared_ptr<Mesh> &getMesh()
    {
        return mesh_;
    }

};

class Section {

public:

    int m;
    nc::NdArray<double> leading_edge;
    double chord;
    double incident;

    Section(
        int m,
        nc::NdArray<double> leading_edge,
        double chord,
        double incident
    )
        : m{ m }
        , leading_edge{ leading_edge }
        , chord{ chord }
        , incident{ incident }
    {};

    void print();

};