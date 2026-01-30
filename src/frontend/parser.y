%code requires {
    // This code goes into parser.hpp (the generated header).
    // We must include <vector> because the union uses std::vector pointers.
    #include <vector>

    namespace loc { namespace ast {
        struct Node;
        struct Program;
        struct MatrixLiteral;
    } }
}

%{
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "loc/frontend/ast.hpp"   // full definitions only needed in parser.cpp

// Flex interface
int yylex(void);
void yyerror(const char* s);

// Expose the parsed AST program to main()
loc::ast::Program* g_program = nullptr;
%}

%union {
    char* str;                            // IDENT
    double num;                           // NUMBER
    loc::ast::Node* node;                 // Expr/Stmt as Node*
    loc::ast::Program* prog;              // Program*

    loc::ast::MatrixLiteral* mat;         // matrix literal
    std::vector<double>* drow;            // one row: [1,2,3]
    std::vector<std::vector<double>>* drows; // rows: [[...],[...]]
}

%token OPERATOR
%token PRINT
%token <str> IDENT
%token <num> NUMBER

%type <node> stmt expr
%type <prog> program

%type <mat>  matrix_lit
%type <drows> rows
%type <drow> row number_list

%start program

// Precedence (lowest -> highest)
%left '+'
%left '@'
%left '*'

%%

program:
      /* empty */
      {
        $$ = new loc::ast::Program();
        g_program = $$;
      }
    | program stmt
      {
        $1->statements.emplace_back(loc::ast::NodePtr($2));
        $$ = $1;
        g_program = $$;
      }
    ;

stmt:
      OPERATOR IDENT ';'
      {
        $$ = new loc::ast::OperatorDecl($2);
        free($2);
      }
    | OPERATOR IDENT '=' matrix_lit ';'
      {
        loc::ast::MatrixLiteral m = std::move(*$4);
        delete $4;

        $$ = new loc::ast::OperatorDecl($2, std::move(m));
        free($2);
      }
    | IDENT '=' expr ';'
      {
        $$ = new loc::ast::AssignStmt($1, loc::ast::NodePtr($3));
        free($1);
      }
    | PRINT expr ';'
      {
        $$ = new loc::ast::PrintStmt(loc::ast::NodePtr($2));
      }
    ;

expr:
      IDENT
      {
        $$ = new loc::ast::IdentExpr($1);
        free($1);
      }
    | '(' expr ')'
      {
        $$ = $2;
      }
    | expr '+' expr
      {
        $$ = new loc::ast::AddExpr(loc::ast::NodePtr($1), loc::ast::NodePtr($3));
      }
    | expr '@' expr
      {
        $$ = new loc::ast::ComposeExpr(loc::ast::NodePtr($1), loc::ast::NodePtr($3));
      }
    | NUMBER '*' expr
      {
        $$ = new loc::ast::ScalarMulExpr($1, loc::ast::NodePtr($3));
      }
    ;

matrix_lit:
      '[' rows ']'
      {
        auto* m = new loc::ast::MatrixLiteral();
        m->rows = std::move(*$2);
        delete $2;
        $$ = m;
      }
    ;

rows:
      row
      {
        auto* rs = new std::vector<std::vector<double>>();
        rs->push_back(std::move(*$1));
        delete $1;
        $$ = rs;
      }
    | rows ',' row
      {
        $1->push_back(std::move(*$3));
        delete $3;
        $$ = $1;
      }
    ;

row:
      '[' number_list ']'
      {
        $$ = $2;
      }
    ;

number_list:
      NUMBER
      {
        auto* r = new std::vector<double>();
        r->push_back($1);
        $$ = r;
      }
    | number_list ',' NUMBER
      {
        $1->push_back($3);
        $$ = $1;
      }
    ;

%%

extern int yylineno;

void yyerror(const char* s) {
    std::cerr << "Parse error at line " << yylineno << ": " << s << std::endl;
}
