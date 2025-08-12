# 1. CONFIGURAÇÕES E VARIÁVEIS
CC = gcc

# Diretórios
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test
LIB_DIR = lib

# 2. ARQUIVOS

SRCS = $(wildcard $(SRC_DIR)/*.c)
LIB_SRCS = $(wildcard $(LIB_DIR)/*/src/*.c)
ALL_SRCS = $(SRCS) $(LIB_SRCS)

OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(ALL_SRCS)))

LIB_DIRS = $(wildcard $(LIB_DIR)/*/)

## Arquivos de teste
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_EXECS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BIN_DIR)/%)

VPATH = $(SRC_DIR) $(patsubst %/,%,$(wildcard $(LIB_DIR)/*/src/))

TARGET_EXEC = main.exe

# 3. FLAGS

CFLAGS = -Wall -g -I$(SRC_DIR) $(foreach dir,$(LIB_DIRS),-I$(dir))

LDFLAGS = -lm

# 4. TARGETS

all: $(BIN_DIR)/$(TARGET_EXEC)

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	if not exist $(subst /,\,$(BIN_DIR)) mkdir $(subst /,\,$(BIN_DIR))
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo Executavel '$(TARGET_EXEC)' criado em '$(BIN_DIR)/'.

$(BUILD_DIR)/%.o: %.c
	if not exist $(subst /,\,$(BUILD_DIR)) mkdir $(subst /,\,$(BUILD_DIR))
	$(CC) $(CFLAGS) -c $< -o $@


## Para os testes
build-tests: $(TEST_EXECS)
	@echo Todos os testes foram compilados com sucesso em '$(BIN_DIR)/'.

run-tests: build-tests
	@echo --- Executando Testes ---
	for %f in ($(subst /,,\,$(TEST_EXECS))) do @%f
	@echo --- Testes Concluidos ---

test: run-tests

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	if not exist $(subst /,\,$(BIN_DIR)) mkdir $(subst /,\,$(BIN_DIR))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	if exist $(subst /,\,$(BUILD_DIR)) rd /s /q $(subst /,\,$(BUILD_DIR))
	if exist $(subst /,\,$(BIN_DIR)) rd /s /q $(subst /,\,$(BIN_DIR))
	@echo Arquivos gerados foram limpos.
#	rm -r $(BIN_DIR)
#	rm -r $(BUILD_DIR)

.PHONY: all clean test build-tests run-tests