#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>

int main()
{

    //std::string filename;
    //
    //std::cout << "plane json: ";
    //std::cin >> filename;
    //
    //std::ifstream f{ filename };
    std::ifstream f{ "D:\\Documents\\Projects\\cpp\\VLM\\data\\plane1.json" };
    if (f.fail()) {
        std::cout << "File not found." << '\n';
        return 0;
    }

    Plane plane1{ f };

    MultiMesh& plane_mesh = *plane1.mesh;
    /*std::vector<Panel>& panels = plane_mesh[0]->getPanels();
    for (auto& p : panels) {
        p.print();
    }*/

    for (Panel& panel : plane_mesh) {
        panel.print();
    }

	return 0;
}