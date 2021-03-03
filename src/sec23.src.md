# Tiny turtle graphics interpreter

A program `turtle` is an example with the combination of TfeTextView and GtkDrawingArea objects.
It is a very small interpreter but it provides a way to draw fractal curves.
The following diagram is a Koch curve, which is a famous example of fractal curves.

![Koch curve](turtle/image/turtle_koch.png){width=8cm height=5.11cm}

This program uses flex and bison.
Flex is a lexical analyzer.
Bison is a parser generator.
These two programs are similar to lex and yacc which are proprietary software developed in Bell Laboratry.
However, flex and bison are open source software.
I will write about how to use those software, but they are not topics about gtk.
So, readers can skip that part of this sections.

## How to use turtle

The documentation of turtle is [here](turtle/turtle_doc.md).
I'll show you a simple example.

~~~
fc (1,0,0) # Foreground color is red, rgb = (1,0,0).
pd         # Pen down.
fd 100     # Go forward by 100 pixels.
tr 90      # Turn right by 90 degrees.
fd 100
tr 90
fd 100
tr 90
fd 100
tr 90
~~~

1. Compile and install `turtle` (See the documentation above).
Then, run `turtle`.
2. Type the program above in the editor (left part of the window).
3. Click on the `Run` button, then a red square appears on the right part of the window.
The side of the square is 100 pixels long.

In the same way, you can draw other curves.
The documentation above shows some fractal curves such as tree, snow and square-koch.
The source code in turtle language is located at [src/turtle/example](turtle/example) directory.
You can read these files by clicking on the `Open` button.

## Combination of TfeTextView and GtkDrawingArea objects

Turtle uses TfeTextView and GtkDrawingArea.
It is similar to `color` program in the previous section.

The body of the interpreter is written with flex and bison.
The codes are not thread safe.
So the handler of "clicked" signal of the `Run` button prevents from reentering.

@@@include
turtle/turtleapplication.c run_cb resize_cb
@@@

- 8-13: The static value `busy` holds a status of the interpreter.
If it is `TRUE`, the interpreter is running and it is not possible to call the interpreter again because it's not a re-entrant program.
If it is `FALSE`, it is safe to call the interpreter.
- 14: Now it is about to call the interpreter so let `busy` variable to be TRUE.
- 15-16: Gets the contents of GtkTextBuffer.
- 17: The varable `surface` is a static variable.
It points to a `cairo_surface_t` object.
It is generated when the GtkDrawingArea object is realized and the each time it is resized.
Therefore, `surface` isn't NULL usually.
But if it is NULL, the interpreter won't be called.
- 18-19: Initializes lexcal analyzer, then parser.
- 20: Calls parser.
Parser analyze the program codes syntactically and generate a tree structured data.
- 24-26: If the parser succesfully parsed, it calls `run` which is the interpreter.
`Run` executes the tree-structured program.
- 27: finalize the lexical analyzer.
- 29: Add the drawing area object to the queue to draw.
- 30: The interpreter program has finished so `busy` is now FALSE.
- 33-38: A handler of "resized" signal.
It generates or regenerates a surface object.

Other part of `turtleapplication.c` is almost same as the codes of `colorapplication.c` in the previous section.
The codes of `turtleapplication.c` is in the [turtle directory](turtle).

## What does the interpreter do?

Suppose that the turtle runs with the following program.

~~~
distance = 100
fd distance*2
~~~

The turtle recognizes the program above and works as follows.

- Generally, a program consists of tokens.
Tokens are "distance", "=", "100", "fd", "*" and "2" in the above example..
- The interpreter `turtle` calls `yylex` to read a token in the source file.
The `yylex` returns a code which is called "token kind" and sets a global variable `yylval` with a value, which is called a semantic value.
The type of `yylval` is union and `yylval.ID` is string and `yylval.NUM` is double.
There are seven tokens in the program so `yylex` is called seven times.

|   |token kind|yylval.ID|yylval.NUM|
|:-:|:--------:|:-------:|:--------:|
| 1 |    ID    |distance |          |
| 2 |    =     |         |          |
| 3 |   NUM    |         |   100    |
| 4 |    FD    |         |          |
| 5 |    ID    |distance |          |
| 6 |    *     |         |          |
| 7 |   NUM    |         |    2     |

