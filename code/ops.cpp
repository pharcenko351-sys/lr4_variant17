#include "ops.h"
#include <fstream>
#include <stdexcept>
using namespace std;

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
