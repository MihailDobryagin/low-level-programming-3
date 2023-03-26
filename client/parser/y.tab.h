/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    QUERY = 258,
    INSERT = 259,
    DELETE = 260,
    UPDATE = 261,
    STRING = 262,
    AND = 263,
    OR = 264,
    NOT = 265,
    LT = 266,
    LE = 267,
    GT = 268,
    GE = 269,
    EQ = 270,
    OPBRACE = 271,
    CLBRACE = 272,
    OPCBRACE = 273,
    CLCBRACE = 274,
    OPSQBRACE = 275,
    CLSQBRACE = 276,
    COLON = 277,
    COMMA = 278,
    QUOTE = 279,
    FALSE = 280,
    TRUE = 281,
    INT_NUMBER = 282
  };
#endif
/* Tokens.  */
#define QUERY 258
#define INSERT 259
#define DELETE 260
#define UPDATE 261
#define STRING 262
#define AND 263
#define OR 264
#define NOT 265
#define LT 266
#define LE 267
#define GT 268
#define GE 269
#define EQ 270
#define OPBRACE 271
#define CLBRACE 272
#define OPCBRACE 273
#define CLCBRACE 274
#define OPSQBRACE 275
#define CLSQBRACE 276
#define COLON 277
#define COMMA 278
#define QUOTE 279
#define FALSE 280
#define TRUE 281
#define INT_NUMBER 282

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 36 "graphQL.y" /* yacc.c:1909  */
uint64_t num; char *string;

#line 111 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (struct View* result_view);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
