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

    //nc::NdArray<double> const getPoints() const {return points;}
    nc::NdArray<double> const &getPoints() const { return points; }
    std::vector<Panel> &getPanels() { return panels; }

};

class MultiMesh {
private:
    std::vector<std::shared_ptr<Mesh>> meshes;

public:

    MultiMesh() {};
    MultiMesh(std::vector<std::shared_ptr<Mesh>> meshes) : meshes{ meshes } {}

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
        std::vector<std::shared_ptr<Mesh>>::iterator currentMesh;
        std::vector<std::shared_ptr<Mesh>>::iterator endMesh;

        std::vector<Panel>::iterator currentPanel;
        std::vector<Panel>::iterator endPanel;

    public:
        // Constructor. Assigns current and end panels and meshes.
        PanelIterator(std::vector<std::shared_ptr<Mesh>> meshes)
            : meshes{ meshes }
            , currentMesh{ meshes.begin() }
            , endMesh{ meshes.end() - 1 }
        {
            currentPanel = currentMesh->get()->getPanels().begin();
            endPanel = endMesh->get()->getPanels().end() - 1;
        }

        // ++ overload. Increments over each mesh then each panel.
        PanelIterator& operator++()
        {
            currentPanel++;

            if (currentPanel == endPanel)
            {
                currentMesh++;
                if (currentMesh != endMesh) {
                    currentPanel = currentMesh->get()->getPanels().begin();
                    endPanel = endMesh->get()->getPanels().end() -1;
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
            return currentMesh->get() != other.endMesh->get() || currentPanel != other.endPanel;
        }

        // != overload. Also for use in for loops.
        bool operator==(const PanelIterator& other) {
            return currentMesh->get() == other.endMesh->get() && currentPanel == other.endPanel;
        }

        // Set the current mesh and panel to their respective iterator ends.
        // Used for end() function for range-based for loops.
        void setEnd() {
            endMesh = meshes.end() - 1;
            currentMesh = endMesh;
            currentPanel = currentMesh->get()->getPanels().end() -1 ;
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

