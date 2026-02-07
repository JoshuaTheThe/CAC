#include "parser.hpp"

AST *Parser::ParseConditional(void)
{
        Tokeniser::Token tok = Peek();
        AST *Node = nullptr;
        if (tok.Type == Tokeniser::TYPE_KEYWORD_IF)
        {
                Consume();
                Node = new AST(AST::AST_IF);
        }
        else if (tok.Type == Tokeniser::TYPE_KEYWORD_WHILE)
        {
                Consume();
                Node = new AST(AST::AST_WHILE);
        }
        else
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_IF});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LPAREN});
        Node->AddChild(ParseExpression());
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RPAREN});
        Node->AddChild(ParseStatement());
        if (Peek().Type == Tokeniser::TYPE_KEYWORD_ELSE)
        {
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_ELSE});
                Node->AddChild(ParseStatement());
        }

        return Node;
}

AST *Parser::ParseBlock(void)
{
        AST *Block = new AST(AST::AST_BLOCK);
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LBRACE});
        while (Peek().Type != Tokeniser::TYPE_RBRACE)
        {
                Block->AddChild(ParseStatement());
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RBRACE});
        return Block;
}

AST *Parser::ParseType(void)
{
        Tokeniser::Token tok = Peek();
        switch (tok.Type)
        {
                case Tokeniser::TYPE_KEYWORD_AUTO:
                case Tokeniser::TYPE_KEYWORD_INT:
                case Tokeniser::TYPE_KEYWORD_SHORT:
                case Tokeniser::TYPE_KEYWORD_CHAR:
                case Tokeniser::TYPE_KEYWORD_SIGNED:
                case Tokeniser::TYPE_KEYWORD_UNSIGNED:
                case Tokeniser::TYPE_KEYWORD_STRUCT:
                case Tokeniser::TYPE_KEYWORD_LONG:
                case Tokeniser::TYPE_KEYWORD_FLOAT:
                case Tokeniser::TYPE_KEYWORD_VOID:
                case Tokeniser::TYPE_KEYWORD_DOUBLE:
                case Tokeniser::TYPE_KEYWORD_CONST:
                case Tokeniser::TYPE_STAR:
                        Consume();
                        AST *Node = new AST(AST::AST_TYPE, tok.Type);
                        AST *Child = ParseType();
                        if (Child)
                                Node->AddChild(Child);
                        return Node;
        }
        return nullptr;
}

AST *Parser::ParseStatement(void)
{
        AST *type = ParseType();
        Tokeniser::Token tok = Peek();
        AST *node = nullptr;

        switch (tok.Type)
        {
        case Tokeniser::TYPE_LBRACE:
                node = ParseBlock();
                break;
        case Tokeniser::TYPE_KEYWORD_IF:
        case Tokeniser::TYPE_KEYWORD_WHILE:
                node = ParseConditional();
                break;
        case Tokeniser::TYPE_KEYWORD_FOR:
                node = ParseForStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_DO:
                node = ParseDoWhileStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_SWITCH:
                node = ParseSwitchStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_RETURN:
                node = ParseReturnStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_BREAK:
                node = ParseBreakStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_CONTINUE:
                node = ParseContinueStatement();
                break;
        case Tokeniser::TYPE_KEYWORD_GOTO:
                node = ParseGotoStatement();
                break;
                break;
        case Tokeniser::TYPE_SEMICOLON:
                Consume();
                node = new AST(AST::AST_EXPR);
                break;
        case Tokeniser::TYPE_IDENTIFIER:
                if (Peek().Type == Tokeniser::TYPE_COLON)
                {
                        node = ParseLabeledStatement();
                        break;
                }
        default:
                node = ParseExpression();
                Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
                break;
        }

        if (node && type)
        {
                type->AddChild(node);
                return type;
        }
        else if (type && !node)
                return type;
        return node;
}

