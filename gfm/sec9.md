Up: [README.md](../README.md),  Prev: [Section 8](sec8.md), Next: [Section 10](sec10.md)

# GtkBuilder and UI file

## New, Open and Save button

We made very simple editor in the previous section.
It reads files at the start and writes them out at the end of the program.
It works, but is not so good.
It would be better if we had "New", "Open", "Save" and "Close" buttons.
This section describes how to put those buttons into the window.

![Screenshot of the file editor](../image/screenshot_tfe2.png)

The screenshot above shows the layout.
The function `app_open` in the source code `tfe2.c` is as follows.

~~~C
 1 static void
 2 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
 3   GtkWidget *win;
 4   GtkWidget *nb;
 5   GtkWidget *lab;
 6   GtkNotebookPage *nbp;
 7   GtkWidget *scr;
 8   GtkWidget *tv;
 9   GtkTextBuffer *tb;
10   char *contents;
11   gsize length;
12   char *filename;
13   int i;
14   GError *err = NULL;
15 
16   GtkWidget *boxv;
17   GtkWidget *boxh;
18   GtkWidget *dmy1;
19   GtkWidget *dmy2;
20   GtkWidget *dmy3;
21   GtkWidget *btnn; /* button for new */
22   GtkWidget *btno; /* button for open */
23   GtkWidget *btns; /* button for save */
24   GtkWidget *btnc; /* button for close */
25 
26   win = gtk_application_window_new (GTK_APPLICATION (app));
27   gtk_window_set_title (GTK_WINDOW (win), "file editor");
28   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
29 
30   boxv = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
31   gtk_window_set_child (GTK_WINDOW (win), boxv);
32 
33   boxh = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
34   gtk_box_append (GTK_BOX (boxv), boxh);
35 
36   dmy1 = gtk_label_new(NULL); /* dummy label for left space */
37   gtk_label_set_width_chars (GTK_LABEL (dmy1), 10);
38   dmy2 = gtk_label_new(NULL); /* dummy label for center space */
39   gtk_widget_set_hexpand (dmy2, TRUE);
40   dmy3 = gtk_label_new(NULL); /* dummy label for right space */
41   gtk_label_set_width_chars (GTK_LABEL (dmy3), 10);
42   btnn = gtk_button_new_with_label ("New");
43   btno = gtk_button_new_with_label ("Open");
44   btns = gtk_button_new_with_label ("Save");
45   btnc = gtk_button_new_with_label ("Close");
46 
47   gtk_box_append (GTK_BOX (boxh), dmy1);
48   gtk_box_append (GTK_BOX (boxh), btnn);
49   gtk_box_append (GTK_BOX (boxh), btno);
50   gtk_box_append (GTK_BOX (boxh), dmy2);
51   gtk_box_append (GTK_BOX (boxh), btns);
52   gtk_box_append (GTK_BOX (boxh), btnc);
53   gtk_box_append (GTK_BOX (boxh), dmy3);
54 
55   nb = gtk_notebook_new ();
56   gtk_widget_set_hexpand (nb, TRUE);
57   gtk_widget_set_vexpand (nb, TRUE);
58   gtk_box_append (GTK_BOX (boxv), nb);
59 
60   for (i = 0; i < n_files; i++) {
61     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, &err)) {
62       scr = gtk_scrolled_window_new ();
63       tv = tfe_text_view_new ();
64       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
65       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
66       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
67 
68       tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
69       gtk_text_buffer_set_text (tb, contents, length);
70       g_free (contents);
71       filename = g_file_get_basename (files[i]);
72       lab = gtk_label_new (filename);
73       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
74       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
75       g_object_set (nbp, "tab-expand", TRUE, NULL);
76       g_free (filename);
77     } else {
78       g_printerr ("%s.\n", err->message);
79       g_clear_error (&err);
80     }
81   }
82   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
83     gtk_window_present (GTK_WINDOW (win));
84   } else
85     gtk_window_destroy (GTK_WINDOW (win));
86 }
~~~

The function `app_open` builds the widgets in the main application window.

- 26-28: Creates a GtkApplicationWindow instance and sets the title and default size.
- 30-31: Creates a GtkBox instance `boxv`.
It is a vertical box and a child of GtkApplicationWindow.
It has two children.
The first child is a horizontal box.
The second child is a GtkNotebook.
- 33-34: Creates a GtkBox instance `boxh` and appends it to `boxv` as the first child.
- 36-41: Creates three dummy labels.
The labels `dmy1` and `dmy3` has a character width of ten.
The other label `dmy2` has hexpand property which is set to be TRUE.
This makes the label expands horizontally as long as possible.
- 42-45: Creates four buttons.
- 47-53: Appends these GtkLabel and GtkButton to `boxh`.
- 55-58: Creates a GtkNotebook instance and sets hexpand and vexpand properties to be TRUE.
This makes it expand horizontally and vertically as big as possible.
It is appended to `boxv` as the second child.

