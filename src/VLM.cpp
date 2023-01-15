#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>
#include <viewer.hpp>

int main()
{

    //std::string filename;
    //
    //std::cout << "plane json: ";
    //std::cin >> filename;
    //
    //std::ifstream f{ filename };
    std::ifstream f{ "D:\\Documents\\C++ Projects\\1. vlm\\data\\plane1.json" };
    if (f.fail()) {
        std::cout << "File not found." << '\n';
        return 0;
    }

    Plane plane1{ f };

    /*for (Panel& panel : *plane1.mesh) {
        panel.print();
    }*/

    Viewer window{plane1.mesh, true, true};
    
    window.startWindowThreadJoined();

	return 0;
}