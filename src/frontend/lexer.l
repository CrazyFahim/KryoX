%option noyywrap nounput noinput

%{
#include "parser.hpp"
#include <cstring>
#include <cstdlib>
%}

%%

[ \t\r\n]+                          ;   /* skip whitespace */

"operator"                          return OPERATOR;
"print"                             return PRINT;

[-+]?([0-9]+(\.[0-9]+)?|\.[0-9]+)  {
                                      yylval.num = atof(yytext);
                                      return NUMBER;
                                   }

[a-zA-Z_][a-zA-Z0-9_]*             {
                                      yylval.str = strdup(yytext);
                                      return IDENT;
                                   }

"@"                                 return '@';
"+"                                 return '+';
"*"                                 return '*';
"="                                 return '=';
"("                                 return '(';
")"                                 return ')';
";"                                 return ';';

"["                                 return '[';
"]"                                 return ']';
","                                 return ',';

.                                   return yytext[0];

%%
