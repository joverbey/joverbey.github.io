.PHONY: all clean

all: incomplete2

lexer.hpp lexer.cpp: lexer.l
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

incomplete2: lexer.cpp lexer.hpp
	g++ -oincomplete2 lexer.cpp

clean:
	rm -f incomplete2 lexer.hpp lexer.cpp
