Up: [README.md](../README.md),  Prev: [Section 28](sec28.md), Next: [Section 30](sec30.md)

# GtkListView

GTK 4 has added new list objects GtkListView, GtkGridView and GtkColumnView.
The new feature is described in [Gtk API Reference -- List Widget Overview](https://docs.gtk.org/gtk4/section-list-widget.html).

GTK 4 has other means to implement lists.
They are GtkListBox and GtkTreeView which are took over from GTK 3.
There's an article in [Gtk Development blog](https://blog.gtk.org/2020/06/07/scalable-lists-in-gtk-4/) about list widgets by Matthias Clasen.
He described why GtkListView are developed to replace GtkTreeView.
GtkTreeView is deprecated since version 4.10.

GtkListView, GtkGridView, GtkColumnView and related objects are described in Section 29 to 33.

## Outline

A list is a sequential data structure.
For example, an ordered string sequence "one", "two", "three", "four" is a list.
Each element is called item.
A list is like an array, but in many cases it is implemented with pointers which point to the next items of the list.
And it has a start point.
So, each item can be referred by the index of the item (first item, second item, ..., nth item, ...).
There are two cases.
The one is the index starts from one (one-based) and the other is from zero (zero-based).

Gio provides GListModel interface.
It is a zero-based list and its items are the same type of GObject descendants, or objects that implement the same interface.
An object implements GListModel is not a widget.
So, the list is not displayed on the screen directly.
There's another object GtkListView which is a widget to display the list.
The items in the list need to be connected to the items in GtkListView.
GtkListItemFactory instance maps items in the list to GtkListView.

![List](../image/list.png)

## GListModel and GtkStringList

If you want to make a list of strings with GListModel, for example, "one", "two", "three", "four", note that strings can't be items of the list.
Because GListModel is a list of GObject objects and strings aren't GObject objects.
The word "GObject" here means "GObject class or its descendant class".
So, you need a wrapper which is a GObject and contains a string.
GtkStringObject is the wrapper object and GtkStringList, implements GListModel, is a list of GtkStringObject.

~~~C
char *array[] = {"one", "two", "three", "four", NULL};
GtkStringList *stringlist = gtk_string_list_new ((const char * const *) array);
~~~

The function `gtk_string_list_new` creates a GtkStringList object.
Its items are GtkStringObject objects which contain the strings "one", "two", "three" and "four".
There are functions to add items to the list or remove items from the list.

- `gtk_string_list_append` appends an item to the list
- `gtk_string_list_remove` removes an item from the list
- `gtk_string_list_get_string` gets a string in the list

See [GTK 4 API Reference -- GtkStringList](https://docs.gtk.org/gtk4/class.StringList.html) for further information.

Other list objects will be explained later.

## GtkSelectionModel

GtkSelectionModel is an interface to support for selections.
Thanks to this model, user can select items by clicking on them.
It is implemented by GtkMultiSelection, GtkNoSelection and GtkSingleSelection objects.
These three objects are usually enough to build an application.
They are created with another GListModel.
You can also create them alone and add a GListModel later.

- GtkMultiSelection supports multiple selection.
- GtkNoSelection supports no selection. This is a wrapper to GListModel when GtkSelectionModel is needed.
- GtkSingleSelection supports single selection.

## GtkListView

GtkListView is a widget to show GListModel items.
GtkListItem is used by GtkListView to represent items of a list model.
But, GtkListItem itself is not a widget, so a user needs to set a widget, for example GtkLabel, as a child of GtkListItem to display an item of the list model.
"item" property of GtkListItem points an object that belongs to the list model.

![GtkListItem](../image/gtklistitem.png)

In case the number of items is very big, for example more than a thousand, GtkListItem is recycled and connected to another item which is newly displayed.
This recycle makes the number of GtkListItem objects fairly small, less than 200.
This is very effective to restrain the growth of memory consumption so that GListModel can contain lots of items, for example, more than a million items.

## GtkListItemFactory

GtkListItemFactory creates or recycles GtkListItem and connects it with an item of the list model.
There are two child classes of this factory, GtkSignalListItemFactory and GtkBuilderListItemFactory.

### GtkSignalListItemFactory

GtkSignalListItemFactory provides signals for users to configure a GtkListItem object.
There are four signals.

1. "setup" is emitted to set up GtkListItem object.
A user sets its child widget in the handler.
For example, creates a GtkLabel widget and sets the child property of the GtkListItem to it.
This setting is kept even the GtkListItem instance is recycled (to bind to another item of GListModel).
2. "bind" is emitted to bind an item in the list model to the widget.
For example, a user gets the item from "item" property of the GtkListItem instance.
Then gets the string of the item and sets the label property of the GtkLabel instance with the string.
This signal is emitted when the GtkListItem is newly created, recycled or some changes has happened to the item of the list.
3. "unbind" is emitted to unbind an item.
A user undoes everything done in step 2 in the signal handler.
If some object are created in step 2, they must be destroyed.
4. "teardown" is emitted to undo everything done in step 1.
So, the widget created in step 1 must be destroyed.
After this signal, the list item will be destroyed.

The following program `list1.c` shows the list of strings "one", "two", "three" and "four".
GtkNoSelection is used, so user can't select any item.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 setup_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
 5   GtkWidget *lb = gtk_label_new (NULL);
 6   gtk_list_item_set_child (listitem, lb);
 7   /* Because gtk_list_item_set_child sunk the floating reference of lb, releasing (unref) isn't necessary for lb. */
 8 }
 9 
10 static void
11 bind_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
12   GtkWidget *lb = gtk_list_item_get_child (listitem);
13   /* Strobj is owned by the instance. Caller mustn't change or destroy it. */
14   GtkStringObject *strobj = gtk_list_item_get_item (listitem);
15   /* The string returned by gtk_string_object_get_string is owned by the instance. */
16   gtk_label_set_text (GTK_LABEL (lb), gtk_string_object_get_string (strobj));
17 }
18 
19 static void
20 unbind_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
21   /* There's nothing to do here. */
22 }
23 
24 static void
25 teardown_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data) {
26   /* There's nothing to do here. */
27   /* GtkListItem instance will be destroyed soon. You don't need to set the child to NULL. */
28 }
29 
30 static void
31 app_activate (GApplication *application) {
32   GtkApplication *app = GTK_APPLICATION (application);
33   GtkWidget *win = gtk_application_window_new (app);
34   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
35   GtkWidget *scr = gtk_scrolled_window_new ();
36   gtk_window_set_child (GTK_WINDOW (win), scr);
37 
38   char *array[] = {
39     "one", "two", "three", "four", NULL
40   };
41   /* sl is owned by ns */
42   /* ns and factory are owned by lv. */
43   /* Therefore, you don't need to care about their destruction. */
44   GtkStringList *sl =  gtk_string_list_new ((const char * const *) array);
45   GtkNoSelection *ns =  gtk_no_selection_new (G_LIST_MODEL (sl));
46 
47   GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
48   g_signal_connect (factory, "setup", G_CALLBACK (setup_cb), NULL);
49   g_signal_connect (factory, "bind", G_CALLBACK (bind_cb), NULL);
50   /* The following two lines can be left out. The handlers do nothing. */
51   g_signal_connect (factory, "unbind", G_CALLBACK (unbind_cb), NULL);
52   g_signal_connect (factory, "teardown", G_CALLBACK (teardown_cb), NULL);
53 
54   GtkWidget *lv = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory);
55   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), lv);
56   gtk_window_present (GTK_WINDOW (win));
57 }
58 
59 /* ----- main ----- */
60 #define APPLICATION_ID "com.github.ToshioCP.list1"
61 
62 int
63 main (int argc, char **argv) {
64   GtkApplication *app;
65   int stat;
66 
67   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
68 
69   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
70 
71   stat =g_application_run (G_APPLICATION (app), argc, argv);
72   g_object_unref (app);
73   return stat;
74 }
~~~

