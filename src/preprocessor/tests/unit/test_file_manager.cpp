// Testes unitários para FileManager
// Validação completa das funcionalidades do gerenciador de arquivos

#include "../../include/file_manager.hpp"
#include "../../include/preprocessor_logger.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>

using namespace Preprocessor;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES DE TESTE
// ============================================================================

void testConstructors();
void testBasicFileOperations();
void testIncludeResolution();
void testSearchPathManagement();
void testCircularInclusionDetection();
void testCacheSystem();
void testFileInformation();
void testDependencyManagement();
void testStatistics();
void testComplexScenarios();
void testBackupAndRestore();
void testFileLocking();
void testCompression();
void testIntegrityCheck();
void testFileMonitoring();
void cleanupTestEnvironment();

// ============================================================================
// FUNÇÕES AUXILIARES DE TESTE
// ============================================================================

void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
    if (expected == actual) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        std::cout << "   Esperado: '" << expected << "'" << std::endl;
        std::cout << "   Atual: '" << actual << "'" << std::endl;
        assert(false);
    }
}

void assertTrue(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        assert(false);
    }
}

void assertFalse(bool condition, const std::string& testName) {
    if (!condition) {
        std::cout << "✅ " << testName << " - PASSOU" << std::endl;
    } else {
        std::cout << "❌ " << testName << " - FALHOU" << std::endl;
        assert(false);
    }
}

// ============================================================================
// SETUP E CLEANUP DE TESTES
// ============================================================================

void setupTestEnvironment() {
    // Cria diretório de teste
    mkdir("./test_files", 0755);
    mkdir("./test_files/subdir", 0755);
    
    // Cria arquivos de teste
    std::ofstream test1("./test_files/test1.h");
    test1 << "#ifndef TEST1_H\n#define TEST1_H\n\nint test_var = 42;\n\n#endif\n";
    test1.close();
    
    std::ofstream test2("./test_files/test2.c");
    test2 << "#include \"test1.h\"\n\nint main() { return test_var; }\n";
    test2.close();
    
    std::ofstream subtest("./test_files/subdir/subtest.h");
    subtest << "#define SUBTEST_MACRO 123\n";
    subtest.close();
}

void cleanupTestEnvironment() {
    // Remove arquivos de teste
    std::remove("./test_files/test1.h");
    std::remove("./test_files/test2.c");
    std::remove("./test_files/subdir/subtest.h");
    std::remove("./test_files/created_file.txt");
    
    // Remove diretórios
    rmdir("./test_files/subdir");
    rmdir("./test_files");
}

// ============================================================================
// TESTES DE CONSTRUTORES E CONFIGURAÇÃO
// ============================================================================

void testConstructors() {
    std::cout << "\n=== Testando Construtores ===" << std::endl;
    
    // Teste construtor padrão
    std::vector<std::string> search_paths = {"./test_files", "/usr/include"};
    PreprocessorLogger logger;
    
    FileManager fm(search_paths, &logger);
    
    std::vector<std::string> paths = fm.getSearchPaths();
    assertTrue(paths.size() == 2, "Construtor com caminhos de busca");
    
    // Teste construtor de movimento
    FileManager fm2 = std::move(fm);
    std::vector<std::string> paths2 = fm2.getSearchPaths();
    assertTrue(paths2.size() == 2, "Construtor de movimento");
}

// ============================================================================
// TESTES DE OPERAÇÕES BÁSICAS DE ARQUIVO
// ============================================================================

void testBasicFileOperations() {
    std::cout << "\n=== Testando Operações Básicas de Arquivo ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Teste fileExists
    assertTrue(fm.fileExists("./test_files/test1.h"), "Arquivo existente detectado");
    assertFalse(fm.fileExists("./test_files/nonexistent.h"), "Arquivo inexistente detectado");
    
    // Teste readFile
    std::string content = fm.readFile("./test_files/test1.h");
    assertTrue(!content.empty(), "Leitura de arquivo");
    assertTrue(content.find("#ifndef TEST1_H") != std::string::npos, "Conteúdo correto lido");
    
    // Teste writeFile
    std::string test_content = "// Arquivo de teste criado\nint test = 1;\n";
    assertTrue(fm.writeFile("./test_files/created_file.txt", test_content), "Escrita de arquivo");
    assertTrue(fm.fileExists("./test_files/created_file.txt"), "Arquivo criado existe");
    
    std::string read_content = fm.readFile("./test_files/created_file.txt");
    assertEqual(test_content, read_content, "Conteúdo escrito e lido");
}

