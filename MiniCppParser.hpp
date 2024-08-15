#pragma once
#include "MiniCppLexer.hpp"
#include <stdexcept>
#include <unordered_map>
#include <string>

class Parser
{
public:
    //constructor
    Parser(Lexer &lexer);
    //funcion principal
    void parse();  //seria el parserInput
    
private:
    std::unordered_map<std::string, double> variables;//eliminar
    //atributos
    Lexer &lexer;
    Token currentToken;
    //funciones auxiliares
    void advance();
    void match(Token validToken);

    void parsePrg(); //prg 
    void parseFunc();//func
    
    void parseParamList();//param_list
    void parseParam();//param 
    void parseVarDecl();//var_decl 
    void parseType();//type
    void parseStmt();//stmt


    void parserCoutArg();//cout_arg
    void parseExpr();//expr
    void parseBoolTerm();//bool_term
    void parseRelExpr();//rel_expr

    void parseArithExpr();//arith_expr
    void parseArithTerm();//arith_term
    void parseArithFactor();//arith_factor
    void parseExprList();///expr_list
};