The file `list1.c` is located under the directory [src/misc](../src/misc).
Make a shell script below and save it to your bin directory, for example `$HOME/bin`.

~~~Shell
gcc `pkg-config --cflags gtk4` $1.c `pkg-config --libs gtk4`
~~~

Change the current directory to the directory includes `list1.c` and type as follows.

~~~
$ chmod 755 $HOME/bin/comp # or chmod 755 (your bin directory)/comp
$ comp list1
$ ./a.out
~~~

Then, the following window appears.

![list1](../image/list1.png)

The program is not so difficult.
If you feel some difficulty, read this section again, especially GtkSignalListItemFactory subsubsection.

### GtkBuilderListItemFactory

GtkBuilderListItemFactory is another GtkListItemFactory.
Its behavior is defined with ui file.

~~~xml
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkLabel">
        <binding name="label">
          <lookup name="string" type="GtkStringObject">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
~~~

Template tag is used to define GtkListItem.
And its child property is GtkLabel object.
The factory sees this template and creates GtkLabel and sets the child property of GtkListItem.
This is the same as what setup handler of GtkSignalListItemFactory did.

Then, bind the label property of the GtkLabel to the string property of a GtkStringObject.
The string object refers to the item property of the GtkListItem.
So, the lookup tag is like this:

~~~
label <- string <- GtkStringObject <- item <- GtkListItem
~~~

