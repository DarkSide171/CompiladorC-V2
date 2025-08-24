# Documentação do Compilador C

Bem-vindo à documentação completa do projeto Compilador C v2.0. Esta pasta contém toda a documentação técnica e de planejamento do projeto.

## Índice da Documentação

### 📋 Documentação Principal

1. **[README.md](../README.md)** - Visão geral do projeto
   - Descrição do projeto e objetivos
   - Roadmap de desenvolvimento
   - Instruções de compilação
   - Status atual do projeto

### 📚 Documentação Técnica Detalhada

2. **[FASES_COMPILADOR.md](./FASES_COMPILADOR.md)** - Detalhamento das fases
   - Análise Léxica (Lexer)
   - Análise Sintática (Parser)
   - Análise Semântica
   - Geração de Código Intermediário
   - Geração de Código Final
   - Estruturas de dados e algoritmos

3. **[ESCOPO_IMPLEMENTACAO.md](./ESCOPO_IMPLEMENTACAO.md)** - Escopo de funcionalidades
   - Subconjunto da linguagem C suportado
   - Funcionalidades implementadas por versão
   - Limitações técnicas
   - Casos de teste e exemplos
   - Critérios de aceitação

4. **[PROGRESSO_MARCOS.md](./PROGRESSO_MARCOS.md)** - Acompanhamento do progresso
   - Status geral do projeto com barra de progresso
   - Marcos detalhados para cada fase
   - Cronograma estimado
   - Sistema de acompanhamento
   - Métricas de qualidade

5. **[ARQUITETURA_TECNICA.md](./ARQUITETURA_TECNICA.md)** - Arquitetura do sistema
   - Design de classes e interfaces
   - Padrões de design utilizados
   - Estrutura modular
   - Pontos de extensibilidade
   - Tratamento de erros

## Como Navegar na Documentação

### Para Desenvolvedores Iniciantes
1. Comece com o [README.md](../README.md) principal
2. Leia [FASES_COMPILADOR.md](./FASES_COMPILADOR.md) para entender o processo
3. Consulte [ESCOPO_IMPLEMENTACAO.md](./ESCOPO_IMPLEMENTACAO.md) para ver o que será implementado

### Para Implementação
1. Consulte [PROGRESSO_MARCOS.md](./PROGRESSO_MARCOS.md) para ver o próximo marco
2. Use [ARQUITETURA_TECNICA.md](./ARQUITETURA_TECNICA.md) como referência de design
3. Siga os critérios de aceitação em [ESCOPO_IMPLEMENTACAO.md](./ESCOPO_IMPLEMENTACAO.md)

### Para Revisão e Manutenção
1. Verifique o progresso em [PROGRESSO_MARCOS.md](./PROGRESSO_MARCOS.md)
2. Consulte a arquitetura em [ARQUITETURA_TECNICA.md](./ARQUITETURA_TECNICA.md)
3. Valide contra o escopo em [ESCOPO_IMPLEMENTACAO.md](./ESCOPO_IMPLEMENTACAO.md)

## Status da Documentação

✅ **Completa** - Toda a documentação base foi criada  
📊 **Progresso Global**: 100% da documentação inicial  
🔄 **Próximo Passo**: Iniciar implementação do Lexer (Marco 1.1)

## Estrutura de Arquivos

```
Docs/
├── README.md                    # Este arquivo (índice)
├── FASES_COMPILADOR.md         # Detalhamento técnico das fases
├── ESCOPO_IMPLEMENTACAO.md     # Funcionalidades e limitações
├── PROGRESSO_MARCOS.md         # Acompanhamento e marcos
└── ARQUITETURA_TECNICA.md      # Design e arquitetura
```

## Convenções da Documentação

### Símbolos de Status
- ✅ **Concluído**: Tarefa/marco completamente finalizado
- 🔄 **Em Progresso**: Atualmente sendo trabalhado
- ⏳ **Pendente**: Aguardando início
- ❌ **Bloqueado**: Impedido por dependências
- ⚠️ **Atenção**: Requer atenção especial

### Barras de Progresso
- ██████████ 100% - Completo
- ████████░░ 80% - Quase completo
- ██████░░░░ 60% - Mais da metade
- ████░░░░░░ 40% - Menos da metade
- ██░░░░░░░░ 20% - Início
- ░░░░░░░░░░ 0% - Não iniciado

### Prioridades
- 🔴 **Alta**: Crítico para o funcionamento
- 🟡 **Média**: Importante mas não crítico
- 🟢 **Baixa**: Desejável mas opcional

## Manutenção da Documentação

A documentação deve ser atualizada:
- ✅ Quando marcos são concluídos
- ✅ Quando arquitetura é modificada
- ✅ Quando escopo é alterado
- ✅ Semanalmente para progresso
- ✅ Quando bugs críticos são encontrados

## Contribuição

Ao contribuir com o projeto:
1. Mantenha a documentação atualizada
2. Siga as convenções estabelecidas
3. Atualize barras de progresso
4. Documente decisões arquiteturais
5. Registre lições aprendidas

---

**Última Atualização**: Documentação inicial completa  
**Próxima Revisão**: Após conclusão do Marco 1.1 (Lexer - Estruturas Básicas)