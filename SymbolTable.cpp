#include "SymbolTable.hpp"
#include <stdexcept>

std::unordered_map<std::string, std::unordered_map<std::string, int>> SymbolTable::symbolTable;
std::unordered_map<std::string, std::unordered_map<std::string, double>> SymbolTable::variableValues;
int SymbolTable::currentOffset = 0;

void SymbolTable::setVarOffset(const std::string &functionName, const std::string &varName)
{
    if (symbolTable[functionName].find(varName) == symbolTable[functionName].end())
    {
        symbolTable[functionName][varName] = currentOffset;
        currentOffset += 4;
    }
}

int SymbolTable::getVarOffset(const std::string &functionName, const std::string &varName)
{
    auto funcIt = symbolTable.find(functionName);
    if (funcIt != symbolTable.end())
    {
        auto varIt = funcIt->second.find(varName);
        if (varIt != funcIt->second.end())
        {
            return varIt->second;
        }
        else
        {
            throw std::runtime_error("Variable not found: " + varName);
        }
    }
    else
    {
        throw std::runtime_error("Function not found: " + functionName);
    }
}

double SymbolTable::getVarValue(const std::string &functionName, const std::string &varName)
{
    auto funcIt = variableValues.find(functionName);
    if (funcIt != variableValues.end())
    {
        auto varIt = funcIt->second.find(varName);
        if (varIt != funcIt->second.end())
        {
            return varIt->second;
        }
        else
        {
            throw std::runtime_error("Variable value not found: " + varName);
        }
    }
    else
    {
        throw std::runtime_error("Function not found: " + functionName);
    }
}

void SymbolTable::setVarValue(const std::string &functionName, const std::string &varName, double value)
{
    variableValues[functionName][varName] = value;
}
