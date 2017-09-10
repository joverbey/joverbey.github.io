.PHONY: all clean

all: demo2

lexer.hpp lexer.cpp: lexer.l lexer-decls.hpp
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

demo2: main.cpp lexer.cpp lexer.hpp lexer-decls.hpp
	g++ -odemo2 main.cpp lexer.cpp

clean:
	rm -f demo2 lexer.hpp lexer.cpp
