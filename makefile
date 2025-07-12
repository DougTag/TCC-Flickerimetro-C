# Makefile Otimizado para Windows

# --- 1. CONFIGURAÇÕES E VARIÁVEIS ---
# Compilador a ser usado
CC = gcc

# Diretórios (usando barras normais, que o make e o gcc entendem)
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
LIB_DIR = lib	# Não sendo usado no momento

# Flags do compilador
CFLAGS = -Wall -g -I.

# Flags do linker
LDFLAGS =


# --- 2. ARQUIVOS E ALVOS ---

# Nome do executável principal
TARGET_EXEC = meu_programa

# Arquivos fonte e objeto do programa principal
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Arquivos fonte e executáveis de teste
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_EXECS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BIN_DIR)/%)

# --- 3. REGRAS DE COMPILAÇÃO PRINCIPAL ---

# Alvo padrão: compila o programa principal
all: $(BIN_DIR)/$(TARGET_EXEC)

# Regra para criar o executável principal
$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@if not exist $(subst /,,\,$(BIN_DIR)) mkdir $(subst /,,\,$(BIN_DIR))
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo Executavel '$(TARGET_EXEC)' criado em '$(BIN_DIR)/'.

# Regra genérica para criar arquivos .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(subst /,,\,$(BUILD_DIR)) mkdir $(subst /,,\,$(BUILD_DIR))
	$(CC) $(CFLAGS) -c $< -o $@

# --- 4. REGRAS DE TESTES ---

# Alvo para APENAS COMPILAR os testes.
build-tests: $(TEST_EXECS)
	@echo Todos os testes foram compilados com sucesso em '$(BIN_DIR)/'.

# Alvo para APENAS EXECUTAR os testes.
run-tests: build-tests
	@echo --- Executando Testes ---
	@for %f in ($(subst /,,\,$(TEST_EXECS))) do @%f
	@echo --- Testes Concluidos ---

# Atalho para compilar e executar os testes
test: run-tests

# Regra genérica para criar um executável de teste.
$(BIN_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	@if not exist $(subst /,,\,$(BIN_DIR)) mkdir $(subst /,,\,$(BIN_DIR))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


# --- 5. ALVOS "PHONY" (AÇÕES) ---

clean:
	@if exist $(subst /,,\,$(BUILD_DIR)) rd /s /q $(subst /,,\,$(BUILD_DIR))
	@if exist $(subst /,,\,$(BIN_DIR)) rd /s /q $(subst /,,\,$(BIN_DIR))
	@echo Arquivos gerados foram limpos.

.PHONY: all clean test build-tests run-tests