AST *Parser::ParseForStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_FOR});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LPAREN});

        AST *ForNode = new AST(AST::AST_FOR);
        if (Peek().Type != Tokeniser::TYPE_SEMICOLON)
        {
//                ForNode->AddChild(ParseDeclarationOrExpression());
        }
        else
        {
                ForNode->AddChild(nullptr);
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
        if (Peek().Type != Tokeniser::TYPE_SEMICOLON)
        {
                ForNode->AddChild(ParseExpression());
        }
        else
        {
                ForNode->AddChild(nullptr);
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
        if (Peek().Type != Tokeniser::TYPE_RPAREN)
        {
                ForNode->AddChild(ParseExpression());
        }
        else
        {
                ForNode->AddChild(nullptr);
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RPAREN});
        ForNode->AddChild(ParseStatement());
        return ForNode;
}

AST *Parser::ParseDoWhileStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_DO});

        AST *DoWhileNode = new AST(AST::AST_DOWHILE);
        DoWhileNode->AddChild(ParseStatement());
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_WHILE});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LPAREN});
        DoWhileNode->AddChild(ParseExpression());
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RPAREN});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});

        return DoWhileNode;
}

AST *Parser::ParseSwitchStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_SWITCH});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LPAREN});
        AST *SwitchNode = new AST(AST::AST_SWITCH);
        SwitchNode->AddChild(ParseExpression());
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RPAREN});
        AST *Body = new AST(AST::AST_BLOCK);
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_LBRACE});

        while (Peek().Type != Tokeniser::TYPE_RBRACE)
        {
                Tokeniser::Token tok = Peek();
                if (tok.Type == Tokeniser::TYPE_KEYWORD_CASE)
                {
                        Consume();
                        AST *CaseNode = new AST(AST::AST_CASE);

                        AST *CaseExpr = ParseExpression();
                        CaseNode->AddChild(CaseExpr);

                        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_COLON});

                        while (Peek().Type != Tokeniser::TYPE_RBRACE &&
                               Peek().Type != Tokeniser::TYPE_KEYWORD_CASE &&
                               Peek().Type != Tokeniser::TYPE_KEYWORD_DEFAULT)
                        {
                                CaseNode->AddChild(ParseStatement());
                        }

                        Body->AddChild(CaseNode);
                }
                else if (tok.Type == Tokeniser::TYPE_KEYWORD_DEFAULT)
                {
                        Consume();
                        AST *DefaultNode = new AST(AST::AST_DEFAULT);

                        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_COLON});

                        while (Peek().Type != Tokeniser::TYPE_RBRACE &&
                               Peek().Type != Tokeniser::TYPE_KEYWORD_CASE &&
                               Peek().Type != Tokeniser::TYPE_KEYWORD_DEFAULT)
                        {
                                DefaultNode->AddChild(ParseStatement());
                        }

                        Body->AddChild(DefaultNode);
                }
                else
                {
                        // Shouldn't happen in valid switch statement
                        break;
                }
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_RBRACE});
        SwitchNode->AddChild(Body);

        return SwitchNode;
}

AST *Parser::ParseReturnStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_RETURN});

        AST *ReturnNode = new AST(AST::AST_RETURN);

        if (Peek().Type != Tokeniser::TYPE_SEMICOLON)
        {
                ReturnNode->AddChild(ParseExpression());
        }
        else
        {
                ReturnNode->AddChild(nullptr);
        }

        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});

        return ReturnNode;
}

AST *Parser::ParseBreakStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_BREAK});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
        return new AST(AST::AST_BREAK);
}

AST *Parser::ParseContinueStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_CONTINUE});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
        return new AST(AST::AST_CONTINUE);
}

AST *Parser::ParseGotoStatement(void)
{
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_KEYWORD_GOTO});
        Tokeniser::Token label = Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_IDENTIFIER});
        AST *GotoNode = new AST(AST::AST_GOTO);
        GotoNode->SetIdentifier(label.Identifier);
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_SEMICOLON});
        return GotoNode;
}

AST *Parser::ParseLabeledStatement(void)
{
        Tokeniser::Token label = Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_IDENTIFIER});
        Consume((Tokeniser::Token){.Type = Tokeniser::TYPE_COLON});
        AST *LabelNode = new AST(AST::AST_LABEL);
        LabelNode->SetIdentifier(label.Identifier);
        LabelNode->AddChild(ParseStatement());
        return LabelNode;
}
