Up: [Readme.md](../Readme.md),  Prev: [Section 22](sec22.md)

# Tiny turtle graphics interpreter

A program `turtle` is an example with the combination of TfeTextView and GtkDrawingArea objects.
It is a very small interpreter but it provides a way to draw fractal curves.
The following diagram is a Koch curve, which is a famous example of fractal curves.

![Kocho curve](../src/turtle/image/turtle_koch.png)

This program uses flex and bison.
Flex is a lexical analyzer.
Bison is a parser generator.
These two programs are similar to lex and yacc which are proprietary software developed in Bell Laboratry.
However, flex and bison are open source software.
I will write about how to use those software, but they are not topics about gtk.
So, readers can skip that part of this sections.

## How to use turtle

The documentation of turtle is [here](../src/turtle/turtle_doc.md).
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
The source code in turtle language is located at [src/turtle/example](../src/turtle/example) directory.
You can read these files by clicking on the `Open` button.

## Combination of TfeTextView and GtkDrawingArea objects

Turtle uses TfeTextView and GtkDrawingArea.
It is similar to `color` program in the previous section.

The body of the interpreter is written with flex and bison.
The codes are not thread safe.
So the handler of "clicked" signal of the `Run` button prevents from reentering.

~~~C
 1 void
 2 run_cb (GtkWidget *btnr) {
 3   GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
 4   GtkTextIter start_iter;
 5   GtkTextIter end_iter;
 6   char *contents;
 7   int stat;
 8   static gboolean busy = FALSE;
 9 
10   /* yyparse() and run() are NOT thread safe. */
11   /* The variable busy avoids reentry. */
12   if (busy)
13     return;
14   busy = TRUE;
15   gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
16   contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
17   if (surface) {
18     init_flex (contents);
19     init_parse ();
20     stat = yyparse ();
21 #ifdef debug
22   g_print ("yyparse returned %d.\n", stat);
23 #endif
24     if (stat == 0) /* No error */ {
25       run ();
26     }
27     finalize_flex ();
28   }
29   gtk_widget_queue_draw (GTK_WIDGET (da));
30   busy = FALSE;
31 }
32 
33 static void
34 resize_cb (GtkDrawingArea *drawing_area, int width, int height, gpointer user_data) {
35   if (surface)
36     cairo_surface_destroy (surface);
37   surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
38 }
~~~

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
The codes of `turtleapplication.c` is in the [turtle directory](../src/turtle).

## What does the interpreter do?

Suppose that the turtle runs with the following program.
The following is the description how turtle recognizes the program and does the work.

~~~
distance = 100
fd distance*2
~~~

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

`yylex` returns a token kind every time, but it doesn't set `yylval.ID` or `yylval.NUM` every time.
It is because keywords (`FD`) and symbols (`=` and `*`) don't have any semantic values.
The function `yylex` is called lexical analyzer or scanner.
- `turtle` makes a tree structured data.
This part of `turtle` is called parser.

![turtle parser tree](../src/turtle_parser_tree_png)

- `turtle` analyzes the tree and executes it.
This part of `turtle` is called runtime routine.
The tree consists of line segments and rectangles between line segments.
The rectangles are called nodes.
For example, N\_PROGRAM, N\_ASSIGN, N\_FD and N\_MUL are nodes.
  1. Goes down from N\_PROGRAM to N\_ASSIGN.
  2. At N_ASSIGN node, `turtle` checks if the first child is ID, that means the left side is a variable.
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
Looks for the varable distance in the variable table and gets the value 100.
The second child is a number 2.
Multiplies 100 by 2 and gets 200.
Then `turtle` goes back to N_FD.
  6. Now `turtle` knows the distance is 200.
It moves the cursor forward by 200.
  8. There are no node follows.
Runtime routine returns to the main routine.
-`turtle` draws GtkDrawingArea then stops.

Actually most programs are more complicated than the example above.
So, `turtle` does much more work to interpret programs.
However, basically it works by the same way above.
Interpritation consists of three parts.

- Lexical analysis
- Syntax Parsing and tree generation
- Execution by the tree

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
2. bison compiles `turtle.y` to `turtle\_parser.c` and generates `turtle\_parser.h`
3. flex compiles `turtle.lex` to `turtle\_lex.c`.
4. gcc compiles `application.c`, `resources.c`, `turtle\_parser.c` and `turtle\_lex.c` with `turtle.h`, `resources.h` and `turtle\_parser.h`.
It generates an executable file `turtle`.

![compile process](../image/turtle_compile_process.png)

Meson controls the process and the instruction is described in meson.build.

~~~meson
 1 project('turtle', 'c')
 2 
 3 compiler = meson.get_compiler('c')
 4 mathdep = compiler.find_library('m', required : true)
 5 
 6 gtkdep = dependency('gtk4')
 7 
 8 gnome=import('gnome')
 9 resources = gnome.compile_resources('resources','turtle.gresource.xml')
