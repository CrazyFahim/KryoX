/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_MEDIA_FAHIM_DELL_EXTERNAL_ALL_ABOUT_MACHINE_LEARNING_AND_DEEP_LEARING_NON_PAPER_IMPLEMENTATION_PROJECTS_ML_COMPILER_EXPLORATION_LINEAR_OPERATOR_COMPILER_BUILD_PARSER_HPP_INCLUDED
# define YY_YY_MEDIA_FAHIM_DELL_EXTERNAL_ALL_ABOUT_MACHINE_LEARNING_AND_DEEP_LEARING_NON_PAPER_IMPLEMENTATION_PROJECTS_ML_COMPILER_EXPLORATION_LINEAR_OPERATOR_COMPILER_BUILD_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "/media/fahim/Dell External/All-About-Machine-Learning-and-Deep-Learing/NON-PAPER-implementation-Projects/ML-Compiler-Exploration/Linear-operator-compiler/src/frontend/parser.y"

    // This code goes into parser.hpp (the generated header).
    // We must include <vector> because the union uses std::vector pointers.
    #include <vector>

    namespace loc { namespace ast {
        struct Node;
        struct Program;
        struct MatrixLiteral;
    } }

#line 61 "/media/fahim/Dell External/All-About-Machine-Learning-and-Deep-Learing/NON-PAPER-implementation-Projects/ML-Compiler-Exploration/Linear-operator-compiler/build/parser.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    OPERATOR = 258,                /* OPERATOR  */
    PRINT = 259,                   /* PRINT  */
    IDENT = 260,                   /* IDENT  */
    NUMBER = 261                   /* NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 29 "/media/fahim/Dell External/All-About-Machine-Learning-and-Deep-Learing/NON-PAPER-implementation-Projects/ML-Compiler-Exploration/Linear-operator-compiler/src/frontend/parser.y"

    char* str;                            // IDENT
    double num;                           // NUMBER
    loc::ast::Node* node;                 // Expr/Stmt as Node*
    loc::ast::Program* prog;              // Program*

    loc::ast::MatrixLiteral* mat;         // matrix literal
    std::vector<double>* drow;            // one row: [1,2,3]
    std::vector<std::vector<double>>* drows; // rows: [[...],[...]]

#line 95 "/media/fahim/Dell External/All-About-Machine-Learning-and-Deep-Learing/NON-PAPER-implementation-Projects/ML-Compiler-Exploration/Linear-operator-compiler/build/parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_MEDIA_FAHIM_DELL_EXTERNAL_ALL_ABOUT_MACHINE_LEARNING_AND_DEEP_LEARING_NON_PAPER_IMPLEMENTATION_PROJECTS_ML_COMPILER_EXPLORATION_LINEAR_OPERATOR_COMPILER_BUILD_PARSER_HPP_INCLUDED  */
