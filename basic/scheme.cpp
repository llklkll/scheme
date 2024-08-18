#include "scheme.h"

std::string Interpreter::Run(const std::string& str) {
    auto ast = Eval(ReadFull(str));

    if (Is<Symbol>(ast)) {
        return As<Symbol>(ast)->GetName();
    } else if (Is<Number>(ast)) {
        return std::to_string(As<Number>(ast)->GetValue());
    } else {
        throw RuntimeError{"runtime error"};
    }
}

std::shared_ptr<Object> Interpreter::Eval(std::shared_ptr<Object> tree) {
    if (Is<Number>(tree)) {
        return tree;
    } else if (Is<Symbol>(tree)) {
        if (As<Symbol>(tree)->GetName() == "#t" || As<Symbol>(tree)->GetName() == "#f") {
            return tree;
        } else {
            throw RuntimeError{"runtime error"};
        }
    } else if (Is<Cell>(tree)) {
        auto next = As<Cell>(tree)->GetFirst();
        if (Is<Symbol>(next)) {
            if (As<Symbol>(next)->GetName() == "quote") {
                if (!(As<Cell>(tree)->GetSecond())) {
                    return std::make_shared<Symbol>(Symbol{"()"});
                } else {
                    return std::make_shared<Symbol>(Symbol{Tostring(As<Cell>(tree)->GetSecond())});
                }
            } else if (As<Symbol>(next)->GetName() == "or") {
                return Or(As<Cell>(tree)->GetSecond());
            } else if (As<Symbol>(next)->GetName() == "and") {
                return And(As<Cell>(tree)->GetSecond());
            } else if (As<Symbol>(next)->GetName() == "list") {
                std::string result = Tostring(As<Cell>(tree)->GetSecond());
                return std::make_shared<Symbol>(Symbol{"(" + result + ")"});
            } else if (As<Symbol>(next)->GetName() == "cons") {
                auto result = std::make_shared<Cell>(Cell());
                if (!Is<Cell>(As<Cell>(tree)->GetSecond())) {
                    throw RuntimeError{"runtime error"};
                }
                auto first = As<Cell>(As<Cell>(tree)->GetSecond());
                auto second = first->GetSecond();
                if (!Is<Cell>(second)) {
                    throw RuntimeError{"runtime error"};
                }
                result->AppendFirst(first->GetFirst());
                result->AppendSecond(As<Cell>(second)->GetFirst());
                return std::make_shared<Symbol>(Symbol{"(" + Tostring(result) + ")"});
            } else if (auto search = function_list.find(As<Symbol>(next)->GetName());
                       search != function_list.end()) {
                std::vector<std::shared_ptr<Object>> args =
                    BuildArguments(As<Cell>(tree)->GetSecond());
                return Execute(As<Symbol>(next)->GetName(), args);
            } else if (auto search2 = for_list.find(As<Symbol>(next)->GetName());
                       search2 != for_list.end()) {
                auto arg = (As<Cell>(tree)->GetSecond());
                if (!Is<Cell>(arg)) {
                    throw RuntimeError{"runtime error"};
                }
                if (!Is<Cell>(As<Cell>(arg)->GetSecond())) {
                    return ExecuteList(As<Symbol>(next)->GetName(), As<Cell>(arg)->GetFirst(),
                                       nullptr);
                }
                return ExecuteList(As<Symbol>(next)->GetName(), As<Cell>(arg)->GetFirst(),
                                   As<Cell>(As<Cell>(arg)->GetSecond())->GetFirst());
            } else {
                throw NameError{"wrong argument"};
            }
        } else {
            throw RuntimeError{"runtime error"};
        }
    } else {
        throw RuntimeError{"runtime error"};
    }
};

std::shared_ptr<Object> Interpreter::ReadFull(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};

    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError{"syntax error"};
    }
    return obj;
};

std::vector<std::shared_ptr<Object>> Interpreter::BuildArguments(std::shared_ptr<Object> pair) {
    std::vector<std::shared_ptr<Object>> args;
    while (pair) {
        if (Is<Symbol>(pair) || Is<Number>(pair)) {
            args.push_back(pair);
            pair = nullptr;
        } else {
            args.push_back(Eval(As<Cell>(pair)->GetFirst()));
            pair = As<Cell>(pair)->GetSecond();
        }
    }
    return args;
};

