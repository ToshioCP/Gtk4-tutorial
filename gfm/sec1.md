Up: [README.md](../README.md),  Next: [Section 2](sec2.md)

# Prerequisite and License

## Prerequisite

### GTK 4 on a Linux OS

This tutorial is about GTK 4 libraries.
It is originally used on Linux with C compiler, but now it is used more widely, on Windows and MacOS, with Vala, Python and so on.
However, this tutorial describes only *C programs on Linux*.

If you want to try the examples in the tutorial, you need:

- PC with Linux distribution like Ubuntu or Debian.
- GCC.
- GTK 4.
The stable version of GTK is 4.8.2 at present (13/Dec/2022), but older version (4.0 or higher) may work.
See [Section 2](sec2.md) for the installation for GTK 4.

### Ruby and rake for making the document

This repository includes Ruby programs.
They are used to make GFM (GitHub Flavoured Markdown) files, HTML files, Latex files and a PDF file.

You need:

- Linux.
- Ruby programming language.
There are two ways to install it.
One is installing the distribution's package.
The other is using rbenv and ruby-build.
If you want to use the latest version of ruby, use rbenv.
- Rake.
You don't need to install it separately because it is a standard library of Ruby.

## License

Copyright (C) 2020-2020,2023  ToshioCP (Toshio Sekiya)

GTK4-tutorial repository contains tutorial documents and programs such as converters, generators and controllers.
All of them make up the 'GTK4-tutorial' package.
This package is simply called 'GTK4-tutorial' in the following description.

GTK4-tutorial is free; you can redistribute it and/or modify it under terms of the following licenses.

- The license of documents in GTK4-tutorial is the GNU Free Documentation License as published by the Free Software Foundation; either version 1.3 of the License or, at your opinion, any later version.
The documents are Markdown, HTML and image files.
If you generate a PDF file by running `rake pdf`, it is also included the documents.
- The license of programs in GTK4-tutorial is the GNU General Public License as published by the Free Software Foundation; either version 3 of the License or, at your option, any later version.
The programs are written in C, Ruby and other languages.

'GTK4-tutorial' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU License web pages for more details.

- [GNU Free Documentation License](https://www.gnu.org/licenses/fdl-1.3.html)
- [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html)

The licenses above is effective since 15/April/2023.
Before that, GPL covered all the contents of the GTK4-tutorial.
But GFDL1.3 is more appropriate for documents so the license was changed.
The license above is the only effective license since 15/April/2023.

Up: [README.md](../README.md),  Next: [Section 2](sec2.md)
