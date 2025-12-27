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
@@@if gfm
There is a document \("[How to build GTK 4 Tutorial](Readme_for_developers.src.md)"\) for further information.
@@@elif html
There is a document \("[How to build GTK 4 Tutorial](Readme_for_developers.src.md)"\) for further information.
@@@elif latex
An appendix "How to build GTK 4 Tutorial" describes how to make them.
@@@end

#### License

The license of this repository is written in [Section1](sec1.src.md).
In short:

- GFDL1.3 for documents
- GPL3 for programs
