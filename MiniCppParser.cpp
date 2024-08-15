#include "MiniCppParser.hpp"
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
        parseFunc();
    }
}

void Parser::parseFunc()
{
    parseType(); //type

}

void Parser::parseType()
{
    if(currentToken == Token::KW_INT){
        //entero solo
        match(Token::KW_INT);
        //Arreglos
        if(currentToken == Token::OPEN_BRACKET){
            match(Token::OPEN_BRACKET);
            match(Token::NUMBER);
            match(Token::CLOSE_BRACKET);
        }
    }
    else{
        throw std::runtime_error("Syntax error: expected type");
    }
}