The last lookup tag has a content `GtkListItem`.
Usually, C type like `GtkListItem` doesn't appear in the content of tags.
This is a special case.
There is an explanation in the [GTK Development Blog](https://blog.gtk.org/2020/09/05/a-primer-on-gtklistview/) by Matthias Clasen.

> Remember that the classname (GtkListItem) in a ui template is used as the “this” pointer referring to the object that is being instantiated.

Therefore, GtkListItem instance is used as the `this` object of the lookup tag when it is evaluated.
`this` object will be explained in [section 31](../src/sec31).

The C source code is as follows.
Its name is `list2.c` and located under [src/misc](../src/misc) directory.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 static void
 4 app_activate (GApplication *application) {
 5   GtkApplication *app = GTK_APPLICATION (application);
 6   gtk_window_present (gtk_application_get_active_window(app));
 7 }
 8 
 9 static void
10 app_startup (GApplication *application) {
11   GtkApplication *app = GTK_APPLICATION (application);
12   GtkWidget *win = gtk_application_window_new (app);
13   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
14   GtkWidget *scr = gtk_scrolled_window_new ();
15   gtk_window_set_child (GTK_WINDOW (win), scr);
16 
17   char *array[] = {
18     "one", "two", "three", "four", NULL
19   };
20   GtkStringList *sl = gtk_string_list_new ((const char * const *) array);
21   GtkSingleSelection *ss = gtk_single_selection_new (G_LIST_MODEL (sl));
22 
23   const char *ui_string =
24 "<interface>"
25   "<template class=\"GtkListItem\">"
26     "<property name=\"child\">"
27       "<object class=\"GtkLabel\">"
28         "<binding name=\"label\">"
29           "<lookup name=\"string\" type=\"GtkStringObject\">"
30             "<lookup name=\"item\">GtkListItem</lookup>"
31           "</lookup>"
32         "</binding>"
33       "</object>"
34     "</property>"
35   "</template>"
36 "</interface>"
37 ;
38   GBytes *gbytes = g_bytes_new_static (ui_string, strlen (ui_string));
39   GtkListItemFactory *factory = gtk_builder_list_item_factory_new_from_bytes (NULL, gbytes);
40 
41   GtkWidget *lv = gtk_list_view_new (GTK_SELECTION_MODEL (ss), factory);
42   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), lv);
43 }
44 
45 /* ----- main ----- */
46 #define APPLICATION_ID "com.github.ToshioCP.list2"
47 
48 int
49 main (int argc, char **argv) {
50   GtkApplication *app;
51   int stat;
52 
53   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
54 
55   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
56   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
57 
58   stat =g_application_run (G_APPLICATION (app), argc, argv);
59   g_object_unref (app);
60   return stat;
61 }
62 
~~~

