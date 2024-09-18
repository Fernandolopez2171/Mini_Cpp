#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "RegisterManager.hpp"
#include "SymbolTable.hpp"

struct CodePlace
{
    std::string code;
    std::string place;
};

class AstNode
{
public:
    virtual ~AstNode() = default;
    virtual CodePlace generateCode() = 0;
};

class BinaryExpr : public AstNode
{
protected:
    AstNode *left;
    AstNode *right;

public:
    BinaryExpr(AstNode *left, AstNode *right) : left(left), right(right) {}
    virtual ~BinaryExpr()
    {
        delete left;
        delete right;
    }
};

class AddExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();
        result.code = leftCode.code + rightCode.code +
                      "add " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";
        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class SubExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();
        result.code = leftCode.code + rightCode.code +
                      "sub " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";
        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class MulExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();
        result.code = leftCode.code + rightCode.code +
                      "mult " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "mflo " + result.place + "\n";
        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class DivExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();
        result.code = leftCode.code + rightCode.code +
                      "div " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "mflo " + result.place + "\n";
        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class ValueExpr : public AstNode
{
    double value;

public:
    ValueExpr(double value) : value(value) {}

    CodePlace generateCode() override
    {
        CodePlace result;
        result.place = allocateTempRegister();
        result.code = "li " + result.place + ", " + std::to_string(static_cast<int>(value)) + "\n";
        return result;
    }
};

class VariableExpr : public AstNode
{
    std::string functionName; 
    std::string varName;      

public:
    
    VariableExpr(const std::string &functionName, const std::string &varName)
        : functionName(functionName), varName(varName) {}

    CodePlace generateCode() override
    {
        CodePlace result;
        result.place = allocateTempRegister();

        
        int offset = SymbolTable::getVarOffset(functionName, varName);

        
        result.code = "lw " + result.place + ", " + std::to_string(offset) + "($sp)\n";

        return result;
    }
};


class AssignExpr : public AstNode
{
    std::string functionName; 
    std::string varName;      
    AstNode *expr;            

public:
    
    AssignExpr(const std::string &functionName, const std::string &varName, AstNode *expr)
        : functionName(functionName), varName(varName), expr(expr)
    {
        
        SymbolTable::setVarOffset(functionName, varName);
    }

    ~AssignExpr() { delete expr; }

    CodePlace generateCode() override
    {
        CodePlace result = expr->generateCode();

        
        int offset = SymbolTable::getVarOffset(functionName, varName);

        
        result.code += "sw " + result.place + ", " + std::to_string(offset) + "($sp)\n";

        return result;
    }
};


class PrintStmt : public AstNode
{
    AstNode *expr;

public:
    PrintStmt(AstNode *expr) : expr(expr) {}
    ~PrintStmt() { delete expr; }

    CodePlace generateCode() override
    {
        
        CodePlace result = expr->generateCode();
 
        result.code += "move $a0, " + result.place + "\n";
        result.code += "li $v0, 1\n";
        result.code += "syscall\n";

        return result;
    }
};

class ProgramNode : public AstNode
{
    std::vector<AstNode *> statements;

public:
    ~ProgramNode()
    {
        for (AstNode *stmt : statements)
        {
            delete stmt;
        }
    }

    void addFunction(AstNode *func)
    {
        statements.push_back(func);  
    }
    

    CodePlace generateCode() override
    {
        CodePlace result;
        
        result.code = ".text\n";
        result.code += "jal main\n";
        for (auto &stmt : statements)
        {
            CodePlace stmtCode = stmt->generateCode();
            result.code += stmtCode.code;
        }

        return result;
    }
};


class ExprList : public AstNode
{
    std::vector<AstNode *> expressions;

public:
    ~ExprList()
    {
        for (AstNode *expr : expressions)
        {
            delete expr;
        }
    }

    void addExpression(AstNode *expr)
    {
        expressions.push_back(expr);
    }

