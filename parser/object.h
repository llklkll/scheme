#pragma once

#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    };

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(std::string name) : name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    };

private:
    std::string name_;
};

class Cell : public Object {
public:
    Cell() = default;
    Cell(std::shared_ptr<Object>& first) : first_(first), second_(nullptr){};
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };
    void AppendFirst(std::shared_ptr<Object> object) {
        first_ = object;
    };

    void AppendSecond(std::shared_ptr<Object> object) {
        second_ = object;
    };

private:
    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

class CloseBracket : public Object {
    bool operator==(const CloseBracket&) const {
        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(obj);
    return p;
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto p = std::dynamic_pointer_cast<T>(obj);
    if (p) {
        return true;
    }
    return false;
};
