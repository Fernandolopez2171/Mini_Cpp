#include <iostream>
#include <fstream>
#include "MiniCppLexer.hpp"
#include "MiniCppParser.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>\n";
        return -1;
    }

    std::ifstream in(argv[1]);

    if (!in.is_open()) {
        std::cerr << "Cannot open input file\n";
        return -1;
    }

    if (in.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Input file is empty\n";
        return -1;
    }

    Lexer lex(in);
    Parser parser(lex);

    try {
        AstNode* result = parser.parse();
        
        std::cout << "Generating code...\n";
        std::string generatedCode = result->generateCode().code;

        std::ofstream outFile(argv[2]);
        if (outFile.is_open()) {
            outFile << generatedCode;
            outFile.close();
            std::cout << "Code successfully saved to " << argv[2] << "\n";
            return 1;  
        } else {
            std::cerr << "Cannot open output file for writing\n";
            return -1;  
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;  
    }

    in.close();
    return 1;  
}
