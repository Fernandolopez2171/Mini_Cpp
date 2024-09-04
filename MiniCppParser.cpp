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
        std::string expectedToken = tokenToString(validToken);
        std::string foundToken = tokenToString(currentToken);
        throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                 " columna " + std::to_string(lexer.getColumn()) +
                                 ". Se esperaba: " + expectedToken + 
                                 ", pero se encontró: " + foundToken + ".");
    }
}

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
        std::string expectedToken = tokenToString(Token::KW_INT);
        std::string foundToken = tokenToString(currentToken);
        throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                 " columna " + std::to_string(lexer.getColumn()) +
                                 ". Se esperaba: " + expectedToken + 
                                 ", pero se encontró: " + foundToken + ".");
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
    if (currentToken == Token::AMPERSANS)
    {
        match(Token::AMPERSANS);
    }
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

        if (currentToken == Token::OP_ASSIGN)
        {
            match(Token::OP_ASSIGN);
            parseExpr();
        }
        else if (currentToken == Token::OPEN_PAR)
        {
            match(Token::OPEN_PAR);
            if (currentToken != Token::CLOSE_PAR)
            {
                parseExprList(); 
            }
            match(Token::CLOSE_PAR);
        }
        else
        {
            throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                     " columna " + std::to_string(lexer.getColumn()) +
                                     ". Token inesperado.");
        }
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
    else if (currentToken == Token::KW_CIN)
    {
        match(Token::KW_CIN);
        match(Token::GT_GT);
        match(Token::IDENT);

        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            parseExpr();
            match(Token::CLOSE_BRACKET);
        }

        match(Token::SEMICOLON);
    }
    else
    {
        throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                 " columna " + std::to_string(lexer.getColumn()) +
                                 ". Token inesperado.");
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
    parseArithExpr();
    while (currentToken == Token::GT || currentToken == Token::LT ||
           currentToken == Token::GTE || currentToken == Token::LTE ||
           currentToken == Token::NE || currentToken == Token::EQ)
    {
        advance();
        parseArithExpr();
    }
}

void Parser::parseArithExpr()
{
    parseArithTerm();
    while (currentToken == Token::OP_ADD || currentToken == Token::OP_SUB)
    {
        advance();
        parseArithTerm();
    }
}

void Parser::parseArithTerm()
{
    parseArithFactor();
    while (currentToken == Token::OP_MULT || currentToken == Token::OP_DIV || currentToken == Token::OP_MOD)
    {
        advance();
        parseArithFactor();
    }
}

void Parser::parseArithFactor()
{
    if (currentToken == Token::NUMBER)
    {
        match(Token::NUMBER);
    }
    else if (currentToken == Token::IDENT)
    {
        match(Token::IDENT);
        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            parseExpr();
            match(Token::CLOSE_BRACKET);
        }
        else if (currentToken == Token::OPEN_PAR)
        {
            match(Token::OPEN_PAR);
            parseExprList();
            match(Token::CLOSE_PAR);
        }
    }
    else if (currentToken == Token::OPEN_PAR)
    {
        match(Token::OPEN_PAR);
        parseExpr();
        match(Token::CLOSE_PAR);
    }
    else
    {
        throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                 " columna " + std::to_string(lexer.getColumn()) +
                                 ". Token inesperado.");
    }
}

void Parser::parseExprList()
{
    if (currentToken != Token::CLOSE_PAR)
    {
        parseExpr();
        while (currentToken == Token::COMMA)
        {
            match(Token::COMMA);
            parseExpr();
        }
    }
}

std::string Parser::tokenToString(Token token)
{
    switch (token)
    {
    case Token::KW_INT:
        return "int";
    case Token::NUMBER:
        return "number";
    case Token::OP_ASSIGN:
        return "=";
    case Token::KW_IF:
        return "if";
    case Token::KW_ELSE:
        return "else";
    case Token::KW_WHILE:
        return "while";
    case Token::KW_COUT:
        return "std::cout";
    case Token::KW_CIN:
        return "std::cin";
    case Token::LT_LT:
        return "<<";
    case Token::GT_GT:
        return ">>";
    case Token::KW_ENDL:
        return "std::endl";
    case Token::STRING_LITERAL:
        return "string literal";
    case Token::BOOL_OR:
        return "||";
    case Token::BOOL_AND:
        return "&&";
    case Token::GT:
        return ">";
    case Token::LT:
        return "<";
    case Token::GTE:
        return ">=";
    case Token::LTE:
        return "<=";
    case Token::NE:
        return "!=";
    case Token::EQ:
        return "==";
    case Token::OP_ADD:
        return "+";
    case Token::OP_SUB:
        return "-";
    case Token::OP_MULT:
        return "*";
    case Token::OP_DIV:
        return "/";
    case Token::OP_MOD:
        return "%";
    case Token::OPEN_PAR:
        return "(";
    case Token::CLOSE_PAR:
        return ")";
    case Token::OPEN_CURLY:
        return "{";
    case Token::CLOSE_CURLY:
        return "}";
    case Token::OPEN_BRACKET:
        return "[";
    case Token::CLOSE_BRACKET:
        return "]";
    case Token::COMMA:
        return ",";
    case Token::SEMICOLON:
        return ";";
    case Token::AMPERSANS:
        return "&";
    case Token::IDENT:
        return "identifier";
    case Token::Eof:
        return "EOF";
    case Token::Error:
        return "error";
    default:
        return "unknown token";
    }
}
