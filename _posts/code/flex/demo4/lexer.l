/* Flex Demo 4 - lexer.l */
%{
#include "lexer-decls.hpp"

#define YY_USER_INIT {  \
    yylineno = 1;       \
    yycolumn = 1;       \
}

#define YY_USER_ACTION {        \
    yylloc->line = yylineno;    \
    yylloc->column = yycolumn;  \
    yylloc->length = yyleng;    \
    yycolumn += yyleng;         \
}
%}

%option reentrant
%option bison-bridge bison-locations yylineno
%option noyywrap nounput
%option warn nodefault

%%

"f"      { yylval->text = yytext; return T_F; }
"foo"    { yylval->text = yytext; return T_FOO; }
"bar"    { }
.        { yylval->text = yytext; return yytext[0]; }
\n       { yylval->text = yytext; return yytext[0]; }
<<EOF>>  { yylval->text = ""; return END_OF_INPUT; }

%%
