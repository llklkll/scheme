#pragma once

#include <string>
#include <map>
#include "object.h"
#include "tokenizer.h"
#include "parser.h"
#include <sstream>
#include <set>
#include "error.h"

static std::set<std::string> function_list{
    "number?", "=", ">",   "<",   ">=",  "<=",       "-",  "+",
    "*",       "/", "max", "min", "abs", "boolean?", "not"};

static std::set<std::string> for_list{"pair?", "null?",    "list?",    "car",
                                      "cdr",   "list-ref", "list-tail"};

class Interpreter {
public:
    std::string Tostring(std::shared_ptr<Object> tree);

    std::vector<std::shared_ptr<Object>> BuildArguments(std::shared_ptr<Object> pair);

    std::shared_ptr<Object> ReadFull(const std::string& str);

    std::shared_ptr<Object> Eval(std::shared_ptr<Object> tree);

    std::string Run(const std::string& str);

    std::shared_ptr<Object> Or(std::shared_ptr<Object> tree);

    std::shared_ptr<Object> And(std::shared_ptr<Object> tree);

    std::shared_ptr<Object> Execute(const std::string& name,
                                    std::vector<std::shared_ptr<Object>> args);

    std::shared_ptr<Object> ExecuteList(const std::string& name, std::shared_ptr<Object> arg,
                                        std::shared_ptr<Object> param);

    friend class Object;
};
