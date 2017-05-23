/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_YYPARSE_H_INCLUDED
# define YY_YY_YYPARSE_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_NOTELSE = 258,
     TOK_IF = 259,
     TOK_ELSE = 260,
     TOK_LE = 261,
     TOK_LT = 262,
     TOK_GE = 263,
     TOK_GT = 264,
     TOK_NE = 265,
     TOK_EQ = 266,
     TOK_NEW = 267,
     TOK_NEG = 268,
     TOK_POS = 269,
     TOK_FUNCTION = 270,
     TOK_FIELD = 271,
     TOK_ARRAY = 272,
     TOK_VOID = 273,
     TOK_CHAR = 274,
     TOK_INT = 275,
     TOK_STRING = 276,
     NUMBER = 277,
     TOK_PROTOTYPE = 278,
     TOK_WHILE = 279,
     TOK_RETURN = 280,
     TOK_RETURNVOID = 281,
     TOK_STRUCT = 282,
     TOK_NULL = 283,
     TOK_INDEX = 284,
     TOK_IDENT = 285,
     TOK_INTCON = 286,
     TOK_CHARCON = 287,
     TOK_STRINGCON = 288,
     TOK_BLOCK = 289,
     TOK_CALL = 290,
     TOK_IFELSE = 291,
     TOK_DECLID = 292,
     TOK_NEWSTRING = 293,
     TOK_NEWARRAY = 294,
     TOK_TYPEID = 295,
     TOK_ORD = 296,
     TOK_CHR = 297,
     TOK_ROOT = 298,
     TOK_PARAMLIST = 299,
     TOK_VARDECL = 300
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_YYPARSE_H_INCLUDED  */
