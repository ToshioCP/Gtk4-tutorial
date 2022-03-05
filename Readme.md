# Gtk4 Tutorial for beginners

#### Contents of this Repository

This tutorial illustrates how to write C programs with the Gtk4 library.
It focuses on beginners so the contents are limited to basic things.
The table of contents is shown at the end of this abstract.

- Section 3 to 21 describes the basics, with the example of a simple editor `tfe` (Text File Editor).
- Section 22 to 24 describes GtkDrawingArea.
- Section 25 to 28 describes the list model and the list view (GtkListView, GtkGridView and GtkColumnView).
It also describes GtkExpression.

The latest version of the tutorial is located at [Gtk4-tutorial github repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it from there directly without having to download anything.

#### Gtk4 Documentation

Please refer to [Gtk API Reference](https://docs.gtk.org/gtk4/index.html)
and [Gnome Developer Documentation Website](https://developer.gnome.org/) for further information.

These websites are newly opened lately (Aug/2021).
The old documentation is located at [Gtk Reference Manual](https://developer-old.gnome.org/gtk4/stable/) and [Gnome Developer Center](https://developer-old.gnome.org/).
The new website is in progress at present, so you might need to refer to the old version.

If you want to know about GObject and the type system, please refer to [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
The GObject details are easy to understand and also necessary to know when writing Gtk4 programs.

#### Contribution

This tutorial is under development and unstable.
Even though the codes of the examples have been tested on Gtk4 version 4.0, bugs may still exist.
If you find any bugs, errors or mistakes in the tutorial and C examples, please let me know.
You can post it to [github issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
You can also post corrected files as a commit to [pull request](https://github.com/ToshioCP/Gtk4-tutorial/pulls).
When you make corrections, correct the source files, which are under the 'src' directory,
then run `rake` to create to create the output file. The GFM files under the 'gfm' directory are automatically updated.

If you have a question, feel free to post it as an issue.
All questions are helpful and will make this tutorial get better.

#### How to get a HTML or PDF version

If you want to get a HTML or PDF version, you can make them with `rake`, which is a ruby version of make.
Type `rake html` for HTML.
Type `rake pdf` for PDF.
There is a documentation \("[How to build Gtk4 Tutorial](gfm/Readme_for_developers.md)"\) that describes how to make them.

## Table of contents

1. [Prerequisite and License](gfm/sec1.md)
1. [Installation of Gtk4 to Linux distributions](gfm/sec2.md)
1. [GtkApplication and GtkApplicationWindow](gfm/sec3.md)
1. [Widgets (1)](gfm/sec4.md)
1. [Widgets (2)](gfm/sec5.md)
1. [String and memory management](gfm/sec6.md)
1. [Widgets (3)](gfm/sec7.md)
1. [Defining a Child object](gfm/sec8.md)
1. [Ui file and GtkBuilder](gfm/sec9.md)
1. [Build system](gfm/sec10.md)
1. [Initialization and destruction of instances](gfm/sec11.md)
1. [Signals](gfm/sec12.md)
1. [Functions in TfeTextView](gfm/sec13.md)
1. [Functions in GtkNotebook](gfm/sec14.md)
1. [tfeapplication.c](gfm/sec15.md)
1. [tfe5 source files](gfm/sec16.md)
1. [Menu and action](gfm/sec17.md)
1. [Stateful action](gfm/sec18.md)
1. [Ui file for menu and action entries](gfm/sec19.md)
1. [GtkMenuButton, accelerators, font, pango and gsettings](gfm/sec20.md)
1. [Template XML and composite widget](gfm/sec21.md)
1. [GtkDrawingArea and Cairo](gfm/sec22.md)
1. [Combine GtkDrawingArea and TfeTextView](gfm/sec23.md)
1. [Tiny turtle graphics interpreter](gfm/sec24.md)
1. [GtkListView](gfm/sec25.md)
1. [GtkGridView and activate signal](gfm/sec26.md)
1. [GtkExpression](gfm/sec27.md)
1. [GtkColumnView](gfm/sec28.md)
