# Author Guide

## Quick Start Guide

This section describes the steps to view the GTK 4 Tutorial on your local machine.

1. You will need a Linux operating system (or a Linux environment like WSL on Windows), Ruby (ver 4.0.1), Rake, Pandoc, and a LaTeX system.
2. Download this repository and extract its contents. See the next section for instructions on how to download the repository.
3. Change your current directory to the root directory of the source files.
4. Run `bundle install` to install the required Gems. `bundle` is a command from Bundler, a Ruby library management tool included with Ruby.
5. To view the tutorial in a web browser on your local PC:
  - Run `bundle exec rake html` to generate HTML files for local use.
  - Run `bundle exec rake serve` and access `http://localhost:8000` in your browser to view the HTML files.
6. To create a PDF version of the tutorial:
  - Run `bundle exec rake pdf` to generate the PDF file.
  - The PDF file will be generated in the `pdf` directory.

## Recommended Environment and Software Setup

To generate the tutorial documents in your local repository, the following environment and software are required.
I have also included the versions used in my own environment for reference.

- Linux Operating System: The programs in this repository were tested on Ubuntu 24.04.3 LTS running via WSL (Windows Subsystem for Linux).
- Ruby, Rake, Pandoc, and LaTeX: These are used to generate the documentation.
  - Ruby: Version 4.0.1 is used. Ruby version 3 or later may work, but you may need to modify the Gemfile, .ruby-version, and mise.toml.
  - Pandoc: Version 3.1.3 was used, but the version provided by your Linux distribution's package manager is likely sufficient. Pandoc converts Markdown (Pandoc's Markdown) into HTML or PDF.
  - LaTeX: When Pandoc converts Markdown to PDF, it generates LaTeX as an intermediate step. A LaTeX system then converts that into a PDF. For this tutorial, TeXLive 2025 with LuaLaTeX (LuaHBTex 1.22.0 engine) was used.

There are two ways to download the repository to your local PC:
- Using Git: Run `git clone https://github.com/ToshioCP/Gtk4-tutorial.git` in your terminal.
- Downloading as a Zip: Click the green `Code` button on the repository's main page and select "Download ZIP." Extract the ZIP file after downloading.

## How to Write .src.md Files

The source files for this tutorial are located in the `src` directory and have the `.src.md` extension.
From here on, these will be referred to as ".src.md files."

### The Document Generation Process

Three types of documents are generated and stored in the `gfm`, `docs`, and `pdf` directories, corresponding to GFM (GitHub Flavored Markdown), HTML, and PDF formats.

These three output formats—`gfm`, `html`, and `pdf`—are referred to as targets.
Note that for the `html` target, the output directory is named `docs` instead of `html`.
This is to align with the directory name used for hosting files on GitHub Pages.

Conversion to a target can happen in two ways: direct generation (no intermediate steps) or indirect generation (using intermediate steps).

Direct Generation

- gfm target: .src.md => GFM

Indirect Generation

- html target: .src.md => Pandoc's Markdown => HTML
- pdf target: .src.md => Pandoc's Markdown => LaTeX => PDF

As seen in this process, the .src.md format is converted into either GFM or Pandoc's Markdown.
This conversion is handled by Ruby scripts.

GFM and Pandoc's Markdown are both extensions of the original Markdown.
Since Markdown has several such "dialects," they share common features but also have distinct differences.
While GFM and Pandoc's Markdown are identical in their basic syntax, they differ in areas such as how info strings and attributes are handled in fenced code blocks.

### The Two Components of .src.md

The content of a .src.md file is divided into two parts:

1. @@@ commands
2. Markdown: specifically, GFM or Pandoc's Markdown

Let’s look at a concrete example:

    This line belongs to "2. Markdown."
    
    @@@shell
    echo Hello World.
    @@@

    This line also belongs to "2. Markdown."

The three lines starting from `@@@shell` to `@@@` belong to "1. @@@ commands."

### @@@ Commands

@@@ commands are defined as blocks with the following structures:

- @@@if command: Start with `@@@if` and end with `@@@end`.
- @@@include command: Start with `@@@include` and end with `@@@`.
- @@@shell command: Start with `@@@shell` and end with `@@@`.
- @@@table command: Start with `@@@table` and end with `@@@`.