The number of widget-build lines is 33(=58-26+1).
We also needed many variables (`boxv`, `boxh`, `dmy1`, ...) and most of them used only for building the widgets.
Are there any good solution to reduce these works?

Gtk provides GtkBuilder.
It reads user interface (UI) data and builds a window.
It reduces this cumbersome work.

## The UI File

Look at the UI file `tfe3.ui` that defines widget structure.

~~~xml
 1 <?xml version="1.0" encoding="UTF-8"?>
 2 <interface>
 3   <object class="GtkApplicationWindow" id="win">
 4     <property name="title">file editor</property>
 5     <property name="default-width">600</property>
 6     <property name="default-height">400</property>
 7     <child>
 8       <object class="GtkBox">
 9         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
10         <child>
11           <object class="GtkBox">
12             <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
13             <child>
14               <object class="GtkLabel">
15                 <property name="width-chars">10</property>
16               </object>
17             </child>
18             <child>
19               <object class="GtkButton">
20                 <property name="label">New</property>
21               </object>
22             </child>
23             <child>
24               <object class="GtkButton">
25                 <property name="label">Open</property>
26               </object>
27             </child>
28             <child>
29               <object class="GtkLabel">
30                 <property name="hexpand">TRUE</property>
31               </object>
32             </child>
33             <child>
34               <object class="GtkButton">
35                 <property name="label">Save</property>
36               </object>
37             </child>
38             <child>
39               <object class="GtkButton">
40                 <property name="label">Close</property>
41               </object>
42             </child>
43             <child>
44               <object class="GtkLabel">
45                 <property name="width-chars">10</property>
46               </object>
47             </child>
48           </object>
49         </child>
50         <child>
51           <object class="GtkNotebook" id="nb">
52             <property name="hexpand">TRUE</property>
53             <property name="vexpand">TRUE</property>
54           </object>
55         </child>
56       </object>
57     </child>
58   </object>
59 </interface>
~~~

The is a XML file.
Tags begin with `<` and end with `>`.
There are two types of tags, the start tag and the end tag.
For example, `<interface>` is a start tag and `</interface>` is an end tag.
The UI file begins and ends with interface tags.
Some tags, for example object tags, can have a class and id attributes in their start tag.

- 1: XML declaration.
It specifies that the XML version is 1.0 and the encoding is UTF-8.
- 3-6: An object tag with `GtkApplicationWindow` class and `win` id.
This is the top level window.
And the three properties of the window are defined.
The `title` property is "file editor", `default-width` property is 600 and `default-height` property is 400.
- 7: Child tag means a child widget.
For example, line 7 tells us that GtkBox object is a child widget of `win`.

Compare this ui file and the lines 26-58 in the `app_open` function of `tfe2.c`.
Both builds the same window with its descendant widgets.

You can check the ui file with `gtk4-builder-tool`.

- `gtk4-builder-tool validate <ui file name>` validates the ui file.
If the ui file includes some syntactical error, `gtk4-builder-tool` prints the error.
- `gtk4-builder-tool simplify <ui file name>` simplifies the ui file and prints the result.
If `--replace` option is given, it replaces the ui file with the simplified one.
If the ui file specifies a value of property but it is default, then it will be removed.
For example, the default orientation is horizontal so the simplification removes line 12.
And some values are simplified.
For example, "TRUE"and "FALSE" becomes "1" and "0" respectively.
However, "TRUE" or "FALSE" is better for maintenance.

It is a good idea to check your ui file before compiling.

## GtkBuilder

GtkBuilder builds widgets based on a ui file.

~~~C
GtkBuilder *build;

build = gtk_builder_new_from_file ("tfe3.ui");
win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
g_object_unref(build);
~~~

The function `gtk_builder_new_from_file` reads the file `tfe3.ui`.
Then, it builds the widgets and creates GtkBuilder object.
All the widgets are connected based on the parent-children relationship described in the ui file.
We can retrieve objects from the builder object with `gtk_builder_get_object` function.
The top level window, its id is "win" in the ui file, is taken and assigned to the variable `win`,
the application property of which is set to `app` with the `gtk_window_set_application` function.
GtkNotebook with the id "nb" in the ui file is also taken and assigned to the variable `nb`.
After the window and application are connected, GtkBuilder instance is useless.
It is released with `g_object_unref` function.

