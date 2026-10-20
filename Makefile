# Makefile — Micro-Compiler Phase 1 (Lexer — C++ Edition)
# =========================================================
# Targets:
#   make        -> build the lexer demo       (main_cpp.exe)
#   make run    -> build & run the demo
#   make test   -> build & run the unit tests (test_lexer.exe)
#   make clean  -> remove build artefacts

CXX      = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++14 -g

# ---- Demo ----------------------------------------------------------------
DEMO_SRCS = Lexer.cpp main.cpp
DEMO_BIN  = main_cpp.exe

# ---- Tests ---------------------------------------------------------------
TEST_SRCS = Lexer.cpp test_lexer.cpp
TEST_BIN  = test_lexer.exe

# --------------------------------------------------------------------------

.PHONY: all run test clean

all: $(DEMO_BIN)

$(DEMO_BIN): $(DEMO_SRCS) Lexer.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(DEMO_SRCS)

run: $(DEMO_BIN)
	./$(DEMO_BIN)

$(TEST_BIN): $(TEST_SRCS) Lexer.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRCS)

test: $(TEST_BIN)
	@echo.
	@echo Running unit tests...
	@echo.
	./$(TEST_BIN)

clean:
	del /Q $(DEMO_BIN) $(TEST_BIN) 2>nul || true
