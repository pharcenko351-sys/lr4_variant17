#pragma once
#include <cstddef>
#include <mutex>
#include <string>

class Data
{
public:
    static const std::size_t M = 2;

    int get(std::size_t i) const;
    void set(std::size_t i, int value);
    operator std::string() const;

private:
    int f_[M] = {0, 0};
    mutable std::mutex mtx_;
};
