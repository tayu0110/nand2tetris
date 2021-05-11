#!/bin/bash

g++ FileHandler.cpp -c -Wall
g++ JackTokenizer2.cpp -c -Wall
g++ CompilationEngine2.cpp -c -Wall
g++ JackAnalyzer.cpp -c -Wall
g++ main.cpp FileHandler.o JackTokenizer2.o CompilationEngine2.o JackAnalyzer.o -o JackAnalyzer.exe -Wall -std=c++17 >& log.txt
code log.txt