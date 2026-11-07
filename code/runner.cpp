#include "runner.h"
#include <chrono>
#include <thread>
#include <latch>
#include "config.h"
#include "data.h"
using namespace std;

static void work(Data &d, const vector<Op> &ops, latch &start, size_t &sink)
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
