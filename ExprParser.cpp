#include "ExprParser.hpp"
#include <iostream>
Parser::Parser(Lexer &lexer) : lexer(lexer) {
    advance();
}

void Parser::advance()
{
    currentToken = lexer.nextToken();
}

void Parser::match(Token validToken)
{
    if (currentToken == validToken)
        advance();
    else
        throw std::runtime_error("Syntax error");
}

//input
void Parser::parse()
{
    parsePrg();
}

void Parser::parsePrg()
{
    while(currentToken != Token::Eof){
        //parseFunc();
        std::cout<<currentToken<<std::endl;
        advance();
    }
}
