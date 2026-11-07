#include "generator.h"
#include <fstream>
#include <random>
#include <format>
using namespace std;

string file_name(const Scenario &s, int chunk)
{
    return format("{}/{}_{}.txt", DIR, s.name, chunk);
}

void generate(const Scenario &s)
{
    mt19937 e{random_device()()};
    discrete_distribution<int> kind(s.w.begin(), s.w.end());
    for (int c = 0; c < CHUNKS; ++c)
    {
        ofstream f(file_name(s, c));
        for (int i = 0; i < OPS_PER_CHUNK; ++i)
        {
            switch (kind(e))
            {
            case 0: f << "read 0\n"; break;
            case 1: f << "write 0 1\n"; break;
            case 2: f << "read 1\n"; break;
            case 3: f << "write 1 1\n"; break;
            default: f << "string\n"; break;
            }
        }
    }
}
