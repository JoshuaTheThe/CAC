#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <map>

class Tokeniser
{
public:
        enum TokenType
        {
                TYPE_NONE, // Error/EOF
                TYPE_NUMBER,
                TYPE_STRING,
                TYPE_IDENTIFIER,
                TYPE_LPAREN,
                TYPE_RPAREN,
                TYPE_LBRACE,   // {
                TYPE_RBRACE,   // }
                TYPE_LBRACKET, // [
                TYPE_RBRACKET, // ]
                TYPE_SEMICOLON,
                TYPE_COMMA,
                TYPE_DOT,
                TYPE_COLON,
                TYPE_QUESTION,
                TYPE_PLUS,
                TYPE_MINUS,
                TYPE_STAR,       // *
                TYPE_SLASH,      // /
                TYPE_PERCENT,    // %
                TYPE_EQ,         // =
                TYPE_EQEQ,       // ==
                TYPE_BANG,       // !
                TYPE_BANGEQ,     // !=
                TYPE_LT,         // <
                TYPE_GT,         // >
                TYPE_LTEQ,       // <=
                TYPE_GTEQ,       // >=
                TYPE_AMP,        // &
                TYPE_PIPE,       // |
                TYPE_CARET,      // ^
                TYPE_TILDE,      // ~
                TYPE_LSHIFT,     // <<
                TYPE_RSHIFT,     // >>
                TYPE_PLUSEQ,     // +=
                TYPE_MINUSEQ,    // -=
                TYPE_STAREQ,     // *=
                TYPE_PERCENTEQ,  // %=
                TYPE_SLASHEQ,    // /=
                TYPE_LSHIFTEQ,   // <<=
                TYPE_RSHIFTEQ,   // >>=
                TYPE_AMPEQ,      // &=
                TYPE_PIPEEQ,     // |=
                TYPE_CARETEQ,    // ^=
                TYPE_PLUSPLUS,   // ++
                TYPE_MINUSMINUS, // --
                TYPE_ARROW,      // ->
                TYPE_PIPEPIPE,   // ||
                TYPE_AMPAMP,     // &&

                TYPE_KEYWORD_BASE, // First keyword type
                TYPE_KEYWORD_IF = TYPE_KEYWORD_BASE,
                TYPE_KEYWORD_ELSE,
                TYPE_KEYWORD_SWITCH,
                TYPE_KEYWORD_CASE,
                TYPE_KEYWORD_DEFAULT,
                TYPE_KEYWORD_FOR,
                TYPE_KEYWORD_WHILE,
                TYPE_KEYWORD_DO,
                TYPE_KEYWORD_BREAK,
                TYPE_KEYWORD_CONTINUE,
                TYPE_KEYWORD_GOTO,
                TYPE_KEYWORD_RETURN,

                // Type keywords
                TYPE_KEYWORD_VOID,
                TYPE_KEYWORD_CHAR,
                TYPE_KEYWORD_SHORT,
                TYPE_KEYWORD_INT,
                TYPE_KEYWORD_LONG,
                TYPE_KEYWORD_FLOAT,
                TYPE_KEYWORD_DOUBLE,
                TYPE_KEYWORD_SIGNED,
                TYPE_KEYWORD_UNSIGNED,

                // Qualifiers
                TYPE_KEYWORD_CONST,
                TYPE_KEYWORD_VOLATILE,
                TYPE_KEYWORD_RESTRICT,

                // Storage class
                TYPE_KEYWORD_AUTO,
                TYPE_KEYWORD_REGISTER,
                TYPE_KEYWORD_STATIC,
                TYPE_KEYWORD_EXTERN,
                TYPE_KEYWORD_TYPEDEF,

                // Struct/union/enum
                TYPE_KEYWORD_STRUCT,
                TYPE_KEYWORD_UNION,
                TYPE_KEYWORD_ENUM,

                // C99/C11
                TYPE_KEYWORD_INLINE,
                TYPE_KEYWORD_NORETURN,

                // Preprocessor (for your dialect)
                TYPE_KEYWORD_IMPORT,

                // Your dialect extensions
                TYPE_KEYWORD_THIS,
                TYPE_KEYWORD_NEW,
                TYPE_KEYWORD_DELETE,
                TYPE_KEYWORD_TRUE,
                TYPE_KEYWORD_FALSE,
                TYPE_KEYWORD_NULL,

                TYPE_KEYWORD_COUNT
        };

        struct Token
        {
                TokenType Type = Tokeniser::TYPE_NONE;
                std::string Identifier = "";
                int64_t Integer = 0;
        };

private:
        const std::array<std::string, 41> Keywords =
            {
                "if", "else", "switch", "case", "default",
                "for", "while", "do", "break", "continue",
                "goto", "return",
                "void", "char", "short", "int", "long", "float", "double",
                "signed", "unsigned",
                "const", "volatile", "restrict",
                "auto", "register", "static", "extern", "typedef",
                "struct", "union", "enum",
                "inline", "noreturn",
                "import", "this", "new", "delete", "true", "false", "null"};
        const std::map<std::string, TokenType> Symbols = {
            {"(", TYPE_LPAREN},
            {")", TYPE_RPAREN},
            {"{", TYPE_LBRACE},
            {"}", TYPE_RBRACE},
            {"[", TYPE_LBRACKET},
            {"]", TYPE_RBRACKET},
            {",", TYPE_COMMA},
            {".", TYPE_DOT},
            {":", TYPE_COLON},
            {";", TYPE_SEMICOLON},
            {"?", TYPE_QUESTION},
            {"+", TYPE_PLUS},
            {"-", TYPE_MINUS},
            {"*", TYPE_STAR},
            {"/", TYPE_SLASH},
            {"=", TYPE_EQ},
            {"<", TYPE_LT},
            {">", TYPE_GT},
            {"&", TYPE_AMP},
            {"|", TYPE_PIPE},
            {"^", TYPE_CARET},
            {"!", TYPE_BANG},
            {"~", TYPE_TILDE},
            {"==", TYPE_EQEQ},
            {"!=", TYPE_BANGEQ},
            {"<=", TYPE_LTEQ},
            {">=", TYPE_GTEQ},
            {"<<", TYPE_LSHIFT},
            {">>", TYPE_RSHIFT},
            {"--", TYPE_MINUSMINUS},
            {"++", TYPE_PLUSPLUS},
            {"+=", TYPE_PLUSEQ},
            {"-=", TYPE_MINUSEQ},
            {"*=", TYPE_STAREQ},
            {"/=", TYPE_SLASHEQ},
            {"->", TYPE_ARROW},
            {"||", TYPE_PIPEPIPE},
            {"&&", TYPE_AMPAMP},
        };
        std::string_view Input;
        std::vector<Token> Output;
        size_t Position;

public:
        ~Tokeniser() = default;
        Tokeniser(std::string_view Input);
        Tokeniser::Token Tokenise(void);
        std::vector<Tokeniser::Token> Fetch(void);
        bool IsWhiteSpace(char X);
        char Advance(void);
        char Peek(void);
        Tokeniser::Token TokeniseNumber(char X);
        Tokeniser::Token TokeniseIdentifier(char X);
        Tokeniser::Token TokeniseString(char X);
};

#endif
