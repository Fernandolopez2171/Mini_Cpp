#include <cstring>
#include "MiniCppLexer.hpp"

/*!max:re2c*/

Lexer::DataBuffer::DataBuffer(std::ifstream &in)
    : in(in),
      buf(new char[SIZE + YYMAXFILL]),
      lim(buf + SIZE),
      cur(lim),
      tok(lim),
      eof(false)
{}

bool Lexer::DataBuffer::fill(size_t need)
{
    if (eof)
        return false;

    const size_t free = tok - buf;
    if (free < need)
        return false;

    memmove(buf, tok, lim - tok);
    lim -= free;
    cur -= free;
    tok -= free;

    in.read(lim, free);
    lim += in.gcount();

    if (lim < buf + SIZE)
    {
        eof = true;
        memset(lim, 0, YYMAXFILL);
        lim += YYMAXFILL;
    }
    return true;
}


Token Lexer::nextToken()
{
    while (true)
    {
        db.tok = db.cur;

        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = db.cur;
            re2c:define:YYLIMIT = db.lim;
            re2c:define:YYFILL = "if (!db.fill(@@)) return Token::Error;";
            re2c:define:YYFILL:naked = 1;
            re2c:define:YYMARKER = db.marker;

            end = "\x00";
            wsp = [ \t]+;
            newline = "\n";
            comment_line = "//" [^\n]*;
            comment_block = "/*"[^*]*"*"[^/]*"*"+"/";

            newline { line++; column = 1; continue; } // Resetea la columna en nueva línea
            comment_line { continue; } // Comentarios de una línea
            comment_block { continue; } // Comentarios de bloque

            kw_int = "int";
            number = [0-9]+("." [0-9]+)?;
            op_assign = "=";
            kw_if = "if";
            kw_else = "else";
            kw_while = "while";
            kw_cout = "std::cout";
            lt_lt = "<<";
            gt_gt = ">>";
            kw_endl = "std::endl";
            string = "\"" [^\"]* "\"";
            bool_or = "||";
            bool_and = "&&";
            gt = ">";
            lt = "<";
            gte = ">=";
            lte = "<=";
            ne = "!=";
            eq = "==";
            op_add = "+";
            op_sub = "-";
            op_mult = "*";
            op_div = "/";
            op_mod = "%";
            open_par = "(";
            close_par = ")";
            open_curly = "{";
            close_curly = "}";
            kw_cin = "std::cin";
            open_bracket = '\x5B';  // ASCII value for '['
            close_bracket = '\x5D'; // ASCII value for ']'
            comma = ",";
            semicolon = ";";
            ampersand = "&";
            ident = [a-zA-Z_][a-zA-Z0-9_]*;

            wsp { column += db.cur - db.tok; continue; } // Incrementa columna según caracteres de espacio
            kw_int { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_INT; }
            number { column += db.cur - db.tok; text = db.tokenText(); return Token::NUMBER; }
            op_assign { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_ASSIGN; }
            kw_if { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_IF; }
            kw_else { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_ELSE; }
            kw_while { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_WHILE; }
            kw_cout { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_COUT; }
            kw_cin { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_CIN; }
            lt_lt { column += db.cur - db.tok; text = db.tokenText(); return Token::LT_LT; }
            gt_gt { column += db.cur - db.tok; text = db.tokenText(); return Token::GT_GT; }
            kw_endl { column += db.cur - db.tok; text = db.tokenText(); return Token::KW_ENDL; }
            string { column += db.cur - db.tok; text = db.tokenText(); return Token::STRING_LITERAL; }
            bool_or { column += db.cur - db.tok; text = db.tokenText(); return Token::BOOL_OR; }
            bool_and { column += db.cur - db.tok; text = db.tokenText(); return Token::BOOL_AND; }
            gt { column += db.cur - db.tok; text = db.tokenText(); return Token::GT; }
            lt { column += db.cur - db.tok; text = db.tokenText(); return Token::LT; }
            gte { column += db.cur - db.tok; text = db.tokenText(); return Token::GTE; }
            lte { column += db.cur - db.tok; text = db.tokenText(); return Token::LTE; }
            ne { column += db.cur - db.tok; text = db.tokenText(); return Token::NE; }
            eq { column += db.cur - db.tok; text = db.tokenText(); return Token::EQ; }
            op_add { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_ADD; }
            op_sub { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_SUB; }
            op_mult { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_MULT; }
            op_div { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_DIV; }
            op_mod { column += db.cur - db.tok; text = db.tokenText(); return Token::OP_MOD; }
            open_par { column += db.cur - db.tok; text = db.tokenText(); return Token::OPEN_PAR; }
            close_par { column += db.cur - db.tok; text = db.tokenText(); return Token::CLOSE_PAR; }
            open_curly { column += db.cur - db.tok; text = db.tokenText(); return Token::OPEN_CURLY; }
            close_curly { column += db.cur - db.tok; text = db.tokenText(); return Token::CLOSE_CURLY; }
            open_bracket { column += db.cur - db.tok; text = db.tokenText(); return Token::OPEN_BRACKET; }
            close_bracket { column += db.cur - db.tok; text = db.tokenText(); return Token::CLOSE_BRACKET; }
            comma { column += db.cur - db.tok; text = db.tokenText(); return Token::COMMA; }
            semicolon { column += db.cur - db.tok; text = db.tokenText(); return Token::SEMICOLON; }
            ampersand { column += db.cur - db.tok; text = db.tokenText(); return Token::AMPERSAND; }
            ident { column += db.cur - db.tok; text = db.tokenText(); return Token::IDENT; }
            
            * { column += db.cur - db.tok; text = db.tokenText(); return Token::Other; }
            end { return (YYMAXFILL == db.lim - db.tok) ? Token::Eof : Token::Error; }
        */
    }
}
