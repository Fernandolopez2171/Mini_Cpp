#pragma once
#include "ExprLexer.hpp"
#include <stdexcept>
#include <unordered_map>
#include <string>

class Parser
{
public:
    Parser(Lexer &lexer);
    bool parse();

private:
    Lexer &lexer;
    Token currentToken;
    
    std::unordered_map<std::string, double> variables;

    void advance();
    void statement();

};
