#pragma once

#include <memory>
#include <numeric>
#include <algorithm>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <vector>
#include <queue>
#include <functional>
#include <optional>
#include <limits>
#include <cmath>

#include "raylib.h"
#include "raymath.h"
#include "util/cell.h"
#include "util/raymath_operators.h"
#include "util/jmath.h"
#include "util/time.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "util/json.h"

enum class Side {
    North = 0,
    East = 1,
    South = 2,
    West = 3
};
