/*
 * Teste de funcionalidades avançadas de preprocessamento
 * Macros complexas, condicionais aninhadas e casos sofisticados
 */

#include <stdio.h>
#include <stdlib.h>

// Macros com múltiplos níveis de expansão
#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)
#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_STRING EXPAND_AND_STRINGIFY(VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH)

// Macro que gera outras macros
#define DECLARE_GETTER_SETTER(type, name) \
    type get_##name(void) { return name; } \
    void set_##name(type value) { name = value; }

// Macro com concatenação complexa
#define CONCAT3(a, b, c) a##b##c
#define MAKE_FUNCTION_NAME(prefix, type, suffix) CONCAT3(prefix, type, suffix)

// Macro recursiva controlada
#define REPEAT_1(x) x
#define REPEAT_2(x) REPEAT_1(x) REPEAT_1(x)
#define REPEAT_4(x) REPEAT_2(x) REPEAT_2(x)
#define REPEAT_8(x) REPEAT_4(x) REPEAT_4(x)

// Macro com argumentos variáveis complexa
#define DEBUG_LEVEL 2
#define LOG(level, fmt, ...) \
    do { \
        if (DEBUG_LEVEL >= level) { \
            printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while(0)

// Macro para criar estruturas automaticamente
#define DEFINE_POINT_TYPE(suffix, type) \
    typedef struct { \
        type x, y; \
        char name[32]; \
    } Point##suffix; \
    \
    void init_##suffix(Point##suffix *p, type x, type y, const char *name) { \
        p->x = x; \
        p->y = y; \
        snprintf(p->name, sizeof(p->name), "%s", name); \
    } \
    \
    void print_##suffix(const Point##suffix *p) { \
        printf(#suffix " Point %s: (" #type ", " #type ")\n", p->name, p->x, p->y); \
    }

// Condicionais aninhadas complexas
#ifdef DEBUG
    #define DEBUG_MODE 1
    #ifdef VERBOSE_DEBUG
        #define LOG_LEVEL 3
        #define TRACE(msg) LOG(3, "TRACE: %s", msg)
    #else
        #define LOG_LEVEL 2
        #define TRACE(msg) /* No trace in non-verbose mode */
    #endif
    
    #if LOG_LEVEL >= 2
        #define INFO(msg) LOG(2, "INFO: %s", msg)
    #else
        #define INFO(msg)
    #endif
    
    #if LOG_LEVEL >= 1
        #define ERROR(msg) LOG(1, "ERROR: %s", msg)
    #else
        #define ERROR(msg) printf("ERROR: %s\n", msg)
    #endif
#else
    #define DEBUG_MODE 0
    #define LOG_LEVEL 0
    #define TRACE(msg)
    #define INFO(msg)
    #define ERROR(msg) printf("ERROR: %s\n", msg)
#endif

// Macro com verificação de compilador
#if defined(__GNUC__)
    #define COMPILER_SPECIFIC __attribute__((unused))
    #define LIKELY(x) __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
    #define COMPILER_SPECIFIC __pragma(warning(suppress: 4100))
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#else
    #define COMPILER_SPECIFIC
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#endif

// Macro para detecção de features do C
#if __STDC_VERSION__ >= 199901L
    #define HAS_C99 1
    #define INLINE inline
    #define RESTRICT restrict
#else
    #define HAS_C99 0
    #define INLINE
    #define RESTRICT
#endif

#if __STDC_VERSION__ >= 201112L
    #define HAS_C11 1
    #include <stdalign.h>
    #define ALIGNAS(x) _Alignas(x)
#else
    #define HAS_C11 0
    #define ALIGNAS(x)
#endif

// Variáveis globais para teste
static int global_counter = 0;
static float temperature = 25.5f;

// Gerar getters e setters usando macro
DECLARE_GETTER_SETTER(int, global_counter)
DECLARE_GETTER_SETTER(float, temperature)

// Gerar tipos de ponto usando macro
DEFINE_POINT_TYPE(Int, int)
DEFINE_POINT_TYPE(Float, float)
DEFINE_POINT_TYPE(Double, double)

// Função com nome gerado por macro
INLINE int MAKE_FUNCTION_NAME(calc, sum, total)(int a, int b) {
    return a + b;
}

int main(void)
{
    printf("=== Advanced Preprocessor Test ===\n");
    printf("Version: %s\n", VERSION_STRING);
    printf("Debug Mode: %s\n", DEBUG_MODE ? "ON" : "OFF");
    printf("C99 Support: %s\n", HAS_C99 ? "YES" : "NO");
    printf("C11 Support: %s\n", HAS_C11 ? "YES" : "NO");
    
    // Teste de logging
    LOG(1, "Starting application");
    INFO("Application initialized");
    TRACE("Entering main function");
    
    // Teste de repetição de macro
    printf("Repeated stars: ");
    REPEAT_8(printf("*");)
    printf("\n");
    
    // Teste de getters e setters gerados
    set_global_counter(42);
    set_temperature(98.6f);
    
    printf("Counter: %d\n", get_global_counter());
    printf("Temperature: %.1f\n", get_temperature());
    
    // Teste de tipos de ponto gerados
    PointInt int_point;
    PointFloat float_point;
    PointDouble double_point;
    
    init_Int(&int_point, 10, 20, "IntegerPoint");
    init_Float(&float_point, 1.5f, 2.5f, "FloatPoint");
    init_Double(&double_point, 3.14159, 2.71828, "DoublePoint");
    
    print_Int(&int_point);
    print_Float(&float_point);
    print_Double(&double_point);
    
    // Teste de função com nome gerado
    int sum = calcsumtotal(15, 25);
    printf("Generated function result: %d\n", sum);
    
    // Teste de condicionais com likely/unlikely
    int random_value = rand() % 100;
    
    if (LIKELY(random_value < 90)) {
        INFO("Common case occurred");
    } else if (UNLIKELY(random_value >= 95)) {
        INFO("Rare case occurred");
    }
    
    // Teste de alinhamento (se C11 disponível)
    #if HAS_C11
    ALIGNAS(16) char aligned_buffer[64];
    printf("Aligned buffer created (C11 feature)\n");
    #endif
    
    TRACE("Exiting main function");
    LOG(1, "Application finished successfully");
    
    return 0;
}

// Macro para cleanup automático
#define AUTO_CLEANUP(type, var, cleanup_func) \
    type var __attribute__((cleanup(cleanup_func)))

// Função de cleanup para demonstração
void cleanup_int(int *ptr) {
    printf("Cleaning up integer: %d\n", *ptr);
}

// Função que demonstra cleanup automático (GCC specific)
void demonstrate_cleanup(void) {
    #ifdef __GNUC__
    AUTO_CLEANUP(int, auto_var, cleanup_int) = 100;
    printf("Auto cleanup variable: %d\n", auto_var);
    // cleanup_int será chamada automaticamente ao sair do escopo
    #endif
}