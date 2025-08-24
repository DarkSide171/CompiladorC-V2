// Testes de Integração - Amostras de Código C
// Testes com exemplos reais de código C

#include "../../include/lexer.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace Lexer;

// Função auxiliar para criar arquivo temporário
void createTempFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

// Função auxiliar para contar tokens válidos
int countValidTokens(const std::string& filename) {
    ErrorHandler errorHandler(100);
    LexerMain lexer(filename, &errorHandler);
    
    int count = 0;
    while (lexer.hasMoreTokens()) {
        Token token = lexer.nextToken();
        if (token.getType() != TokenType::END_OF_FILE) {
            count++;
        } else {
            break;
        }
    }
    return count;
}

// Função auxiliar para verificar se processamento é bem-sucedido
bool processSuccessfully(const std::string& filename) {
    ErrorHandler errorHandler(100);
    LexerMain lexer(filename, &errorHandler);
    
    while (lexer.hasMoreTokens()) {
        Token token = lexer.nextToken();
        if (token.getType() == TokenType::UNKNOWN) {
            return false;
        }
        if (token.getType() == TokenType::END_OF_FILE) {
            break;
        }
    }
    return !errorHandler.hasErrors();
}

// Teste 1: Programa "Hello World"
void testHelloWorld() {
    std::cout << "Testando programa Hello World..." << std::endl;
    
    std::string helloWorldCode = 
        "#include <stdio.h>\n"
        "\n"
        "int main() {\n"
        "    printf(\"Hello, World!\\n\");\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_hello.c", helloWorldCode);
    
    assert(processSuccessfully("test_hello.c"));
    assert(countValidTokens("test_hello.c") > 10);
    
    std::cout << "✓ Teste Hello World passou!" << std::endl;
}

// Teste 2: Declarações de variáveis
void testVariableDeclarations() {
    std::cout << "Testando declarações de variáveis..." << std::endl;
    
    std::string varCode = 
        "int x = 10;\n"
        "float y = 3.14f;\n"
        "double z = 2.718;\n"
        "char c = 'A';\n"
        "char* str = \"Hello\";\n"
        "const int MAX = 100;\n"
        "static long counter = 0L;\n"
        "unsigned short port = 8080U;\n";
    
    createTempFile("test_vars.c", varCode);
    
    assert(processSuccessfully("test_vars.c"));
    assert(countValidTokens("test_vars.c") > 30);
    
    std::cout << "✓ Teste de declarações de variáveis passou!" << std::endl;
}

// Teste 3: Definições de funções
void testFunctionDefinitions() {
    std::cout << "Testando definições de funções..." << std::endl;
    
    std::string funcCode = 
        "int add(int a, int b) {\n"
        "    return a + b;\n"
        "}\n"
        "\n"
        "void printMessage(const char* msg) {\n"
        "    printf(\"%s\\n\", msg);\n"
        "}\n";
    
    createTempFile("test_funcs.c", funcCode);
    
    assert(processSuccessfully("test_funcs.c"));
    assert(countValidTokens("test_funcs.c") > 20);
    
    std::cout << "✓ Teste de definições de funções passou!" << std::endl;
}

// Teste 4: Estruturas de controle
void testControlStructures() {
    std::cout << "Testando estruturas de controle..." << std::endl;
    
    std::string controlCode = 
        "int main() {\n"
        "    int i, n = 10;\n"
        "    if (n > 0) {\n"
        "        printf(\"Positive\");\n"
        "    } else {\n"
        "        printf(\"Zero or negative\");\n"
        "    }\n"
        "    for (i = 0; i < n; i++) {\n"
        "        printf(\"%d \", i);\n"
        "    }\n"
        "    while (i > 0) {\n"
        "        i--;\n"
        "    }\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_control.c", controlCode);
    
    assert(processSuccessfully("test_control.c"));
    assert(countValidTokens("test_control.c") > 40);
    
    std::cout << "✓ Teste de estruturas de controle passou!" << std::endl;
}

// Teste 5: Diretivas de pré-processador
void testPreprocessorDirectives() {
    std::cout << "Testando diretivas de pré-processador..." << std::endl;
    
    std::string preprocCode = 
        "#include <stdio.h>\n"
        "#define MAX_SIZE 1024\n"
        "#define MIN(a,b) ((a)<(b)?(a):(b))\n"
        "#ifdef DEBUG\n"
        "    #define LOG(x) printf(x)\n"
        "#else\n"
        "    #define LOG(x)\n"
        "#endif\n";
    
    createTempFile("test_preproc.c", preprocCode);
    
    assert(processSuccessfully("test_preproc.c"));
    assert(countValidTokens("test_preproc.c") > 15);
    
    std::cout << "✓ Teste de diretivas de pré-processador passou!" << std::endl;
}

// Teste 6: Expressões complexas
void testComplexExpressions() {
    std::cout << "Testando expressões complexas..." << std::endl;
    
    std::string exprCode = 
        "int main() {\n"
        "    int a = 5, b = 3, c = 2;\n"
        "    int result;\n"
        "    result = a + b * c - (a / b) % c;\n"
        "    result = ++a + b-- - --c;\n"
        "    int flag = (a > b) && (b < c) || !(a == c);\n"
        "    result = a & b | c ^ (~a << 2) >> 1;\n"
        "    a += b;\n"
        "    b -= c;\n"
        "    c *= a;\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_expr.c", exprCode);
    
    assert(processSuccessfully("test_expr.c"));
    assert(countValidTokens("test_expr.c") > 50);
    
    std::cout << "✓ Teste de expressões complexas passou!" << std::endl;
}

