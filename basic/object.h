#pragma once

#include <memory>
#include <vector>
#include "error.h"

class Object;

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(obj);
    return p;
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto p = std::dynamic_pointer_cast<T>(obj);
    if (p) {
        return true;
    }
    return false;
}

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    };

private:
    int64_t value_;
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

class CloseBracket : public Object {
    bool operator==(const CloseBracket&) const {
        return true;
    }
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

class IsNumber : public Object {
public:
    IsNumber() = default;
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool isnumber = true;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                isnumber = false;
                break;
            }
        }
        if (isnumber) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Equal : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool equal = true;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(elem)->GetValue() != As<Number>(args[0])->GetValue()) {
                equal = false;
                break;
            }
        }
        if (equal) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Increasing : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool increase = true;
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i - 1]) || !Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(args[i])->GetValue() >= As<Number>(args[i - 1])->GetValue()) {
                increase = false;
                break;
            }
        }
        if (increase) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Decreasing : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool decrease = true;
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i - 1]) || !Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(args[i])->GetValue() <= As<Number>(args[i - 1])->GetValue()) {
                decrease = false;
                break;
            }
        }
        if (decrease) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Nondecreasing : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool increase = true;
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i - 1]) || !Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(args[i])->GetValue() > As<Number>(args[i - 1])->GetValue()) {
                increase = false;
                break;
            }
        }
        if (increase) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Nonincreasing : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool decrease = true;
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i - 1]) || !Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(args[i])->GetValue() < As<Number>(args[i - 1])->GetValue()) {
                decrease = false;
                break;
            }
        }
        if (decrease) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Add : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        int64_t result = 0;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                throw RuntimeError{"runtime-error"};
            }
            result += As<Number>(elem)->GetValue();
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Sub : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.empty()) {
            throw RuntimeError{"runtime-error"};
        }
        if (!Is<Number>(args[0])) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t result = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            result -= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Mul : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        int64_t result = 1;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                throw RuntimeError{"runtime-error"};
            }
            result *= As<Number>(elem)->GetValue();
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Div : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.empty()) {
            throw RuntimeError{"runtime-error"};
        }
        if (!Is<Number>(args[0])) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t result = As<Number>(args[0])->GetValue();
        for (size_t i = 1; i < args.size(); ++i) {
            if (!Is<Number>(args[i])) {
                throw RuntimeError{"runtime-error"};
            }
            result /= As<Number>(args[i])->GetValue();
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Max : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.empty()) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t result = -10e9;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(elem)->GetValue() > result) {
                result = As<Number>(elem)->GetValue();
            }
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Min : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.empty()) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t result = 10e9;
        for (auto elem : args) {
            if (!Is<Number>(elem)) {
                throw RuntimeError{"runtime-error"};
            }
            if (As<Number>(elem)->GetValue() < result) {
                result = As<Number>(elem)->GetValue();
            }
        }
        return std::make_shared<Number>(Number{result});
    }
};

class Abs : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 1) {
            throw RuntimeError{"runtime-error"};
        }
        if (!Is<Number>(args[0])) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t result = std::abs(As<Number>(args[0])->GetValue());
        return std::make_shared<Number>(Number{result});
    }
};

