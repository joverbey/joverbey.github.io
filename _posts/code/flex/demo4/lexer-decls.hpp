// Flex Demo 3 - lexer-decls.hpp

#ifndef LEXER_DECLS_HPP
#define LEXER_DECLS_HPP

typedef struct { const char *text; } YYSTYPE;

#define END_OF_INPUT 0
#define T_F 257
#define T_FOO 258

#endif
