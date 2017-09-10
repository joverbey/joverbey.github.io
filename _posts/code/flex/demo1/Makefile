.PHONY: all clean

all: demo1

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

demo1: main.cpp lexer.cpp lexer.hpp
	g++ -odemo1 main.cpp lexer.cpp

clean:
	rm -f demo1 lexer.hpp lexer.cpp
