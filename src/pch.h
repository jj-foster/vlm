#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <thread>
#include <cmath>
#include <algorithm>
#include <chrono>

#include <NumCpp/NdArray.hpp>
#include <NumCpp/Functions/zeros.hpp>
#include <NumCpp/Functions/norm.hpp>
#include <NumCpp/Functions/dot.hpp>
#include <NumCpp/Functions/cross.hpp>
#include <NumCpp/Functions/append.hpp>
#include <NumCpp/Functions/vstack.hpp>
#include <NumCpp/Functions/sin.hpp>
#include <NumCpp/Functions/cos.hpp>
#include <NumCpp/Functions/deg2rad.hpp>
#include <NumCpp/Functions/floor.hpp>
#include <NumCpp/Functions/ceil.hpp>
#include <NumCpp/Linalg/solve.hpp>
#include <NumCpp/Functions/matmul.hpp>

#include <nlohmann/json.hpp>

namespace rl {
#include <raylib.h>
#include <raymath.h>

}

#if defined(_WIN32)
	#define NOGDI
	#define NOUSER
#endif

#include <indicators.hpp>

//#if defined (_WIN32)
//	#undef near
//	#undef far
//#endif