# Ui file and GtkBuilder

## New, open and save button

We made the simplest editor in the previous section.
It reads the files in `on_open` function at start-up and writes it when closing the window.
It works but is not good.
It is better to make "New", "Open", "Save" and "Close" buttons.
This section describes how to put those buttons into the window.
Signals and handlers will be explained later.

![Screenshot of the file editor](../image/screenshot_tfe2.png){width=9.3cm height=6.825cm}

The screenshot above shows the layout.
The function `on_open` in the source code `tfe2.c` is as follows.

@@@ tfe/tfe2.c on_open

The point is how to build the window.

- 25-27: Generate GtkApplicationWindow and set its title and default size.
- 29-30: Generate GtkBox `boxv`.
It is a vertical box and a child of GtkApplicationWindow.
It has two children.
The first child is a horizontal box includes buttons.
The second child is GtkNotebook.
- 32-33: Generate GtkBox `boxh` and append it to 'boxv' as a first child.
- 35-40: Generate three dummy labels.
The labels `dmy1` and `dmy3` has a character width of ten.
The other label `dmy2` is set hexpand property TRUE.
This makes the label expands horizontally as long as possible.
- 41-44: Generate four buttons.
- 46-52: Append these GtkLabel and GtkButton to `boxh`.
- 54-57: Generate GtkNotebook and set hexpand and vexpand properties TRUE.
This makes it expands horizontally and vertically as big as possible.
It is appended to `boxv` as the second child.

The number of lines is 33(=57-25+1) to build the widgets.
And we needed many variables (boxv, boxh, dmy1 ...).
Most of them aren't necessary except building the widgets.
Are there any good solution to reduce these work?

Gtk provides GtkBuilder.
It reads ui data and builds a window.
It reduces the cumbersom work.

## Ui file

First, let's look at the ui file `tfe3.ui` that defines a structure of the widgets.

@@@ tfe/tfe3.ui

This is coded with XML structure.
Constructs beginning with `<` and ending with `>` are called tags, and are divided into two parts, start tag and end tag.
For example, `<interface>` is a start tag and `</interface>` is an end tag.
Ui file begins and ends with interface tags.
Some tags, for example, object tags can have a class and id attributes inside the start tag.

- 2-5: An object with `GtkApplicationWindow` class and `win` id is defined.
This is the top level window.
And the three properties of the window are defined.
`title` property is "file editor", `default-width` property is 400 and `default-height` property is 300.
- 6: child tag means a child of the object above.
For example, line 7 tells us that GtkBox object which id is "boxv" is a child of `win`.

Compare this ui file and the lines 25-57 in the source code of `on_open` function.
Those two decribe the same structure of widgets.

## GtkBuilder

GtkBuilder builds widgets based on the ui file.

~~~C
GtkBuilder *build;

build = gtk_builder_new_from_file ("tfe3.ui");
win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
~~~

The function `gtk_builder_new_from_file` reads the file given as an argument, build the widgets, generate GtkBuilder object and set pointers to the widgets in it.
The function `gtk_builder_get_object (build, "win")` returns the pointer to the widget `win`, which is the id in the ui file.
All the widgets are connected based on the parent-children relationship described in the ui file.
We only need `win` and `nb` for the program after this, so we don't need to take out any other widgets.
This reduces lines in the C source file.

$$$
cd tfe; diff tfe2.c tfe3.c
$$$

`60,103c61,65` means 42 (=103-60+1) lines change to 5 (=65-61+1) lines.
Therefore 37 lines are reduced.
Using ui file not only shortens C source files, but also makes the widgets' structure clear.

Now I'll show you the C source code `tfe3.c`.
Only functions `on_open` are shown as follows.

@@@ tfe/tfe3.c on_open

The source code of `tfe3.c` is stored in [src/tfe](https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/tfe) directory.
If you want to see it, click the link above.
In the same way, you can get the source files below in the directory [src/tfe](https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/tfe).

### Using ui string

GtkBuilder can build widgets using string.
Use the function gtk\_builder\_new\_from\_string instead of gtk\_builder\_new\_from\_file.

~~~C
char *uistring;

uistring =
"<interface>"
  "<object class="GtkApplicationWindow" id="win">"
    "<property name=\"title\">file editor</property>"
    "<property name=\"default-width\">600</property>"
    "<property name=\"default-height\">400</property>"
    "<child>"
      "<object class=\"GtkBox\" id=\"boxv\">"
        "<property name="orientation">GTK_ORIENTATION_VERTICAL</property>"
... ... ...
... ... ...
"</interface>";

build = gtk_builder_new_from_stringfile (uistring);
~~~

This method has an advantage and disadvantage.
The advantage is that the ui string is written in the source code.
So ui file is not necessary on runtime.
The disadvantage is that writing C string is a bit bothersome because of the double quotes.
If you want to use this method, you should write a script that transforms ui file into C-string.

- add backslash before each double quote.
- add double quote at the left and right.

### Using Gresource

Using Gresource is similar to using string.
But Gresource is compressed binary data, not text data.
And there's a compiler that compiles ui file into Gresource.
It can compile not only text files but also binary files such as images, sounds and so on.
And after compilation, it bundles them up into one Gresource object.

An xml file is necessary for the resource compiler `glib-compile-resources`.
It describes resource files.

@@@ tfe/tfe3.gresource.xml

- 2: gresources tag can include mulitple gresources (gresource tags).
However, this xml has only one gresource.
- 3: The gresource has a prefix `/com/github/ToshioCP/tfe3`.
- 4: The gresource has tfe3.ui.
And it is pointed by `/com/github/ToshioCP/tfe3/tfe3.ui` because it needs prefix. 
If you want to add more files, then insert them between line 4 and 5.

Save this xml text to `tfe3.gresource.xml`.
The gresource compiler `glib-compile-resources` shows its usage with the argument `--help`.

$$$
LANG=C glib-compile-resources --help
$$$

Now run the compiler.

    $ glib-compile-resources tfe3.gresource.xml --target=resources.c --generate-source

Then a C source file `resources.c` is generated.
Modify tfe3.c and save it as tfe3_r.c

~~~C
#include "resources.c"
... ... ...
... ... ...
build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe3/tfe3.ui");
... ... ...
... ... ...
~~~

Then, compile and run it.
The window appears and it is the same as the screenshot at the beginning of this page.

