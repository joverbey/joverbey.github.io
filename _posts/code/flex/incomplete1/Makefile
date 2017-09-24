.PHONY: all clean

all: incomplete1

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

incomplete1: lexer.cpp lexer.hpp
	g++ -oincomplete1 lexer.cpp

clean:
	rm -f incomplete1 lexer.hpp lexer.cpp
