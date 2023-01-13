#pragma once

#include <pch.h>

#include <panel.hpp>
#include <plane.hpp>

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
    // Constructor.
    MultiMesh(std::vector<std::shared_ptr<Mesh>> meshes) : meshes{ meshes } {}

    // [] overload. Allows easy access for each mesh in multimesh container.
    Mesh* operator[](int index) {
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
            , endMeshIndex{ meshes.size() }
        {
            currentPanel = meshes[0]->getPanels().begin();
            endPanel = meshes[0]->getPanels().end();
        }

        // ++ overload. Increments over each mesh then each panel.
        PanelIterator& operator++()
        {
            // If all panels in the current mesh have been iterated through,
            // increment the mesh.
            if (currentPanel == endPanel)
            {
                currentMeshIndex++;
                // Redefine panels using next mesh.
                if (currentMeshIndex != endMeshIndex) {
                    currentPanel = meshes[currentMeshIndex]->getPanels().begin();
                    endPanel = meshes[currentMeshIndex]->getPanels().end();
                }
            }
            else
            {
                currentPanel++;
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
            currentMeshIndex = endMeshIndex - 1;
            currentPanel = meshes[currentMeshIndex]->getPanels().end();
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

    void draw();

};

