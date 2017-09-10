/* Flex Demo 2 - lexer.l */
%{
#include "lexer-decls.hpp"
%}

%option reentrant
%option noyywrap nounput
%option warn nodefault

%%

[\t\v\f \n\r]*  { }
"="             { return T_EQ; }
"=="            { return T_EQ_EQ; }
"break"         { return T_BREAK; }
.               { return yytext[0]; }

%%
