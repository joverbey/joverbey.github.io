%option noyywrap nounput
%option warn nodefault
%%

[\t\v\f \n\r]*  { }
.               { }
\n              { }

%%
int main(int, char **) { return 0; }
