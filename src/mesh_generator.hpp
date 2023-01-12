#pragma once

#include <pch.h>

#include <panel.hpp>
#include <plane.hpp>

class Wing;

class Mesh {
private:
    nc::NdArray<double> points;
    std::vector<Panel> panels;

    void calc_points(Wing* wing);
    void calc_panels(Wing* wing);

public:
    Mesh() = default;

    void generate(Wing* wing);

    nc::NdArray<double> const getPoints() const {return points;}
    std::vector<Panel> const getPanels() const { return panels; }

};


/// <summary>
/// Iterator for a container of multiple mesh objects.
/// Returns each panel in-turn of the meshes.
/// </summary>
class MeshIterator
{
private:
    // Mesh container to iterate over
    std::vector<std::shared_ptr<Mesh>> meshes;
    // Iterators to keep track of current position and end position of the meshes container.
    std::vector<std::shared_ptr<Mesh>>::iterator currentMesh, endMesh;
    // Iterators to keep track of current position andend position of panels vector of the
    // mesh object that the iterator is currently pointing to.
    std::vector<Panel>::const_iterator currentPanel, endPanel;

public:
    // Constructor
    MeshIterator(std::vector<std::shared_ptr<Mesh>> meshes);

    /// <summary>
    /// Returns reference to the Panel object that currentPanel iterator is currently
    /// pointing to.
    /// </summary>
    /// <returns>currentPanel</returns>
    const Panel& operator*() const {
        return *currentPanel;
    }

    MeshIterator& operator++();

    /// <summary>
    /// != operator overload.
    /// Compares currentMesh and currentPanel iterators with the end iterators endMesh and
    /// endPanel, respectively, to check whether the iterator has reached the end of the
    /// combined panels vector. Used in loops.
    /// </summary>
    /// <param name="other"></param>
    /// <returns></returns>
    bool operator!=(const MeshIterator& other) {
        return currentMesh != other.endMesh || currentPanel != other.endPanel;
    }

    static MeshIterator begin(std::vector<std::shared_ptr<Mesh>> meshes) {
        return MeshIterator(meshes);
    }

    static MeshIterator end(std::vector<std::shared_ptr<Mesh>> meshes) {
        MeshIterator iter{ meshes };
        iter.endMesh = meshes.end();

        return iter;
    }

};