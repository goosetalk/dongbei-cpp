#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>

enum class TokenType {
    IDENTIFIER,     // 老王
    NUMBER,         // 250
    KW_BE,          // 是
    TYPE_INT,       // [规整]
    KW_IS_VAR,      // 活雷锋
    KW_BECOME,      // 装
    KW_PLUS,        // 加
    KW_MINUS,       // 减
    KW_TIMES,       // 乘
    KW_IF,
    KW_THEN,
    KW_ELSE,
    KW_FROM,
    KW_TO,
    KW_FOR,
    KW_FOR_END,
    KW_EQ,
    KW_DIVIDE_BY,  // 除以
    KW_SAY,         // 唠唠
    KW_COLON,       // ：
    KW_PERIOD,      // 。
    END_OF_FILE
};

inline std::vector<std::pair<std::string, TokenType>> dongbei_keywords = {
    {"一样一样的", TokenType::KW_EQ},
    {"磨叽完了",  TokenType::KW_FOR_END},
{"[规整]", TokenType::TYPE_INT},
    {"活雷锋", TokenType::KW_IS_VAR},
    {"唠唠",   TokenType::KW_SAY},
    {"嘀咕",   TokenType::KW_SAY}, // 对应你代码里的 stderr 打印
    {"要是",     TokenType::KW_IF},
    {"磨叽",     TokenType::KW_FOR},

{"那么",     TokenType::KW_THEN},
{"否则",     TokenType::KW_ELSE},
    {"除以",   TokenType::KW_DIVIDE_BY},
    {"是",     TokenType::KW_BE},
    {"装",     TokenType::KW_BECOME},
    {"加",     TokenType::KW_PLUS},
    {"减",     TokenType::KW_MINUS},
    {"乘",     TokenType::KW_TIMES},
    {"加",     TokenType::KW_PLUS},
    {"从",       TokenType::KW_FROM},
{"到",       TokenType::KW_TO},
    {"。",     TokenType::KW_PERIOD},
    {"：",     TokenType::KW_COLON}
};