The ui file reduces lines in the C source file.

~~~
$ cd tfe; diff tfe2.c tfe3.c
59a60
>   GtkBuilder *build;
61,104c62,66
<   GtkWidget *boxv;
<   GtkWidget *boxh;
<   GtkWidget *dmy1;
<   GtkWidget *dmy2;
<   GtkWidget *dmy3;
<   GtkWidget *btnn; /* button for new */
<   GtkWidget *btno; /* button for open */
<   GtkWidget *btns; /* button for save */
<   GtkWidget *btnc; /* button for close */
< 
<   win = gtk_application_window_new (GTK_APPLICATION (app));
<   gtk_window_set_title (GTK_WINDOW (win), "file editor");
<   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
< 
<   boxv = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
<   gtk_window_set_child (GTK_WINDOW (win), boxv);
< 
<   boxh = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
<   gtk_box_append (GTK_BOX (boxv), boxh);
< 
<   dmy1 = gtk_label_new(NULL); /* dummy label for left space */
<   gtk_label_set_width_chars (GTK_LABEL (dmy1), 10);
<   dmy2 = gtk_label_new(NULL); /* dummy label for center space */
<   gtk_widget_set_hexpand (dmy2, TRUE);
<   dmy3 = gtk_label_new(NULL); /* dummy label for right space */
<   gtk_label_set_width_chars (GTK_LABEL (dmy3), 10);
<   btnn = gtk_button_new_with_label ("New");
<   btno = gtk_button_new_with_label ("Open");
<   btns = gtk_button_new_with_label ("Save");
<   btnc = gtk_button_new_with_label ("Close");
< 
<   gtk_box_append (GTK_BOX (boxh), dmy1);
<   gtk_box_append (GTK_BOX (boxh), btnn);
<   gtk_box_append (GTK_BOX (boxh), btno);
<   gtk_box_append (GTK_BOX (boxh), dmy2);
<   gtk_box_append (GTK_BOX (boxh), btns);
<   gtk_box_append (GTK_BOX (boxh), btnc);
<   gtk_box_append (GTK_BOX (boxh), dmy3);
< 
<   nb = gtk_notebook_new ();
<   gtk_widget_set_hexpand (nb, TRUE);
<   gtk_widget_set_vexpand (nb, TRUE);
<   gtk_box_append (GTK_BOX (boxv), nb);
< 
---
>   build = gtk_builder_new_from_file ("tfe3.ui");
>   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
>   gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
>   nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
>   g_object_unref(build);
138c100
<   app = gtk_application_new ("com.github.ToshioCP.tfe2", G_APPLICATION_HANDLES_OPEN);
---
>   app = gtk_application_new ("com.github.ToshioCP.tfe3", G_APPLICATION_HANDLES_OPEN);
144a107
> 
~~~

`61,104c62,66` means that 44 (=104-61+1) lines are changed to 5 (=66-62+1) lines.
Therefore, 39 lines are reduced.
Using ui file not only shortens C source files, but also makes the widgets structure clear.

Now I'll show you `app_open` function in the C file `tfe3.c`.

~~~C
 1 static void
 2 app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {
 3   GtkWidget *win;
 4   GtkWidget *nb;
 5   GtkWidget *lab;
 6   GtkNotebookPage *nbp;
 7   GtkWidget *scr;
 8   GtkWidget *tv;
 9   GtkTextBuffer *tb;
10   char *contents;
11   gsize length;
12   char *filename;
13   int i;
14   GError *err = NULL;
15   GtkBuilder *build;
16 
17   build = gtk_builder_new_from_file ("tfe3.ui");
18   win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
19   gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));
20   nb = GTK_WIDGET (gtk_builder_get_object (build, "nb"));
21   g_object_unref(build);
22   for (i = 0; i < n_files; i++) {
23     if (g_file_load_contents (files[i], NULL, &contents, &length, NULL, &err)) {
24       scr = gtk_scrolled_window_new ();
25       tv = tfe_text_view_new ();
26       tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
27       gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
28       gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
29 
30       tfe_text_view_set_file (TFE_TEXT_VIEW (tv),  g_file_dup (files[i]));
31       gtk_text_buffer_set_text (tb, contents, length);
32       g_free (contents);
33       filename = g_file_get_basename (files[i]);
34       lab = gtk_label_new (filename);
35       gtk_notebook_append_page (GTK_NOTEBOOK (nb), scr, lab);
36       nbp = gtk_notebook_get_page (GTK_NOTEBOOK (nb), scr);
37       g_object_set (nbp, "tab-expand", TRUE, NULL);
38       g_free (filename);
39     } else {
40       g_printerr ("%s.\n", err->message);
41       g_clear_error (&err);
42     }
43   }
44   if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (nb)) > 0) {
45     gtk_window_present (GTK_WINDOW (win));
46   } else
47     gtk_window_destroy (GTK_WINDOW (win));
48 }
~~~