10 
11 flex = find_program('flex')
12 bison = find_program('bison')
13 turtleparser = custom_target('turtleparser', input: 'turtle.y', output: ['turtle_parser.c', 'turtle_parser.h'], command: [bison, '-d', '-o', 'turtle_parser.c', '@INPUT@'])
14 turtlelexer = custom_target('turtlelexer', input: 'turtle.lex', output: 'turtle_lex.c', command: [flex, '-o', '@OUTPUT@', '@INPUT@'])
15 
16 sourcefiles=files('turtleapplication.c', '../tfetextview/tfetextview.c')
17 
18 executable('turtle', sourcefiles, resources, turtleparser, turtlelexer, turtleparser[1], dependencies: [mathdep, gtkdep], export_dynamic: true, install: true)
19 
~~~

- 3: Gets C compiler.
It is usually `gcc` in linux.
- 4: Gets math library.
This program uses trigonometry functions.
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
- 18: Compiles C source files including generated files by glib^compile-resources, bison and flex.
The argument `turtleparser[1]` refers to `tirtle_parser.h` which is the secound output in the line 13.

## Turtle.lex

This subsection and the following subsection are description about flex and bison.
If you want to focus on gtk4, you can skip these subsections.

### What does flex do?

Flex creates lexical analizer from flex source file.
Flex source file is a text file and its syntactic rule will be explained later.
Generated lexical analyzer is a C source file.
It is also called scanner.
It reads a text file, which is a source file of a program language, and gets variable names, numbers and symbols.
Suppose here is a text file.

~~~
fc (1,0,0) # Foreground color is red, rgb = (1,0,0).
pd         # Pen down.
distance = 100
angle = 90
fd distance    # Go forward by distance (100) pixels.
tr angle     # Turn right by angle (90) degrees.
~~~

The content of the textfile is separated into `fc`, `(`, `1`, `,` and so on.
The words `fc`, `pd`, `distance` and `100` are called token.
The characters `(`, `<` and `=` are called symbol.
( Sometimes those symbols called token, too.)

A scanner has `yylex` function and `yytext` variable.
The function returns a type of token and set `yytext` to contain the name of the token.
Each time `yylexc` is called, it returns the followings.


Turtle.lex isn't so big program.

~~~lex
 1 %top{
 2 #include <stdlib.h>
 3 #include "turtle.h"
 4 
 5   static int nline = 1;
 6   static int ncolumn = 1;
 7   static void get_location (char *text);
 8 
 9   /* Dinamically allocated memories are added to the single list. They will be freed in the finalize function. */
10   extern GSList *list;
11 }
12 
13 %option noyywrap
14 
15 DIGIT [0-9]
16 REAL_NUMBER (0|[1-9][0-9]*)(\.[0-9]+)?
17 IDENTIFIER [a-zA-Z][a-zA-Z0-9]*
18 %%
19   /* rules */
20 #.*\n             nline++; ncolumn = 1; /* comment */
21 #.*               ;                     /* comment in the last line */
22 [ ]               ncolumn++;
23 \t                ncolumn += 8; /* assume that tab is 8 spaces. */
24 \n                nline++; ncolumn = 1;
25   /* reserved keywords */
26 pu                get_location (yytext); return PU; /* pen up */
27 pd                get_location (yytext); return PD; /* pen down */
28 pw                get_location (yytext); return PW; /* pen width = line width */
29 fd                get_location (yytext); return FD; /* forward */
30 tr                get_location (yytext); return TR; /* turn right */
31 bc                get_location (yytext); return BC; /* background color */
32 fc                get_location (yytext); return FC; /* foreground color */
33 dp                get_location (yytext); return DP; /* define procedure */
34 if                get_location (yytext); return IF; /* if statement */
35 rt                get_location (yytext); return RT; /* return statement */
36 rs                get_location (yytext); return RS; /* reset the status */
37   /* constant */
38 {REAL_NUMBER}     get_location (yytext); yylval.NUM = atof (yytext); return NUM;
39   /* identifier */
40 {IDENTIFIER}      { get_location (yytext); yylval.ID = g_strdup(yytext);
41                     list = g_slist_prepend (list, yylval.ID);
42                     return ID;
43                   }
44 "="               get_location (yytext); return '=';
45 ">"               get_location (yytext); return '>';
46 "<"               get_location (yytext); return '<';
47 "+"               get_location (yytext); return '+';
48 "-"               get_location (yytext); return '-';
49 "*"               get_location (yytext); return '*';
50 "/"               get_location (yytext); return '/';
51 "("               get_location (yytext); return '(';
52 ")"               get_location (yytext); return ')';
53 "{"               get_location (yytext); return '{';
54 "}"               get_location (yytext); return '}';
55 ","               get_location (yytext); return ',';
56 .                 ncolumn++;             return YYUNDEF;
57 %%
58 
59 static void
60 get_location (char *text) {
61   yylloc.first_line = yylloc.last_line = nline;
62   yylloc.first_column = ncolumn;
63   yylloc.last_column = (ncolumn += strlen(text)) - 1;
64 }
65 
66 static YY_BUFFER_STATE state;
67 
68 void
69 init_flex (const char *text) {
70   state = yy_scan_string (text);
71 }
72 
73 void
74 finalize_flex (void) {
75   yy_delete_buffer (state);
76 }
77 
~~~




## Turtle.y source file compiled by bison


Up: [Readme.md](../Readme.md),  Prev: [Section 22](sec22.md)
