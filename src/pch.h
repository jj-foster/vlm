#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <thread>

#include <NumCpp/NdArray.hpp>
#include <NumCpp/Functions/zeros.hpp>
#include <NumCpp/Functions/norm.hpp>
#include <NumCpp/Functions/cross.hpp>
#include <NumCpp/Functions/append.hpp>
#include <NumCpp/Functions/vstack.hpp>
#include <NumCpp/Functions/sin.hpp>
#include <NumCpp/Functions/cos.hpp>
#include <NumCpp/Functions/deg2rad.hpp>
#include <NumCpp/Functions/floor.hpp>
#include <NumCpp/Functions/ceil.hpp>
#include <NumCpp/Functions/sort.hpp>

#include <nlohmann/json.hpp>

namespace rl
{ 
#include <raylib.h>
#include <raymath.h>
}