The table command (`@@@table`) isn't used in the conversion and will be explained in a later section. 

### The @@@if Command

This command begins with `@@@if`, optionally followed by `@@@elif` or `@@@else`, and ends with `@@@end`.
These work similarly to the C preprocessor directives `#if`, `#elif`, `#else`, and `#endif`.

`@@@if` and `@@@elif` are followed by a condition.
Conditions can be:

- One of the generation targets: gfm, html, or pdf.
- Two targets joined by the `|` (OR) operator. The condition is true if the generation target matches either one.

Example:

    @@@if gfm
    ```ruby
    @@@else
    ```{.ruby .numberLines}
    @@@end
    def hello
      print("Hello World.\n")
    end
    ```

This example resolves the difference in how fenced code blocks handle metadata between GFM and Pandoc's Markdown:

- In GFM, you can append an info string (usually the language name) after the backticks.
- In Pandoc's Markdown, you can append attributes enclosed in curly braces after the backticks. This allows for language specification as well as additional features like line numbering.

### @@@include

The `@@@include` command imports external files.

    @@@include
    sample.c
    @@@

In this example, the C source file `sample.c` specified between `@@@include` and the `@@@` marker is imported.
The source file path must be relative from the `src` directory.
If the content of `sample.c` is as shown below, the @@@include command is replaced by that C code.
To ensure it fits within the Markdown text, it is wrapped in a fenced code block.

```
#include <stdio.h>

void
hello() {
    printf("Hello World.\n");
}

int
main() {
    hello();
    return 0;
}
```

You can extract a specific function by adding the function name after the filename.

    @@@include
    sample.c hello
    @@@

This will only include the `hello` function from `sample.c`.
The resulting replacement will look like this (wrapped in a fenced code block, which is omitted here for clarity):

```
void
hello() {
    printf("Hello World.\n");
}
```

While you can import source files from other languages, function-level extraction is only supported for C source files.
Additionally, the C function extraction feature assumes the following formatting:

```
Return_Type
Function_Name(Arguments) {
  Function body
}
```

Specifically, the return type must be on the line immediately preceding the function name, and the function must end with a closing brace `}` at the beginning of a line.
If these conditions are not met, the function cannot be extracted correctly.

Inserted text is converted into a fenced code block, surrounded by lines starting with three backticks.

If the output target is `gfm`, an info string (language name) is added to the starting fence.
Using the previous C code example:

    @@@include
    sample.c hello
    @@@

Since the file extension `.c` indicates C, the info string will be `c`.
The resulting output becomes:

    ```c
    void
    hello() {
        printf("Hello World.\n");
    }
    ```

Info strings like c, ruby, or xml are determined by the file extension:

- `.c`   => c
- `.rb`  => ruby
- `.xml` => xml

A list of supported languages is defined in [/data/ext.yml](https://github.com/ToshioCP/Gtk4-tutorial/blob/main/data/ext.yml).
Note that this path starts with a slash to represent the repository root, not the operating system root.

If the output target is `html` or `pdf`, attributes of Pandoc's Markdown are added after the starting fence.

    @@@include
    sample.c hello
    @@@

This @@@ command is converted as follows:

    ```{.c .numberLines}
    void
    hello() {
        printf("Hello World.\n");
    }
    ```

These Pandoc's Markdown attributes provide both syntax highlighting for the C code and line numbers.
Line numbering is only available for `html` and `pdf` targets that use Pandoc's Markdown.
Unfortunately, GFM does not have a Markdown extension for line numbers, so only the code itself is output for that target.

The @@@include command must be written in exactly three lines (start marker, pathname with optional function names, end marker).
Consequently, you cannot include multiple files in a single command block.

### @@@shell

This command consists of the following three lines:

    @@@shell
    Shell command to execute
    @@@


Make sure your current directory is the src directory of the local repository.
All paths in the shell commands must be within src; any attempt to access files outside this directory will cause an error.

The three lines of the @@@shell command are replaced by the command line, its standard output, and the subsequent prompt.

For example:

    @@@shell
    wc author_guide.src.md
    @@@

This is converted into:

    ```
    $ wc author_guide.src.md
      509  2850 18698 author_guide.src.md
    $
    ```

No info strings or attributes are added to this fenced code block.

### How to Write Links

Links use URLs to direct the browser to a destination.
There are three ways to write a URL:

- Absolute URL: A URL starting with http or https. (e.g., `https://example.com/a/b/c.html`)
- Root-relative URL: A path starting with a slash, representing the root of the site. In the example above, the root-relative version would be `/a/b/c.html`.
- Relative URL: A path relative to the current URL. If the current page is `https://example.com/a/index.html`, the relative URL to `https://example.com/a/b/c.html` would be `b/c.html`. Note that a relative URL does not start with a slash.

