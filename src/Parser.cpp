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
        if (peek().type == TokenType::IDENTIFIER) {
            program->addStatement(parseVarOrAssign());
        }
        else if (peek().type == TokenType::KW_SAY) {
            program->addStatement(parseSay());
        }
        else {
            consume();
        }
    }
    return program;
}