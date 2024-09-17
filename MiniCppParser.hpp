#pragma once
#include "MiniCppLexer.hpp"
#include "Ast.hpp"
#include <stdexcept>

class Parser
{
public:
    Parser(Lexer &lexer);
    AstNode *parse();

private:
    Lexer &lexer;
    Token currentToken;
    void advance();
    std::string tokenToString(Token token);
    void match(Token validToken);

    std::string functionName; // nombre de la función actual
    std::string prevFunctionName;
    bool includeReturn = false; // actica el jr $ra
    AstNode *parsePrg();
    AstNode *parseFunc();
    AstNode *parseParamList(); // param_list
    AstNode *parseParam();     // param
    AstNode *parseVarDecl();   // var_decl
    AstNode *parseType();      // type
    AstNode *parseStmt();      // stmt

    AstNode *parserCoutArg(); // cout_arg
    AstNode *parseExpr();     // expr
    AstNode *parseBoolTerm(); // bool_term
    AstNode *parseRelExpr();  // rel_expr

    AstNode *parseArithExpr();   // arith_expr
    AstNode *parseArithTerm();   // arith_term
    AstNode *parseArithFactor(); // arith_factor
    AstNode *parseExprList();    /// expr_list
};
