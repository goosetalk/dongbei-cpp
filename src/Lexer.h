#pragma once
#include "Parser.h" // 确保能拿到 Token 和 TokenType
#include <iostream>
#include <algorithm>

class Lexer {
    std::string src;
    size_t pos = 0;

public:
    Lexer(std::string source) : src(source), pos(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < src.length()) {
            char c = src[pos];

            // 1. 忽略空白字符
            if (isspace(static_cast<unsigned char>(c))) {
                pos++;
                continue;
            }

            // 2. 处理数字 (NUMBER)
            if (isdigit(static_cast<unsigned char>(c))) {
                std::string num;
                while (pos < src.length() && isdigit(src[pos])) {
                    num += src[pos++];
                }
                tokens.push_back({ TokenType::NUMBER, num });
                continue;
            }

            // 3. 处理关键字和标识符 (最关键的一步)
            if (!handleKeywords(tokens)) {
                // 如果不是预定义的关键字，就当作标识符（变量名）
                handleIdentifier(tokens);
            }
        }
        tokens.push_back({ TokenType::END_OF_FILE, "" });
        return tokens;
    }

private:
    bool handleKeywords(std::vector<Token>& tokens) {
        // 按照关键字长度降序排列查找，防止“是活雷锋”被拆成“是”
        for (const auto& kv : dongbei_keywords) {
            const std::string& kw = kv.first;
            if (src.substr(pos, kw.length()) == kw) {
                tokens.push_back({ kv.second, kw });
                pos += kw.length();
                return true;
            }
        }
        return false;
    }

    void handleIdentifier(std::vector<Token>& tokens) {
        std::string id;
        while (pos < src.length()) {
            unsigned char uc = static_cast<unsigned char>(src[pos]);

            // 1. 碰到空白符，停！
            if (isspace(uc)) break;

            // 2. 偷瞄一眼，后面是不是关键字或者标点？
            // 注意：把“。”和“：”也放进 dongbei_keywords 表里统一处理
            if (isNextAKeyword()) break;

            // 3. 实在是没招了，才当成标识符读一个字节
            id += src[pos++];
        }
        if (!id.empty()) {
            tokens.push_back({ TokenType::IDENTIFIER, id });
        }
    }

    bool isNextAKeyword() {
        for (const auto& kv : dongbei_keywords) {
            if (src.substr(pos, kv.first.length()) == kv.first) return true;
        }
        return false;
    }
};