- `yylex` returns a token kind every time, but it doesn't set `yylval.ID` or `yylval.NUM` every time.
It is because keywords (`FD`) and symbols (`=` and `*`) don't have any semantic values.
The function `yylex` is called lexical analyzer or scanner.
- `turtle` makes a tree structured data.
This part of `turtle` is called parser.

![turtle parser tree](../image/turtle_parser_tree.png){width=12cm height=5.34cm}

- `turtle` analyzes the tree and executes it.
This part of `turtle` is called runtime routine.
The tree consists of line segments and rectangles between line segments.
The rectangles are called nodes.
For example, N\_PROGRAM, N\_ASSIGN, N\_FD and N\_MUL are nodes.
  1. Goes down from N\_PROGRAM to N\_ASSIGN.
  2. N_ASSIGN node has two children, ID and NUM.
This node comes from "distance = 100" which is "ID = NUM" syntactically.
First, `turtle` checks if the first child is ID.
If it's ID, then `turtle` looks for the variable in the variable table.
If it doesn't exist, it registers the ID (`distance`) to the table.
Then go back to the N\_ASSIGN node.
  3. `turtle` calculates the second child.
In this case its a number 100.
Saves 100 to the variable table at the `distance` record.
  4. `turtle` goes back to N\_PROGRAM then go to the next node N\_FD.
It has only one child.
Goes down to the child N\_MUL.
  5. The first child is ID (distance).
Searches the variable table for the varable `distance` and gets the value 100.
The second child is a number 2.
Multiplies 100 by 2 and gets 200.
Then `turtle` goes back to N_FD.
  6. Now `turtle` knows the distance is 200.
It moves the cursor forward by 200.
  8. There are no node follows.
Runtime routine returns to the main routine.
-`turtle` draws a segment on GtkDrawingArea then stops.

Actually most programs are more complicated than the example above.
So, `turtle` does much more work to interpret programs.
However, basically it works by the same way above.
Interpritation consists of three parts.

- Lexical analysis
- Syntax Parsing and tree generation
- Interprit the tree and execute commands.

## Compilation flow

The source files are:

- flex source file ~> turtle.lex
- bison source file => turtle.y
- C header file => turtle.h, turtle_lex.h
- C source file => turtleapplication.c
- other files => turtle.ui, turtle.gresources.xml, meson.build

The compilation process is a bit complicated.

1. glib-compile-resources compiles `turtle.ui` to `resources.c` according to `turtle.gresource.xml`.
It also generates `resources.h`.
2. bison compiles `turtle.y` to `turtle_parser.c` and generates `turtle_parser.h`
3. flex compiles `turtle.lex` to `turtle_lex.c`.
4. gcc compiles `application.c`, `resources.c`, `turtle_parser.c` and `turtle_lex.c` with `turtle.h`, `resources.h` and `turtle_parser.h`.
It generates an executable file `turtle`.

![compile process](../image/turtle_compile_process.png){width=12cm height=9cm}

Meson controls the process and the instruction is described in meson.build.

@@@include
turtle/meson.build
@@@

