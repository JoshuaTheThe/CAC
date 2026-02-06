#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include "tokeniser.hpp"
#include "parser/parser.hpp"

void display(AST *tree, size_t depth = 0)
{
        if (!tree)
                return;

        std::cerr << std::string(depth * 2, ' ');

        switch (tree->GetType())
        {
        default:
                std::cerr << "Num: " << tree->GetNumber() << " Ident: " << tree->GetIdentifier() << " Type: " << tree->GetType() << "\n";
                break;
        }

        for (AST *child : tree->GetChildren())
        {
                display(child, depth + 1);
        }
}

int main(int argc, char **argv)
{
        if (argc < 2)
        {
                std::cerr << "Usage: " << argv[0] << " <file.ca>\n";
                return 1;
        }

        std::ifstream file(argv[1]);
        if (!file)
        {
                std::cerr << "Error: could not open file " << argv[1] << "\n";
                return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();

        std::string_view sv(code);
        Tokeniser tok(sv);
        auto tokens = tok.Fetch();
        for (auto& x : tokens)
        {
                std::cout << " [INFO]" << " type=" << x.Type << " ident=" << x.Identifier << " integer=" << x.Integer << std::endl;
        }

        Parser p(tokens);
        AST *tree = new AST(AST::AST_BLOCK);
        while (p.Peek().Type != Tokeniser::TYPE_NONE)
        {
                tree->AddChild(p.ParseStatement());
        }
        display(tree, 0);
        // CodeGenerator codegen = CodeGenerator();
        // codegen.Generate(tree);
        // std::cout << "\tsection .text" << std::endl;
        // 
        // for (auto &line : codegen.GetLines())
        // {
        //         std::cout << line << std::endl;
        // }
        // 
        // std::cout << "\tsection .rodata" << std::endl;
        // 
        // size_t i = 1;
        // for (auto &line : codegen.GetStrings())
        // {
        //         std::cout << "_string_" << std::to_string(i) << ":" << std::endl;
        //         for (auto &x : line)
        //         {
        //                 std::cout << "\tdb " << std::to_string((int)x) << std::endl;
        //         }
        //         std::cout << "\tdb 0" << std::endl;
        //         i++;
        // }

        delete tree;
        return 0;
}
