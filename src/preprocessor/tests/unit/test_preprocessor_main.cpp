#include "../../include/preprocessor.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        std::cout << "=== Teste do Processador Principal (Fase 2.4) ===\n\n";
        
        // Criar instância do preprocessor
        Preprocessor::PreprocessorMain preprocessor("");
        
        std::cout << "✓ Preprocessor criado com sucesso\n";
        
        // Teste de processamento de string simples
        std::string test_code = R"(
#define MAX_SIZE 100
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int array[MAX_SIZE];
int result = MIN(10, 20);
)";
        
        std::cout << "\n--- Código de entrada ---\n";
        std::cout << test_code << std::endl;
        
        // Processar string
        bool success = preprocessor.processString(test_code);
        
        if (success) {
            std::cout << "✓ Processamento de string bem-sucedido\n";
            
            // Obter código expandido
            std::string expanded = preprocessor.getExpandedCode();
            std::cout << "\n--- Código expandido ---\n";
            std::cout << expanded << std::endl;
            
            // Obter dependências
            auto deps = preprocessor.getDependencies();
            std::cout << "\n--- Dependências ---\n";
            if (deps.empty()) {
                std::cout << "Nenhuma dependência encontrada\n";
            } else {
                for (const auto& dep : deps) {
                    std::cout << "- " << dep << "\n";
                }
            }
            
            // Testar definição de macro
            preprocessor.defineMacro("TEST_MACRO", "42");
            std::cout << "\n--- Teste de macro ---\n";
            std::cout << "TEST_MACRO definida: " << (preprocessor.isMacroDefined("TEST_MACRO") ? "Sim" : "Não") << "\n";
            
            // Reset do preprocessor
            preprocessor.reset();
            std::cout << "\n✓ Reset do preprocessor realizado\n";
            
        } else {
            std::cout << "✗ Falha no processamento de string\n";
            return 1;
        }
        
        std::cout << "\n=== Teste concluído com sucesso! ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Erro durante o teste: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "✗ Erro desconhecido durante o teste\n";
        return 1;
    }
}