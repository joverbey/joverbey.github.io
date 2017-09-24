.PHONY: all clean

all: demo3

lexer.hpp lexer.cpp: lexer.l lexer-decls.hpp
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

demo3: main.cpp lexer.cpp lexer.hpp lexer-decls.hpp
	g++ -odemo3 main.cpp lexer.cpp

clean:
	rm -f demo3 lexer.hpp lexer.cpp
