#pragma once

#include <string>
#include <fstream>

enum Token {
    IDENT,          // Identificador
    KW_INT,         // int
    NUMBER,         // Número
    OP_ASSIGN,      // =
    KW_IF,          // if
    KW_ELSE,        // else
    KW_WHILE,       // while
    KW_COUT,        // cout
    LT_LT,          // <<
    GT_GT,          // >>
    KW_ENDL,        // endl
    STRING_LITERAL, // Literal de cadena
    BOOL_OR,        // ||
    BOOL_AND,       // &&
    GT,             // >
    LT,             // <
    GTE,            // >=
    LTE,            // <=
    NE,             // !=
    EQ,             // ==
    OP_ADD,         // +
    OP_SUB,         // -
    OP_MULT,        // *
    OP_DIV,         // /
    OP_MOD,         // %
    OPEN_PAR,       // (
    CLOSE_PAR,      // )
    OPEN_CURLY,     // {
    CLOSE_CURLY,    // }
    OPEN_BRACKET,   // [
    CLOSE_BRACKET,  // ]
    COMMA,          // ,
    SEMICOLON,      // ;
    AVPERSANS,      // &
    Other,          // Otros tokens no categorizados
    Error,          // Token de error
    Eof             // Fin de archivo
};


class Lexer
{
public:
    static const size_t SIZE = 512;

    Lexer(std::ifstream &in) : db(in) {}

    Token nextToken();

    std::string tokenText() const { return text; }

private:
    struct DataBuffer
    {
        char *buf;
        char *lim;
        char *cur;
        char *tok;
        char* marker;
        bool eof;
        std::ifstream &in;

        DataBuffer(std::ifstream &in);

        std::string tokenText() const { return std::string(tok, cur); }

        ~DataBuffer() { delete[] buf; }

        bool fill(size_t need);
    };

private:
    DataBuffer db;
    std::string text;
};