    CodePlace generateCode() override
    {
        CodePlace result;
        for (auto &expr : expressions)
        {
            CodePlace exprCode = expr->generateCode();
            result.code += exprCode.code;
        }
        return result;
    }
};

class FunctionCallExpr : public AstNode
{
    std::string functionName;
    std::vector<AstNode *> arguments;

public:
    FunctionCallExpr(const std::string &functionName, std::vector<AstNode *> args)
        : functionName(functionName), arguments(std::move(args)) {}

    ~FunctionCallExpr()
    {
        for (AstNode *arg : arguments)
        {
            delete arg;
        }
    }

    CodePlace generateCode() override
    {
        CodePlace result;
        std::vector<std::string> argRegisters;

        
        for (auto &arg : arguments)
        {
            CodePlace argCode = arg->generateCode();
            result.code += argCode.code;
            argRegisters.push_back(argCode.place); 
        }

        
        for (size_t i = 0; i < argRegisters.size(); ++i)
        {
            if (i < 4) 
            {
                result.code += "move $a" + std::to_string(i) + ", " + argRegisters[i] + "\n";
            }
            else
            {
                
                result.code += "sw " + argRegisters[i] + ", " + std::to_string(i * 4) + "($sp)\n";
            }
            freeTempRegister(argRegisters[i]);
        }

        
        result.code += "jal " + functionName + "\n";
        
        
        result.place = allocateTempRegister();
        result.code += "move " + result.place + ", $v0\n";

        return result;
    }
};


class ModExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();
        result.code = leftCode.code + rightCode.code +
                      "div " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "mfhi " + result.place + "\n"; 
        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class GreaterExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "slt " + result.place + ", " + rightCode.place + ", " + leftCode.place + "\n";

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class LessExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "slt " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class GreaterEqualExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "slt " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "xori " + result.place + ", " + result.place + ", 1\n";  

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class LessEqualExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "slt " + result.place + ", " + rightCode.place + ", " + leftCode.place + "\n";
        result.code += "xori " + result.place + ", " + result.place + ", 1\n";  

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class NotEqualExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "xor " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "sltu " + result.place + ", $zero, " + result.place + "\n";  

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class EqualExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "xor " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";
        result.code += "sltiu " + result.place + ", " + result.place + ", 1\n";  

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class AndExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "and " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class OrExpr : public BinaryExpr
{
public:
    using BinaryExpr::BinaryExpr;

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace leftCode = left->generateCode();
        CodePlace rightCode = right->generateCode();
        result.place = allocateTempRegister();

        
        result.code = leftCode.code + rightCode.code +
                      "or " + result.place + ", " + leftCode.place + ", " + rightCode.place + "\n";

        freeTempRegister(leftCode.place);
        freeTempRegister(rightCode.place);
        return result;
    }
};

class StringLiteralExpr : public AstNode
{
    std::string value;

public:
    StringLiteralExpr(const std::string& value) : value(value) {}

    CodePlace generateCode() override
    {
        CodePlace result;

        
        if (!value.empty() && value.front() == '"' && value.back() == '"')
        {
            value = value.substr(1, value.size() - 2);  
        }

        
        for (char c : value)
        {
            result.code += "li $a0, " + std::to_string(static_cast<int>(c)) + "\n";  
            result.code += "li $v0, 11\n";  
            result.code += "syscall\n";
        }


        return result;
    }
};

class EndlExpr : public AstNode
{
public:
    CodePlace generateCode() override
    {
        CodePlace result;
        result.code = "li $a0, 10\n";  
        result.code += "li $v0, 11\n";  
        result.code += "syscall\n";
        return result;
    }
};

class VarDeclExpr : public AstNode
{
    std::string functionName; 
    std::string varName;

public:
    VarDeclExpr(const std::string&functionName,const std::string& varName) :functionName(functionName) , varName(varName) {}

