#include "MiniCppParser.hpp"

Parser::Parser(Lexer &lexer) : lexer(lexer), currentToken(Token::Eof)
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
    case Token::AMPERSAND:
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

AstNode *Parser::parse()
{
    return parsePrg();
}

AstNode *Parser::parsePrg()
{

    ProgramNode *programNode = new ProgramNode();

    while (currentToken != Token::Eof)
    {
        programNode->addFunction(parseFunc());
    }

    return programNode;
}

AstNode *Parser::parseFunc()
{

    AstNode *returnType = parseType();

    std::string funcName = lexer.tokenText();

    functionName = funcName;

    match(Token::IDENT);

    match(Token::OPEN_PAR);
    AstNode *paramList = parseParamList();
    match(Token::CLOSE_PAR);

    FunctionDeclExpr *funcDecl = new FunctionDeclExpr(funcName, returnType, paramList, includeReturn);

    match(Token::OPEN_CURLY);

    while (currentToken == Token::KW_INT)
    {
        funcDecl->addVarDecl(parseVarDecl());
    }

    while (currentToken != Token::CLOSE_CURLY && currentToken != Token::Eof)
    {
        funcDecl->addStatement(parseStmt());
    }

    match(Token::CLOSE_CURLY);

    Token next = lexer.peekNextToken();

    if (next == Token::Error)
    {
        funcDecl->setIncludeReturn(false);
    }
    else
    {
        funcDecl->setIncludeReturn(true);
    }

    return funcDecl;
}

