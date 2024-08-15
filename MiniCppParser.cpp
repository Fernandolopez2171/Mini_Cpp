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
    if (currentToken == validToken){
        advance();
    }
    else{
        throw std::runtime_error("Syntax error");
    }
      
}

//input
void Parser::parse()
{
    parsePrg();
}

void Parser::parsePrg()
{
    while(currentToken != Token::Eof){
        //std::cout<<currentToken<<std::endl;
        //advance();
        parseFunc();
    }
}

void Parser::parseFunc()
{
    parseType(); 
    match(Token::IDENT); 
    match(Token::OPEN_PAR); 
    //parseParamList(); 
    match(Token::CLOSE_PAR); 
 
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
