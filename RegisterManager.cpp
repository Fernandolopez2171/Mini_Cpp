#include "RegisterManager.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>

static std::vector<std::string> tempRegisters = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
static std::vector<bool> tempRegisterUsed(10, false);

std::string allocateTempRegister()
{
    for (size_t i = 0; i < tempRegisters.size(); ++i)
    {
        if (!tempRegisterUsed[i])
        {
            tempRegisterUsed[i] = true;
            return tempRegisters[i];
        }
    }
    throw std::runtime_error("No available temporary registers");
}

void freeTempRegister(const std::string &reg)
{
    auto it = std::find(tempRegisters.begin(), tempRegisters.end(), reg);
    if (it != tempRegisters.end())
    {
        tempRegisterUsed[it - tempRegisters.begin()] = false;
    }
    else
    {
        throw std::runtime_error("Trying to free a register that is not temporary");
    }
}
