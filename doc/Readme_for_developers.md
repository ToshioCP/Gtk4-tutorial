# How to build Gtk4 Tutorial

## Prerequisites

- To clone the repository, git is necessary. Most distribution has its package.
- Ruby and rake.
- To generate html and latex files, pandoc is necessary.
- To make pdf, latex system is necessary. Texlive2020 or later version is recommended.

## Clone the repository

type the following command.

    $ git clone https://github.com/ToshioCP/Gtk4-tutorial.git

## Github flavored markdown

When you see [gtk4_tutorial github page](https://github.com/ToshioCP/Gtk4-tutorial), you'll find `Readme.md` contents below the list of files.
This file is written in markdown language, of which the file has `.md` suffix.

There are several kinds of markdown language.
`Readme.md` uses 'github flavored markdown', which is often shortened as GFM.
Markdown files in the top directory also written in GFM.
If you are not familiar with it, refer the website [github flavor markdown spec](https://github.github.com/gfm/).

## Pandoc's markdown

This tutorial uses another markdown -- pandoc's markdown.
Pandoc is a converter between markdown, html, latex, word docx and so on.
This type of markdown is used to generate html and latex files in this tutorial.

## Src.md file

Src.md is similar to markdown but it has a command which isn't included in markdown syntax.
It is @@@ command.
The command starts at a line begins with "@@@" and ends at a line begins with "@@@".
For example, 

~~~
@@@include
tfeapplication.c
@@@
~~~

There are three types of @@@ command.

### @@@include

This type of @@@ command starts with a line begins "@@@include".

~~~
@@@include
tfeapplication.c
@@@
~~~

This command replaces itself with the text read from the C source file.
If a function list follows the filename, only the functions in the C source file are read.
If no function list is given, the command can read any text files even it is not C source file.

~~~
@@@include
tfeapplication.c main startup
@@@
~~~

~~~
@@@include
lib_src2md.rb
@@@
~~~

The inserted text is converted to fence code block.
If the target markdown is GFM, then an info string follows the beginning fence.
The following example shows the command includes a C source file "sample.c".

    $ cat src/sample.c
    int
    main (int argc, char **argv) {
      ... ...
    }
    $cat src/sample.src.md
      ... ...
    @@@include -N
    sample.c
    @@@
      ... ...
    $ ruby src2md.rb src/sample.src.md gfm/sample.md
    $ cat gfm/sample.md
      ... ...
    ~~~C
    int
    main (int argc, char **argv) {
      ... ...
    }
    ~~~
      ... ...

Info strings are usually language like C, ruby, xml and so on.
This string is decided with the filename extension.

A line number is inserted at the top of each line in the code block.
If you don't want to insert it, give "-N" option to @@@include command.

Options:

- "-n": Inserts a line number at the top of each line (default).
- "-N": No line number is inserted.

    $cat src/sample.src.md
      ... ...
    @@@include
    sample.c
    @@@
      ... ...
    $ ruby src2md.rb src/sample.src.md gfm/sample.md
    $ cat gfm/sample.md
      ... ...
    ~~~C
     1 int
     2 main (int argc, char **argv) {
      ... ...
    14 }
    ~~~
      ... ...

If the target markdown is an intermediate file to html, then another type of info string follows the beginning fence.
If @@@include command doesn't have -N option, then the generated markdown is:

    ~~~{.C .numberLines}
    int
    main (int argc, char **argv) {
      ... ...
    }
    ~~~
The info string `.C` specifies C language.
The info string `.numberLines` is a class of the pandoc markdown.
If the class is given, pandoc generates CSS to insert line numbers to the source code in the html file.
That's why the fence code block in the markdown doesn't have line numbers, which is different from gfm markdown.
If "-N" option is given, then the info string is `{.C}` only.

If the target markdown is an intermediate file to latex, then an info string follows the beginning fence.

    ~~~{.C .numberLines}
    int
    main (int argc, char **argv) {
      ... ...
    }
    ~~~

Rake uses pandoc with --listings option when it converts markdown to latex.
The generated latex file uses listings package to list source files instead of verbatim environment.
The markdwon above is converted to the following latex source file.

    \begin{lstlisting}[language=C, numbers=left]
    int
    main (int argc, char **argv) {
      ... ...
    }
    \end{lstlisting}

Listing package can color or emphasize keywords, strings, comments and directives.
But it doesn't analyze the syntax or token of the language, so the kind of emphasis target is limited.

@@@include command have two advantages.

1. Less typing.
2. You don't need to modify your src.md file, even if the C source file is modified.

### @@@shell

This type of @@@ command starts with a line begins "@@@shell".

~~~
@@@shell
shell command
 ... ...
@@@
~~~

This command replaces itself with:

- the shell command
- the standard output from the shell command

For example,

~~~
@@@shell
wc Rakefile
@@@
~~~

This is converted to:

~~~
$ wc Rakefile
164  475 4971 Rakefile
~~~

### @@@if series

This type of @@@ command starts with a line begins "@@@if", "@@@elif", "@@@else" or "@@@end".
This command is similar to "#if", "#elif", #else" and "#end" directives in C preprocessor.
For example,

~~~
@@@if gfm
Refer to  [tfetextview API reference](tfetextview/tfetextview_doc.md)
@@@elif html
Refer to  [tfetextview API reference](tfetextview_doc.html)
@@@elif latex
Refer to tfetextview API reference in appendix.
@@@end
~~~

Conditions follow "@@@if" or "@@@elif".
They are "gfm", "html" or "latex" so far.
Other words or expressions may be available in the future version.

## Conversion

The @@@ commands are carried out by `src2md.rb`.
In addition, some conversion is made by `src2md.rb`.

- Relative links are changed according to the change of base directory.
- Size option in image link is removed when the destination is GFM or html.
- Relative link is removed when the destination is latex.
- Lines in fence code block are folded when the destination is latex.

There's a method `src2md` in the `lib/lib_src2md.rb`.
This method converts src.md file into md file.
The script `src2md.rb` just invokes this method.
In the same way, the method is called in the action in `Rakefile`.

The code analyzing @@@if series command in the method is rather complicated.
It is based on the state diagram below.

![state diagram](../image/state_diagram.png)

## mktbl.rb script

The script "mktbl.rb" is in `src` directory.
This script makes a table easy to read.
For example, a text file "sample.md" has a table like this:

~~~
Price list

|item |price|
|:---:|:---:|
|mouse| $10 |
| PC  |$500 |
~~~

Run the script.

~~~
$ ruby mktbl.rb sample.md
~~~

Then, the file is changed to:

~~~
Price list

|item |price|
|:---:|:---:|
|mouse| $10 |
| PC  |$500 |
~~~

The script makes a backup file "sample.md.bak".

The task of the script seems easy, but the program is not so simple.
The script "mktbl.rb" uses a library script "lib/lib\_mktbl.rb"
This script is independent from "src2md.rb".

## Directory structure

There are six directories under `gtk4_tutorial` directory.
They are `gfm`, `src`, `image`, `html`, `latex` and `lib`.
Three directories `gfm`, `html` and `latex` are the destination directories for GFM, html and latex files respectively.
It is possible that these three directories don't exist before the conversion.

- src: This directory contains src.md files and C-related source files.
- image: This directory contains image files like png or jpg.
- gfm: This directory is empty at first. A ruby script will convert src.md files to GFM files and store them in this directory.
- html: This directory is empty at first. A ruby script will convert src.md files to html files and store them in this directory.
- latex: This directory is empty at first. A ruby script will convert src.md files to latex files and store them in this directory.
- lib: This directory includes ruby library files.
 
## Src and top directories

Src directory contains src.md files and C-related source files.
The top directory, which is gtk\_tutorial directory, contains `Rakefile`, `Readme_for_developers.md` and some other files.
`Readme.md` is generated and located at the top directory by rake.
`Readme.md` has title, abstract, table of contents and links to GFM files,
which rake also generates under `gfm` directory.

    $ rake

Rakefile describes how to convert src.md files into GFM files.
Rake carries out the conversion according to the instruction written in Rakefile.

## The name of files in src directory

Each file in src directory is an abstract or section of the whole document.
The name of the abstract file is "abstract.src.md".
The name of the section files are "sec", number of the section and ".src.md" suffix.
For example, "sec1.src.md", "sec5.src.md" or "sec12.src.md".
They are the files correspond to section 1, section 5 and section 12 respectively.

## C source file directory

Src.md files might have @@@ commands and they include C source files.
Such C source files are located in the src directory or its subdirectories.

Usually, those C files are compiled and tested.
At that time, some auxiliary files and target file like a.out are generated.
If you locate the C source files under src directory, those temporary files make the directory messy.
Therefore, It is a good idea to make subdirectories under src directory and put each C source file under the corresponding subdirectory.

The name of the subdirectories should be independent of section names.
It is because of renumbering, which will be explained in the next subsection.

## Renumbering

Sometimes you want to insert a section.
For example, inserting it between section 4 and section 5.
You can make a temporary section 4.5, that is a rational number between 4 and 5.
However, section numbers are usually integer so it must change to section 5 and the numbers of following sections also must be added by one.

This renumbering is done by a method `renum` of the class `Sec_files`.

- It changes file names.
- If there are references to sections in src.md files, the section numbers will be automatically renumbered.

## Rakefile

Rakefile is a similar file to Makefile but controlled by rake, which is a make-like program written in ruby.
Rakefile has the following tasks.

- md: generate GFM markdown files. This is the default.
- html: generate html files.
- pdf: generate latex files and a pdf file, which is generated by pdflatex.
- latex: generate latex files.
- all: generate md, html, latex and pdf files.

Rake does renumbering before the tasks above.

## Generate GFM markdown files

Markdown files (GFM) are generated by rake.

    $ rake

This command generates `Readme.md`, which has no original src.md file.
At the same time, it converts each .src.md file into GFM file under `gfm` directory.
When translated, it is added a navigation line at the top and bottom.

You can describe width and height of images in .src.md files.
For example,

    ![sample image](../image/sample_image.png){width=10cm height=6cm}

The size between left brace and right brace is used in latex file and it is not fit to GFM syntax.
So the size is removed in the conversion above.

If a src.md file has relative URL link, it will be changed by conversion.
Because src.md files are located under `src` directory and GFM files are located under `gfm` directory, base URL of GFM files is different from base URL of src.md files.
For example, `[Sample.c](sample.c)` is translated to `[Sample.c](../src/sample.c)`.

If a link points another src.md file, then the target filename will be changed to .md file.
For example, `[Section 5](sec5.src.md)` is translated to `[Section 5](sec5.md)`.

If you want to clean the directory, that means remove all the generated markdown files, type `rake clean`.

    $ rake clean

If you see the github repository (ToshioCP/Gtk4-tutorial), `Readme.md` is shown below the list of the top directory.
And `Readme.md` includes links to each markdown files.
The repository not only stores source files but also shows the tutorial in it.

## Generate html files

Src.md files can be translated to html files.
You need pandoc to do this.
Most linux distribution has pandoc package.
Refer to your distribution document to install it.

Type `rake html` to generate html files.

    $ rake html

First, it generates pandoc's markdown files under `html` directory.
Then, pandoc converts them to html files.
The description of the width and height of image files is removed.

`index.html` is the top html file.
If you want to clean `html` directory, type `rake cleanhtml`

    $ rake cleanhtml

Every html file has stylesheet in its header.
This comes from `header` string in `Rakefile`.
You can customize the style by modifying `Rakefile`.

## Generate latex files and a pdf file

Src.md files can be translated to latex files.
You need pandoc to do this.

Type `rake latex` to generate latex files.

    $ rake latex

First, it generates pandoc's markdown files under `latex` directory.
Then, pandoc converts them to latex files.
Links to files or directories are removed because latex doesn't support them.
However, links to full URL are kept.

`main.tex` is the top latex file.
If you want to clean `latex` directory, type `rake cleanlatex`

    $ rake cleanlatex

You can customize `main.tex` and `helper.tex`, which describes preamble, by modifying `Rakefile`.

You can generate pdf file by typing `rake pdf`.

    $ rake pdf

