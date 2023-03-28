%{
#include "y.tab.h"
void yyerror (struct View* syntax, char *s);
%}
%%
"query"         return QUERY;
"insert"        return INSERT;
"delete"        return DELETE;
"update"        return UPDATE;
"and"           return AND;
"or"            return OR;
"not"           return NOT;
"eq"            return EQ;
"lt"            return LT;
"le"            return LE;
"gt"            return GT;
"ge"            return GE;
\(              return OPBRACE;
\)              return CLBRACE;
\{              return OPCBRACE;
\}              return CLCBRACE;
\[              return OPSQBRACE;
\]              return CLSQBRACE;
:               return COLON;
,               return COMMA;
True            return TRUE_TOK;
False           return FALSE_TOK;
\"[^\"]*\"        {yylval.string = strdup(yytext); strcpy(yylval.string, yylval.string + 1); yylval.string[strlen(yylval.string) - 1] = '\0'; return QUOTED_STRING;}
[a-zA-Z][a-zA-Z_0-9]*        {yylval.string = strdup(yytext); return STRING;}
[-]?[0-9]+                    {yylval.num = atoi(yytext); return INT_NUMBER;}
[ \t\n\r]                ;
.                    {ECHO; yyerror(NULL, "unexpected character: '"); yyerror(NULL, yytext); yyerror(NULL, "'");}

%%

int yywrap(void){
    return 1;
}
