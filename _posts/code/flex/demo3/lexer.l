/* Flex Demo 3 - lexer.l */
%{
  #include <iostream>
%}

%option noyywrap nounput
%option warn nodefault

DecimalIntegerLiteral    0|[1-9][0-9]*
DecimalDigits            [0-9]+
ExponentPart             [eE]("+"|"-")?{DecimalDigits}
DecimalLiteral1          {DecimalIntegerLiteral}"."{DecimalDigits}?{ExponentPart}?
DecimalLiteral2          "."{DecimalDigits}{ExponentPart}?
DecimalLiteral3          {DecimalIntegerLiteral}{ExponentPart}?
DecimalLiteral           {DecimalLiteral1}|{DecimalLiteral2}|{DecimalLiteral3}

%%

[\t\v\f \n\r]*      { }
{DecimalLiteral}    { std::cout << "Decimal Literal: " << yytext << std::endl; }
.                   { std::cout << "Unexpected Character: " << yytext << std::endl; }

%%

int main(int, char **)
{
    yylex();
    return 0;
}
