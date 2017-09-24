%option noyywrap nounput
%option warn nodefault
%%

[A-Za-z$_][A-Za-z0-9$_]*    { /* Identifier */ }
"break"                     { /* Keyword */    }
.|\n                        { }

%%
int main(int, char **) { return 0; }
