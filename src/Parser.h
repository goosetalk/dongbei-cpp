#pragma once
#include "Keywords.h"
struct Token {
    TokenType type;
    std::string value; // 比如 "100" 或者 "老王"
};
class Node {
public:
    virtual ~Node() = default;
    // 核心：转译成 C++ 代码的逻辑
    virtual std::string to_cpp() const = 0;
};
class VariableNode : public Node {
    std::string name;
public:
    VariableNode(std::string n) : name(std::move(n)) {}
    std::string to_cpp() const override {
        return name; // 假设 dongbei 变量名直接兼容 C++，否则得做个映射
    }
};
class NegateNode : public Node {
    std::unique_ptr<Node> operand;
public:
    NegateNode(std::unique_ptr<Node> op) : operand(std::move(op)) {}
    std::string to_cpp() const override {
        // 加个括号保平安，防止运算优先级整串了
        return "(-(" + operand->to_cpp() + "))";
    }
};
class BinaryNode : public Node {
    TokenType op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
public:
    BinaryNode(TokenType o, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : op(o), left(std::move(l)), right(std::move(r)) {
    }

    std::string to_cpp() const override {
        std::string op_str;
        switch (op) {
        case TokenType::KW_PLUS:       op_str = "+"; break;
        case TokenType::KW_MINUS:      op_str = "-"; break;
        case TokenType::KW_TIMES:      op_str = "*"; break;
        case TokenType::KW_DIVIDE_BY:  op_str = "/"; break;
        //case TokenType::KW_EQUAL:      op_str = "=="; break;
        default: op_str = " /* 啥玩意儿 */ ";
        }
        // 生成形如 (left + right) 的代码
        return "(" + left->to_cpp() + " " + op_str + " " + right->to_cpp() + ")";
    }
};
class NumberNode : public Node {
    std::string value;
public:
    NumberNode(std::string v) : value(std::move(v)) {}
    std::string to_cpp() const override {
        return value;
    }
};
struct VarDeclNode : Node {
    std::string name;
    std::string cpp_type; // 映射自 [规整] -> int
    VarDeclNode(std::string n, std::string t) : name(n), cpp_type(t) {}
    std::string to_cpp() const override { return cpp_type + " " + name + ";"; }
};

class IfNode : public Node {
    std::unique_ptr<Node> condition;
    std::vector<std::unique_ptr<Node>> then_body;
    std::vector<std::unique_ptr<Node>> else_body;
public:
    IfNode(std::unique_ptr<Node> cond,
        std::vector<std::unique_ptr<Node>> t_body,
        std::vector<std::unique_ptr<Node>> e_body)
        : condition(std::move(cond)), then_body(std::move(t_body)), else_body(std::move(e_body)) {
    }

    std::string to_cpp() const override {
        std::string res = "if (" + condition->to_cpp() + ") {\n";
        for (const auto& node : then_body) res += "    " + node->to_cpp() + "\n";
        res += "}";
        if (!else_body.empty()) {
            res += " else {\n";
            for (const auto& node : else_body) res += "    " + node->to_cpp() + "\n";
            res += "}";
        }
        return res;
    }
};
class ForNode : public Node {
    std::string var_name;
    std::unique_ptr<Node> start_expr;
    std::unique_ptr<Node> end_expr;
    std::vector<std::unique_ptr<Node>> body;
public:
    ForNode(std::string name, std::unique_ptr<Node> start, std::unique_ptr<Node> end, std::vector<std::unique_ptr<Node>> b)
        : var_name(name), start_expr(std::move(start)), end_expr(std::move(end)), body(std::move(b)) {
    }

    std::string to_cpp() const override {
        std::string s = "for (int " + var_name + " = " + start_expr->to_cpp() + "; " +
            var_name + " <= " + end_expr->to_cpp() + "; ++" + var_name + ") {\n";
        for (const auto& node : body) s += "    " + node->to_cpp() + "\n";
        s += "}";
        return s;
    }
};
class ProgramNode : public Node {
    std::vector<std::unique_ptr<Node>> statements;
public:
    void addStatement(std::unique_ptr<Node> stmt) {
        statements.push_back(std::move(stmt));
    }

    std::string to_cpp() const override {
        std::string code = "#include <iostream>\n\n";
        code += "int main() {\n";
        for (const auto& stmt : statements) {
            code += "    " + stmt->to_cpp() + "\n";
        }
        code += "    return 0;\n";
        code += "}\n";
        return code;
    }
};
struct AssignNode : Node {
    std::string var_name;
    std::unique_ptr<Node> expr; // 改成 Node 指针
    AssignNode(std::string n, std::unique_ptr<Node> e)
        : var_name(n), expr(std::move(e)) {
    }

    std::string to_cpp() const override {
        return var_name + " = " + expr->to_cpp() + ";";
    }
};

struct SayNode : Node {
    std::unique_ptr<Node> expr; // 支持打印复杂表达式
    SayNode(std::unique_ptr<Node> e) : expr(std::move(e)) {}

    std::string to_cpp() const override {
        return "std::cout << " + expr->to_cpp() + " << std::endl;";
    }
};
class MiniParser {
    std::vector<Token> tokens;
    int pos = 0;

    Token peek() { return (pos < tokens.size()) ? tokens[pos] : Token{ TokenType::END_OF_FILE }; }
    Token consume() { return tokens[pos++]; }

public:
    MiniParser(std::vector<Token> t) : tokens(t) {}
    std::unique_ptr<ProgramNode> parseProgram();
private:
    std::unique_ptr<Node> parseVarOrAssign();
    std::unique_ptr<Node> parseSay();
    std::unique_ptr<Node> parsePrimary();
    std::unique_ptr<Node> parseExpression(int min_precision);
    int getPrecision(TokenType type);
};