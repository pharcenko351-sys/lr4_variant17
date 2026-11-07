#pragma once
#include <vector>
#include "ops.h"

double run(const std::vector<std::vector<Op>> &chunks, int threads);