std::string Interpreter::Tostring(std::shared_ptr<Object> tree) {
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
                tree = nullptr;
            } else if (Is<Cell>(next)) {
                open = true;
                ans += "(";
                ans += Tostring(next);
                tree = As<Cell>(tree)->GetSecond();
            } else {
                ans += Tostring(next);
                if (Is<Number>(next) && Is<Number>(As<Cell>(tree)->GetSecond())) {
                    ans += " .";
                };
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

std::shared_ptr<Object> Interpreter::Or(std::shared_ptr<Object> pair) {
    bool result = false;
    std::shared_ptr<Object> first_true = nullptr;
    while (pair) {
        if ((Is<Symbol>(pair) && As<Symbol>(pair)->GetName() == "#t") || Is<Number>(pair)) {
            first_true = pair;
            result = true;
            break;
        } else if (Is<Symbol>(pair)) {
            pair = nullptr;
        } else {
            auto object = Eval(As<Cell>(pair)->GetFirst());
            if ((Is<Symbol>(object) && As<Symbol>(object)->GetName() == "#t") ||
                Is<Number>(object)) {
                first_true = object;
                result = true;
                break;
            }
            pair = As<Cell>(pair)->GetSecond();
        }
    }
    if (result) {
        return first_true;
    }
    return std::make_shared<Symbol>(Symbol{"#f"});
}

std::shared_ptr<Object> Interpreter::And(std::shared_ptr<Object> pair) {
    bool result = true;
    std::shared_ptr<Object> first_false = nullptr;
    std::shared_ptr<Object> last = std::make_shared<Symbol>(Symbol{"#t"});
    while (pair) {
        if (Is<Symbol>(pair) && As<Symbol>(pair)->GetName() == "#f") {
            first_false = pair;
            result = false;
            break;
        } else if (Is<Symbol>(pair) || Is<Number>(pair)) {
            last = pair;
            pair = nullptr;
        } else {
            auto object = Eval(As<Cell>(pair)->GetFirst());
            if (Is<Symbol>(object) && As<Symbol>(object)->GetName() == "#f") {
                first_false = object;
                result = false;
                break;
            } else if (Is<Symbol>(object) || Is<Number>(object)) {
                last = object;
            }
            pair = As<Cell>(pair)->GetSecond();
        }
    }
    if (result) {
        return last;
    }
    return first_false;
}

std::shared_ptr<Object> Interpreter::Execute(const std::string& name,
                                             std::vector<std::shared_ptr<Object>> args) {
    if (name == "number?") {
        return IsNumber{}(args);
    } else if (name == "=") {
        return Equal{}(args);
    } else if (name == ">") {
        return Increasing{}(args);
    } else if (name == "<") {
        return Decreasing{}(args);
    } else if (name == ">=") {
        return Nondecreasing{}(args);
    } else if (name == "<=") {
        return Nonincreasing{}(args);
    } else if (name == "+") {
        return Add{}(args);
    } else if (name == "-") {
        return Sub{}(args);
    } else if (name == "*") {
        return Mul{}(args);
    } else if (name == "/") {
        return Div{}(args);
    } else if (name == "max") {
        return Max{}(args);
    } else if (name == "min") {
        return Min{}(args);
    } else if (name == "abs") {
        return Abs{}(args);
    } else if (name == "boolean?") {
        return IsBool{}(args);
    } else if (name == "not") {
        return Not{}(args);
    } else {
        throw RuntimeError{"runtime-error"};
    }
}

std::shared_ptr<Object> Interpreter::ExecuteList(const std::string& name,
                                                 std::shared_ptr<Object> arg,
                                                 std::shared_ptr<Object> param) {
    if (name == "pair?") {
        return IsPair{}(arg, param);
    } else if (name == "null?") {
        return IsNull{}(arg, param);
    } else if (name == "list?") {
        return IsList{}(arg, param);
    } else if (name == "car") {
        return Car{}(arg, param);
    } else if (name == "cdr") {
        return Cdr{}(arg, param);
    } else if (name == "list-ref") {
        return ListRef{}(arg, param);
    } else if (name == "list-tail") {
        return ListTail{}(arg, param);
    } else {
        throw RuntimeError{"runtime-error"};
    }
}