// ============================================================================
// TESTES DE RESOLUÇÃO DE INCLUSÕES
// ============================================================================

void testIncludeResolution() {
    std::cout << "\n=== Testando Resolução de Inclusões ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files", "./test_files/subdir"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Teste resolução de include local
    std::string resolved = fm.resolveInclude("test1.h", false, "./test_files/test2.c");
    assertTrue(!resolved.empty(), "Resolução de include local");
    assertTrue(resolved.find("test1.h") != std::string::npos, "Caminho resolvido correto");
    
    // Teste resolução de include de sistema
    std::string resolved_sys = fm.resolveInclude("subtest.h", true, "");
    assertTrue(!resolved_sys.empty(), "Resolução de include de sistema");
    
    // Teste include inexistente
    try {
        fm.resolveInclude("nonexistent.h", false, "");
        assertTrue(false, "Exceção esperada para include inexistente");
    } catch (const std::runtime_error&) {
        assertTrue(true, "Exceção lançada para include inexistente");
    }
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE CAMINHOS DE BUSCA
// ============================================================================

void testSearchPathManagement() {
    std::cout << "\n=== Testando Gerenciamento de Caminhos de Busca ===" << std::endl;
    
    std::vector<std::string> initial_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(initial_paths, &logger);
    
    // Teste addSearchPath
    fm.addSearchPath("./test_files/subdir");
    std::vector<std::string> paths = fm.getSearchPaths();
    assertTrue(paths.size() == 2, "Caminho adicionado");
    
    // Teste setSearchPaths
    std::vector<std::string> new_paths = {"/usr/include", "/usr/local/include"};
    fm.setSearchPaths(new_paths);
    paths = fm.getSearchPaths();
    assertTrue(paths.size() == 2, "Caminhos redefinidos");
    
    // Teste caminho duplicado
    fm.addSearchPath("/usr/include");
    paths = fm.getSearchPaths();
    assertTrue(paths.size() == 2, "Caminho duplicado não adicionado");
}

// ============================================================================
// TESTES DE DETECÇÃO DE INCLUSÕES CIRCULARES
// ============================================================================

void testCircularInclusionDetection() {
    std::cout << "\n=== Testando Detecção de Inclusões Circulares ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Teste sem inclusão circular
    std::vector<std::string> include_stack1 = {"file1.h", "file2.h"};
    assertFalse(fm.checkCircularInclusion("file3.h", include_stack1), "Sem inclusão circular");
    
    // Teste com inclusão circular
    std::vector<std::string> include_stack2 = {"file1.h", "file2.h", "file3.h"};
    assertTrue(fm.checkCircularInclusion("file1.h", include_stack2), "Inclusão circular detectada");
    
    // Teste com pilha vazia
    std::vector<std::string> empty_stack;
    assertFalse(fm.checkCircularInclusion("file1.h", empty_stack), "Pilha vazia sem circular");
}

// ============================================================================
// TESTES DE SISTEMA DE CACHE
// ============================================================================

void testCacheSystem() {
    std::cout << "\n=== Testando Sistema de Cache ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Primeira leitura (cache miss)
    std::string content1 = fm.readFile("./test_files/test1.h");
    FileStats stats1 = fm.getStatistics();
    assertTrue(stats1.cache_misses > 0, "Cache miss na primeira leitura");
    
    // Segunda leitura (cache hit)
    std::string content2 = fm.readFile("./test_files/test1.h");
    FileStats stats2 = fm.getStatistics();
    assertTrue(stats2.cache_hits > stats1.cache_hits, "Cache hit na segunda leitura");
    
    assertEqual(content1, content2, "Conteúdo idêntico do cache");
    
    // Teste clearCache
    fm.clearCache();
    std::string content3 = fm.readFile("./test_files/test1.h");
    FileStats stats3 = fm.getStatistics();
    assertTrue(stats3.cache_misses > stats2.cache_misses, "Cache miss após limpeza");
}

// ============================================================================
// TESTES DE INFORMAÇÕES DE ARQUIVO
// ============================================================================

void testFileInformation() {
    std::cout << "\n=== Testando Informações de Arquivo ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Teste getFileSize
    size_t size = fm.getFileSize("./test_files/test1.h");
    assertTrue(size > 0, "Tamanho de arquivo válido");
    
    size_t invalid_size = fm.getFileSize("./test_files/nonexistent.h");
    assertTrue(invalid_size == 0, "Tamanho zero para arquivo inexistente");
    
    // Teste getLastModified
    auto modified = fm.getLastModified("./test_files/test1.h");
    assertTrue(modified != std::chrono::system_clock::time_point{}, "Data de modificação válida");
    
    auto invalid_modified = fm.getLastModified("./test_files/nonexistent.h");
    assertTrue(invalid_modified == std::chrono::system_clock::time_point{}, "Data inválida para arquivo inexistente");
}

// ============================================================================
// TESTES DE GERENCIAMENTO DE DEPENDÊNCIAS
// ============================================================================

void testDependencyManagement() {
    std::cout << "\n=== Testando Gerenciamento de Dependências ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Lê alguns arquivos para criar dependências
    fm.readFile("./test_files/test1.h");
    fm.readFile("./test_files/test2.c");
    
    // Teste getDependencies
    std::vector<std::string> deps = fm.getDependencies();
    assertTrue(deps.size() >= 2, "Dependências registradas");
    
    // Verifica se os arquivos estão nas dependências
    bool found_test1 = false, found_test2 = false;
    for (const auto& dep : deps) {
        if (dep.find("test1.h") != std::string::npos) found_test1 = true;
        if (dep.find("test2.c") != std::string::npos) found_test2 = true;
    }
    
    assertTrue(found_test1, "test1.h nas dependências");
    assertTrue(found_test2, "test2.c nas dependências");
}

// ============================================================================
// TESTES DE ESTATÍSTICAS
// ============================================================================

void testStatistics() {
    std::cout << "\n=== Testando Estatísticas ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Estado inicial
    FileStats initial_stats = fm.getStatistics();
    assertTrue(initial_stats.files_read == 0, "Estatísticas iniciais zeradas");
    
    // Após operações
    fm.readFile("./test_files/test1.h");
    fm.resolveInclude("test1.h", false, "./test_files/test2.c");
    
    FileStats final_stats = fm.getStatistics();
    assertTrue(final_stats.files_read > initial_stats.files_read, "Arquivos lidos incrementados");
    assertTrue(final_stats.path_resolutions > initial_stats.path_resolutions, "Resoluções incrementadas");
    
    // Teste resetStatistics
    fm.resetStatistics();
    FileStats reset_stats = fm.getStatistics();
    assertTrue(reset_stats.files_read == 0, "Estatísticas resetadas");
}

// ============================================================================
// TESTES DE CENÁRIOS COMPLEXOS
// ============================================================================

void testComplexScenarios() {
    std::cout << "\n=== Testando Cenários Complexos ===" << std::endl;
    
    std::vector<std::string> search_paths = {"./test_files", "./test_files/subdir"};
    PreprocessorLogger logger;
    FileManager fm(search_paths, &logger);
    
    // Cenário: Múltiplas inclusões com cache
    for (int i = 0; i < 3; i++) {
        std::string content = fm.readFile("./test_files/test1.h");
        assertTrue(!content.empty(), "Leitura múltipla " + std::to_string(i+1));
    }
    
    FileStats stats = fm.getStatistics();
    assertTrue(stats.cache_hits >= 2, "Cache hits em leituras múltiplas");
    
    // Cenário: Resolução com diferentes tipos de include
    std::string local_include = fm.resolveInclude("test1.h", false, "./test_files/test2.c");
    std::string system_include = fm.resolveInclude("subtest.h", true, "");
    
    assertTrue(!local_include.empty(), "Include local resolvido");
    assertTrue(!system_include.empty(), "Include de sistema resolvido");
    
    // Cenário: Operações com caminhos inválidos
    assertFalse(fm.fileExists(""), "Caminho vazio não existe");
    assertTrue(fm.getFileSize("") == 0, "Tamanho zero para caminho vazio");
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE TESTES
// ============================================================================

int main() {
    std::cout << "Executando Testes do FileManager..." << std::endl;
    
    setupTestEnvironment();
    
    try {
        testConstructors();
        testBasicFileOperations();
        testIncludeResolution();
        testSearchPathManagement();
        testCircularInclusionDetection();
        testCacheSystem();
        testFileInformation();
        testDependencyManagement();
        testStatistics();
        testComplexScenarios();
        
        // Testes dos novos métodos da fase 2.2
        testBackupAndRestore();
        testFileLocking();
        testCompression();
        testIntegrityCheck();
        testFileMonitoring();
        
        std::cout << "🎉 Todos os testes do FileManager passaram com sucesso!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n💥 Erro durante os testes: " << e.what() << std::endl;
        cleanupTestEnvironment();
        return 1;
    }
    
    cleanupTestEnvironment();
    return 0;
}

// ============================================================================
// TESTES DOS NOVOS MÉTODOS DA FASE 2.2
// ============================================================================

void testBackupAndRestore() {
    std::cout << "\n=== Testando Backup e Restauração ===" << std::endl;
    
    FileManager fm;
    
    // Cria arquivo de teste
    std::string test_file = "./test_files/backup_test.txt";
    std::string original_content = "Conteúdo original para teste de backup";
    
    assert(fm.writeFile(test_file, original_content));
    std::cout << "✅ Arquivo de teste criado - PASSOU" << std::endl;
    
    // Testa criação de backup
    assert(fm.createBackup(test_file));
    std::cout << "✅ Backup criado - PASSOU" << std::endl;
    
    // Verifica se arquivo de backup existe
    assert(fm.fileExists(test_file + ".bak"));
    std::cout << "✅ Arquivo de backup existe - PASSOU" << std::endl;
    
    // Modifica arquivo original
    std::string modified_content = "Conteúdo modificado";
    assert(fm.writeFile(test_file, modified_content));
    
    // Verifica modificação
    assert(fm.readFile(test_file) == modified_content);
    std::cout << "✅ Arquivo modificado - PASSOU" << std::endl;
    
    // Restaura do backup
    assert(fm.restoreFromBackup(test_file));
    std::cout << "✅ Restauração do backup - PASSOU" << std::endl;
    
    // Verifica restauração
    assert(fm.readFile(test_file) == original_content);
    std::cout << "✅ Conteúdo restaurado corretamente - PASSOU" << std::endl;
    
    // Limpeza
    std::remove(test_file.c_str());
    std::remove((test_file + ".bak").c_str());
}

void testFileLocking() {
    std::cout << "\n=== Testando Controle de Acesso ===" << std::endl;
    
    FileManager fm;
    
    // Cria arquivo de teste
    std::string test_file = "./test_files/lock_test.txt";
    assert(fm.writeFile(test_file, "Teste de bloqueio"));
    
    // Testa bloqueio
    assert(fm.lockFile(test_file));
    std::cout << "✅ Arquivo bloqueado - PASSOU" << std::endl;
    
    // Tenta bloquear novamente (deve falhar)
    assert(!fm.lockFile(test_file));
    std::cout << "✅ Bloqueio duplo rejeitado - PASSOU" << std::endl;
    
    // Desbloqueia
    assert(fm.unlockFile(test_file));
    std::cout << "✅ Arquivo desbloqueado - PASSOU" << std::endl;
    
    // Tenta desbloquear novamente (deve falhar)
    assert(!fm.unlockFile(test_file));
    std::cout << "✅ Desbloqueio duplo rejeitado - PASSOU" << std::endl;
    
    // Limpeza
    std::remove(test_file.c_str());
}

void testCompression() {
    std::cout << "\n=== Testando Compressão ===" << std::endl;
    
    FileManager fm;
    
    // Cria arquivo de teste
    std::string test_file = "./test_files/compress_test.txt";
    std::string original_content = "Este é um arquivo para teste de compressão com conteúdo repetitivo";
    
    assert(fm.writeFile(test_file, original_content));
    
    // Testa compressão
    assert(fm.compressFile(test_file));
    std::cout << "✅ Arquivo comprimido - PASSOU" << std::endl;
    
    // Verifica se arquivo comprimido existe
    std::string compressed_file = test_file + ".gz";
    assert(fm.fileExists(compressed_file));
    std::cout << "✅ Arquivo comprimido existe - PASSOU" << std::endl;
    
    // Testa descompressão
    std::string decompressed_file = "./test_files/decompress_test.txt";
    assert(fm.decompressFile(compressed_file, decompressed_file));
    std::cout << "✅ Arquivo descomprimido - PASSOU" << std::endl;
    
    // Verifica conteúdo descomprimido
    assert(fm.readFile(decompressed_file) == original_content);
    std::cout << "✅ Conteúdo descomprimido correto - PASSOU" << std::endl;
    
    // Limpeza
    std::remove(test_file.c_str());
    std::remove(compressed_file.c_str());
    std::remove(decompressed_file.c_str());
}

void testIntegrityCheck() {
    std::cout << "\n=== Testando Verificação de Integridade ===" << std::endl;
    
    FileManager fm;
    
    // Cria arquivo de teste
    std::string test_file = "./test_files/integrity_test.txt";
    std::string content = "Conteúdo para teste de integridade";
    
    assert(fm.writeFile(test_file, content));
    
    // Calcula hash
    std::string hash1 = fm.calculateFileHash(test_file);
    assert(!hash1.empty());
    std::cout << "✅ Hash calculado - PASSOU" << std::endl;
    
    // Verifica integridade
    assert(fm.verifyFileIntegrity(test_file, hash1));
    std::cout << "✅ Integridade verificada - PASSOU" << std::endl;
    
    // Calcula hash novamente (deve usar cache)
    std::string hash2 = fm.calculateFileHash(test_file);
    assert(hash1 == hash2);
    std::cout << "✅ Hash do cache - PASSOU" << std::endl;
    
    // Modifica arquivo
    assert(fm.writeFile(test_file, content + " modificado"));
    
    // Verifica que integridade falha
    assert(!fm.verifyFileIntegrity(test_file, hash1));
    std::cout << "✅ Integridade falha detectada - PASSOU" << std::endl;
    
    // Limpeza
    std::remove(test_file.c_str());
}

void testFileMonitoring() {
    std::cout << "\n=== Testando Monitoramento ===" << std::endl;
    
    FileManager fm;
    
    // Cria arquivo de teste
    std::string test_file = "./test_files/monitor_test.txt";
    assert(fm.writeFile(test_file, "Arquivo para monitoramento"));
    
    // Inicia monitoramento
    assert(fm.monitorFileChanges(test_file));
    std::cout << "✅ Monitoramento iniciado - PASSOU" << std::endl;
    
    // Tenta monitorar novamente (deve retornar true mas avisar)
    assert(fm.monitorFileChanges(test_file));
    std::cout << "✅ Monitoramento duplicado tratado - PASSOU" << std::endl;
    
    // Testa eventos do sistema de arquivos
    fm.handleFileSystemEvents("MODIFIED", test_file);
    std::cout << "✅ Evento MODIFIED processado - PASSOU" << std::endl;
    
    fm.handleFileSystemEvents("CREATED", "./test_files/new_file.txt");
    std::cout << "✅ Evento CREATED processado - PASSOU" << std::endl;
    
    fm.handleFileSystemEvents("DELETED", test_file);
    std::cout << "✅ Evento DELETED processado - PASSOU" << std::endl;
    
    // Limpeza
    std::remove(test_file.c_str());
}