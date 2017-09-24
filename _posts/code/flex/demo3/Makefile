.PHONY: all clean

all: demo3

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

demo3: lexer.cpp lexer.hpp
	g++ -odemo3 lexer.cpp

clean:
	rm -f demo3 lexer.hpp lexer.cpp
