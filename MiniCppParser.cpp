#include "MiniCppParser.hpp"
#include <iostream>
Parser::Parser(Lexer &lexer) : lexer(lexer)
{
    advance();
}

void Parser::advance()
{
    currentToken = lexer.nextToken();
}

void Parser::match(Token validToken)
{
    if (currentToken == validToken)
    {
        advance();
    }
    else
    {
        std::cout << currentToken << std::endl;
        throw std::runtime_error("Syntax error");
    }
}

// input
void Parser::parse()
{
    parsePrg();
}

void Parser::parsePrg()
{
    while (currentToken != Token::Eof)
    {

        parseFunc();
    }
}

void Parser::parseFunc()
{
    parseType();
    match(Token::IDENT);

    match(Token::OPEN_PAR);
    parseParamList();
    match(Token::CLOSE_PAR);

    match(Token::OPEN_CURLY);

    while (currentToken == Token::KW_INT)
    {
        parseVarDecl();
    }

    while (currentToken != Token::CLOSE_CURLY && currentToken != Token::Eof)
    {
        parseStmt();
    }

    match(Token::CLOSE_CURLY);
}

void Parser::parseType()
{
    if (currentToken == Token::KW_INT)
    {
        match(Token::KW_INT);
    }
    else
    {
        throw std::runtime_error("Syntax error: expected type");
    }
}

void Parser::parseParamList()
{
    if (currentToken == Token::CLOSE_PAR)
    {
        return;
    }
    parseParam();
    while (currentToken == Token::COMMA)
    {
        match(Token::COMMA);
        parseParam();
    }
}

void Parser::parseParam()
{
    parseType();
    match(Token::IDENT);

    if (currentToken == Token::OPEN_BRACKET)
    {
        match(Token::OPEN_BRACKET);
        match(Token::NUMBER);
        match(Token::CLOSE_BRACKET);
    }
}

void Parser::parseVarDecl()
{
    parseType();

    match(Token::IDENT);

    if (currentToken == Token::OPEN_BRACKET)
    {
        match(Token::OPEN_BRACKET);
        match(Token::NUMBER);
        match(Token::CLOSE_BRACKET);
    }

    while (currentToken == Token::COMMA)
    {
        match(Token::COMMA);
        match(Token::IDENT);

        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            match(Token::NUMBER);
            match(Token::CLOSE_BRACKET);
        }
    }

    match(Token::SEMICOLON);
}

void Parser::parseStmt()
{
    
    if (currentToken == Token::IDENT)
    {
        match(Token::IDENT);

        if (currentToken == Token::OPEN_BRACKET)
        {

            match(Token::OPEN_BRACKET);
            parseExpr();
            match(Token::CLOSE_BRACKET);
        }

        match(Token::OP_ASSIGN);
        parseExpr();

        match(Token::SEMICOLON);
    }
    else if (currentToken == Token::KW_IF)
    {
        match(Token::KW_IF);
        match(Token::OPEN_PAR);
        parseExpr();
        match(Token::CLOSE_PAR);
        match(Token::OPEN_CURLY);

        while (currentToken != Token::CLOSE_CURLY)
        {
            parseStmt();
        }

        match(Token::CLOSE_CURLY);

        if (currentToken == Token::KW_ELSE)
        {
            match(Token::KW_ELSE);
            match(Token::OPEN_CURLY);

            while (currentToken != Token::CLOSE_CURLY)
            {
                parseStmt();
            }

            match(Token::CLOSE_CURLY);
        }
    }
    else if (currentToken == Token::KW_WHILE)
    {
        match(Token::KW_WHILE);
        match(Token::OPEN_PAR);
        parseExpr();
        match(Token::CLOSE_PAR);
        match(Token::OPEN_CURLY);

        while (currentToken != Token::CLOSE_CURLY)
        {
            parseStmt();
        }

        match(Token::CLOSE_CURLY);
    }
    else if (currentToken == Token::KW_COUT)
    {
        match(Token::KW_COUT);
        match(Token::LT_LT);
        parserCoutArg();

        while (currentToken == Token::LT_LT)
        {
            match(Token::LT_LT);
            parserCoutArg();
        }

        match(Token::SEMICOLON);
    }
    else if (currentToken == Token::GT_GT)
    {

        match(Token::GT_GT);
        match(Token::IDENT);

        if (currentToken == Token::OPEN_CURLY)
        {
            match(Token::OPEN_CURLY);
            parseExpr();
            match(Token::CLOSE_CURLY);
        }

        match(Token::SEMICOLON);
    }
    else
    {
        throw std::runtime_error("Syntax error: unexpected token in statement");
    }
}

void Parser::parserCoutArg()
{
    if (currentToken == Token::STRING_LITERAL)
    {
        match(Token::STRING_LITERAL);
    }
    else if (currentToken == Token::KW_ENDL)
    {
        match(Token::KW_ENDL);
    }
    else
    {
        parseExpr();
    }
}

void Parser::parseExpr()
{
    parseBoolTerm();
    while (currentToken == Token::BOOL_OR)
    {
        match(Token::BOOL_OR);
        parseBoolTerm();
    }
}

void Parser::parseBoolTerm()
{
    parseRelExpr();
    while (currentToken == Token::BOOL_AND)
    {
        match(Token::BOOL_AND);
        parseRelExpr();
    }
}

void Parser::parseRelExpr()
{
    //parseArithExpr();
    while (currentToken == Token::GT || currentToken == Token::LT ||
           currentToken == Token::GTE || currentToken == Token::LTE ||
           currentToken == Token::NE || currentToken == Token::EQ)
    {
        advance();  
        //parseArithExpr();
    }
}

