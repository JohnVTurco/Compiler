# WLP4 Compiler

## Overview
This project is a compiler for WLP4, a C++-like language, developed in C++. The compiler converts high-level code into MIPS assembly language. It incorporates various stages of compilation including tokenization, parse tree construction, syntax analysis, and semantic analysis.

## Features
- **Tokenization**: Utilizes a deterministic finite automaton (DFA) to tokenize input programs.
- **Parse Tree Construction**: Builds a parse tree to represent the grammatical structure of the source code.
- **Syntax Analysis**: Ensures the source code adheres to the language's grammar rules.
- **Semantic Analysis**: Checks for semantic correctness and annotates the parse tree with type information.
- **Code Generation**: Translates the parse tree into optimized MIPS assembly code.
