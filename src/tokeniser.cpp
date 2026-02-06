#include "tokeniser.hpp"

bool Tokeniser::IsWhiteSpace(char X)
{
        return (X == ' ') || (X == '\t') || (X == '\n') || (X == '\r');
}

char Tokeniser::Advance(void)
{
        return this->Input[this->Position++];
}

char Tokeniser::Peek(void)
{
        return this->Input[this->Position];
}

Tokeniser::Token Tokeniser::TokeniseNumber(char X)
{
        Tokeniser::Token Token{.Type = Tokeniser::TYPE_NUMBER, .Integer = 0};
        while (X >= '0' && X <= '9' && this->Position <= this->Input.size())
        {
                Token.Integer = Token.Integer * 10 + X - '0';
                X = this->Advance();
        }

        this->Position -= 1;
        return Token;
}

Tokeniser::Token Tokeniser::TokeniseIdentifier(char X)
{
        Tokeniser::Token Token{.Type = Tokeniser::TYPE_IDENTIFIER, .Integer = 0};
        while (((X >= 'A' && X <= 'Z') || (X >= 'a' && X <= 'z') || X == '_' || X == '-') && this->Position <= this->Input.size())
        {
                if (X == '-')
                {
                        X = '_';
                }
                std::string Y{X};
                Token.Identifier.append(Y);
                X = this->Advance();
        }

        for (size_t i = 0; i < this->Keywords.size(); ++i)
        {
                if (Token.Identifier == this->Keywords[i])
                {
                        Token.Type = (Tokeniser::TokenType)((size_t)Tokeniser::TYPE_KEYWORD_BASE + i);
                }
        }

        this->Position -= 1;
        return Token;
}

Tokeniser::Token Tokeniser::TokeniseString(char X)
{
        Tokeniser::Token Token;
        Token.Type = Tokeniser::TYPE_STRING;

        while (this->Position < this->Input.size())
        {
                char C = this->Advance();

                if (C == '"')
                        break;

                if (C == '\\')
                {
                        if (this->Position >= this->Input.size())
                                break;

                        char Esc = this->Advance();
                        switch (Esc)
                        {
                        case 'n':
                                Token.Identifier += '\n';
                                break;
                        case 't':
                                Token.Identifier += '\t';
                                break;
                        case '"':
                                Token.Identifier += '\"';
                                break;
                        case '\\':
                                Token.Identifier += '\\';
                                break;
                        default:
                                Token.Identifier += Esc;
                                break;
                        }
                }
                else
                {
                        Token.Identifier += C;
                }
        }

        return Token;
}

Tokeniser::Token Tokeniser::Tokenise(void)
{
        Tokeniser::Token Token;
        char X;

        do
        {
                X = this->Advance();
        } while (this->IsWhiteSpace(X) && this->Position < this->Input.size());

        if (X == '/' && this->Peek() == '/')
        {
                while (this->Position < this->Input.size() && X != '\n' && X != '\r')
                {
                        X = this->Advance();
                }
                return Tokenise();
        }

        if (X == '/' && this->Peek() == '*')
        {
                this->Advance();
                while (this->Position < this->Input.size())
                {
                        X = this->Advance();
                        if (X == '*' && this->Peek() == '/')
                        {
                                this->Advance();
                                return Tokenise();
                        }
                }

                Token.Type = TYPE_NONE;
                return Token;
        }

        std::string twoChar(1, X);
        if (this->Position < this->Input.size())
        {
                twoChar += this->Peek();
        }

        std::string oneChar(1, X);

        auto it2 = this->Symbols.find(twoChar);
        if (it2 != this->Symbols.end())
        {
                Token.Type = it2->second;
                this->Advance();
                return Token;
        }

        auto it1 = this->Symbols.find(oneChar);
        if (it1 != this->Symbols.end())
        {
                Token.Type = it1->second;
                return Token;
        }

        switch (X)
        {
        case '"':
                return TokeniseString(X);
        default:
                break;
        }

        if (X >= '0' && X <= '9')
        {
                return TokeniseNumber(X);
        }
        else if ((X >= 'A' && X <= 'Z') || (X >= 'a' && X <= 'z') || X == '_')
        {
                return TokeniseIdentifier(X);
        }

        Token.Type = TYPE_NONE;
        return Token;
}

Tokeniser::Tokeniser(std::string_view Input)
{
        this->Position = 0;
        this->Input = Input;
        this->Output.clear();
        // std::cout << this->Input.size() << std::endl;
        while (this->Position < this->Input.size())
                this->Output.push_back(this->Tokenise());
}

std::vector<Tokeniser::Token> Tokeniser::Fetch(void)
{
        return this->Output;
}
