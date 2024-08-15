#include <iostream>
#include "MiniCppLexer.hpp"
#include "MiniCppParser.hpp"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cerr << "Not enough arguments\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (in.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "Empty file\n";
        return 1;
    }
    if (!in.is_open())
    {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    Lexer lex(in);
    Parser parser(lex);

    try
    {
        parser.parse();
        std::cout<<"Parsing successful\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Parsing failed\n";
        std::cerr << e.what() << "\n";
    }


    return 0;
}