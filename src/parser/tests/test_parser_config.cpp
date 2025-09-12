#include "../include/parser_config.hpp"
#include <iostream>
#include <cassert>

namespace Parser {

void testParserConfigBasics() {
    std::cout << "Testing ParserConfig basics..." << std::endl;
    
    // Test default constructor
    ParserConfig config;
    assert(config.getCStandard() == CStandard::C17);
    assert(config.isRecoveryEnabled() == true);
    assert(config.getMaxErrors() == 100);
    assert(config.getMaxASTNodes() == 1000000);
    assert(config.getMaxParseDepth() == 1000);
    
    std::cout << "✓ Default constructor works" << std::endl;
}

void testParserConfigSetters() {
    std::cout << "Testing ParserConfig setters..." << std::endl;
    
    ParserConfig config;
    
    // Test C standard
    config.setCStandard(CStandard::C99);
    assert(config.getCStandard() == CStandard::C99);
    
    // Test recovery settings
    config.setRecoveryEnabled(false);
    assert(config.isRecoveryEnabled() == false);
    
    config.setMaxErrors(50);
    assert(config.getMaxErrors() == 50);
    
    // Test memory limits
    config.setMaxASTNodes(500000);
    assert(config.getMaxASTNodes() == 500000);
    
    config.setMaxParseDepth(500);
    assert(config.getMaxParseDepth() == 500);
    
    std::cout << "✓ Setters work correctly" << std::endl;
}

void testParserConfigValidation() {
    std::cout << "Testing ParserConfig validation..." << std::endl;
    
    ParserConfig config;
    
    // Valid configuration should pass
    assert(config.validate() == true);
    
    // Invalid configuration should fail
    config.setMaxErrors(0);
    assert(config.validate() == false);
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "✓ Validation works correctly" << std::endl;
}

void testParserConfigPresets() {
    std::cout << "Testing ParserConfig presets..." << std::endl;
    
    // Test default preset
    auto defaultConfig = ParserConfig::createDefault();
    assert(defaultConfig.getCStandard() == CStandard::C17);
    assert(defaultConfig.isRecoveryEnabled() == true);
    
    // Test strict preset
    auto strictConfig = ParserConfig::createStrict();
    assert(strictConfig.getCStandard() == CStandard::C17);
    assert(strictConfig.isStrictMode() == true);
    
    // Test permissive preset
    auto permissiveConfig = ParserConfig::createPermissive();
    assert(permissiveConfig.isGnuExtensionsEnabled() == true);
    assert(permissiveConfig.isMsExtensionsEnabled() == true);
    
    std::cout << "✓ Presets work correctly" << std::endl;
}

void runAllTests() {
    std::cout << "=== ParserConfig Tests ===" << std::endl;
    
    testParserConfigBasics();
    testParserConfigSetters();
    testParserConfigValidation();
    testParserConfigPresets();
    
    std::cout << "\n✅ All ParserConfig tests passed!" << std::endl;
}

} // namespace Parser

int main() {
    try {
        Parser::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}