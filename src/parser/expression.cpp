#include "parser.hpp"

AST *Parser::ParseFactor(void)
{
        Tokeniser::Token Token = Peek();
        switch (Token.Type)
        {
        case Tokeniser::TYPE_NUMBER:
                Consume();
                return new AST(Token.Integer);
        case Tokeniser::TYPE_IDENTIFIER:
                Consume();
                return new AST(Token.Identifier, AST::AST_IDENTIFIER);
        default:
                return nullptr;
        }
}

AST *Parser::ParsePrimary(void)
{
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_LPAREN)
        {
                Consume();
                AST *node = ParseExpression();
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RPAREN});
                return node;
        }

        return ParseFactor();
}

AST *Parser::ParseUnary(void)
{
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_PLUSPLUS ||
            tok.Type == Tokeniser::TYPE_MINUSMINUS ||
            tok.Type == Tokeniser::TYPE_PLUS ||
            tok.Type == Tokeniser::TYPE_MINUS ||
            tok.Type == Tokeniser::TYPE_BANG ||
            tok.Type == Tokeniser::TYPE_TILDE ||
            tok.Type == Tokeniser::TYPE_STAR ||
            tok.Type == Tokeniser::TYPE_AMP)
        {
                AST *node = new AST(AST::AST_UNARY, tok.Type);
                Consume();
                node->AddChild(ParseUnary());
                return node;
        }

        return ParsePrimary();
}

AST *Parser::ParseMultiplicative(void)
{
        AST *first = ParseUnary();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_STAR || tok.Type == Tokeniser::TYPE_SLASH || tok.Type == Tokeniser::TYPE_PERCENT)
        {
                AST *node = new AST(AST::AST_MULTIPLICATIVE, tok.Type);
                node->AddChild(first);
                Consume();
                node->AddChild(ParseUnary());
                return node;
        }

        return first;
}

AST *Parser::ParseAdditive(void)
{
        AST *first = ParseMultiplicative();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_PLUS || tok.Type == Tokeniser::TYPE_MINUS)
        {
                AST *node = new AST(AST::AST_ADDITIVE, tok.Type);
                node->AddChild(first);
                Consume();
                node->AddChild(ParseMultiplicative());
                return node;
        }

        return first;
}

AST *Parser::ParseShift(void)
{
        AST *first = ParseAdditive();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_LSHIFT || tok.Type == Tokeniser::TYPE_RSHIFT)
        {
                AST *node = new AST(AST::AST_SHIFT, tok.Type);
                node->AddChild(first);
                Consume();
                node->AddChild(ParseAdditive());
                return node;
        }

        return first;
}

AST *Parser::ParseRelational(void)
{
        AST *first = ParseShift();
        Tokeniser::Token tok = Peek();

        if (tok.Type >= Tokeniser::TYPE_LT && tok.Type <= Tokeniser::TYPE_GTEQ)
        {
                AST *node = new AST(AST::AST_RELATIONAL, tok.Type);
                node->AddChild(first);
                Consume();
                node->AddChild(ParseShift());
                return node;
        }

        return first;
}

AST *Parser::ParseEquality(void)
{
        AST *first = ParseRelational();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_EQEQ)
        {
                AST *node = new AST(AST::AST_EQUALITY);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_EQEQ});
                node->AddChild(ParseRelational());
                return node;
        }

        return first;
}

AST *Parser::ParseBAnd(void)
{
        AST *first = ParseEquality();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_AMP)
        {
                AST *node = new AST(AST::AST_BAND);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_AMP});
                node->AddChild(ParseEquality());
                return node;
        }

        return first;
}

AST *Parser::ParseBXor(void)
{
        AST *first = ParseBAnd();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_CARET)
        {
                AST *node = new AST(AST::AST_BXOR);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_CARET});
                node->AddChild(ParseBAnd());
                return node;
        }

        return first;
}

AST *Parser::ParseBOr(void)
{
        AST *first = ParseBXor();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_PIPE)
        {
                AST *node = new AST(AST::AST_BOR);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_PIPE});
                node->AddChild(ParseBXor());
                return node;
        }

        return first;
}

AST *Parser::ParseAnd(void)
{
        AST *first = ParseBOr();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_AMPAMP)
        {
                AST *node = new AST(AST::AST_AND);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_AMPAMP});
                node->AddChild(ParseBOr());
                return node;
        }

        return first;
}

AST *Parser::ParseOr(void)
{
        AST *first = ParseAnd();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_PIPEPIPE)
        {
                AST *node = new AST(AST::AST_OR);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_PIPEPIPE});
                node->AddChild(ParseAnd());
                return node;
        }

        return first;
}

AST *Parser::ParseTernary(void)
{
        AST *first = ParseOr();
        Tokeniser::Token tok = Peek();

        if (tok.Type == Tokeniser::TYPE_QUESTION)
        {
                AST *node = new AST(AST::AST_TERNARY);
                node->AddChild(first);
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_QUESTION});
                node->AddChild(ParseOr());
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_COLON});
                node->AddChild(ParseOr());
                return node;
        }

        return first;
}

AST *Parser::ParseAssignment(void)
{
        AST *first = ParseTernary();
        Tokeniser::Token tok = Peek();

        if (tok.Type >= Tokeniser::TYPE_PLUSEQ && tok.Type <= Tokeniser::TYPE_CARETEQ)
        {
                AST *node = new AST(AST::AST_ASSIGNMENT);
                Consume();
                node->AddChild(first);
                node->AddChild(ParseAssignment());
                return node;
        }

        return first;
}

AST *Parser::ParseExpression(void)
{
        AST *first = ParseAssignment();
        Tokeniser::Token tok = Peek();

        if (tok.Type != Tokeniser::TYPE_COMMA)
        {
                return first;
        }

        AST *node = new AST(AST::AST_EXPR);
        node->AddChild(first);

        while (tok.Type == Tokeniser::TYPE_COMMA)
        {
                Consume();
                node->AddChild(ParseAssignment());
                tok = Peek();
        }

        return node;
}