AstNode *Parser::parseType()
{
    if (currentToken == Token::KW_INT)
    {
        match(Token::KW_INT);
        return new TypeExpr("int");
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

AstNode *Parser::parseParamList()
{

    if (currentToken == Token::CLOSE_PAR)
    {
        return nullptr;
    }

    ExprList *paramList = new ExprList();

    paramList->addExpression(parseParam());

    while (currentToken == Token::COMMA)
    {
        match(Token::COMMA);
        paramList->addExpression(parseParam());
    }

    return paramList;
}

AstNode *Parser::parseParam()
{

    parseType();

    bool isReference = false;
    if (currentToken == Token::AMPERSAND)
    {
        match(Token::AMPERSAND);
        isReference = true;
    }

    std::string paramName = lexer.tokenText();
    match(Token::IDENT);

    return new ParamExpr(functionName, paramName, isReference);
}

AstNode *Parser::parseVarDecl()
{
    parseType(); 

    
    ExprList *varDeclList = new ExprList();

    std::string varName = lexer.tokenText();
    match(Token::IDENT); 

    
    if (currentToken == Token::OPEN_BRACKET)
    {
        match(Token::OPEN_BRACKET);
        std::string arraySize = lexer.tokenText(); 
        match(Token::NUMBER);
        match(Token::CLOSE_BRACKET);

        
        // varDeclList->addExpression(new VarDeclExpr(functionName, varName, arraySize));
    }
    else
    {
        
        varDeclList->addExpression(new VarDeclExpr(functionName, varName));
    }

    
    while (currentToken == Token::COMMA)
    {
        match(Token::COMMA);
        varName = lexer.tokenText(); 
        match(Token::IDENT);

        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            std::string arraySize = lexer.tokenText();
            match(Token::NUMBER);
            match(Token::CLOSE_BRACKET);

            
            // varDeclList->addExpression(new VarDeclExpr(functionName, varName, arraySize));
        }
        else
        {
            
            varDeclList->addExpression(new VarDeclExpr(functionName, varName));
        }
    }

    match(Token::SEMICOLON); 

    return varDeclList; 
}

AstNode *Parser::parseStmt()
{
    if (currentToken == Token::IDENT)
    {
        std::string varName = lexer.tokenText();
        match(Token::IDENT);

        
        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            AstNode *indexExpr = parseExpr();
            match(Token::CLOSE_BRACKET);

            
            if (currentToken == Token::OP_ASSIGN)
            {
                match(Token::OP_ASSIGN);
                AstNode *expr = parseExpr();
                match(Token::SEMICOLON);
                return nullptr;
                // return new ArrayAssignExpr(functionName, varName, indexExpr, expr);
            }
            else
            {
                throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                         " columna " + std::to_string(lexer.getColumn()) +
                                         ". Se esperaba una asignación.");
            }
        }

        
        else if (currentToken == Token::OP_ASSIGN)
        {
            match(Token::OP_ASSIGN);
            AstNode *expr = parseExpr();
            match(Token::SEMICOLON);
            return new AssignExpr(functionName, varName, expr);
        }
        
        else if (currentToken == Token::OPEN_PAR)
        {
            match(Token::OPEN_PAR);
            prevFunctionName = functionName;
            functionName = varName;
            std::vector<AstNode *> args;

            if (currentToken != Token::CLOSE_PAR)
            {
                args.push_back(parseExpr());
                while (currentToken == Token::COMMA)
                {
                    match(Token::COMMA);
                    args.push_back(parseExpr());
                }
            }

            match(Token::CLOSE_PAR);
            match(Token::SEMICOLON);
            functionName = prevFunctionName;

            return new FunctionCallExpr(varName, args);
        }
        else
        {
            throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                     " columna " + std::to_string(lexer.getColumn()) +
                                     ". Se esperaba una asignación o llamada a función.");
        }
    }
    
    else if (currentToken == Token::KW_COUT)
    {
        match(Token::KW_COUT);
        match(Token::LT_LT);
        AstNode *firstArg = parserCoutArg();

        ExprList *exprList = new ExprList();
        exprList->addExpression(firstArg);

        while (currentToken == Token::LT_LT)
        {
            match(Token::LT_LT);
            exprList->addExpression(parserCoutArg());
        }

        match(Token::SEMICOLON);
        return exprList;
    }
    else if (currentToken == Token::KW_IF)
    {
        match(Token::KW_IF);
        match(Token::OPEN_PAR);
        AstNode *condition = parseExpr();
        match(Token::CLOSE_PAR);
        match(Token::OPEN_CURLY);

        ExprList *ifBodyList = new ExprList();
        while (currentToken != Token::CLOSE_CURLY)
        {
            ifBodyList->addExpression(parseStmt());
        }
        match(Token::CLOSE_CURLY);

        ExprList *elseBodyList = nullptr;
        if (currentToken == Token::KW_ELSE)
        {
            match(Token::KW_ELSE);
            match(Token::OPEN_CURLY);
            elseBodyList = new ExprList();
            while (currentToken != Token::CLOSE_CURLY)
            {
                elseBodyList->addExpression(parseStmt());
            }
            match(Token::CLOSE_CURLY);
        }

        return new IfStmt(condition, ifBodyList, elseBodyList);
    }

    else if (currentToken == Token::KW_WHILE)
    {
        match(Token::KW_WHILE);
        match(Token::OPEN_PAR);
        AstNode *condition = parseExpr();
        match(Token::CLOSE_PAR);
        match(Token::OPEN_CURLY);

        ExprList *whileBodyList = new ExprList(); 
        while (currentToken != Token::CLOSE_CURLY)
        {
            whileBodyList->addExpression(parseStmt());
        }
        match(Token::CLOSE_CURLY);

        return new WhileStmt(condition, whileBodyList); 
    }

    
    else if (currentToken == Token::KW_CIN)
    {
        match(Token::KW_CIN);
        match(Token::GT_GT);
        std::string varName = lexer.tokenText();
        match(Token::IDENT);

        if (currentToken == Token::OPEN_BRACKET)
        {
            match(Token::OPEN_BRACKET);
            AstNode *indexExpr = parseExpr();
            match(Token::CLOSE_BRACKET);

            match(Token::SEMICOLON);
            return new ArrayCinExpr(varName, indexExpr);
        }
        else
        {
            match(Token::SEMICOLON);
            return new CinExpr(varName, functionName);
        }
    }
    else
    {
        throw std::runtime_error("Error en línea " + std::to_string(lexer.getLine()) +
                                 " columna " + std::to_string(lexer.getColumn()) +
                                 ". Token inesperado.");
    }
}

AstNode *Parser::parserCoutArg()
{
    if (currentToken == Token::STRING_LITERAL)
    {
        std::string stringLiteral = lexer.tokenText();
        match(Token::STRING_LITERAL);
        return new StringLiteralExpr(stringLiteral);
    }
    else if (currentToken == Token::KW_ENDL)
    {
        match(Token::KW_ENDL);
        return new EndlExpr();
    }
    else
    {
        return new PrintStmt(parseExpr());
    }
}

