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
            wsp = [ \t\n]+;

        
            
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
            
            open_bracket = '\x5B';  // ASCII value for '['
            close_bracket = '\x5D'; // ASCII value for ']'



            comma = ",";
            semicolon = ";";
            ampersands = "&";

            ident = [a-zA-Z_][a-zA-Z0-9_]*;
            "//"[^'\n']*    {continue; } //linea
            "/*"([^*]*"*"[^/])*[^*]*"*/"  {continue; }//bloque

            
            
            //* { return Token::Other; }
            end{ return (YYMAXFILL == db.lim - db.tok)? Token::Eof : Token::Error;}
            wsp {continue;}

            
            kw_int { text = db.tokenText(); return Token::KW_INT; }
            number { text = db.tokenText(); return Token::NUMBER; }
            op_assign { text = db.tokenText(); return Token::OP_ASSIGN; }
            kw_if { text = db.tokenText(); return Token::KW_IF; }
            kw_else { text = db.tokenText(); return Token::KW_ELSE; }
            kw_while { text = db.tokenText(); return Token::KW_WHILE; }
            kw_cout { text = db.tokenText(); return Token::KW_COUT; }
            lt_lt { text = db.tokenText(); return Token::LT_LT; }
            gt_gt { text = db.tokenText(); return Token::GT_GT; }
            kw_endl { text = db.tokenText(); return Token::KW_ENDL; }
            string { text = db.tokenText(); return Token::STRING_LITERAL; }
            bool_or { text = db.tokenText(); return Token::BOOL_OR; }
            bool_and { text = db.tokenText(); return Token::BOOL_AND; }
            gt { text = db.tokenText(); return Token::GT; }
            lt { text = db.tokenText(); return Token::LT; }
            gte { text = db.tokenText(); return Token::GTE; }
            lte { text = db.tokenText(); return Token::LTE; }
            ne { text = db.tokenText(); return Token::NE; }
            eq { text = db.tokenText(); return Token::EQ; }
            op_add { text = db.tokenText(); return Token::OP_ADD; }
            op_sub { text = db.tokenText(); return Token::OP_SUB; }
            op_mult { text = db.tokenText(); return Token::OP_MULT; }
            op_div { text = db.tokenText(); return Token::OP_DIV; }
            op_mod { text = db.tokenText(); return Token::OP_MOD; }
            open_par { text = db.tokenText(); return Token::OPEN_PAR; }
            close_par { text = db.tokenText(); return Token::CLOSE_PAR; }
            open_curly { text = db.tokenText(); return Token::OPEN_CURLY; }
            close_curly { text = db.tokenText(); return Token::CLOSE_CURLY; }
            
            open_bracket { text = db.tokenText(); return Token::OPEN_BRACKET; }

            close_bracket { text = db.tokenText(); return Token::CLOSE_BRACKET; }
            comma { text = db.tokenText(); return Token::COMMA; }
            semicolon { text = db.tokenText(); return Token::SEMICOLON; }
            ampersands { text = db.tokenText(); return Token::AMPERSANS; }
            ident { text = db.tokenText(); return Token::IDENT; }
        */
    }
}