#include "parser.hpp"

Tokeniser::Token Parser::Peek(void)
{
        if (Position < Tokens.size())
                return Tokens[Position];
        return Tokeniser::Token{};
}

Tokeniser::Token Parser::Peek(size_t N)
{
        if (Position+N < Tokens.size())
                return Tokens[Position+N];
        return Tokeniser::Token{};
}

Tokeniser::Token Parser::Consume(void)
{
        if (Position < Tokens.size())
                return Tokens[Position++];
        return Tokeniser::Token{};
}

Tokeniser::Token Parser::Consume(Tokeniser::Token test)
{
        if (Peek().Type == test.Type)
        {
                if (Position < Tokens.size())
                        return Tokens[Position++];
                return Tokeniser::Token{};
        }
        else
        {
                std::cerr << " [ERROR] Expected '" << (int)test.Type << "', found '" << Peek().Type << "'" << std::endl;
                exit(1);
        }
}
