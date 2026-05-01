Up: [README.md](../README.md),  Next: [Section 2](sec2.md)

# Prerequisite and License

## Prerequisite

### GTK on a Linux OS

This tutorial describes GTK version 4 libraries.
Although GTK is primarily designed for Linux using the C compiler, it is now widely used on other platforms, including Windows and macOS, through various language bindings such as Vala, Python, Rust, and Ruby.
However, this tutorial describes only **C programs on Linux**.

If you want to try the examples in the tutorial, you need:

- PC with a Linux distribution like Ubuntu or Debian (including WSL - Windows Subsystem for Linux). 
- GCC.
- GTK 4 (version 4.10.1 or later).

The stable version of GTK is 4.20.3 at present (December 28, 2025).
The examples in this tutorial have been tested with GTK version 4.10.
Version 4.10 added some new classes and functions and deprecated some old ones.
Some example programs in this tutorial don't work on older versions.

### Note for WSL2 (WSLg) Users

When running GTK4 applications on WSL2, you may encounter several warnings in your terminal.
These are typically due to interactions between GTK4's rendering system and the WSLg Wayland compositor, rather than bugs in your code.
These warnings are often harmless, but in some cases they may affect rendering performance or popup behavior.

For professional-grade performance testing or pixel-perfect UI behavior, a native Linux installation is recommended.

### Ruby and Rake for Making the Document

This repository includes Ruby programs.
They are used to make GFM (GitHub Flavored Markdown), HTML, and PDF files.

You need:

- Linux OS (including WSL)
- Ruby 3.0 or later (tested on 4.0.1) 
- Rake

There are two ways to install Ruby.

- Install your distribution's package.
- Use a Ruby version manager such as rbenv or mise.

**Note:** If you choose to use a Ruby version other than 4.0.1, you must update the version settings in `Gemfile`.
Additionally, depending on your version manager, you may need to modify `.ruby-version` or `mise.toml` as well.

## License

Copyright (C) 2020, 2023 ToshioCP (Toshio Sekiya)

The GTK4-tutorial repository contains tutorial documents and programs such as converters, generators and controllers.
All of them comprise the 'GTK4-tutorial' package.
This package is simply called 'GTK4-tutorial' in the following description.

GTK4-tutorial is free; you can redistribute it and/or modify it under the terms of the following licenses.

- The license of documents in GTK4-tutorial is the GNU Free Documentation License as published by the Free Software Foundation; either version 1.3 of the License or, at your option, any later version.
The documents are Markdown, HTML and image files.
If you generate a PDF file by running `rake pdf`, it is also considered part of the documents.
- The license of programs in GTK4-tutorial is the GNU General Public License as published by the Free Software Foundation; either version 3 of the License or, at your option, any later version.
The programs are written in C, Ruby and other languages.

GTK4-tutorial is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU License web pages for more details.

- [GNU Free Documentation License](https://www.gnu.org/licenses/fdl-1.3.html)
- [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html)

The licenses above are effective as of April 15, 2023.
Before that, GPL covered all the contents of the GTK4-tutorial.
But GFDL1.3 is more appropriate for documents so the license was changed.

Up: [README.md](../README.md),  Next: [Section 2](sec2.md)
