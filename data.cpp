#include "data.h"
#include <format>
#include <mutex>
#include <stdexcept>
using namespace std;

int Data::get(size_t i) const
{
    if (i >= M)
        throw out_of_range("Incorrect field number!");
    lock_guard lock(mtx_);
    return f_[i];
}

void Data::set(size_t i, int value)
{
    if (i >= M)
        throw out_of_range("Incorrect field number!");
    lock_guard lock(mtx_);
    f_[i] = value;
}

Data::operator string() const
{
    int a, b;
    {
        lock_guard lock(mtx_);
        a = f_[0];
        b = f_[1];
    }
    return format("({}, {})", a, b);
}
