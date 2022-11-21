# How to build Gtk4-Tutorial

## Quick start guide

1. You need linux operationg system, ruby, rake, pandoc and latex system.
2. download this repository and uncompress the file.
3. change your current directory to the top directory of the source files.
4. type `rake html` to create html files. The files are created under the `docs` directory.
5. type `rake pdf` to create pdf a file. The file is created under the `latex` directory.

## Prerequisites

- Linux operationg system.
The programs in this repository has been tested on Ubuntu 21.04.
- Download the files in the repository.
There are two ways to download.
  1. Use git.
Type `git clone https://github.com/ToshioCP/Gtk4-tutorial.git` on the command-line.
  2. Download a zip file.
Click on the `Code` button (green button) on the top page of the repository.
Then, click "Download ZIP".
- Ruby and rake.
- Pandoc. It is used to convert markdown to html and/or latex.
- Latex system. Texlive2020 or later version is recommended.
It is used to generate the pdf file.

## GitHub Flavored Markdown

When you see [gtk4_tutorial GitHub repository](https://github.com/ToshioCP/Gtk4-tutorial), you'll find the contents of the `Readme.md` file.
This file is written in markdown language.
Markdown files have `.md` suffix.

There are several kinds of markdown language.
`Readme.md` uses 'GitHub Flavored Markdown', which is often shortened as GFM.
Markdown files in the `gfm` directory are written in GFM.
If you are not familiar with it, refer to the page [GitHub Flavor Markdown spec](https://github.github.com/gfm/).

## Pandoc's markdown

This tutorial also uses another markdown -- 'pandoc's markdown'.
Pandoc is a converter between markdown, html, latex, word docx and so on.
This type of markdown is used to convert markdown to html and/or latex.

## .Src.md file

.Src.md file has ".src.md" suffix.
The syntax of .src.md file is similar to markdown but it has a special command which isn't included in markdown syntax.
It is @@@ command.
The command starts with a line begins with "@@@" and ends with a line "@@@".
For example,

    @@@include
    tfeapplication.c
    @@@

There are four types of @@@ command.

### @@@include

This type of @@@ command starts with a line "@@@include".

    @@@include
    tfeapplication.c
    @@@

This command replaces itself with the texts read from the C source files surrounded by `@@@include` and `@@@`.
If a function list follows the filename, only the functions are read.

    @@@include
    tfeapplication.c main startup
    @@@

The command above is replaced by the contents of `main` and `startup` functions in the file `tfeapplication.c`.

Other language's source files are also possible.
The following example shows that the ruby file 'lib_src2md.rb' is inserted by the command.

    @@@include
    lib_src2md.rb
    @@@

You can't specify functions or methods unless the file is C source.

The inserted text is converted to fence code block.
Fence code block begins with `~~~` and ends with `~~~`.
The contents are displayed verbatim.
`~~~` is look like a fence so the block is called "fence code block".

If the target markdown is GFM, then an info string can follow the beginning fence.
The following example shows that the @@@ command includes a C source file `sample.c`.

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
    $ ruby src2md.rb src/sample.src.md
    $ cat gfm/sample.md
      ... ...
    ~~~C
    int
    main (int argc, char **argv) {
      ... ...
    }
    ~~~
      ... ...

Info strings are usually languages like C, ruby, xml and so on.
This string is decided with the filename extension.

- `.c`   => C
- `.rb`  => ruby
- `.xml` => xml

The supported languages are written in the `lang` method in `lib/lib_src2md.rb`.

A line number will be inserted at the top of each line in the code block.
If you don't want to insert it, give "-N" option to @@@include command.

Options:

- `-n`: Inserts a line number at the top of each line (default).
- `-N`: No line number is inserted.

The following shows that the line numbers are inserted at the beginning of each line.

    $cat src/sample.src.md
      ... ...
    @@@include
    sample.c
    @@@
      ... ...
    $ ruby src2md.rb src/sample.src.md
    $ cat gfm/sample.md
      ... ...
    ~~~C
     1 int
     2 main (int argc, char **argv) {
      ... ...
    14 }
    ~~~
      ... ...

If a markdown is an intermediate file to html, another type of info string follows the fence.
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
If `-N` option is given, then the info string is `{.C}` only.

If a markdown is an intermediate file to latex, the same info string follows the beginning fence.

    ~~~{.C .numberLines}
    int
    main (int argc, char **argv) {
      ... ...
    }
    ~~~

Rake uses pandoc with --listings option to convert the markdown to a latex file.
The generated latex file uses 'listings package' to list source files instead of verbatim environment.
The markdown above is converted to the following latex source file.

    \begin{lstlisting}[language=C, numbers=left]
    int
    main (int argc, char **argv) {
      ... ...
    }
    \end{lstlisting}

Listing package can color or emphasize keywords, strings, comments and directives.
But it doesn't really analyze the syntax of the language, so the emphasis tokens are limited.

 @@@include command has two advantages.

1. Less typing.
2. You don't need to modify your .src.md file, even if the C source file is modified.

### @@@shell

This type of @@@ command starts with a line begins with "@@@shell".

    @@@shell
    shell command
     ... ...
    @@@

This command replaces itself with:

- the shell command
- the standard output from the shell command

For example,

    @@@shell
    wc Rakefile
    @@@

This is converted to:

    ~~~
    $ wc Rakefile
    164  475 4971 Rakefile
    ~~~

### @@@if series

This type of @@@ command starts with a line begins with "@@@if", and followed by "@@@elif", "@@@else" or "@@@end".
This command is similar to "#if", "#elif", #else" and "#endif" directives in the C preprocessor.
For example,

    @@@if gfm
    Refer to  [tfetextview API reference](tfetextview_doc.md)
    @@@elif html
    Refer to  [tfetextview API reference](tfetextview_doc.html)
    @@@elif latex
    Refer to tfetextview API reference in appendix.
    @@@end

`@@@if` and `@@@elif` have conditions.
They are `gfm`, `html` or `latex` so far.

- gfm: if the target is GFM
- html: if the target is html
- latex: if the target is pdf.

Other type of conditions may be available in the future version.

The code analyzing @@@if series command is rather complicated.
It is based on the state diagram below.

![state diagram](../image/state_diagram.png){width=15cm height=8.4cm}

### @@@table

This type of @@@ command starts with a line begins with "@@@table".
The contents of this command is a table of the GFM or pandoc's markdown.
The command makes a table easy to read.
For example, a text file `sample.md` has a table like this:

    Price list

    @@@table
    |item|price|
    |:---:|:---:|
    |mouse|$10|
    |PC|$500|
    @@@

The command changes this into:

~~~
Price list

|item |price|
|:---:|:---:|
|mouse| $10 |
| PC  |$500 |
~~~

This command just changes the appearance of the table.
There's no influence on html/latex files that is converted from the markdown.
Notice that the command supports only the above type of markdown table format.

A script `mktbl.rb` supports this command.
If you run the script like this:

~~~
$ ruby mktbl.rb sample.md
~~~

Then, the tables in 'sample.md' will be arranged.
The script also makes a backup file `sample.md.bak`.

The task of the script seems easy, but the program is not so simple.
The script `mktbl.rb` uses a library `lib/lib_src2md.rb`

@@@commands are effective in the whole text.
This means you can't stop the @@@commands.
But sometimes you want to show the commands literally like this document.
One solution is to add four blanks at the top of the line.
Then @@@commands are not effective because @@@commands must be at the top of the line.

## Conversion

The @@@ commands are carried out by a method `src2md`,
which is in the file `lib/lib_src2md.rb`.
This method converts `.src.md` file into `.md` file.
In addition, some other conversions are made by `src2md` method.

- Relative links are changed according to the change of the base directory.
- Size option in an image link is removed when the destination is GFM or html.
- Relative link is removed except .src.md files when the destination is html.
- Relative link is removed when the destination is latex.

The order of the conversions are:

1. @@@if
2. @@@table
3. @@@include
4. @@@shell
5. others

There is the `src2md.rb` file in the top directory of this repository.
It just invokes the method `src2md`.
In the same way, the method is called in the action in the `Rakefile`.

## Directory structure

There are seven directories under `gtk4_tutorial` directory.
They are `gfm`, `docs`, `latex`, `src`, `image`, `test` and `lib`.
Three directories `gfm`, `docs` and `latex` are the destination directories for GFM, html and latex files respectively.
It is possible that these three directories don't exist before the conversion.

- src: This directory contains .src.md files and C-related source files.
- image: This directory contains image files like png or jpg.
- gfm: `rake` converts .src.md files to GFM files and store them in this directory.
- docs: `rake html` will convert .src.md files to html files and store them in this directory.
- latex: `rake pdf` will convert .src.md files to latex files and store them in this directory.
Finally it creates a pdf file in `latex` directory.
- lib: This directory includes ruby library files.
- test: This directory contains test files.
The tests are carried out by typing `rake test` on the terminal.

## Src directory and the top directory

Src directory contains .src.md files and C-related source files.
The top directory, which is gtk\_tutorial directory, contains `Rakefile`, `src2md.rb` and some other files.
When `Readme.md` is generated, it will be located at the top directory.
`Readme.md` has title, abstract, table of contents with links to GFM files.

Rakefile describes how to convert .src.md files into GFM, html and/or pdf files.
Rake carries out the conversion according to the `Rakefile`.

## The name of files in src directory

Files in `src` directory are an abstract, sections of the document and other .src.md files.
An `abstract.src.md` contains the abstract of this tutorial.
Each section filename is "sec", number of the section and ".src.md" suffix.
For example, "sec1.src.md", "sec5.src.md" or "sec12.src.md".
They are the files correspond to the section 1, section 5 and section 12 respectively.

## C source file directory

Most of .src.md files have `@@@include` commands and they include C source files.
Such C source files are located in the subdirectories of `src` directory.

Those C files have been compiled and tested.
When you compile source files, some auxiliary files and a target file like `a.out` are created.
Or `_build` directory is made when `meson` and `ninja` is used when compiling.
Those files are not tracked by `git` because they are specified in `.gitignore`.

The name of the subdirectories should be independent of section names.
It is because of renumbering, which will be explained in the next subsection.

## Renumbering

Sometimes you might want to insert a new section.
For example, you want to insert it between section 4 and section 5.
You can make a temporary section 4.5, that is a rational number between 4 and 5.
However, section numbers are usually integer so section 4.5 must be changed to section 5.
And the numbers of the following sections must be increased by one.

This renumbering is done by the `renumber` method in the `lib/lib_renumber.rb` file.

- It changes file names.
- If there are references (links) to sections in .src.md files, the section numbers will be automatically renumbered.

## Rakefile

Rakefile is similar to Makefile but controlled by rake, which is a ruby script.
Rakefile in this tutorial has the following tasks.

- md: generate GFM markdown files. This is the default.
- html: generate html files.
- pdf: generate latex files and a pdf file, which is compiled by lualatex.
- all: generate md, html and pdf files.
- clean: delete latex intermediate files.
- clobber: delete all the generated files.

Rake does renumbering before the tasks above.

## Generate GFM markdown files

Markdown files (GFM) are generated by rake.

    $ rake

This command generates `Readme.md` with `src/abstract.src.md` and titles of each `.src.md` file.
At the same time, it converts each .src.md file into a GFM file under the `gfm` directory.
Navigation lines are added at the top and bottom of each markdown section file.

You can describe width and height of images in .src.md files.
For example,

    ![sample image](../image/sample_image.png){width=10cm height=6cm}

The size between left brace and right brace is used in latex file and it is not fit to GFM syntax.
So the size will be removed in the conversion.

If a .src.md file has relative URL links, they will be changed by conversion.
Because .src.md files are located under the `src` directory and GFM files are located under the `gfm` directory.
That means the base directory of the relative link are different.
For example, `[src/sample.c](sample.c)` is translated to `[src/sample.c](../src/sample.c)`.

If a link points another .src.md file, then the target filename will be changed to .md file.
For example, `[Section 5](sec5.src.md)` is translated to `[Section 5](sec5.md)`.

If you want to clean the directory, that means remove all the generated markdown files, type `rake clobber`.

    $ rake clobber

Sometimes this is necessary before generating GFM files.

    $ rake clobber
    $ rake

For example, if you append a new section and other files are still the same as before, `rake clobber` is necessary.
Because the navigation of the previous section of the newly added section needs to be updated.
If you don't do `rake clobber`, then it won't be updated because the the timestamp of .md file in gfm is newer than the one of .src.md file.
In this case, using `touch` to the previous section .src.md also works to update the file.

If you see the GitHub repository (ToshioCP/Gtk4-tutorial), `Readme.md` is shown below the code.
And `Readme.md` includes links to each markdown files.
The repository not only stores source files but also shows the whole tutorial.

## Generate html files

Src.md files can be translated to html files.
You need pandoc to do this.
Most linux distribution has pandoc package.
Refer to your distribution document to install it.

Type `rake html` to generate html files.

    $ rake html

First, it generates pandoc's markdown files under `docs` directory.
Then, pandoc converts them to html files.
The width and height of image files are removed.
Links to .src.md files will be converted like this.

    [Section 5](sec5.src.md) => [Section 5](sec5.html)

Image files are copied to `docs/image` direcotiry and links to them will be converted like this:

    [sample.png](../image/sample.png) => [sample.png](image/sample.png)

Other relative links will be removed.

`index.html` is the top html file.
If you want to clean html files, type `rake clobber` or `cleanhtml`.

    $ rake clobber

Every html file has a header (`<head> -- </head>`).
It is created by pandoc with '-s' option.
You can customize the output with your own template file for pandoc.
Rake uses `lib/lib_mk_html_template.rb` to create its own template.
The template inserts bootstrap CSS and Javascript through `jsDelivr`.

The `docs` directory contains all the necessary html files.
They are used in the [GitHub pages](https://ToshioCP.github.io/Gtk4-tutorial) of this repository.

So if you want to publish this tutorial on your own web site, just upload the files in the `docs` directory to your site.

## Generate a pdf file

You need pandoc to convert markdown files into latex source files.

Type `rake pdf` to generate latex files and finally make a pdf file.

    $ rake pdf

First, it generates pandoc's markdown files under `latex` directory.
Then, pandoc converts them into latex files.
Links to files or directories are removed because latex doesn't support them.
However, links to full URL and image files are kept.
Image size is set with the size between the left brace and right brace.

    ![sample image](../image/sample_image.png){width=10cm height=6cm}

You need to specify appropriate width and height.
It is almost `0.015 x pixels` cm.
For example, if the width of an image is 400 pixels, the width in a latex file will be almost 6cm.

A file `main.tex` is the root file of all the generated latex files.
It has `\input` commands, which inserts each section file, between `\begin{document}` and `\end{document}`.
It also has `\input`, which inserts `helper.tex`, in the preamble.
Two files `main.tex` and `helper.tex` are created by `lib/lib_gen_main_tex.rb`.
It has a sample markdown code and converts it witn `pandoc -s`.
Then, it extracts the preamble in the generated file and puts it into `helper.tex`.
You can customize `helper.tex` by modifying `lib/lib_gen_main_tex.rb`.

Finally, lualatex compiles the `main.tex` into a pdf file.

If you want to clean `latex` directory, type `rake clobber` or `rake cleanlatex`

    $ rake clobber

This removes all the latex source files and a pdf file.