#### External links

Use absolute URLs for links to external sites.
For example, the following sentence includes an external link to the GFM specification page:

```
The GFM documentation can be found in the [GitHub Flavored Markdown Spec](https://github.github.com/gfm/).
```

#### Internal links

For internal links, use root-relative or relative URLs to ensure portability.
When writing root-relative URLs in `.src.md` files, treat the `src` directory as the site root.
The Ruby script automatically adjusts these paths to refer to the appropriate locations during the conversion.

We recommend using root-relative links for image files, though relative links also work.

Examples:

Root-relative URL:

- `[Section 1](/sec1.src.md)`
- `[Image file](/images/sample.png)`
- `[C file pr1.c](/misc/pr1.c)`
- `[Directory misc](/misc/)`

Relative URL:

- `[Section 1](sec1.src.md)`
- `[Image file](images/sample.png)`
- `[C file pr1.c](misc/pr1.c)`
- `[Directory misc](misc/)`

The requirements for the pdf target differ from the gfm or html targets:

- Section links: Must use fragment identifiers (e.g., `#link-to-files`). They link to ATX header IDs created by Pandoc. For further information, see [Pandoc User's Manual - Extension: auto_identifiers](https://pandoc.org/MANUAL.html#extension-auto_identifiers).
- Other files: Both root-relative and relative URLs are supported. The links usually refer to C source files or directories. The Ruby script converts these links to point to the corresponding blob or tree pages in the GitHub Gtk4-tutorial repository.

Example:

- `[Section 1](#prerequisite-and-license)`
- `[Image file](/images/image.png)`
- `[C source file](misc/pr1.c)`
- `[Directory](/misc/)`

**Path Validation:**

Ensure that all internal links remain within the `src` directory.
The Ruby script will detect and warn you if a path (e.g., ../../../etc/passwd) attempts to escape the directory.

Note: This check prevents broken links and helps avoid "path traversal" issues, ensuring all referenced resources are correctly included in the final output.

For the pdf target, you can specify image sizes in units like cm.

```
![Image a](/images/a.png){width=6cm height=5cm}
```

This attribute is a Pandoc's Markdown extension and is not available in GFM.
The units (cm, % etc.) depend on the target (HTML or PDF).
These attributes are preserved only when the target is html or pdf.
If the target is gfm, the Ruby script removes the attribute.

### Other Notes on Writing .src.md

The conversion from .src.md to GFM or Pandoc's Markdown by the Ruby script follows this order:

1. Pass 1: Replacement of @@@if commands
2. Pass 2: Replacement of @@@include and @@@shell commands
3. Pass 3: Separation into code and non-code sections; code is left as-is, while links in non-code sections are converted.

Failure to account for this order can lead to unexpected results.

    ```
    @@@if gfm
    Some statement for the target gfm
    @@@elif html
    Some statement for the target html
    @@@else
    Some statement for the target pdf
    @@@end
    ```

Even if you intend to display the @@@if command itself, it will not be preserved as-is.
This is because @@@if replacement (Pass 1) occurs before code block protection (Pass 3).
If the target is gfm, the resulting output will be:

    ```
    Some statement for the target gfm
    ```

To achieve the intended output, use indented code blocks instead of fenced code blocks.
@@@ commands must start at the beginning of a line.
If they are indented, they are not recognized as commands, allowing them to pass through Pass 1 unchanged and be treated as an (indented) code block in Pass 3.

As explained later, section files (`sec<num>.src.md`) must be placed directly under the root of the source directory.
However, other .src.md files can be organized in subdirectories within the source directory.
This directory structure is mirrored in the target directory when the target is gfm or html.
If the target is pdf, these .src.md files are included in the appendices.
Note that only directories containing .src.md files are copied.
C programs are not copied, and image files are copied to the `docs` directory only when generating HTML.

Finally, since .src.md uses GFM or Pandoc's Markdown, please refer to their respective manuals for proper syntax.
If the dialects differ, use @@@if to write the appropriate version for each.
Links to the manuals are below:

- [GitHub Flavored Markdown Spec](https://github.github.com/gfm/)
- [Pandoc User’s Guide](https://pandoc.org/MANUAL.html)

### Table Formatting

Formatting tables is not strictly required, but it can improve the readability of .src.md files.

For example, suppose you have the following table in a text file named `sample.src.md`:

    Price List
    
    @@@table
    |item|price|
    |:---:|:---:|
    |mouse|$10|
    |PC|$500|
    @@@

The @@@table command formats it as follows, aligning items vertically for better aesthetics:

    Price List
    
    |item |price|
    |:---:|:---:|
    |mouse| $10 |
    | PC  |$500 |

This command only affects the visual alignment within the Markdown file and does not change the final HTML or PDF output.
Note that this command only supports the specific table syntax shown above.

The @@@table command is not supported during Rakefile-based conversion.
You must use the standalone script `bin/table_formatter.rb`.

```
$ bundle exec ruby bin/table_formatter.rb sample.src.md
```

Whether to format tables is a matter of personal preference.
If you don't find it necessary, you don't need to use the @@@table command.

## Directory Structure

This section covers the primary directory structure.
This structure can be modified, but if you do so, you must also update `/path_manager.rb`, which manages these directories.

- `/src`: Contains all source files (.src.md files, C source code, etc.).
- `/src/images`: Contains image files like PNG and JPG.
- `/gfm`: Contains generated GFM files.
- `/docs`: Contains generated HTML files and copies of image files.
- `/pdf`: Contains generated PDF files.
- `/lib`: Contains Ruby library files.
- `/test`: Contains test files. Run all tests by executing `bundle exec rake -f test/Rakefile` from the terminal.
- `/data`: Contains `chapters.yml` (defining chapters for PDF generation), other definition files, and templates.
- `/bin`: Contains standalone Ruby scripts.

### Source Directory Structure

#### index.src.md

`index.src.md` is placed directly in the source directory.
In GFM generation, it is converted to `README.md` at the repository root.
In HTML generation, it becomes `/docs/index.html`, serving as the website's homepage.
In PDF generation, it forms the initial introductory section.

#### Section Files

Section files are named with the prefix "sec" followed by the section number and the `.src.md` extension (e.g., `sec1.src.md`, `sec5.src.md`, `sec12.src.md`).
These correspond to Section 1, Section 5, and Section 12, respectively.

Section numbers must be consecutive natural numbers starting from 1.
However, if you want to insert a new section in the middle, renaming many files manually is tedious.
For example, if you want to insert a new section between `sec8.src.md` and `sec9.src.md`, first create a temporary section using a decimal, like `sec8.5.src.md`.
Then, use the `/bin/renumber.rb` script to automatically renumber the sections:

- `sec8.src.md` remains `sec8.src.md`
- `sec8.5.src.md` becomes `sec9.src.md`
- `sec9.src.md` becomes `sec10.src.md`

All subsequent sections will also be renumbered accordingly.

## Rake

This repository's conversion system uses Rake, a Ruby build tool.
Execute Rake by passing a task as an argument:

```
$ bundle exec rake <task name>
```

Available tasks:

- `gfm`: Generates GFM files. (Default task; executed if the task name is omitted).
- `html`: Generates HTML files.
- `pdf`: Generates PDF files.
- `all`: Generates GFM, HTML, and PDF files.
- `serve`: Starts a local server with `docs` as the site root (Port 8000).

### Modes

The environment variable `G4T_ENV` determines the mode used when generating HTML files.

- development: Development mode. Generates files for local serving.
- production: Production mode. Generates files for GitHub Pages.

The primary difference is the base URL, which affects root-relative link URLs.
GitHub Pages uses the repository name as the base URL, while the local environment has no base URL.
The Ruby script determines the mode by reading `G4T_ENV`.

```
$ G4T_ENV=development bundle exec rake html # Development mode
$ G4T_ENV=production bundle exec rake html # Production mode
```

You must generate the HTML files in production mode before pushing to GitHub.
If the environment variable assignment is omitted, the files will be generated in development mode.
