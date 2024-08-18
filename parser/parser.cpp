#include <parser.h>
#include "error.h"

bool TokenIsNumber(Token& token) {
    bool isnumber = true;
    try {
        std::get<ConstantToken>(token);
    } catch (...) {
        isnumber = false;
    }
    return isnumber;
}

bool TokenIsSymbol(Token& token) {
    bool issymbol = true;
    try {
        std::get<SymbolToken>(token);
    } catch (...) {
        issymbol = false;
    }
    return issymbol;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("syntax error");
    }
    Token token = tokenizer->GetToken();
    if (token == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        return ReadList(tokenizer);
    } else if (TokenIsNumber(token)) {
        tokenizer->Next();
        return std::make_shared<Number>(Number{std::get<ConstantToken>(token).value});
    } else if (TokenIsSymbol(token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(Symbol{std::get<SymbolToken>(token).name});
    } else if (token == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return std::make_shared<CloseBracket>();
    } else if (token == Token{DotToken{}}) {
        tokenizer->Next();
        return Read(tokenizer);
    } else {
        throw SyntaxError("syntax error");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto root = std::make_shared<Cell>(Cell());
    auto list = root;
    auto object = Read(tokenizer);
    if (Is<CloseBracket>(object)) {
        return nullptr;
    }
    while (!Is<CloseBracket>(object)) {
        if (tokenizer->GetToken() == Token{DotToken{}}) {
            if (list->GetFirst() && object) {
                throw SyntaxError("syntax error");
            }
            list->AppendFirst(object);
            object = Read(tokenizer);
            list->AppendSecond(object);
            object = Read(tokenizer);
        } else {
            if (list->GetFirst() && object) {
                throw SyntaxError("syntax error");
            }
            list->AppendFirst(object);
            if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                auto next = std::make_shared<Cell>(Cell());
                list->AppendSecond(next);
                list = next;
            }
            object = Read(tokenizer);
        }
    }
    return root;
}