    CodePlace generateCode() override
    {
        CodePlace result;
        
        result.place = "";  

        
        SymbolTable::setVarOffset(functionName,varName);

        
        
        result.code = "";  

        return result;
    }
};

class ParamExpr : public AstNode
{
    std::string functionName; 
    std::string paramName;    
    bool isReference;         

public:
    
    ParamExpr(const std::string& functionName, const std::string& paramName, bool isReference)
        : functionName(functionName), paramName(paramName), isReference(isReference) {}

    CodePlace generateCode() override
    {
        CodePlace result;

        
        SymbolTable::setVarOffset(functionName, paramName);

        
        int offset = SymbolTable::getVarOffset(functionName, paramName);

        
        if (isReference)
        {
            
            result.code = "la " + result.place + ", " + std::to_string(offset) + "($sp)\n";
        }
        else
        {
            
            result.code = "lw " + result.place + ", " + std::to_string(offset) + "($sp)\n";
        }

        return result;
    }
};


class TypeExpr : public AstNode
{
    std::string typeName;

public:
    TypeExpr(const std::string& typeName) : typeName(typeName) {}

    CodePlace generateCode() override
    {
        CodePlace result;
        result.code = "";  
        result.place = "";  
        return result;
    }

    std::string getType() const
    {
        return typeName;
    }
};

class FunctionDeclExpr : public AstNode
{
    std::string funcName;
    AstNode* returnType;
    AstNode* paramList;
    std::vector<AstNode*> statements;
    bool includeReturn;

public:
    FunctionDeclExpr(const std::string& name, AstNode* retType, AstNode* params, bool includeReturn = false)
        : funcName(name), returnType(retType), paramList(params), includeReturn(includeReturn) {}

    ~FunctionDeclExpr()
    {
        delete returnType;
        delete paramList;
        for (auto stmt : statements)
        {
            delete stmt;
        }
    }

    void addVarDecl(AstNode* varDecl)
    {
        
    }

    void addStatement(AstNode* stmt)
    {
        statements.push_back(stmt);
    }

    
    void setIncludeReturn(bool include)
    {
        includeReturn = include;
    }

    CodePlace generateCode() override
    {
        CodePlace result;
        result.code = funcName + ":\n";
        int stackSize = SymbolTable::getCurrentOffset(); 
        result.code += "addi $sp, $sp, -" + std::to_string(stackSize) + "\n";

        for (auto& stmt : statements)
        {
            CodePlace stmtCode = stmt->generateCode();
            result.code += stmtCode.code;
        }
        result.code += "addi $sp, $sp, " + std::to_string(stackSize) + "\n";
        if (includeReturn)
        {
            result.code += "jr $ra\n";
        }

        return result;
    }
};

class IfStmt : public AstNode
{
    AstNode* condition;
    AstNode* ifBody;
    AstNode* elseBody; 

public:
    IfStmt(AstNode* condition, AstNode* ifBody, AstNode* elseBody = nullptr)
        : condition(condition), ifBody(ifBody), elseBody(elseBody) {}

    ~IfStmt() {
        delete condition;
        delete ifBody;
        if (elseBody) delete elseBody;
    }

    CodePlace generateCode() override
    {
        CodePlace result;

        
        CodePlace conditionCode = condition->generateCode();

        
        static int labelCount = 0;
        std::string elseLabel = "else_" + std::to_string(labelCount);
        std::string endLabel = "end_if_" + std::to_string(labelCount);
        labelCount++;

        
        
        result.code = conditionCode.code;
        result.code += "beqz " + conditionCode.place + ", " + elseLabel + "\n"; 

        
        CodePlace ifBodyCode = ifBody->generateCode();
        result.code += ifBodyCode.code;

        
        result.code += "j " + endLabel + "\n";

        
        result.code += elseLabel + ":\n";
        if (elseBody)
        {
            CodePlace elseBodyCode = elseBody->generateCode();
            result.code += elseBodyCode.code;
        }

        
        result.code += endLabel + ":\n";

        
        freeTempRegister(conditionCode.place);

        return result;
    }
};

