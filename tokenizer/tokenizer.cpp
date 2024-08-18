#include <tokenizer.h>
#include <iostream>

bool SymbolToken::operator==(const SymbolToken& other) const {
    if (name == other.name) {
        return true;
    }
    return false;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    if (value == other.value) {
        return true;
    }
    return false;
}

Tokenizer::Tokenizer(std::istream* in) {
    in_ = in;
    Next();
}

bool Tokenizer::IsNumber(const char& elem) {
    if (elem >= 48 && elem <= 57) {
        return true;
    }
    return false;
}
bool Tokenizer::IsStartSymbol(const char& elem) {
    if ((elem >= 65 && elem <= 90) || (elem >= 97 && elem <= 122) || (elem >= 60 && elem <= 62) ||
        elem == 42 || elem == 47 || elem == 35) {
        return true;
    }
    return false;
}

bool Tokenizer::IsSymbol(const char& elem) {
    if (IsStartSymbol(elem) || IsNumber(elem) || elem == 63 || elem == 33 || elem == 45) {
        return true;
    }
    return false;
}

bool Tokenizer::IsEnd() {
    if (cur_char_ == EOF) {
        return true;
    }
    return false;
}
void Tokenizer::Next() {
    char elem = in_->get();
    cur_char_ = elem;
    if (!IsEnd()) {
        if (IsStartSymbol(elem) && cur_symbol_.empty()) {
            cur_symbol_ += elem;
            if (IsSymbol(in_->peek())) {
                Next();
            } else {
                token_ = Token{SymbolToken{cur_symbol_}};
                cur_symbol_.clear();
            }
        } else if (IsSymbol(elem) && !cur_symbol_.empty()) {
            cur_symbol_ += elem;
            if (IsSymbol(in_->peek())) {
                Next();
            } else {
                token_ = Token{SymbolToken{cur_symbol_}};
                cur_symbol_.clear();
            }
        } else if ((elem == 45 || elem == 43) && cur_symbol_.empty()) {
            if (IsNumber(in_->peek())) {
                cur_const_ += elem;
                Next();
            } else {
                cur_symbol_ += elem;
                token_ = Token{SymbolToken{cur_symbol_}};
                cur_symbol_.clear();
            }
        } else if (IsNumber(elem)) {
            if (IsNumber(in_->peek())) {
                cur_const_ += elem;
                Next();
            } else {
                cur_const_ += elem;
                token_ = Token{ConstantToken{std::stoi(cur_const_)}};
                cur_const_.clear();
            }
        } else if (elem == 39) {
            token_ = Token{QuoteToken{}};
        } else if (elem == 46) {
            token_ = DotToken{};
        } else if (elem == 41) {
            token_ = Token{BracketToken::CLOSE};
        } else if (elem == 40) {
            token_ = Token{BracketToken::OPEN};
        } else if (elem == 32 || elem == 10) {
            Next();
        }
    }
}
Token Tokenizer::GetToken() {
    return token_;
}
