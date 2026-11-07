#pragma once
#include <string>
#include <vector>

struct Op
{
    char type;
    int field;
    int value;
};

std::vector<Op> load(const std::string &name);
