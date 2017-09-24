.PHONY: all clean

all: demo4

lexer.hpp lexer.cpp: lexer.l lexer-decls.hpp
	flex -olexer.cpp --header-file=lexer.hpp lexer.l

demo4: main.cpp lexer.cpp lexer.hpp lexer-decls.hpp
	g++ -odemo4 main.cpp lexer.cpp

clean:
	rm -f demo4 lexer.hpp lexer.cpp