// Teste 7: Operações com ponteiros
void testPointerOperations() {
    std::cout << "Testando operações com ponteiros..." << std::endl;
    
    std::string ptrCode = 
        "int main() {\n"
        "    int x = 10;\n"
        "    int* ptr = &x;\n"
        "    int** pptr = &ptr;\n"
        "    int value = *ptr;\n"
        "    *ptr = 20;\n"
        "    ptr++;\n"
        "    ptr--;\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_ptr.c", ptrCode);
    
    assert(processSuccessfully("test_ptr.c"));
    assert(countValidTokens("test_ptr.c") > 25);
    
    std::cout << "✓ Teste de operações com ponteiros passou!" << std::endl;
}

// Teste 8: Estruturas, uniões e enums
void testStructUnionEnum() {
    std::cout << "Testando estruturas, uniões e enums..." << std::endl;
    
    std::string structCode = 
        "struct Point {\n"
        "    int x, y;\n"
        "};\n"
        "\n"
        "union Data {\n"
        "    int i;\n"
        "    float f;\n"
        "    char c;\n"
        "};\n"
        "\n"
        "enum Color {\n"
        "    RED, GREEN, BLUE\n"
        "};\n";
    
    createTempFile("test_struct.c", structCode);
    
    assert(processSuccessfully("test_struct.c"));
    assert(countValidTokens("test_struct.c") > 20);
    
    std::cout << "✓ Teste de estruturas, uniões e enums passou!" << std::endl;
}

// Teste 9: Operações com arrays
void testArrayOperations() {
    std::cout << "Testando operações com arrays..." << std::endl;
    
    std::string arrayCode = 
        "int main() {\n"
        "    int arr[10];\n"
        "    int matrix[3][3];\n"
        "    arr[0] = 1;\n"
        "    arr[1] = arr[0] + 1;\n"
        "    matrix[0][0] = 5;\n"
        "    int* ptr = arr;\n"
        "    ptr[2] = 3;\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_array.c", arrayCode);
    
    assert(processSuccessfully("test_array.c"));
    assert(countValidTokens("test_array.c") > 30);
    
    std::cout << "✓ Teste de operações com arrays passou!" << std::endl;
}

// Teste 10: Literais de string
void testStringLiterals() {
    std::cout << "Testando literais de string..." << std::endl;
    
    std::string stringCode = 
        "int main() {\n"
        "    char* str1 = \"Hello, World!\";\n"
        "    char* str2 = \"Line 1\\nLine 2\";\n"
        "    char* str3 = \"Tab\\tSeparated\";\n"
        "    char* str4 = \"Quote: \\\"Hello\\\"\";\n"
        "    char* empty = \"\";\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_strings.c", stringCode);
    
    assert(processSuccessfully("test_strings.c"));
    assert(countValidTokens("test_strings.c") > 20);
    
    std::cout << "✓ Teste de literais de string passou!" << std::endl;
}

// Teste 11: Literais numéricos
void testNumericLiterals() {
    std::cout << "Testando literais numéricos..." << std::endl;
    
    std::string numCode = 
        "int main() {\n"
        "    int decimal = 123;\n"
        "    int hex = 0x1A2B;\n"
        "    int octal = 0755;\n"
        "    float f1 = 3.14f;\n"
        "    double d1 = 2.718;\n"
        "    double d2 = 1.23e-4;\n"
        "    long l1 = 123L;\n"
        "    unsigned u1 = 456U;\n"
        "    return 0;\n"
        "}\n";
    
    createTempFile("test_numbers.c", numCode);
    
    assert(processSuccessfully("test_numbers.c"));
    assert(countValidTokens("test_numbers.c") > 25);
    
    std::cout << "✓ Teste de literais numéricos passou!" << std::endl;
}

// Teste 12: Estilos de comentários
void testCommentStyles() {
    std::cout << "Testando estilos de comentários..." << std::endl;
    
    std::string commentCode = 
        "// Comentário de linha\n"
        "int x = 10; // Comentário no final da linha\n"
        "\n"
        "/* Comentário de bloco */\n"
        "int y = /* comentário inline */ 20;\n"
        "\n"
        "/*\n"
        " * Comentário\n"
        " * multi-linha\n"
        " */\n"
        "int z = 30;\n";
    
    createTempFile("test_comments.c", commentCode);
    
    assert(processSuccessfully("test_comments.c"));
    assert(countValidTokens("test_comments.c") > 10);
    
    std::cout << "✓ Teste de estilos de comentários passou!" << std::endl;
}

// Função principal para executar todos os testes
int main() {
    std::cout << "=== Executando Testes de Integração - Amostras de Código C ===" << std::endl;
    
    try {
        testHelloWorld();
        testVariableDeclarations();
        testFunctionDefinitions();
        testControlStructures();
        testPreprocessorDirectives();
        testComplexExpressions();
        testPointerOperations();
        testStructUnionEnum();
        testArrayOperations();
        testStringLiterals();
        testNumericLiterals();
        testCommentStyles();
        
        std::cout << "\n✅ Todos os testes de amostras de código C passaram!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    }
}