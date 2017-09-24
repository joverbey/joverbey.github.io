%option noyywrap nounput
%option warn nodefault
%%

[A-Za-z]*    { }
.            { }

%%
int main(int, char **) { return 0; }
