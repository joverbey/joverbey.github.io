.PHONY: all clean

all: unnecessary1

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

unnecessary1: lexer.cpp lexer.hpp
	g++ -ounnecessary1 lexer.cpp

clean:
	rm -f unnecessary1 lexer.hpp lexer.cpp
