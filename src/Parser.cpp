#include "Parser.h"
std::unique_ptr<Node> MiniParser::parseVarOrAssign() {
    std::string name = consume().value; // 拿到 "laowang"

    if (peek().type == TokenType::KW_BE) {
        consume(); // 是
        consume(); // [规整]
        consume(); // 活雷锋
        consume(); // 。
        return std::make_unique<VarDeclNode>(name, "int");
    }
    else if (peek().type == TokenType::KW_BECOME) {
        consume(); // 装
        // 这里调用 parseExpression ！！！
        auto expr = parseExpression(0);
        consume(); // 。
        // 注意：AssignNode 也要改，让它接收 unique_ptr<Node> 而不是 string
        return std::make_unique<AssignNode>(name, std::move(expr));
    }
    return nullptr;
}

// 解析：唠唠：老王。
std::unique_ptr<Node> MiniParser::parseSay() {
    consume(); // 唠唠
    consume(); // ：
    auto expr = parseExpression(0); // 支持唠唠：1 + 2。
    consume(); // 。
    return std::make_unique<SayNode>(std::move(expr));
}

// 补充：parsePrimary 的实现
std::unique_ptr<Node> MiniParser::parsePrimary() {
    Token t = consume();
    if (t.type == TokenType::NUMBER) {
        return std::make_unique<NumberNode>(t.value);
    }
    else if (t.type == TokenType::IDENTIFIER) {
        return std::make_unique<VariableNode>(t.value);
    }
    return nullptr; // 理论上这里应该抛出“整叉劈了”的异常
}
// 在 Parser 类里添加
std::unique_ptr<Node> MiniParser::parseExpression(int min_precision) {
    auto left = parsePrimary(); // 先读一个数字或变量

    while (true) {
        Token op = peek();
        int precision = getPrecision(op.type);
        if (precision < min_precision) break; // 优先级不够，撤！

        consume(); // 吃掉运算符
        auto right = parseExpression(precision + 1); // 递归处理右侧
        left = std::make_unique<BinaryNode>(op.type, std::move(left), std::move(right));
    }
    return left;
}

int MiniParser::getPrecision(TokenType type) {
    if (type == TokenType::KW_PLUS || type == TokenType::KW_MINUS) return 1;
    if (type == TokenType::KW_TIMES || type == TokenType::KW_DIVIDE_BY) return 2;
    return -1;
}
std::unique_ptr<ProgramNode> MiniParser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (peek().type != TokenType::END_OF_FILE) {
        // 调用统一的语句解析接口
        auto stmt = parseStatement();
        if (stmt) {
            program->addStatement(std::move(stmt));
        }
        else {
            // 如果解析不出有效的语句，为了防止死循环，吃掉一个 Token
            consume();
        }
    }
    return program;
}

// 调度中心：根据开头 Token 决定去哪条路
std::unique_ptr<Node> MiniParser::parseStatement() {
    TokenType t = peek().type;

    switch (t) {
    case TokenType::KW_IF:     return parseIf();     // 处理“要是”
    case TokenType::KW_FROM:   return parseFor();    // 处理“从”
    case TokenType::KW_SAY:    return parseSay();    // 处理“唠唠”
    case TokenType::IDENTIFIER: return parseVarOrAssign(); // 处理“变量/赋值”
    case TokenType::KW_PERIOD:
        consume(); // 孤零零的句号直接吃掉
        return nullptr;
    default:
        return nullptr;
    }
}
std::unique_ptr<Node> MiniParser::parseIf() {
    consume(); // 要是
    auto cond = parseExpression(0);
    consume(); // 那么

    std::vector<std::unique_ptr<Node>> then_body;
    // 简单实现：读到“否则”或“。”为止
    while (peek().type != TokenType::KW_ELSE && peek().type != TokenType::KW_PERIOD) {
        then_body.push_back(parseStatement());
    }

    std::vector<std::unique_ptr<Node>> else_body;
    if (peek().type == TokenType::KW_ELSE) {
        consume(); // 否则
        while (peek().type != TokenType::KW_PERIOD) {
            else_body.push_back(parseStatement());
        }
    }
    consume(); // 。
    return std::make_unique<IfNode>(std::move(cond), std::move(then_body), std::move(else_body));
}
std::unique_ptr<Node> MiniParser::parseFor() {
    consume(); // 从
    std::string var = consume().value; // 循环变量名
    consume(); // 读掉可能存在的“装”或者直接解析起始值
    auto start_val = parseExpression(0);
    consume(); // 到
    auto end_val = parseExpression(0);
    consume(); // 磨叽

    std::vector<std::unique_ptr<Node>> body;
    while (peek().type != TokenType::KW_FOR_END) {
        body.push_back(parseStatement());
    }
    consume(); // 磨叽完了
    consume(); // 。
    return std::make_unique<ForNode>(var, std::move(start_val), std::move(end_val), std::move(body));
}