The whole source code of `tfe3.c` is stored in the [src/tfe](../src/tfe) directory.

### Using ui string

GtkBuilder can build widgets with string.
Use `gtk_builder_new_from_string` instead of `gtk_builder_new_from_file`.

~~~C
char *uistring;

uistring =
"<interface>"
  "<object class="GtkApplicationWindow" id="win">"
    "<property name=\"title\">file editor</property>"
    "<property name=\"default-width\">600</property>"
    "<property name=\"default-height\">400</property>"
    "<child>"
      "<object class=\"GtkBox\">"
        "<property name="orientation">GTK_ORIENTATION_VERTICAL</property>"
... ... ...
... ... ...
"</interface>";

build = gtk_builder_new_from_string (uistring, -1);
~~~

This method has an advantage and disadvantage.
The advantage is that the ui string is written in the source code.
So, no ui file is needed on runtime.
The disadvantage is that writing C string is a bit bothersome because of the double quotes.
If you want to use this method, you should write a script that transforms ui file into C-string.

- Add backslash before each double quote.
- Add double quotes at the left and right of the string in each line.

### Gresource

Gresource is similar to string.
But Gresource is compressed binary data, not text data.
And there's a compiler that compiles ui file into Gresource.
It can compile not only text files but also binary files such as images, sounds and so on.
And after compilation, it bundles them up into one Gresource object.

An xml file is necessary for the resource compiler `glib-compile-resources`.
It describes resource files.

~~~xml
1 <?xml version="1.0" encoding="UTF-8"?>
2 <gresources>
3   <gresource prefix="/com/github/ToshioCP/tfe3">
4     <file>tfe3.ui</file>
5   </gresource>
6 </gresources>
~~~

- 2: `gresources` tag can include multiple gresources (gresource tags).
However, this xml has only one gresource.
- 3: The gresource has a prefix `/com/github/ToshioCP/tfe3`.
- 4: The name of the gresource is `tfe3.ui`.
The resource will be pointed with `/com/github/ToshioCP/tfe3/tfe3.ui` by GtkBuilder.
The pattern is "prefix" + "name".
If you want to add more files, insert them between line 4 and 5.

Save this xml text to `tfe3.gresource.xml`.
The gresource compiler `glib-compile-resources` shows its usage with the argument `--help`.

```
$ glib-compile-resources --help
Usage:
  glib-compile-resources [OPTION..] FILE

Compile a resource specification into a resource file.
Resource specification files have the extension .gresource.xml,
and the resource file have the extension called .gresource.

Help Options:
  -h, --help                   Show help options

Application Options:
  --version                    Show program version and exit
  --target=FILE                Name of the output file
  --sourcedir=DIRECTORY        The directories to load files referenced in FILE from (default: current directory)
  --generate                   Generate output in the format selected for by the target filename extension
  --generate-header            Generate source header
  --generate-source            Generate source code used to link in the resource file into your code
  --generate-dependencies      Generate dependency list
  --dependency-file=FILE       Name of the dependency file to generate
  --generate-phony-targets     Include phony targets in the generated dependency file
  --manual-register            Don't automatically create and register resource
  --internal                   Don't export functions; declare them G_GNUC_INTERNAL
  --external-data              Don't embed resource data in the C file; assume it's linked externally instead
  --c-name                     C identifier name used for the generated source code
  -C, --compiler               The target C compiler (default: the CC environment variable)
  ```

Now run the compiler.

~~~
$ glib-compile-resources tfe3.gresource.xml --target=resources.c --generate-source
~~~

Then a C source file `resources.c` is generated.
Modify `tfe3.c` and save it as `tfe3_r.c`.

~~~C
#include "resources.c"
... ... ...
... ... ...
build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe3/tfe3.ui");
... ... ...
... ... ...
~~~

The function `gtk_builder_new_from_resource` builds widgets from a resource.

Then, compile and run it.

~~~
$ comp tfe3_r
$ ./a.out tfe2.c
~~~

A window appears and it is the same as the screenshot at the beginning of this page.

Generally, resource is the best way for C language.
If you use other languages like Ruby, string is better than resource.

Up: [README.md](../README.md),  Prev: [Section 8](sec8.md), Next: [Section 10](sec10.md)
