# GTK 4 Tutorial for beginners

The GitHub page of this tutorial is also available. Click [here](https://toshiocp.github.io/Gtk4-tutorial/).

#### Contents of this Repository

Now that GTK 4 is widely used, "GTK" usually means GTK 4.
Therefore, this tutorial simply says "GTK" except where the version needs to be explicitly mentioned.

This tutorial illustrates how to write C programs with the GTK library.
It focuses on beginners, so the contents are limited to the basics.
The table of contents is at the end of this README.md.

- Section 3 to 23 describe the basics, with the example of a simple editor `tfe` (Text File Editor).
- Section 24 to 27 describe GtkDrawingArea.
- Section 28 describes Drag and Drop.
- Section 29 to 33 describe the list model and the list view (GtkListView, GtkGridView, and GtkColumnView).
It also describes GtkExpression.

The latest version of the tutorial is located at the [GTK4-tutorial GitHub repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it directly without downloading.

There's a [GitHub Page](https://toshiocp.github.io/Gtk4-tutorial/) which is the HTML version of the tutorial.

#### GTK Documentation

Please refer to the [GTK API Reference](https://docs.gtk.org/gtk4/index.html)
and the [GNOME Developer Documentation Website](https://developer.gnome.org/) for further information.
These websites were opened in August 2021.

If you want to know about GObject and the type system, please refer to the [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
GObject is the base of GTK, so it is important for developers to understand it as well as GTK.

#### Contribution

This tutorial is still under development and unstable.
Even though the example codes have been tested on GTK (version 4.10.1), bugs may still exist.
If you find any bugs, errors, or mistakes in the tutorial and C examples, please let me know.
You can post them to [GitHub issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
You can also post updated files to [pull request](https://github.com/ToshioCP/Gtk4-tutorial/pulls).
One thing you need to be careful about is to correct the source files, which are under the 'src' directory.
Don't modify the files under `gfm` or `html` directories.
After modifying some source files, run `rake` to create GFM (GitHub Flavoured Markdown) files and run `rake html` to create HTML files.

If you have a question, feel free to post it to `issue`.
All questions are helpful and will make this tutorial better.

#### How to get Gtk 4 tutorial in HTML or PDF format

If you want to get the tutorial in HTML or PDF format, build them with `rake` command, which is a ruby version of make.
Type `rake html` for HTML.
Type `rake pdf` for PDF.
There is a document \("[How to build GTK 4 Tutorial](gfm/Readme_for_developers.md)"\) for further information.

#### License

The license of this repository is written in [Section1](gfm/sec1.md).
In short:

- GFDL1.3 for documents
- GPL3 for programs

## Table of contents

1. [Prerequisite and License](gfm/sec1.md)
1. [Preparation](gfm/sec2.md)
1. [GtkApplication and GtkApplicationWindow](gfm/sec3.md)
1. [Widgets (1)](gfm/sec4.md)
1. [Widgets (2)](gfm/sec5.md)
1. [Strings and memory management](gfm/sec6.md)
1. [Widgets (3)](gfm/sec7.md)
1. [Defining a final class](gfm/sec8.md)
1. [GtkBuilder and UI file](gfm/sec9.md)
1. [Build system](gfm/sec10.md)
1. [Instance Initialization and destruction](gfm/sec11.md)
1. [Signals](gfm/sec12.md)
1. [TfeTextView class](gfm/sec13.md)
1. [Functions in GtkNotebook](gfm/sec14.md)
1. [Tfe main program](gfm/sec15.md)
1. [How to build tfe (text file editor)](gfm/sec16.md)
1. [Menus and actions](gfm/sec17.md)
1. [Stateful action](gfm/sec18.md)
1. [Ui file for menu and action entries](gfm/sec19.md)
1. [Composite widgets and alert dialog](gfm/sec20.md)
1. [GtkFontDialogButton and Gsettings](gfm/sec21.md)
1. [TfeWindow](gfm/sec22.md)
1. [Pango, CSS and Application](gfm/sec23.md)
1. [GtkDrawingArea and Cairo](gfm/sec24.md)
1. [Periodic Events](gfm/sec25.md)
1. [Custom drawing](gfm/sec26.md)
1. [Tiny turtle graphics interpreter](gfm/sec27.md)
1. [Drag and drop](gfm/sec28.md)
1. [GtkListView](gfm/sec29.md)
1. [GtkGridView and activate signal](gfm/sec30.md)
1. [GtkExpression](gfm/sec31.md)
1. [GtkColumnView](gfm/sec32.md)
1. [GtkSignalListItemFactory](gfm/sec33.md)