class IsBool : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        bool isbool = true;
        for (auto elem : args) {
            if (!Is<Symbol>(elem)) {
                isbool = false;
                break;
            } else if (As<Symbol>(elem)->GetName() != "#t" && As<Symbol>(elem)->GetName() != "#f") {
                isbool = false;
                break;
            }
        }
        if (isbool) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Not : public Object {
public:
    std::shared_ptr<Object> operator()(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 1) {
            throw RuntimeError{"runtime-error"};
        }
        if (Is<Symbol>(args[0]) && As<Symbol>(args[0])->GetName() == "#f") {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class FunctionForList : public Object {
public:
    std::string Tostring(std::shared_ptr<Object> tree) {
        std::string ans;
        bool open = false;
        while (tree != nullptr) {
            if (Is<Symbol>(tree)) {
                ans += As<Symbol>(tree)->GetName();
                tree = nullptr;
            } else if (Is<Number>(tree)) {
                ans += std::to_string(As<Number>(tree)->GetValue());
                tree = nullptr;
            } else {
                auto next = As<Cell>(tree)->GetFirst();
                if (!As<Cell>(tree)->GetFirst() && !As<Cell>(tree)->GetSecond()) {
                    ans += "()";
                }
                if (Is<Cell>(next)) {
                    open = true;
                    ans += "(";
                    ans += Tostring(next);
                    tree = As<Cell>(tree)->GetSecond();
                } else {
                    ans += Tostring(next);
                    if (Is<Number>(next) && Is<Number>(As<Cell>(tree)->GetSecond())) {
                        ans += " .";
                    }
                    tree = As<Cell>(tree)->GetSecond();
                    if (tree != nullptr) {
                        ans += " ";
                    }
                }
            }
        }
        if (open) {
            ans += ")";
        }
        return ans;
    };
    ~FunctionForList() = default;
    virtual std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                               std::shared_ptr<Object> param = nullptr) = 0;
};

class IsPair : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param = nullptr) override {
        if (Is<Cell>(arg)) {
            auto pair = (As<Cell>(arg))->GetSecond();
            if (Is<Cell>(pair) && Is<Cell>(As<Cell>(pair)->GetFirst())) {
                return std::make_shared<Symbol>(Symbol{"#t"});
            }
        } else {
            RuntimeError{"runtime-error"};
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class IsNull : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param = nullptr) override {
        if (Is<Cell>(arg)) {
            auto pair = (As<Cell>(arg))->GetSecond();
            if (Is<Cell>(pair) && (As<Cell>(pair)->GetFirst() == nullptr)) {
                return std::make_shared<Symbol>(Symbol{"#t"});
            }
        } else {
            throw RuntimeError{"runtime-error"};
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class IsList : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param = nullptr) override {
        bool islist = true;
        if (Is<Cell>(arg)) {
            auto pair = As<Cell>(arg)->GetSecond();
            auto list = As<Cell>(pair)->GetFirst();
            while (list) {
                if (Is<Cell>(list)) {
                    list = As<Cell>(list)->GetSecond();
                } else {
                    islist = false;
                    list = nullptr;
                }
            }
        }
        if (islist) {
            return std::make_shared<Symbol>(Symbol{"#t"});
        }
        return std::make_shared<Symbol>(Symbol{"#f"});
    }
};

class Car : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param = nullptr) override {
        if (Is<Cell>(arg)) {
            auto pair = (As<Cell>(arg))->GetSecond();
            if (Is<Cell>(pair) && (As<Cell>(pair)->GetFirst() != nullptr)) {
                auto next = As<Cell>(As<Cell>(pair)->GetFirst());
                return std::make_shared<Symbol>(Symbol{Tostring(next->GetFirst())});
            } else {
                throw RuntimeError{"runtime-error"};
            }
        } else {
            throw RuntimeError{"runtime-error"};
        }
    }
};

class Cdr : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param = nullptr) override {
        if (Is<Cell>(arg)) {
            auto pair = (As<Cell>(arg))->GetSecond();
            if (Is<Cell>(pair) && (As<Cell>(pair)->GetFirst() != nullptr)) {
                auto next = As<Cell>(As<Cell>(pair)->GetFirst());
                if (Is<Cell>(next->GetSecond()) || next->GetSecond() == nullptr) {
                    return std::make_shared<Symbol>(
                        Symbol{"(" + Tostring(next->GetSecond()) + ")"});
                }
                return std::make_shared<Symbol>(Symbol{Tostring(next->GetSecond())});
            } else {
                throw RuntimeError{"runtime-error"};
            }
        } else {
            throw RuntimeError{"runtime-error"};
        }
    }
};

class ListRef : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param) override {
        if (!Is<Number>(param)) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t count = As<Number>(param)->GetValue();
        auto list = As<Cell>(As<Cell>(arg)->GetSecond())->GetFirst();
        std::shared_ptr<Symbol> result = nullptr;
        int64_t i = 0;
        while (list && i <= count) {
            if (Is<Cell>(list)) {
                if (i == count) {
                    result = std::make_shared<Symbol>(Symbol{Tostring(As<Cell>(list)->GetFirst())});
                }
                list = As<Cell>(list)->GetSecond();
                i += 1;
            } else {
                if (i == count) {
                    result = std::make_shared<Symbol>(Symbol{Tostring(list)});
                }
                result = std::make_shared<Symbol>(Symbol{Tostring(list)});
                i += 1;
                list = nullptr;
            }
        }
        if (i - 1 == count) {
            return result;
        } else {
            throw RuntimeError{"runtime-error"};
        }
    }
};

class ListTail : public FunctionForList {
public:
    std::shared_ptr<Object> operator()(std::shared_ptr<Object> arg,
                                       std::shared_ptr<Object> param) override {
        if (!Is<Number>(param)) {
            throw RuntimeError{"runtime-error"};
        }
        int64_t count = As<Number>(param)->GetValue();
        auto list = As<Cell>(As<Cell>(arg)->GetSecond())->GetFirst();
        std::shared_ptr<Symbol> result = nullptr;
        int64_t i = 1;
        while (list && i <= count) {
            if (Is<Cell>(list)) {
                if (i == count) {
                    result = std::make_shared<Symbol>(
                        Symbol{"(" + Tostring(As<Cell>(list)->GetSecond()) + ")"});
                }
                list = As<Cell>(list)->GetSecond();
                i += 1;
            } else {
                if (i == count) {
                    result = std::make_shared<Symbol>(Symbol{"(" + Tostring(list) + ")"});
                }
                i += 1;
                list = nullptr;
            }
        }
        if (i - 1 == count) {
            return result;
        } else {
            throw RuntimeError{"runtime-error"};
        }
    }
};