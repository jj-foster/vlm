#pragma once

#include <pch.h>

#include <mesh.hpp>
#include <aerofoil.hpp>
#include <wake.hpp>

class Mesh;
class MultiMesh;
class Wake;

class Wing;
class Section;

class Plane {

private:
    void read_json(std::ifstream& file);
    void calc_ref();

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
    std::vector<std::shared_ptr<Wake>> wakes;
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
    std::shared_ptr<Aerofoil> aerofoil;

    Section(
        int m,
        nc::NdArray<double> leading_edge,
        double chord,
        double incident,
        std::shared_ptr<Aerofoil> aerofoil
    )
        : m{ m }
        , leading_edge{ leading_edge }
        , chord{ chord }
        , incident{ incident }
        , aerofoil{ aerofoil }
    {
        
    };

    void print();

};