AstNode *Parser::parseExpr()
{

    AstNode *left = parseBoolTerm();

    while (currentToken == Token::BOOL_OR)
    {
        match(Token::BOOL_OR);
        AstNode *right = parseBoolTerm();

        left = new OrExpr(left, right);
    }

    return left;
}

AstNode *Parser::parseBoolTerm()
{

    AstNode *left = parseRelExpr();

    while (currentToken == Token::BOOL_AND)
    {
        match(Token::BOOL_AND);
        AstNode *right = parseRelExpr();

        left = new AndExpr(left, right);
    }

    return left;
}

AstNode *Parser::parseRelExpr()
{
    AstNode *left = parseArithExpr();

    while (currentToken == Token::GT || currentToken == Token::LT ||
           currentToken == Token::GTE || currentToken == Token::LTE ||
           currentToken == Token::NE || currentToken == Token::EQ)
    {
        Token op = currentToken;
        advance();
        AstNode *right = parseArithExpr();

        if (op == Token::GT)
        {
            left = new GreaterExpr(left, right);
        }
        else if (op == Token::LT)
        {
            left = new LessExpr(left, right);
        }
        else if (op == Token::GTE)
        {
            left = new GreaterEqualExpr(left, right);
        }
        else if (op == Token::LTE)
        {
            left = new LessEqualExpr(left, right);
        }
        else if (op == Token::NE)
        {
            left = new NotEqualExpr(left, right);
        }
        else if (op == Token::EQ)
        {
            left = new EqualExpr(left, right);
        }
    }

    return left;
}

AstNode *Parser::parseArithExpr()
{
    AstNode *left = parseArithTerm();
    while (currentToken == Token::OP_ADD || currentToken == Token::OP_SUB)
    {
        Token op = currentToken;
        advance();
        AstNode *right = parseArithTerm();

        if (op == Token::OP_ADD)
        {
            left = new AddExpr(left, right);
        }
        else if (op == Token::OP_SUB)
        {
            left = new SubExpr(left, right);
        }
    }
    return left;
}

AstNode *Parser::parseArithTerm()
{
    AstNode *left = parseArithFactor();
    while (currentToken == Token::OP_MULT || currentToken == Token::OP_DIV || currentToken == Token::OP_MOD)
    {
        Token op = currentToken;
        advance();
        AstNode *right = parseArithFactor();

        if (op == Token::OP_MULT)
        {
            left = new MulExpr(left, right);
        }
        else if (op == Token::OP_DIV)
        {
            left = new DivExpr(left, right);
        }
        else if (op == Token::OP_MOD)
        {
            left = new ModExpr(left, right);
        }
    }
    return left;
}

AstNode *Parser::parseArithFactor()
{
    if (currentToken == Token::NUMBER)
    {
        double value = std::stod(lexer.tokenText());
        match(Token::NUMBER);
        return new ValueExpr(value);
    }
    else if (currentToken == Token::IDENT)
    {
        std::string varName = lexer.tokenText();
        match(Token::IDENT);

        if (currentToken == Token::OPEN_PAR)
        {
            match(Token::OPEN_PAR);
            std::vector<AstNode *> exprList;

            if (currentToken != Token::CLOSE_PAR)
            {
                exprList.push_back(parseExpr());
                while (currentToken == Token::COMMA)
                {
                    match(Token::COMMA);
                    exprList.push_back(parseExpr());
                }
            }

            match(Token::CLOSE_PAR);
            return new FunctionCallExpr(varName, exprList);
        }
        else
        {
            return new VariableExpr(functionName, varName);
        }
    }
    else if (currentToken == Token::OPEN_PAR)
    {
        match(Token::OPEN_PAR);
        AstNode *expr = parseExpr();
        match(Token::CLOSE_PAR);
        return expr;
    }
    else
    {
        throw std::runtime_error("Error: Token inesperado en parseArithFactor.");
    }
}

AstNode *Parser::parseExprList()
{
    ExprList *exprList = new ExprList();

    if (currentToken != Token::CLOSE_PAR)
    {

        exprList->addExpression(parseExpr());

        while (currentToken == Token::COMMA)
        {
            match(Token::COMMA);
            exprList->addExpression(parseExpr());
        }
    }

    return exprList;
}
