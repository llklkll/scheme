#pragma once

#include <variant>
#include <optional>
#include <istream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int64_t value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

    bool IsNumber(const char& elem);

    bool IsStartSymbol(const char& elem);

    bool IsSymbol(const char& elem);

private:
    std::istream* in_;
    std::string cur_const_;
    std::string cur_symbol_;
    int cur_char_;
    Token token_;
};