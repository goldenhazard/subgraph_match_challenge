/**
 * @file common.h
 * @brief common headers and type definitions
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "math.h"

using Vertex = int32_t;
using Label = int32_t;
using VertexPair = std::pair<Vertex, Vertex>;
// (u7, <v9, v11>)
using Cmu = std::pair<Vertex, std::set<Vertex>>;

#endif  // COMMON_H_
