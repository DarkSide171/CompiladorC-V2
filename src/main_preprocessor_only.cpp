#include "preprocessor/include/preprocessor.hpp"
#include "preprocessor/include/preprocessor_lexer_interface.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__APPLE__)
    #include <unistd.h>
    #include <sys/types.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <sys/types.h>
#endif

// Função para detectar caminhos de include do sistema automaticamente
std::vector<std::string> getSystemIncludePaths() {
    std::vector<std::string> paths;
    
#ifdef _WIN32
    // Windows - Visual Studio e MinGW
    const char* vcinstalldir = std::getenv("VCINSTALLDIR");
    if (vcinstalldir) {
        paths.push_back(std::string(vcinstalldir) + "include");
    }
    
    // MinGW paths
    paths.push_back("C:\\MinGW\\include");
    paths.push_back("C:\\msys64\\mingw64\\include");
    
#elif defined(__APPLE__)
    // macOS - Command Line Tools e Xcode
    paths.push_back("/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include");
    paths.push_back("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include");
    paths.push_back("/usr/local/include");
    paths.push_back("/opt/homebrew/include");
    
#elif defined(__linux__)
    // Linux - GCC paths
    paths.push_back("/usr/include");
    paths.push_back("/usr/local/include");
    paths.push_back("/usr/include/x86_64-linux-gnu");
    
    // Detectar versão do GCC dinamicamente
    const char* gcc_version = std::getenv("GCC_VERSION");
    if (gcc_version) {
        paths.push_back("/usr/lib/gcc/x86_64-linux-gnu/" + std::string(gcc_version) + "/include");
    } else {
        // Tentar versões comuns do GCC
        paths.push_back("/usr/lib/gcc/x86_64-linux-gnu/11/include");
        paths.push_back("/usr/lib/gcc/x86_64-linux-gnu/9/include");
        paths.push_back("/usr/lib/gcc/x86_64-linux-gnu/7/include");
    }
    
#else
    // Sistema desconhecido - usar caminhos genéricos
    paths.push_back("/usr/include");
    paths.push_back("/usr/local/include");
#endif
    
    return paths;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.c>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ TESTE ISOLADO DO PRÉ-PROCESSADOR                            ║" << std::endl;
    std::cout << "║ Arquivo: " << std::left << std::setw(48) << filename << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n" << std::endl;

    try {
        // Criar interface do pré-processador
        auto preprocessor_interface = std::make_unique<Preprocessor::PreprocessorLexerInterface>();
        
        std::cout << "[INFO] Inicializando pré-processador..." << std::endl;
        
        // Inicializar a interface
        Preprocessor::PreprocessorConfig config;
        if (!preprocessor_interface->initialize(config)) {
            std::cerr << "[ERRO] Falha na inicialização do pré-processador" << std::endl;
            return 1;
        }
        
        // Configurar caminhos de busca padrão do sistema
        std::cout << "[INFO] Configurando caminhos de busca padrão..." << std::endl;
        
        // Detectar e adicionar caminhos de busca baseado no sistema operacional
        std::vector<std::string> system_paths = getSystemIncludePaths();
        
        for (const auto& path : system_paths) {
            struct stat info;
            if (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
                std::cout << "[DEBUG] Adicionando caminho: " << path << std::endl;
                preprocessor_interface->addIncludePath(path);
            } else {
                std::cout << "[DEBUG] Caminho não encontrado (ignorando): " << path << std::endl;
            }
        }
        
        std::cout << "[INFO] Caminhos configurados automaticamente para o sistema atual" << std::endl;
        
        // Processar arquivo
        std::cout << "[INFO] Processando arquivo: " << filename << std::endl;
        auto result = preprocessor_interface->processFile(filename);
        
        if (result.hasErrors) {
            std::cerr << "[ERRO] Falha no processamento do arquivo" << std::endl;
            for (const auto& error : result.errorMessages) {
                std::cerr << "  - " << error << std::endl;
            }
            return 1;
        }
        
        // Mostrar avisos se houver
        if (!result.warningMessages.empty()) {
            std::cout << "\n[AVISOS]:" << std::endl;
            for (const auto& warning : result.warningMessages) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        // Obter código processado
        std::string processed_code = result.processedCode;
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "CÓDIGO PROCESSADO PELO PRÉ-PROCESSADOR:" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << processed_code << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Obter macros definidas
        std::cout << "\nMACROS DEFINIDAS:" << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        if (result.macroDefinitions.empty()) {
            std::cout << "Nenhuma macro definida." << std::endl;
        } else {
            for (const auto& [name, value] : result.macroDefinitions) {
                std::cout << "#define " << name << " " << value << std::endl;
            }
        }
        
        // Obter dependências
        std::cout << "\nDEPENDÊNCIAS ENCONTRADAS:" << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        if (result.includedFiles.empty()) {
            std::cout << "Nenhuma dependência encontrada." << std::endl;
        } else {
            for (const auto& dep : result.includedFiles) {
                std::cout << "#include " << dep << std::endl;
            }
        }
        
        std::cout << "\n[SUCESSO] Pré-processamento concluído com sucesso!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "[EXCEÇÃO] Erro durante o processamento: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[ERRO] Erro desconhecido durante o processamento" << std::endl;
        return 1;
    }
}