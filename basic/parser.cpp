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

bool TokenIsQuote(Token& token) {
    bool isquote = true;
    try {
        std::get<QuoteToken>(token);
    } catch (...) {
        isquote = false;
    }
    return isquote;
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
    } else if (TokenIsQuote(token)) {
        tokenizer->Next();
        if (tokenizer->IsEnd()) {
            throw SyntaxError("syntax error");
        }
        token = tokenizer->GetToken();
        if (!TokenIsNumber(token) && !TokenIsSymbol(token) &&
            (token != Token{BracketToken::OPEN})) {
            throw SyntaxError("syntax error");
        }
        auto quote = Read(tokenizer);
        auto list = std::make_shared<Cell>(Cell());
        auto symbol = std::make_shared<Symbol>(Symbol{"quote"});
        list->AppendFirst(symbol);
        auto list_of_quote = std::make_shared<Cell>(Cell());
        list_of_quote->AppendSecond(nullptr);
        list_of_quote->AppendFirst(quote);
        list->AppendSecond(list_of_quote);
        return list;
    } else {
        throw SyntaxError("syntax error");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto root = std::make_shared<Cell>(Cell());
    auto list = root;
    if (tokenizer->GetToken() == Token{DotToken{}}) {
        throw SyntaxError("syntax error");
    }
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
            if (Is<CloseBracket>(object)) {
                throw SyntaxError("syntax error");
            }
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