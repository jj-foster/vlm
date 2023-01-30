#include <pch.h>

#include <plane.hpp>
#include <mesh.hpp>
#include <viewer.hpp>
#include <vlm.hpp>

int main()
{

    //std::string filename;
    //
    //std::cout << "plane json: ";
    //std::cin >> filename;
    //
    //std::ifstream f{ filename };
    std::ifstream f{ "D:\\Documents\\C++ Projects\\1. vlm\\data\\wing.json" };
    if (f.fail()) {
        std::cout << "File not found." << '\n';
        return 0;
    }

    Plane plane{ f };

    /*for (Panel& panel : *plane.mesh) {
        panel.print();
    }*/

    
    Vlm vlm{ &plane };
    double Qinf{ 10 };
    double alpha{ 10 };
    double beta{ 0 };
    double rho{ 1.225 };

    auto start{ std::chrono::high_resolution_clock::now() };
    //vlm.runLiftingLine(Qinf, alpha, beta, rho);
    vlm.runLiftingSurface(Qinf, alpha, beta, rho, 10);
    //exit(0);

    auto stop{ std::chrono::high_resolution_clock::now() };

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start) << '\n';

    // std::cout << "CL: " << vlm.CL << '\n';
    // std::cout << "CDi: " << vlm.CDi << "\n\n";

    Viewer window{ &vlm, false, false };
    window.startWindowThreadJoined();

    //std::cin.get();

    return 0;
}