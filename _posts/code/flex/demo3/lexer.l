/* Flex Demo 3 - lexer.l */
%{
#include "lexer-decls.hpp"
%}

%option reentrant
%option bison-bridge
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
