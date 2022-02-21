#pragma once

#include <vector>
#include "../ext/libtmx-parser/src/tmxparser.h"
#include "tiny_ecs.hpp"

std::vector<Entity> getTiles(const std::string& filepath);