- 3: Gets C compiler.
It is usually `gcc` in linux.
- 4: Gets math library.
This program uses trigonometric functions.
They are defined in the math library, but the library is optional.
So, it is necessary to include the library by `#include <math.h>` and also link the library with the linker.
- 6: Gets gtk4 library.
- 8: Gets gnome module.
Module is a system provided by meson.
See [Meson build system website](https://mesonbuild.com/Gnome-module.html#gnome-module) for further information.
- 9: Compiles ui file to C source file according to the XML file `turtle.gresource.xml`.
- 11: Gets flex.
- 12: Gets bison.
- 13: Compiles `turtle.y` to `turtle_parser.c` and `turtle_parser.h` by bison.
The function `custom_target`creates a custom top level target.
See [Meson build system website](https://mesonbuild.com/Reference-manual.html#custom_target) for further information.
- 14: Compiles `turtle.lex` to `turtle_lex.c` by flex.
- 16: Specifies C surce files.
- 18: Compiles C source files including generated files by glib-compile-resources, bison and flex.
The argument `turtleparser[1]` refers to `tirtle_parser.h` which is the secound output in the line 13.

## Turtle.lex

### What does flex do?

Flex creates lexical analizer from flex source file.
Flex source file is a text file and its syntactic rule will be explained later.
Generated lexical analyzer is a C source file.
It is also called scanner.
It reads a text file, which is a source file of a program language, and gets variable names, numbers and symbols.
Suppose here is a turtle source file.

~~~
fc (1,0,0) # Foreground color is red, rgb = (1,0,0).
pd         # Pen down.
distance = 100
angle = 90
fd distance    # Go forward by distance (100) pixels.
tr angle     # Turn right by angle (90) degrees.
~~~

The content of the text file is separated into `fc`, `(`, `1`, `,` and so on.
The words `fc`, `pd`, `distance` and `100` are called tokens.
The characters `(`, `<` and `=` are called symbols.
( Sometimes those symbols called token, too.)

Flex reads `turtle.lex` and generates a scanner.
The file `turtle.lex` specifies tokens, symbols and the behavior which corresponds to each token or symbol.
Turtle.lex isn't a big program.

@@@include
turtle/turtle.lex
@@@

The file consists of three sections which are separated by "%%" (line 19 and 58).
They are definitions, rules and user code sections.

### Definitions section

First, look at the definitions section.

- 1-12: Lines between "%top{" and "}" are C source codes.
They will be copied to the top of the generated C source file.
- 2-3: The function `strlen`, in line 64, is defined in `string.h`
The function `atof`, in line 39, is defined in `stdlib.h`.
- 6-8: The current input position is pointed by `nline` and `ncolumn`.
The function `get_location` (line 60-65) sets `yylloc`to point the start and end point of `yytext` in the buffer.
This function is declared here so that it can be called before the function is defined.
- 11: GSlist is used to keep allocated memories.
- 14: This option (`%option noyywrap`) must be specified when you have only single source file to the scanner. Reffur to "9 The Generated Scanner" in the flex documentation in your distribution for further information.
(The documentation is not on the internet.)
- 16-17: `REAL_NUMBER` and `IDENTIFIER` are names.
A name begins with a letter or an underscore followed by zero or more letters, digits, underscores (`_`) or dashes (`-`).
They are followed by regular expressions which are the definition of them.
They will be used in rules section and will expand to the definition.
You can leave out such definitions here and use regular expressions in rules section directly.

### Rules section

This section is the most important part.
Rules consist of patterns and actions.
For example, line 37 is a rule.

- `{REAL_NUMBER}` is a pattern
- `get_location (yytext); yylval.NUM = atof (yytext); return NUM;` is an action.

`{REAL_NUMBER}` is defined in the 16th line, so it expands to `(0|[1-9][0-9]*)(\.[0-9]+)?`.
This regular expression matches numbers like `0`, `12` and `1.5`.
If the input is a number, it matches the pattern in line 37.
Then the matched text is assigned to `yytext` and corresponding action is executed.
A function `get_location` changes the location variables.
It assigns `atof( yytext)`, which is double sized number converted from `yytext`, to `yylval.NUM` and return `NUM`.
`NUM` is an integer defined by `turtle.y`.

The scanner generated by flex and C compiler has `yylex` function.
If `yylex` is called and the input is "123.4", then it works as follows.

1. A string "123.4" matches `{REAL_NUMBER}`.
2. Update the location variable `ncolumn` and `yylloc`.
3. `atof` converts the string "123.4" to double sized floating point number 123.4.
4. It is assigned to `yylval.NUM`.
5. `yylex` returns `NUM` to the caller.

Then the caller knows the input is `NUM` (number), and its value is 123.4.

- 19-55: Rules section.
- 20: Comment begins `#` followed by any characters except newline.
No action happens.
- 21: White space just increases a varable `ncolumn` by one.
- 22: Tab is assumed to be equal to eight spaces.
- 23: New line increases a variable `nline` by one and resets `ncolumn`.
- 25-35: Keywords just updates the location variables `ncolumn` and `yylloc`, and return the codes of the keywords.
- 37: Real number constant.
- 38: Identifier is defined in line 17.
It begins alphabet followed by zero or more alphabet or digit.
The location variables are updated and the name of the identifier is assigned to `yylval.ID`.
The memory of the name is allocated by the function `g_strdup`.
The memory is registerd to the list (GSlist type list).
The memory will be freed after the runtime routine finishes.
Returns `ID`.
- 43-54: Symbols just update the location variable and return the codes.
The code is the same as the symbol itself.
- 55: If the input doesn't match above patterns, then it is error.
Returns `YYUNDEF`.

### User code section

This section is just copied to C source file.

- 58-63: A function `get_location`.
The location of the input is recorded to `nline` and `ncolumn`.
These two variables are for the scanner.
A variable `yylloc` is shared by the scanner and the parser.
It is a C structure and has four members, `first_line`, `first_column`, `last_line` and `last_column`.
They point the start and end of the current input text.
- 65: `YY_BUFFER_STATE` is a type of the pointer points the input buffer.
- 67-70: `init_flex` is called by `run_cb` signal handler, which is called when `Run` button is clicked on.
`run_cb` has one argument which is the copy of the content of GtkTextBuffer.
`yy_scan_string` sets the input buffer to read from the text.
- 72-75: `finalize_flex` is called after runtime routine finishes.
It deletes the input buffer.

## Turtle.y

Turtle.y has more than 800 lines so it is difficult to explain all the source code.
So I will explain the key points and leave out other less important parts.

### What does bison do?

Bison creates C source file of a parser from bison source file.
Bison source file is a text file.
A parser analyzes a program source code according to its grammar.
Suppose here is a turtle source file.

~~~
fc (1,0,0) # Foreground color is red, rgb = (1,0,0).
pd         # Pen down.
distance = 100
angle = 90
fd distance    # Go forward by distance (100) pixels.
tr angle     # Turn right by angle (90) degrees.
~~~

The parser calls `yylex` to get a token.
The token consists of its type (token kind) and value (semantic value).
So, the parser gets items in the following table whenever it calls `yylex`.

|   |token kind|yylval.ID|yylval.NUM|
|:-:|:--------:|:-------:|:--------:|
| 1 |    FC    |         |          |
| 2 |    (     |         |          |
| 3 |   NUM    |         |   1.0    |
| 4 |    ,     |         |          |
| 5 |   NUM    |         |   0.0    |
| 6 |    ,     |         |          |
| 7 |   NUM    |         |   0.0    |
| 8 |    )     |         |          |
| 9 |    PD    |         |          |
|10 |    ID    |distance |          |
|11 |    =     |         |          |
|12 |   NUM    |         |  100.0   |
|13 |    ID    |  angle  |          |
|14 |    =     |         |          |
|15 |   NUM    |         |   90.0   |
|16 |    FD    |         |          |
|17 |    ID    |distance |          |
|18 |    TR    |         |          |
|19 |    ID    |  angle  |          |

Bison source code specifies the grammar rules  of turtle language.
For example, `fc (1,0,0)` is called primary procedure.
A procedure is like a void type function in C source code.
It doesn't return any values.
Programmers can define their own procedures.
On the other hand, `fc` is a built-in procedure.
Such procedures are called primary procedures.
It is described in Bison source code like:

~~~
primary_procedure: FC '(' expression ',' expression ',' expression ')';
expression: ID | NUM;
~~~

This means:

- Primary procedure is FC followed by '(', expression, ',', expression, ',', expression and ')'.
- expression is ID or NUM.

The description above is called BNF (Backus-Naur form).
More precisely, it is similar to BNF.

The first line is:

~~~
FC '(' NUM ',' NUM ',' NUM ')';
~~~

You can find this is a primary_procedure easily.
The parser of the turtle language analyzes the turtle source code in the same way.

The grammar of turtle is described in the [document](turtle/turtle_doc.md).
The following is an extract from the document.

~~~
program:
  statement
| program statement
;

statement:
  primary_procedure
| procedure_definition
;

primary_procedure:
  PU
| PD
| PW expression
| FD expression
| TR expression
| BC '(' expression ',' expression ',' expression ')'
| FC '(' expression ',' expression ',' expression ')'
| ID '=' expression
| IF '(' expression ')' '{' primary_procedure_list '}'
| RT
| RS
| ID '(' ')'
| ID '(' argument_list ')'
;

procedure_definition:
  DP ID '('  ')' '{' primary_procedure_list '}'
| DP ID '(' parameter_list ')' '{' primary_procedure_list '}'
;

parameter_list:
  ID
| parameter_list ',' ID
;

argument_list:
  expression
| argument_list ',' expression
;

primary_procedure_list:
  primary_procedure
| primary_procedure_list primary_procedure
;

expression:
  expression '=' expression
| expression '>' expression
| expression '<' expression
| expression '+' expression
| expression '-' expression
| expression '*' expression
| expression '/' expression
| '-' expression %prec UMINUS
| '(' expression ')'
| ID
| NUM
;
~~~

The grammar rule defines `program` first.

- program is a statement or a program followed by a statement.

The definition is recursive.

- `statement` is program.
- `statement statement` is `program statemet`.
Therefore, it is program.
- `statement statement statement` is `program statemet`.
Therefore, it is program.

You can find that a list of statements is program like this.

`program` and `statement` aren't tokens.
They don't appear in the input.
They are called non terminal symbols.
On the other hand, tokens are called terminal symbols.
The word "token" used here has wide meaning, it includes tokens and symbols which appear in the input.
Non terminal symbols are often shortend to nterm.

Let's analyze the program above as bison does.

|   |token kind|yylval.ID|yylval.NUM|parse                               |S/R|
|:-:|:--------:|:-------:|:--------:|:-----------------------------------|:-:|
| 1 |    FC    |         |          |FC                                  | S |
| 2 |    (     |         |          |FC(                                 | S |
| 3 |   NUM    |         |   1.0    |FC(NUM                              | S |
|   |          |         |          |FC(expression                       | R |
| 4 |    ,     |         |          |FC(expression,                      | S |
| 5 |   NUM    |         |   0.0    |FC(expression,NUM                   | S |
|   |          |         |          |FC(expression,expression            | R |
| 6 |    ,     |         |          |FC(expression,expression,           | S |
| 7 |   NUM    |         |   0.0    |FC(expression,expression,NUM        | S |
|   |          |         |          |FC(expression,expression,expression | R |
| 8 |    )     |         |          |FC(expression,expression,expression)| S |
|   |          |         |          |primary_procedure                   | R |
|   |          |         |          |statement                           | R |
|   |          |         |          |program                             | R |
| 9 |    PD    |         |          |program PD                          | S |
|   |          |         |          |program primary_procedure           | R |
|   |          |         |          |program statement                   | R |
|   |          |         |          |program                             | R |
|10 |    ID    |distance |          |program ID                          | S |
|11 |    =     |         |          |program ID=                         | S |
|12 |   NUM    |         |  100.0   |program ID=NUM                      | S |
|   |          |         |          |program ID=expression               | R |
|   |          |         |          |program primary_procedure           | R |
|   |          |         |          |program statement                   | R |
|   |          |         |          |program                             | R |
|13 |    ID    |  angle  |          |program ID                          | S |
|14 |    =     |         |          |program ID=                         | S |
|15 |   NUM    |         |   90.0   |program ID=NUM                      | S |
|   |          |         |          |program ID=expression               | R |
|   |          |         |          |program primary_procedure           | R |
|   |          |         |          |program statement                   | R |
|   |          |         |          |program                             | R |
|16 |    FD    |         |          |program FD                          | S |
|17 |    ID    |distance |          |program FD ID                       | S |
|   |          |         |          |program FD expression               | R |
|   |          |         |          |program primary_procedure           | R |
|   |          |         |          |program statement                   | R |
|   |          |         |          |program                             | R |
|18 |    TR    |         |          |program TR                          | S |
|19 |    ID    |  angle  |          |program TR ID                       | S |
|   |          |         |          |program TR expression               | R |
|   |          |         |          |program primary_procedure           | R |
|   |          |         |          |program statement                   | R |
|   |          |         |          |program                             | R |

The right most column shows shift/reduce.
Shift is appending an input to the buffer.
Reduce is substituing a higher nterm for the pattern in the buffer.
For example, NUM is replaced by expression in the forth row.
This substitution is "reduce".

Bison repeats shift and reduction until the end of the input.
If the result is reduced to `program`, the input is syntactically valid.
Bison executes an action whenever reduction occers.
Actions build a tree.
The tree is analyzed and executed by runtime routine .

Bison source files are called bison grammar files.
A bison grammar file consists of four sections, prologue, declarations, rules and epilogue.
The format is as follows.

~~~
%{
prologue
%}
declarations
%%
rules
%%
epilogue
~~~

### Prologue

Prologue section consists of C codes and the codes are copied to the parser implementation file.
You can use `%code` directives to qualify the prologue and identifies the purpose explisitely.
The following is an extract from `turtle.y`.

@@@if gfm
~~~bison
@@@elif html
~~~{.bison}
@@@else
~~~
@@@end
%code top{
  #include <stdarg.h>
  #include <setjmp.h>
  #include <math.h>
  #include "turtle.h"

  /* error reporting */
  static void yyerror (char const *s) { /* for syntax error */
    g_print ("%s from line %d, column %d to line %d, column %d\n",s, yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
  }
  /* Node type */
  enum {
    N_PU,
    N_PD,
    N_PW,
 ... ... ...
  };
}
~~~

The directove `%code top` copies its contents to the top of the parser implementation file.
It usually includes `#include` directives, declarations of functions and definitions of constants.
A function `yyerror` reports a syntax error and is called by the parser.
Node type identifies a node in the tree.

Another directive `%code requires` copies its contents to both the parser implementation file and header file.
The header file is read by the scanner C source file and other files.

@@@if gfm
~~~bison
@@@elif html
~~~{.bison}
@@@else
~~~
@@@end
%code requires {
  int yylex (void);
  void init_parse (void);
  int yyparse (void);
  void run (void);

  /* semantic value type */
  typedef struct _node_t node_t;
  struct _node_t {
    int type;
    union {
      struct {
        node_t *child1;
        node_t *child2;
        node_t *child3;
      } child;
      char *name;
      double value;
    } content;
  };
}
~~~

The contents of this directive are used by the other files.

- `yylex` is shared by parser implemetation file and scanner file.
- `init_parse`, `yyparse` and `run` is called by `run_cb' in `turtleapplication.c`.
- `node_t` is the type of the semantic value of nterms.
The header file defines `YYSTYPE`, which is the semantic value type, with all the token and nterm value types.
The following is extracted from the header file.

~~~
/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
  char * ID;                               /* ID  */
  double NUM;                              /* NUM  */
  node_t * program;                        /* program  */
  node_t * statement;                      /* statement  */
  node_t * primary_procedure;              /* primary_procedure  */
  node_t * primary_procedure_list;         /* primary_procedure_list  */
  node_t * procedure_definition;           /* procedure_definition  */
  node_t * parameter_list;                 /* parameter_list  */
  node_t * argument_list;                  /* argument_list  */
  node_t * expression;                     /* expression  */
};
~~~

Other useful macros and declarations are put into the `%code` directive.

~~~
%code {
/* The following macro is convenient to get the member of the node. */
  #define child1(n) (n)->content.child.child1
  #define child2(n) (n)->content.child.child2
  #define child3(n) (n)->content.child.child3
  #define name(n) (n)->content.name
  #define value(n) (n)->content.value

  /* start of nodes */
  static node_t *node_top = NULL;
  /* functions to generate trees */
  static node_t *tree1 (int type, node_t *child1, node_t *child2, node_t *child3);
  static node_t *tree2 (int type, double value);
  static node_t *tree3 (int type, char *name);
}
~~~

### Bison declarations

Bison declarations defines terminal and nonterminal symbols.
It also specifies some directives.

~~~
%locations
%define api.value.type union /* YYSTYPE, the type of semantic values, is union of following types */
 /* key words */
%token PU
%token PD
%token PW
%token FD
%token TR
%token BC
%token FC
%token DP
%token IF
%token RT
%token RS
 /* constant */
%token <double> NUM
 /* identirier */
%token <char *> ID
 /* non terminal symbol */
%nterm <node_t *> program
%nterm <node_t *> statement
%nterm <node_t *> primary_procedure
%nterm <node_t *> primary_procedure_list
%nterm <node_t *> procedure_definition
%nterm <node_t *> parameter_list
%nterm <node_t *> argument_list
%nterm <node_t *> expression
 /* logical relation symbol */
%left '=' '<' '>'
 /* arithmetic symbol */
%left '+' '-'
%left '*' '/'
%precedence UMINUS /* unary minus */
~~~

`%locations` directive inserts the location structure into the header file.
It is like this.

~~~
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
~~~

This type is shared by the scanner file and the parser implementation file.
The error report function `yyerror` uses it so that it can inform the location that error occers.

`%define api.value.type union` generates semantic value type with tokens and nterms and inserts it to the header file.
The inserted part is shown in the previous section as the extracts that shows the value type (YYSTYPE).

`%token` and `%nterm` directives define tokens and directives respectively.

~~~
%token PU
... ...
%token <double> NUM
~~~

These directives define a token `PU` and NUM.
The values of token kinds `PU` and `NUM` are defined as an enumeration constant in the header file.

~~~
  enum yytokentype
  {
  ... ... ...
    PU = 258,                      /* PU  */
  ... ... ...
    NUM = 269,                     /* NUM  */
  ... ... ...
  };
  typedef enum yytokentype yytoken_kind_t;
~~~

In addition, the type of the semantic value of NUM is defined as double in the header file because of `<double>` tag.

~~~
union YYSTYPE
{
  char * ID;                               /* ID  */
  double NUM;                              /* NUM  */
  ... ...
}
~~~

All the nterm symbols have the same type `* node_t` of the semantic value.

`%left` and `%precedence` directives define the precedence of operation symbols.

~~~
 /* logical relation symbol */
%left '=' '<' '>'
 /* arithmetic symbol */
%left '+' '-'
%left '*' '/'
%precedence UMINUS /* unary minus */
~~~

`%left` directive defines the following symbols as left-associated operator.
If an operator `+` is left-associated, then

~~~
A + B + C = (A + B) + C
~~~

That is, the calculation is carried out the left operator first, then the right operator.
If an operator `*` is right-assiciated, then:

~~~
A * B * C = A * (B * C)
~~~

The definition above decides the behavior of the parser.
Addition and multiplication hold associative law so the result of `(A+B)+C` and `A+(B+C)` are equal in terms of mathematics.
However, the parser will be confused if left (or right) assosiativity is not specified.

`%left` and `%precedence` directive show the precedence of operators.
Later declared operators have higher precedence than former declared ones.
The declaration above says, for example,

~~~
v=w+z*5+7 is the same as v=((w+(z*5))+7)
~~~

Be careful.
The operator `=` is a logical equal operator, not assignment operator.
Assignment is not expression in turtle language.
It is primary_procedure.
Therefore, if `=` appears in an expression, it is *NOT* an assignment.

### Grammar rules

Grammar rules section defines the syntactic grammar of the language.
It is similar to BNF form.

~~~
result: components { action };
~~~

- result is a nterm.
- components are list of tokens or nterms.
- action is C codes. It is executed whenever the components are reduced to the result.
Action can be left out.

The following is the first eleven lines of the grammar rule in `turtle.y`.

~~~
program:
  statement { node_top = $$ = $1; }
| program statement {
        node_top = $$ = tree1 (N_program, $1, $2, NULL);
        }
;

statement:
  primary_procedure
| procedure_definition
;
~~~

The first six lines show that `program` is `statement` or `program` followed by `statement`.

- Whenever `statement` is reduced to `program`, an action `node_top=$$=$1;` is executed.
- In the same way, whenever `program` followed by `statment is reduced to `program`,
`node_top=$$=tree1(N_program,$1,$2,NULL);` is executed.
- `node_top` is a static variable.
It points the top node of the tree.
- `$$` is a semantic value of the result, which is `program` in the example above.
The semantic value of `program` is a pointer to `node_t` type structure.
It was defined in the declaration section.
- `$1` is a semantic value of the first component, which is `statement` in the example.
The semantic value of `statement` is also a pointer to `node_t`.
- The function `tree1` generates tree node.



### Epilogue
