# Makefile — Micro-Compiler Phase 1 (Lexer)
# ==========================================
# Targets:
#   make          -> build the lexer demo (main.exe)
#   make test     -> build & run the unit test suite
#   make clean    -> remove all build artefacts

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g

# ---- Demo executable ------------------------------------------------
DEMO_SRCS = lexer.c main.c
DEMO_BIN  = main.exe

# ---- Test executable ------------------------------------------------
TEST_SRCS = lexer.c test_lexer.c
TEST_BIN  = test_lexer.exe

# ------------------------------------------------------------------ #

.PHONY: all test clean

all: $(DEMO_BIN)

$(DEMO_BIN): $(DEMO_SRCS) lexer.h
	$(CC) $(CFLAGS) -o $@ $(DEMO_SRCS)

$(TEST_BIN): $(TEST_SRCS) lexer.h
	$(CC) $(CFLAGS) -o $@ $(TEST_SRCS)

test: $(TEST_BIN)
	@echo ""
	@echo "Running unit tests..."
	@echo ""
	./$(TEST_BIN)

clean:
	del /Q $(DEMO_BIN) $(TEST_BIN) 2>nul || true
