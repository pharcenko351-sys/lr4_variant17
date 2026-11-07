#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <latch>
#include <format>
#include <filesystem>
#include <cstdlib>
#include "data.h"
using namespace std;

const int CHUNKS = 6;
const int OPS_PER_CHUNK = 400000;
const int REPEATS = 5;
const char *DIR = "tests";

struct Scenario
{
    string name;
    vector<double> w;
};

const vector<Scenario> scenarios = {
    {"a", {5, 5, 30, 5, 55}},
    {"b", {20, 20, 20, 20, 20}},
    {"c", {2, 60, 3, 30, 5}},
};

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

struct Op
{
    char type;
    int field;
    int value;
};

vector<Op> load(const string &name)
{
    ifstream f(name);
    if (!f)
        throw runtime_error("Cannot open " + name);
    vector<Op> ops;
    string cmd;
    while (f >> cmd)
    {
        Op op{cmd[0], 0, 0};
        if (cmd == "read")
            f >> op.field;
        else if (cmd == "write")
            f >> op.field >> op.value;
        ops.push_back(op);
    }
    return ops;
}

void work(Data &d, const vector<Op> &ops, latch &start, size_t &sink)
{
    size_t local = 0;
    start.arrive_and_wait();
    for (const auto &op : ops)
    {
        if (op.type == 'r')
            local += d.get(op.field);
        else if (op.type == 'w')
            d.set(op.field, op.value);
        else
            local += string(d).size();
    }
    sink = local;
}

double run(const vector<vector<Op>> &chunks, int threads)
{
    Data d;
    vector<vector<Op>> mine(threads);
    for (int c = 0; c < CHUNKS; ++c)
        mine[c % threads].insert(mine[c % threads].end(),
                                 chunks[c].begin(), chunks[c].end());
    vector<size_t> sinks(threads, 0);
    latch start(threads + 1);
    chrono::steady_clock::time_point t1;
    {
        vector<jthread> pool;
        for (int t = 0; t < threads; ++t)
            pool.emplace_back(work, ref(d), cref(mine[t]), ref(start), ref(sinks[t]));
        start.arrive_and_wait();
        t1 = chrono::steady_clock::now();
    }
    auto t2 = chrono::steady_clock::now();
    return chrono::duration<double>(t2 - t1).count();
}

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
