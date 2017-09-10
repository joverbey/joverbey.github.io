/* Flex Demo 1 - lexer.l */

%option reentrant
%option noyywrap
%option warn nodefault

%%

[\t\v\f \n\r]*  { return 1; }
"="             { return 2; }
"=="            { return 3; }
"break"         { return 4; }
.               { return 5; }

%%
