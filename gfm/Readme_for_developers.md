# How to build Gtk4 Tutorial

## Quick start guide

1. You need linux operationg system, ruby, rake, pandoc and latex system.
2. download this repository and uncompress the files.
3. change your current directory to the top directory of the source files.
4. type `rake html` to create html files. The files are generated under `html` directory.
5. type `rake pdf` to create pdf file. The file is generated under `latex` directory.

## Prerequisites

- Linux operationg system.
The programs in the repository has been tested on Ubuntu 2.04.
- Download the files in this repository.
There are two ways to download.
  1. Use git.
Type `git clone https://github.com/ToshioCP/Gtk4-tutorial.git` on the command-line.
  2. Download a zip file.
Click on the code button (green button) in the top page of this repository.
Then, click "Download ZIP".
- Ruby and rake.
- Pandoc. It is used to convert markdown to html and latex.
- Latex system. Texlive2020 or later version is recommended.
It is used to generate pdf file.

## Github flavored markdown

When you see [gtk4_tutorial github page](https://github.com/ToshioCP/Gtk4-tutorial), you'll find the contents of `Readme.md` below the list of files.
This file is written in markdown language.
A markdown file has `.md` suffix.

There are several kinds of markdown language.
`Readme.md` uses 'github flavored markdown', which is often shortened as GFM.
Markdown files in the `gfm` directory also written in GFM.
If you are not familiar with it, refer to the page [github flavor markdown spec](https://github.github.com/gfm/).

## Pandoc's markdown

This tutorial also uses another markdown -- pandoc's markdown.
Pandoc is a converter between markdown, html, latex, word docx and so on.
This type of markdown is used to convert markdown to html and latex.

## Src.md file

Src.md file has ".src.md" suffix.
The syntax of .src.md file is similar to markdown but it has a special command which isn't included in markdown syntax.
It is @@@ command.
The command starts with a line that begins with "@@@" and it ends with a line "@@@".
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

This command replaces itself with the text read from the C source files surrounded by `@@@include` and `@@@`.
If a function list follows the filename, only the functions are read.
If no function list is given, the command can read any text file other than C source file.

    @@@include
    tfeapplication.c main startup
    @@@

The command above is replaced by the contents of `main` and `startup` functions in `tfeapplication.c`.

    @@@include
    lib_src2md.rb
    @@@

This command is replaced by the contents of `lib_src2md.rb` which is a ruby script (not C file).

The inserted text is converted to fence code block.
Fence code block begins with `~~~` and ends with `~~~`.
The contents are displayed verbatim.
`~~~` is look like a fence so the block is called "fence code block".

If the target markdown is GFM, then an info string follows the beginning fence.
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

Info strings are usually languages like C, ruby, xml and so on.
This string is decided with the filename extension.

- `.c`   => C
- `.rb`  => ruby
- `.xml` => xml

The supported language is written in line 290 and 291 in `lib/lib_src2md.rb`.

A line number is inserted at the top of each line in the code block.
If you don't want to insert it, give "-N" option to @@@include command.

Options:

- `-n`: Inserts a line number at the top of each line (default).
- `-N`: No line number is inserted.

The following shows that line numbers are inserted at the beginning of lines.

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
If `-N` option is given, then the info string is `{.C}` only.

If the target markdown is an intermediate file to latex, then the same info string follows the beginning fence.

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
This command is similar to "#if", "#elif", #else" and "#endif" directives in C preprocessor.
For example,

    @@@if gfm
    Refer to  [tfetextview API reference](tfetextview_doc.md)
    @@@elif html
    Refer to  [tfetextview API reference](../src/tfetextview/tfetextview_doc.src.html)
    @@@elif latex
    Refer to tfetextview API reference in appendix.
    @@@end

`@@@if` and `@@@elif` have conditions.
They are `gfm`, `html` or `latex` so far.

- gfm: if the target is GFM
- html: if the target is html
- latex: if the target is latex (or pdf).

Other type of conditions may be available in the future version.

## Conversion

The @@@ commands above (@@@include, @@@shell and @@@if series) are carried out by `src2md.rb`.
In addition, some other conversions are made by `src2md.rb`.

- Relative links are changed according to the change of the base directory.
- Size option in image link is removed when the destination is GFM or html.
- Relative link is removed when the destination is latex.

There's a method `src2md` in the `lib/lib_src2md.rb`.
This method converts src.md file into md file.
The script `src2md.rb` just invokes this method.
In the same way, the method is called in the action in `Rakefile`.

The code analyzing @@@if series command is rather complicated.
It is based on the state diagram below.

![state diagram](../image/state_diagram.png)

## mktbl.rb script

The fourth @@@ command begins with "@@@table".
The contents of this command is a table of GFM or pandoc's markdown.
The script `mktbl.rb` in `src` directory makes a table easy to read.
For example, a text file `sample.md` has a table like this:

    Price list

    @@@table
    |item|price|
    |:---:|:---:|
    |mouse|$10|
    |PC|$500|
    @@@

Run the script.

~~~
$ cd src
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

The script makes a backup file `sample.md.bak`.

The task of the script seems easy, but the program is not so simple.
The script `mktbl.rb` uses a library `lib/lib_mktbl.rb`
This script is independent from `src2md.rb`.

## Directory structure

There are six directories under `gtk4_tutorial` directory.
They are `gfm`, `src`, `image`, `html`, `latex` and `lib`.
Three directories `gfm`, `html` and `latex` are the destination directories for GFM, html and latex files respectively.
It is possible that these three directories don't exist before the conversion.

- src: This directory contains .src.md files and C-related source files.
- image: This directory contains image files like png or jpg.
- gfm: `rake` converts src.md files to GFM files and store them in this directory.
- html: This directory is empty at first. `rake html` will convert src.md files to html files and store them in this directory.
- latex: This directory is empty at first. `rake latex` will convert src.md files to latex files and store them in this directory.
`rake pdf` creates pdf file in `latex` directory.
- lib: This directory includes ruby library files.
 
## Src directory and the top directory

Src directory contains .src.md files and C-related source files.
The top directory, which is gtk\_tutorial directory, contains `Rakefile`, `src2md.rb` and some other files.
When `Readme.md` is generated, it will be located at the top directory.
`Readme.md` has title, abstract, table of contents with links to GFM files.

Rakefile describes how to convert .src.md files into GFM files.
Rake carries out the conversion according to the `Rakefile`.

## The name of files in src directory

Each file in `src` directory is an abstract, sections of the whole document and other .src.md files.
An `abstract.src.md` contains the abstract of this tutorial.
Each section filename is "sec", number of the section and ".src.md" suffix.
For example, "sec1.src.md", "sec5.src.md" or "sec12.src.md".
They are the files correspond to section 1, section 5 and section 12 respectively.

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

Sometimes you might want to insert a section.
For example, you want to insert it between section 4 and section 5.
You can make a temporary section 4.5, that is a rational number between 4 and 5.
However, section numbers are usually integer so section 4.5 must be changed to section 5.
And the numbers of the following sections must be increased by one.

This renumbering is done by a method `renum` of the class `Sec_files`.
The method and class is written in `lib/lib_sec_file.rb`.

- It changes file names.
- If there are references to sections in .src.md files, the section numbers will be automatically renumbered.

## Rakefile

Rakefile is a similar file to Makefile but controlled by rake, which is a make-like program written in ruby.
Rakefile in this tutorial has the following tasks.

- md: generate GFM markdown files. This is the default.
- html: generate html files.
- pdf: generate latex files and a pdf file, which is compiled by lualatex.
- latex: generate latex files.
- all: generate md, html, latex and pdf files.

Rake does renumbering before the tasks above.

## Generate GFM markdown files

Markdown files (GFM) are generated by rake.

    $ rake

This command generates `Readme.md` with `src/abstract.src.md` and titles of .src.md files.
At the same time, it converts each .src.md file into GFM file under `gfm` directory.
Navigation lines are added at the top and bottom of each markdown section file.

You can describe width and height of images in .src.md files.
For example,

    ![sample image](../image/sample_image.png)

The size between left brace and right brace is used in latex file and it is not fit to GFM syntax.
So the size is removed in the conversion.

If a .src.md file has relative URL link, it will be changed by conversion.
Because .src.md files are located under `src` directory and GFM files are located under `gfm` directory, base URL of GFM files is different from base URL of .src.md files.
For example, `[src/sample.c](../src/sample.c)` is translated to `[src/sample.c](../src/sample.c)`.

If a link points another .src.md file, then the target filename will be changed to .md file.
For example, `[Section 5](sec5.md)` is translated to `[Section 5](../src/sec5.md)`.

If you want to clean the directory, that means remove all the generated markdown files, type `rake clean`.

    $ rake clean

Sometimes this is necessary before generating GFM files.

    $ rake clean
    $ rake

For example, if you append a new section and other files are still the same as before, `rake clean` is necessary.
Because the navigation of the previous section of the newly added section needs to be updated.
If you don't do `rake clean`, then it won't be updated because the the timestamp of .md file in gfm is newer than the one of .src.md file.
In this case, using `touch` to the previous section .src.md also works to update the file.

If you see the github repository (ToshioCP/Gtk4-tutorial), `Readme.md` is shown below the code.
And `Readme.md` includes links to each markdown files.
The repository not only stores source files but also shows the whole tutorial.

## Generate html files

Src.md files can be translated to html files.
You need pandoc to do this.
Most linux distribution has pandoc package.
Refer to your distribution document to install it.

Type `rake html` to generate html files.

    $ rake html

First, it generates pandoc's markdown files under `html` directory.
Then, pandoc converts them to html files.
The width and height of image files are removed.

`index.html` is the top html file.
If you want to clean `html` directory, type `rake cleanhtml`

    $ rake cleanhtml

Every html file has stylesheet in its header.
This is created by `lib/lib_add_head_tail_html.rb`.
This script has a sample markdown code and convert it with pandoc and `-s` option.
Pandoc generates a html file with header.
The script extracts the header and use it for html files.
You can customize the style by modifying `lib/lib_add_head_tail_html.rb`.

`html` directory contains all the necessary html files.
So if you want to upload the html files to your own web site, just upload the files in the `html` directory.

## Generate latex files and a pdf file

You need pandoc to convert .src.md files to latex source files.

Type `rake latex` to generate latex files.

    $ rake latex

First, it generates pandoc's markdown files under `latex` directory.
Then, pandoc converts them to latex files.
Links to files or directories are removed because latex doesn't support them.
However, links to full URL are kept.
Image size is set with the size between the left brace and right brace.

    ![sample image](../image/sample_image.png)

You need to specify appropriate width and height.
It is almost `0.015 x pixels` cm.
For example, if the width of an image is 400 pixels, the width in a latex file will be almost 6cm.

`main.tex` is the top latex file.
It is a root file and it includes each section file between `\begin{document}` and `\end{document}`.
It also includes `helper.tex` in its preamble.
`main.tex` and `helper.tex` is created by `lib/lib_gen_main_tex.rb`.
It has a sample markdown code and convert it witn pandoc and `-s` option.
Pandoc generates preamble.
`lib/lib_gen_main_tx.rb` extracts the preamble and put a part of it into `helper.tex`.
You can customize `helper.tex` by modifying `lib/lib_gen_main_tex.rb`.

You can generate pdf file by typing `rake pdf`.

    $ rake pdf

This does `rake latex` and compiles the latex files with lualatex.

If you want to clean `latex` directory, type `rake cleanlatex`

    $ rake cleanlatex

This removes all the latex source files and a pdf file.

