#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include "../tokeniser.hpp"

class AST
{
public:
        enum ASTType
        {
                /* Statements */
                AST_IF,
                AST_WHILE,
                AST_DOWHILE,
                AST_FOR,
                AST_SWITCH,
                AST_CASE,
                AST_DEFAULT,
                AST_BLOCK,
                AST_RETURN,
                AST_BREAK,
                AST_CONTINUE,
                AST_GOTO,
                AST_LABEL,
                AST_DECLARATION,
                AST_VARDECL,
                AST_FNDECL,

                AST_TYPE,

                /* Factors */
                AST_NUMBER,
                AST_IDENTIFIER,

                /* Expressions */
                AST_ASSIGNMENT,
                AST_TERNARY,
                AST_OR,
                AST_AND,
                AST_BOR,
                AST_BXOR,
                AST_BAND,
                AST_EQUALITY,
                AST_RELATIONAL,
                AST_SHIFT,
                AST_ADDITIVE,
                AST_MULTIPLICATIVE,
                AST_UNARY,
                AST_EXPR,

                AST_MEMBER_ACCESS,
                AST_ARRAY_SUBSCRIPT,
                AST_PTR_MEMBER_ACCESS,
        };

private:
        ASTType Type;
        std::string Identifier;
        Tokeniser::TokenType TType;
        int64_t Number;
        std::vector<AST *> Children;

public:
        std::vector<std::string> FunctionArguments; /* Only used when defining a function */
        AST(ASTType type) : Type(type), TType((Tokeniser::TokenType)0), Number(0) {}
        AST(ASTType type, Tokeniser::TokenType ttype) : Type(type), TType(ttype), Number(0) {}
        AST(int64_t num) : Type(AST_NUMBER), Number(num) {}
        AST(const std::string &id, ASTType type) : Type(type), Identifier(id) {}

        void AddChild(AST *child) { Children.push_back(child); }
        const std::vector<AST *> &GetChildren() const { return Children; }
        ASTType GetType() const { return Type; }
        const std::string &GetIdentifier() const { return Identifier; }
        void SetIdentifier(std::string i) { Identifier = i; }
        int64_t GetNumber() const { return Number; }

        ~AST()
        {
                for (AST *child : Children)
                {
                        delete child;
                }
        }
};

class Parser
{
        std::vector<Tokeniser::Token> Tokens;
        size_t Position;

public:
        Parser(const std::vector<Tokeniser::Token> &tokens) : Tokens(tokens), Position(0) {}

        Tokeniser::Token Peek(void);
        Tokeniser::Token Peek(size_t N);
        Tokeniser::Token Consume(void);
        Tokeniser::Token Consume(Tokeniser::Token test);
        AST *ParseFactor(void);
        AST *ParsePrimary(void);
        AST *ParseUnary(void);
        AST *ParsePostfix(void);
        AST *ParseMultiplicative(void);
        AST *ParseAdditive(void);
        AST *ParseShift(void);
        AST *ParseRelational(void);
        AST *ParseEquality(void);
        AST *ParseBAnd(void);
        AST *ParseBXor(void);
        AST *ParseBOr(void);
        AST *ParseAnd(void);
        AST *ParseOr(void);
        AST *ParseTernary(void);
        AST *ParseAssignment(void);
        AST *ParseExpression(void);
        AST *ParseStatement(void);
        AST *ParseConditional(void);
        AST *ParseBlock(void);
        AST *ParseForStatement(void);
        AST *ParseDoWhileStatement(void);
        AST *ParseSwitchStatement(void);
        AST *ParseReturnStatement(void);
        AST *ParseBreakStatement(void);
        AST *ParseContinueStatement(void);
        AST *ParseGotoStatement(void);
        AST *ParseLabeledStatement(void);
        AST *ParseType(void);
};

#endif