class WhileStmt : public AstNode
{
    AstNode* condition;
    AstNode* whileBody;

public:
    WhileStmt(AstNode* condition, AstNode* whileBody)
        : condition(condition), whileBody(whileBody) {}

    ~WhileStmt() {
        delete condition;
        delete whileBody;
    }

    CodePlace generateCode() override
    {
        CodePlace result;

        
        static int labelCount = 0;
        std::string startLabel = "while_start_" + std::to_string(labelCount);
        std::string endLabel = "while_end_" + std::to_string(labelCount);
        labelCount++;

        
        result.code += startLabel + ":\n";

        
        CodePlace conditionCode = condition->generateCode();

        
        result.code += conditionCode.code;
        result.code += "beqz " + conditionCode.place + ", " + endLabel + "\n"; 

        
        CodePlace whileBodyCode = whileBody->generateCode();
        result.code += whileBodyCode.code;

        
        result.code += "j " + startLabel + "\n";

        
        result.code += endLabel + ":\n";

        
        freeTempRegister(conditionCode.place);

        return result;
    }
};

class ArrayCinExpr : public AstNode
{
    std::string varName;
    AstNode* indexExpr;

public:
    ArrayCinExpr(const std::string& varName, AstNode* indexExpr)
        : varName(varName), indexExpr(indexExpr) {}

    ~ArrayCinExpr() {
        delete indexExpr;
    }

    CodePlace generateCode() override
    {
        CodePlace result;

        
        CodePlace indexCode = indexExpr->generateCode();

        
        std::string tempRegister = allocateTempRegister();

        
        result.code += indexCode.code;
        result.code += "mul " + indexCode.place + ", " + indexCode.place + ", 4\n";  

        
        result.code += "la " + tempRegister + ", " + varName + "\n";  

        
        result.code += "add " + tempRegister + ", " + tempRegister + ", " + indexCode.place + "\n";

        
        result.code += "lw " + tempRegister + ", 0(" + tempRegister + ")\n";

        
        result.place = tempRegister;

        
        freeTempRegister(indexCode.place);

        return result;
    }
};

class CinExpr : public AstNode
{
    std::string varName;
    std::string functionName;
public:
    CinExpr(const std::string& varName, const std::string& functionName) : varName(varName),functionName(functionName) {}

    CodePlace generateCode() override
    {
        CodePlace result;

        result.code += "li $v0, 5\n";              
        result.code += "syscall\n";                

        
        int offset = SymbolTable::getVarOffset(functionName, varName); 
        result.code += "sw $v0, " + std::to_string(offset) + "($sp)\n"; 

        return result;
    }
};

class ArrayAssignExpr : public AstNode
{
    std::string functionName;  
    std::string varName;       
    AstNode *indexExpr;        
    AstNode *expr;             

public:
    ArrayAssignExpr(const std::string &functionName, const std::string &varName, AstNode *indexExpr, AstNode *expr)
        : functionName(functionName), varName(varName), indexExpr(indexExpr), expr(expr) {}

    ~ArrayAssignExpr()
    {
        delete indexExpr;
        delete expr;
    }

    CodePlace generateCode() override
    {
        CodePlace result;
        CodePlace indexCode = indexExpr->generateCode();
        CodePlace exprCode = expr->generateCode();
        
        result.place = "";  

        
        result.code = indexCode.code;
        result.code += "sll $t1, " + indexCode.place + ", 2\n";  
        result.code += "add $t1, $t1, $sp\n";  
        
        
        result.code += exprCode.code;
        result.code += "sw " + exprCode.place + ", 0($t1)\n";
        
        
        freeTempRegister(indexCode.place);
        freeTempRegister(exprCode.place);

        return result;
    }
};