No signal handler is needed for GtkBulderListItemFactory.
GtkSingleSelection is used, so user can select one item at a time.

Because this is a small program, the ui data is given as a string.

## GtkDirectoryList

GtkDirectoryList is a list model containing GFileInfo objects which are information of files under a certain directory.
It uses `g_file_enumerate_children_async()` to get the GFileInfo objects.
The list model is created by `gtk_directory_list_new` function.

~~~C
GtkDirectoryList *gtk_directory_list_new (const char *attributes, GFile *file);
~~~

`attributes` is a comma separated list of file attributes.
File attributes are key-value pairs.
A key consists of a namespace and a name.
For example, "standard::name" key is the name of a file.
"standard" means general file information.
"name" means filename.
The following table shows some example.

|key             |meaning                                                             |
|:---------------|:-------------------------------------------------------------------|
|standard::type  |file type. for example, regular file, directory, symbolic link, etc.|
|standard::name  |filename                                                            |
|standard::size  |file size in bytes                                                  |
|access::can-read|read privilege if the user is able to read the file                 |
|time::modified  |the time the file was last modified in seconds since the UNIX epoch |

The current directory is ".".
The following program makes GtkDirectoryList `dl` and its contents are GFileInfo objects under the current directory.

~~~C
GFile *file = g_file_new_for_path (".");
GtkDirectoryList *dl = gtk_directory_list_new ("standard::name", file);
g_object_unref (file);
~~~

It is not so difficult to make file listing program by changing `list2.c` in the previous subsection.
One problem is that GInfoFile doesn't have properties.
Lookup tag look for a property, so it is useless for looking for a filename from a GFileInfo object.
Instead, closure tag is appropriate in this case.
Closure tag specifies a function and the type of the return value of the function.

~~~C
char *
get_file_name (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}
... ...
... ...

"<interface>"
  "<template class=\"GtkListItem\">"
    "<property name=\"child\">"
      "<object class=\"GtkLabel\">"
        "<binding name=\"label\">"
          "<closure type=\"gchararray\" function=\"get_file_name\">"
            "<lookup name=\"item\">GtkListItem</lookup>"
          "</closure>"
        "</binding>"
      "</object>"
    "</property>"
  "</template>"
"</interface>"
~~~

