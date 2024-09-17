#pragma once
#include <string>
#include <unordered_map>

class SymbolTable
{
public:
    static void setVarOffset(const std::string &functionName, const std::string &varName);

    static int getVarOffset(const std::string &functionName, const std::string &varName);

    static double getVarValue(const std::string &functionName, const std::string &varName);

    static void setVarValue(const std::string &functionName, const std::string &varName, double value);

    static int getCurrentOffset() { return currentOffset; }

private:
    static std::unordered_map<std::string, std::unordered_map<std::string, int>> symbolTable;
    static std::unordered_map<std::string, std::unordered_map<std::string, double>> variableValues;
    static int currentOffset;
};
