#include "Parser.h"
#include "Lexer.h"

int main() {
    // 1. 东北话源码 (你可以尝试更复杂的四则运算)
    // 语义：老王是整数。老王装 (10 + 5 * 2)。唠唠老王。
    std::string source = "从 i 到 1 到 5 磨叽： 唠唠：i。 磨叽完了。";

    try {
        // 2. 词法分析 (Lexer) -> 变零件
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 3. 语法分析 (Parser) -> 种树
        MiniParser parser(tokens);

        // 调用我们新改的 parse()，它返回一棵完整的 AST 树
        std::unique_ptr<ProgramNode> programTree = parser.parseProgram();

        // 4. 代码生成 (Codegen) -> 喷代码
        if (programTree) {
            std::cout << "// === 由 dongbei-cpp 自动生成 ===\n" << std::endl;

            // 这一步会递归调用所有 Node 的 to_cpp()，最后带上 main 函数外壳
            std::string finalCppCode = programTree->to_cpp();

            std::cout << finalCppCode << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "哎呀妈呀，整岔劈了: " << e.what() << std::endl;
    }

    return 0;
}