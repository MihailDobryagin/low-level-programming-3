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
    QUOTED_STRING = 263,
    AND = 264,
    OR = 265,
    NOT = 266,
    LT = 267,
    LE = 268,
    GT = 269,
    GE = 270,
    EQ = 271,
    OPBRACE = 272,
    CLBRACE = 273,
    OPCBRACE = 274,
    CLCBRACE = 275,
    OPSQBRACE = 276,
    CLSQBRACE = 277,
    COLON = 278,
    COMMA = 279,
    QUOTE = 280,
    FALSE_TOK = 281,
    TRUE_TOK = 282,
    INT_NUMBER = 283
  };
#endif
/* Tokens.  */
#define QUERY 258
#define INSERT 259
#define DELETE 260
#define UPDATE 261
#define STRING 262
#define QUOTED_STRING 263
#define AND 264
#define OR 265
#define NOT 266
#define LT 267
#define LE 268
#define GT 269
#define GE 270
#define EQ 271
#define OPBRACE 272
#define CLBRACE 273
#define OPCBRACE 274
#define CLCBRACE 275
#define OPSQBRACE 276
#define CLSQBRACE 277
#define COLON 278
#define COMMA 279
#define QUOTE 280
#define FALSE_TOK 281
#define TRUE_TOK 282
#define INT_NUMBER 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 36 "graphQL.y" /* yacc.c:1909  */
uint64_t num; char *string;

#line 113 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (struct View* result_view);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
