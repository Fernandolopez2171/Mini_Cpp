#include "ExprParser.hpp"
#include <iostream>
#include <unordered_map>
#include <string>

Parser::Parser(Lexer &lexer) : lexer(lexer), currentToken(lexer.nextToken()) {}

bool Parser::parse()
{
    while (currentToken != Token::Eof)
    {
        statement();
    }
    return true;
}

void Parser::statement() {
    
}

void Parser::advance() {
    currentToken = lexer.nextToken();
}
