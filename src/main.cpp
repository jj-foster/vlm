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

    for (Panel& panel : *plane.mesh) {
        panel.print();
    }


    Vlm vlm{ &plane };
    double Qinf{ 1 };
    double alpha{ 0 };
    double beta{ 0 };
    double rho{ 1.225 };
    vlm.runHorseshoe(Qinf, alpha, beta, rho);

    double CL{ vlm.getCL() };
    double CDi{ vlm.getCDi() };

    std::cout << "CL: " << CL << '\n';
    std::cout << "CDi: " << CDi << "\n\n";

    Viewer window{ plane.mesh, true, true };
    window.startWindowThreadJoined();

    //std::cin.get();

    return 0;
}