- The string "gchararray" is a type name.
The type "gchar" is a type name and it is the same as C type "char".
Therefore, "gchararray" is "an array of char type", which is the same as string type.
It is used to get the type of GValue object.
GValue is a generic value and it can contain various type of values.
For example, the type name can be gboolean, gchar (char), gint (int), gfloat (float), gdouble (double), gchararray (char *) and so on.
These type names are the names of the fundamental types that are registered to the type system.
See [GObject tutorial](https://github.com/ToshioCP/Gobject-tutorial/blob/main/gfm/sec5.md#gvalue).
- Closure tag has type attribute and function attribute.
Function attribute specifies the function name and type attribute specifies the type of the return value of the function.
The contents of closure tag (it is between \<closure...\> and\</closure\>) is parameters of the function.
`<lookup name="item">GtkListItem</lookup>` gives the value of the item property of the GtkListItem.
This will be the second argument of the function.
The first parameter is always the GListItem instance, which is a 'this' object.
The 'this' object is explained in section 31.
- `gtk_file_name` function is the callback function for the closure tag.
It first checks the `info` parameter.
Because it can be NULL when GListItem `item` is unbounded.
If it's GFileInfo, it returns the copied filename.
Because the return value (filename) of `g_file_info_get_name` is owned by GFileInfo object.
So, the the string needs to be duplicated to give the ownership to the caller.
Binding tag binds the "label" property of the GtkLabel to the closure tag.

The whole program (`list3.c`) is as follows.
The program is located in [src/misc](../src/misc) directory.

~~~C
 1 #include <gtk/gtk.h>
 2 
 3 char *
 4 get_file_name (GtkListItem *item, GFileInfo *info) {
 5   return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
 6 }
 7 
 8 static void
 9 app_activate (GApplication *application) {
10   GtkApplication *app = GTK_APPLICATION (application);
11   gtk_window_present (gtk_application_get_active_window(app));
12 }
13 
14 static void
15 app_startup (GApplication *application) {
16   GtkApplication *app = GTK_APPLICATION (application);
17   GtkWidget *win = gtk_application_window_new (app);
18   gtk_window_set_default_size (GTK_WINDOW (win), 600, 400);
19   GtkWidget *scr = gtk_scrolled_window_new ();
20   gtk_window_set_child (GTK_WINDOW (win), scr);
21 
22   GFile *file = g_file_new_for_path (".");
23   GtkDirectoryList *dl = gtk_directory_list_new ("standard::name", file);
24   g_object_unref (file);
25   GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (dl));
26 
27   const char *ui_string =
28 "<interface>"
29   "<template class=\"GtkListItem\">"
30     "<property name=\"child\">"
31       "<object class=\"GtkLabel\">"
32         "<binding name=\"label\">"
33           "<closure type=\"gchararray\" function=\"get_file_name\">"
34             "<lookup name=\"item\">GtkListItem</lookup>"
35           "</closure>"
36         "</binding>"
37       "</object>"
38     "</property>"
39   "</template>"
40 "</interface>"
41 ;
42   GBytes *gbytes = g_bytes_new_static (ui_string, strlen (ui_string));
43   GtkListItemFactory *factory = gtk_builder_list_item_factory_new_from_bytes (NULL, gbytes);
44 
45   GtkWidget *lv = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory);
46   gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), lv);
47 }
48 
49 /* ----- main ----- */
50 #define APPLICATION_ID "com.github.ToshioCP.list3"
51 
52 int
53 main (int argc, char **argv) {
54   GtkApplication *app;
55   int stat;
56 
57   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
58 
59   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
60   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
61 
62   stat =g_application_run (G_APPLICATION (app), argc, argv);
63   g_object_unref (app);
64   return stat;
65 }
~~~

The ui data (xml data above) is used to build the GListItem template at runtime.
GtkBuilder refers to the symbol table to find the function `get_file_name`.

Generally, a symbol table is used by a linker to link objects to an executable file.
It includes function names and their location.
A linker usually doesn't put a symbol table into the created executable file.
But if `--export-dynamic` option is given, the linker adds the symbol table to the executable file.

To accomplish it, an option `-Wl,--export-dynamic` is given to the C compiler.

- `-Wl` is a C compiler option that passes the following option to the linker.
- `--export-dynamic` is a linker option.
The following is cited from the linker document.
"When creating a dynamically linked executable, add all symbols to the dynamic symbol table.
The dynamic symbol table is the set of symbols which are visible from dynamic objects at run time."

Compile and execute it.

~~~
$ gcc -Wl,--export-dynamic `pkg-config --cflags gtk4` list3.c `pkg-config --libs gtk4`
~~~

You can also make a shell script to compile `list3.c`

~~~bash
gcc -Wl,--export-dynamic `pkg-config --cflags gtk4` $1.c `pkg-config --libs gtk4`
~~~

Save this one liner to a file `comp`.
Then, copy it to `$HOME/bin` and give it executable permission.

~~~
$ cp comp $HOME/bin/comp
$ chmod +x $HOME/bin/comp
~~~

You can compile `list3.c` and execute it, like this:

~~~
$ comp list3
$ ./a.out
~~~

![screenshot list3](../image/list3.png)

Up: [README.md](../README.md),  Prev: [Section 28](sec28.md), Next: [Section 30](sec30.md)
