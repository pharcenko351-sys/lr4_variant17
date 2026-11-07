#include <iostream>
#include <vector>
#include <format>
#include <filesystem>
#include <cstdlib>
#include "config.h"
#include "generator.h"
#include "ops.h"
#include "runner.h"
using namespace std;

int main()
{
#if defined(_MSVC_LANG)
    system("chcp 65001");
#endif
    filesystem::create_directories(DIR);
    cout << "Generating test files..." << endl;
    for (const auto &s : scenarios)
        generate(s);

    double table[3][3] = {};
    for (size_t si = 0; si < scenarios.size(); ++si)
    {
        const auto &s = scenarios[si];
        vector<vector<Op>> chunks;
        for (int c = 0; c < CHUNKS; ++c)
            chunks.push_back(load(file_name(s, c)));
        cout << "\nScenario " << s.name << endl;
        for (int threads = 1; threads <= 3; ++threads)
        {
            double sum = 0;
            for (int r = 0; r < REPEATS; ++r)
            {
                double t = run(chunks, threads);
                cout << format("  threads={} run={} time={:.4f} s\n", threads, r + 1, t);
                sum += t;
            }
            table[si][threads - 1] = sum / REPEATS;
        }
    }

    cout << format("\nAverage time over {} runs, seconds ({} operations each):\n",
                   REPEATS, CHUNKS * OPS_PER_CHUNK);
    cout << "scenario   1 thread   2 threads  3 threads\n";
    for (size_t si = 0; si < scenarios.size(); ++si)
        cout << format("   {}       {:8.4f}   {:8.4f}   {:8.4f}\n", scenarios[si].name,
                       table[si][0], table[si][1], table[si][2]);
    return 0;
}
