#pragma once

#include <pch.h>

#include <panel.hpp>
#include <plane.hpp>

using rl::Vector3;


class Wing;

/// <summary>
/// Mesh object (per wing). Holds vertex and panel data.
/// </summary>
class Mesh {
private:
    nc::NdArray<double> points;
    std::vector<Panel> panels;

    void calc_points(Wing* wing);
    void calc_panels(Wing* wing);

public:
    Mesh() = default;

    void generate(Wing* wing);

    nc::NdArray<double> const &getPoints() const { return points; }

    // getPanels cannot be constant because the panel needs to be updated
    // when vlm is running.
    std::vector<Panel> &getPanels() { return panels; }

};

class MultiMesh {
private:
    std::vector<std::shared_ptr<Mesh>> meshes;

public:
    const int nPanels;

    // Constructor.
    MultiMesh(std::vector<std::shared_ptr<Mesh>> meshes);

    // [] overload. Allows easy access for each mesh in multimesh container.
    Mesh* at(int index) {
        return meshes[index].get();
    }

    /// <summary>
    /// Iterates over each panel for each mesh in meshes.
    /// </summary>
    class PanelIterator
    {
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;
        // Using indices because mesh iterators would go out of scope when
        // temp objects are created and copied by the range-based for loop.
        size_t currentMeshIndex, endMeshIndex;
        std::vector<Panel>::iterator currentPanel, endPanel;

    public:
        // Constructor. Assigns current and end panels and meshes.
        PanelIterator(std::vector<std::shared_ptr<Mesh>> meshes)
            : meshes{ meshes }
            , currentMeshIndex{ 0 }
            , endMeshIndex{ meshes.size()}  // One past the end of the array
        {
            currentPanel = meshes[0]->getPanels().begin();
            endPanel = meshes[0]->getPanels().end();    // One past the end of the array
        }

        // ++ overload. Increments over each mesh then each panel.
        PanelIterator& operator++()
        {
            // If all panels in the current mesh have been iterated through,
            // increment the mesh.
            currentPanel++;
            if (currentPanel == endPanel)
            {
                currentMeshIndex++;
                // Redefine panels using next mesh.
                if (currentMeshIndex != endMeshIndex) {
                    currentPanel = meshes[currentMeshIndex]->getPanels().begin();
                    endPanel = meshes[currentMeshIndex]->getPanels().end();
                }
            }

            return *this;
        }

        // ++ overload but different? Used for traditional for loops.
        PanelIterator operator++(int)
        {
            // If all panels in the current mesh have been iterated through,
            // increment the mesh.
            currentPanel++;
            if (currentPanel == endPanel)
            {
                currentMeshIndex++;
                // Redefine panels using next mesh.
                if (currentMeshIndex != endMeshIndex) {
                    currentPanel = meshes[currentMeshIndex]->getPanels().begin();
                    endPanel = meshes[currentMeshIndex]->getPanels().end();
                }
            }

            return *this;
        }

        // * overload. Returns a pointer to the current panel in the iterator.
        Panel& operator*() {
            return *currentPanel;
        }

        // != overload. For use in for loops.
        bool operator!=(const PanelIterator& other) {
            return !(*this == other);
        }

        // == overload. Also for use in for loops.
        bool operator==(const PanelIterator& other) {
            return currentMeshIndex == other.currentMeshIndex &&
                currentPanel == other.currentPanel;
        }

        // Set the current mesh and panel to their respective iterator ends.
        // Used for end() function for range-based for loops.
        void setEnd() {
            currentMeshIndex = endMeshIndex;
            currentPanel = meshes[currentMeshIndex - 1]->getPanels().end();
        }
    };

    PanelIterator begin() {
        return PanelIterator(meshes);
    }

    PanelIterator end() {
        PanelIterator iter{ meshes };
        iter.setEnd();

        return iter;
    }

    const std::vector<std::array<rl::Vector3, 2>> getRlLines();

    const int sumPanels() {
        int total{ 0 };
        for (auto it = this->begin(); it != this->end(); it++) {
            total++;
        }

        return total;
    }

};
