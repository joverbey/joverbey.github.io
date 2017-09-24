.PHONY: all clean

all: unnecessary2

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

unnecessary2: lexer.cpp lexer.hpp
	g++ -ounnecessary2 lexer.cpp

clean:
	rm -f unnecessary2 lexer.hpp lexer.cpp
