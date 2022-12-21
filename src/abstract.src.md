#### Contents of this Repository

This tutorial illustrates how to write C programs with the GTK 4 library.
It focuses on beginners so the contents are limited to the basics.
The table of contents is at the end of this abstract.

- Section 3 to 21 describes the basics, with the example of a simple editor `tfe` (Text File Editor).
- Section 22 to 25 describes GtkDrawingArea.
- Section 26 to 29 describes the list model and the list view (GtkListView, GtkGridView and GtkColumnView).
It also describes GtkExpression.

The latest version of the tutorial is located at [GTK4-tutorial GitHub repository](https://github.com/ToshioCP/GTK4-tutorial).
You can read it directly without download.

There's a GitHub Page of this tutorial at [`https://toshiocp.github.io/GTK4-tutorial/`](https://toshiocp.github.io/GTK4-tutorial/).
It is easier to read than the repository.

#### GTK 4 Documentation

Please refer to [GTK 4 API Reference](https://docs.gtk.org/gtk4/index.html)
and [GNOME Developer Documentation Website](https://developer.gnome.org/) for further information.

These websites were opened in August of 2021.
The old documents are located at [GTK Reference Manual](https://developer-old.gnome.org/gtk4/stable/) and [GNOME Developer Center](https://developer-old.gnome.org/).

If you want to know about GObject and the type system, please refer to [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial).
GObject is the base of GTK 4, so it is important for developers to understand GObject for writing GTK 4 programs.

#### Contribution

This tutorial is still under development and unstable.
Even though the codes of the examples have been tested on GTK 4 (version 4.8.1), bugs may still exist.
If you find any bugs, errors or mistakes in the tutorial and C examples, please let me know.
You can post it to [GitHub issues](https://github.com/ToshioCP/GTK4-tutorial/issues).
You can also post updated files to [pull request](https://github.com/ToshioCP/GTK4-tutorial/pulls).
One thing you need to be careful is to correct the source files, which are under the 'src' directory.
Don't modify the files under `gfm` or `html` directories.
After modifying some source files (under `src` directory), run `rake` to create GFM (GitHub Flavoured Markdown) files or run `rake html` to create HTML files.

If you have a question, feel free to post it to `issue`.
All questions are helpful and will make this tutorial get better.

#### How to get Gtk 4 tutorial with HTML or PDF format

If you want to get HTML or PDF format tutorial, make them with `rake` command, which is a ruby version of make.
Type `rake html` for HTML.
Type `rake pdf` for PDF.
@@@if gfm
There is a document \("[How to build GTK 4 Tutorial](Readme_for_developers.src.md)"\) for further information.
@@@elif html
There is a document \("[How to build GTK 4 Tutorial](Readme_for_developers.src.md)"\) for further information.
@@@elif latex
An appendix "How to build GTK 4 Tutorial" describes how to make them.
@@@end
