#pragma once
#include <string>
#include <vector>

inline const int CHUNKS = 6;
inline const int OPS_PER_CHUNK = 400000;
inline const int REPEATS = 5;
inline const char *DIR = "tests";

struct Scenario
{
    std::string name;
    std::vector<double> w;
};

inline const std::vector<Scenario> scenarios = {
    {"a", {5, 5, 30, 5, 55}},
    {"b", {20, 20, 20, 20, 20}},
    {"c", {2, 60, 3, 30, 5}},
};
