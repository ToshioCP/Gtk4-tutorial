Up: [README.md](../README.md),  Prev: [Section 29](sec29.md), Next: [Section 31](sec31.md)

# GtkGridView and Activate Signal

GtkGridView is similar to GtkListView.
It displays a GListModel as a grid, which is like a square tessellation.

![Grid](/src/images/list4.png)

This is often seen when you use a file browser like GNOME Files (Nautilus).

In this section, let's make a very simple file browser `list4`.
It just shows the files in the current directory.
And a user can choose either list or grid layout by clicking on buttons in the tool bar.
Each item in the list or grid has an icon and a filename.
In addition, `list4` provides the way to open the `tfe` text editor to show a text file.
A user can do that by double clicking on an item or pressing enter key when an item is selected.

## GtkDirectoryList

GtkDirectoryList implements GListModel and it contains information of files in a certain directory.
The items of the list are GFileInfo objects.

In the `list4` source files, GtkDirectoryList is described in a UI file and built by GtkBuilder.
The GtkDirectoryList instance is assigned to the "model" property of a GtkSingleSelection instance.
And the GtkSingleSelection instance is assigned to the "model" property of a GListView or GGridView instance.

~~~
GtkListView (model property) => GtkSingleSelection (model property) => GtkDirectoryList
GtkGridView (model property) => GtkSingleSelection (model property) => GtkDirectoryList
~~~

![DirectoryList](/src/images/directorylist.png)

The following is a part of the UI file `list4.ui`.

~~~xml
<object class="GtkListView" id="list">
  <property name="model">
    <object class="GtkSingleSelection" id="singleselection">
      <property name="model">
        <object class="GtkDirectoryList" id="directory_list">
          <property name="attributes">standard::name,standard::icon,standard::content-type</property>
        </object>
      </property>
    </object>
  </property>
</object>
<object class="GtkGridView" id="grid">
  <property name="model">singleselection</property>
</object>
~~~

GtkDirectoryList has an "attributes" property.
They are attributes of GFileInfo such as "standard::name", "standard::icon" and "standard::content-type".

- standard::name is a filename.
- standard::icon is an icon of the file. It is a GIcon object.
- standard::content-type is a content-type.
Content-type is the same as MIME type.
For example, "text/plain" is a text file, "text/x-csrc" is a C source code and so on.
("text/x-csrc"is not registered to IANA media types.
Such "x-" subtype is not a standard mime type.)
Content type is also used by desktop systems.

GtkGridView uses the same GtkSingleSelection instance (`singleselection`).
So, its model property is set to it.

## Ui File of the Window

The window is built with the following UI file.
(See the screenshot at the beginning of this section).

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <object class="GtkApplicationWindow" id="win">
    <property name="title">file list</property>
    <property name="default-width">600</property>
    <property name="default-height">400</property>
    <child>
      <object class="GtkBox" id="boxv">
        <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
        <child>
          <object class="GtkBox" id="boxh">
            <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
            <child>
              <object class="GtkLabel" id="dmy1">
                <property name="hexpand">TRUE</property>
              </object>
            </child>
            <child>
              <object class="GtkButton" id="btnlist">
                <property name="name">btnlist</property>
                <property name="action-name">win.view</property>
                <property name="action-target">&apos;list&apos;</property>
                <child>
                  <object class="GtkImage">
                    <property name="resource">/com/github/ToshioCP/list4/list.png</property>
                  </object>
                </child>
              </object>
            </child>
            <child>
              <object class="GtkButton" id="btngrid">
                <property name="name">btngrid</property>
                <property name="action-name">win.view</property>
                <property name="action-target">&apos;grid&apos;</property>
                <child>
                  <object class="GtkImage">
                    <property name="resource">/com/github/ToshioCP/list4/grid.png</property>
                  </object>
                </child>
              </object>
            </child>
            <child>
              <object class="GtkLabel" id="dmy2">
                <property name="width-chars">10</property>
              </object>
            </child>
          </object>
        </child>
        <child>
          <object class="GtkScrolledWindow" id="scr">
            <property name="hexpand">TRUE</property>
            <property name="vexpand">TRUE</property>
          </object>
        </child>
      </object>
    </child>
  </object>
  <object class="GtkListView" id="list">
    <property name="model">
      <object class="GtkSingleSelection" id="singleselection">
        <property name="model">
          <object class="GtkDirectoryList" id="directory_list">
            <property name="attributes">standard::name,standard::icon,standard::content-type</property>
          </object>
        </property>
      </object>
    </property>
  </object>
  <object class="GtkGridView" id="grid">
    <property name="model">singleselection</property>
  </object>
</interface>

```

The file consists of two parts.
The first part begins at the line 3 and ends at line 57.
This part is the widgets from the top level window to the scrolled window.
It also includes two buttons.
The second part begins at line 58 and ends at line 71.
This is the part of GtkListView and GtkGridView.

- 13-17, 42-46: Two labels are dummy labels.
They just work as a space to put the two buttons at the appropriate position.
- 18-41: GtkButton `btnlist` and `btngrid`.
These two buttons work as selection buttons to switch from list to grid and vice versa.
These two buttons are connected to a stateful action `win.view`.
This action has a parameter.
Such action consists of prefix, action name and parameter.
The prefix of the action is `win`, which means the action belongs to the top level window (GtkApplicationWindow).
The prefix defines the scope of the action.
The action name is `view`.
The parameters are `list` or `grid`, which show the state of the action.
A parameter is also called a target, because it is a target to which the action changes its state.
We often write the detailed action like "win.view::list" or "win.view::grid".
- 21-22: The properties "action-name" and "action-target" belong to GtkActionable interface.
The GtkButton class implements the GtkActionable interface.
The action name is "win.view" and the target is "list".
Generally, a target is GVariant, which can be string, integer, float and so on.
You need to use GVariant text format to write GVariant value in UI files.
If the type of the GVariant value is string, then the value with GVariant text format is enclosed in single quotes or double quotes.
Because UI files are xml format text, single quote cannot be written without escaping it.
Its escape sequence is \&apos;.
Therefore, the target 'list' is written as \&apos;list\&apos;.
Because the button is connected to the action, "clicked" signal handler isn't needed.
- 23-27: The child widget of the button is GtkImage.
GtkImage has a "resource" property.
It is a GResource and GtkImage reads an image data from the resource and sets the image.
This resource is built from 24x24-sized png image data, which is an original icon.
- 50-53: GtkScrolledWindow.
Its child widget will be set with GtkListView or GtkGridView.

The action `view` is created, connected to the "activate" signal handler and inserted to the window (action map) as follows.

~~~C
act_view = g_simple_action_new_stateful ("view", g_variant_type_new("s"), g_variant_new_string ("list"));
g_signal_connect (act_view, "activate", G_CALLBACK (view_activated), NULL);
g_action_map_add_action (G_ACTION_MAP (win), G_ACTION (act_view));
~~~

The signal handler `view_activated` will be explained later.

## Factories

Each view (GtkListView and GtkGridView) has its own factory because its items have different structure of widgets.
The factories are GtkBuilderListItemFactory objects.
Their UI files are as follows.

factory_list.ui

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkBox">
        <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
        <property name="spacing">20</property>
        <child>
          <object class="GtkImage">
            <binding name="gicon">
              <closure type="GIcon" function="get_icon">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
        <child>
          <object class="GtkLabel">
            <property name="hexpand">TRUE</property>
            <property name="xalign">0</property>
            <binding name="label">
              <closure type="gchararray" function="get_file_name">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
      </object>
    </property>
  </template>
</interface>

```

factory_grid.ui

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkBox">
        <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
        <property name="spacing">20</property>
        <child>
          <object class="GtkImage">
            <property name="icon-size">GTK_ICON_SIZE_LARGE</property>
            <binding name="gicon">
              <closure type="GIcon" function="get_icon">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
        <child>
          <object class="GtkLabel">
            <property name="hexpand">TRUE</property>
            <property name="xalign">0.5</property>
            <binding name="label">
              <closure type="gchararray" function="get_file_name">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
      </object>
    </property>
  </template>
</interface>

```

The two files above are almost same.
The difference is:

- The orientation of the box
- The icon size
- The position of the text of the label

```
$ cd list4; diff factory_list.ui factory_grid.ui
6c6
<         <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
---
>         <property name="orientation">GTK_ORIENTATION_VERTICAL</property>
9a10
>             <property name="icon-size">GTK_ICON_SIZE_LARGE</property>
20c21
<             <property name="xalign">0</property>
---
>             <property name="xalign">0.5</property>
$
```

Two properties "gicon" (property of GtkImage) and "label" (property of GtkLabel) are in the UI files above.
Because GFileInfo doesn't have properties correspond to icon or filename, the factory uses closure tag to bind "gicon" and "label" properties to GFileInfo information.
A function `get_icon` gets GIcon from the GFileInfo object.
And a function `get_file_name` gets a filename from the GFileInfo object.

```c
GIcon *
get_icon (GtkListItem *item, GFileInfo *info) {
  GIcon *icon;

   /* g_file_info_get_icon can return NULL */
  icon = G_IS_FILE_INFO (info) ? g_file_info_get_icon (info) : NULL;
  return icon ? g_object_ref (icon) : NULL;
}

char *
get_file_name (GtkListItem *item, GFileInfo *info) {
  return G_IS_FILE_INFO (info) ? g_strdup (g_file_info_get_name (info)) : NULL;
}
```

One important thing is the ownership of the return values.
The return value is owned by the caller.
So, `g_object_ref` or `g_strdup` is necessary.

## An Activate Signal Handler of the Button Action

An activate signal handler `view_activate` switches the view.
It does two things.

- Changes the child widget of GtkScrolledWindow.
- Changes the CSS of buttons to show the current state.

```c
static void
view_activated(GSimpleAction *action, GVariant *parameter) {
  const char *view = g_variant_get_string (parameter, NULL);
  const char *other;
  char *css;

  if (strcmp (view, "list") == 0) {
    other = "grid";
    gtk_scrolled_window_set_child (scr, GTK_WIDGET (list));
  }else {
    other = "list";
    gtk_scrolled_window_set_child (scr, GTK_WIDGET (grid));
  }
  css = g_strdup_printf ("button#btn%s {background: silver;} button#btn%s {background: white;}", view, other);
  gtk_css_provider_load_from_data (provider, css, -1);
  g_free (css);
  g_action_change_state (G_ACTION (action), parameter);
}
```

The second parameter of this handler is the target value associated with the clicked button.
Its type is GVariant.

- If `btnlist` has been clicked, then `parameter` is a GVariant of the string "list".
- If `btngrid` has been clicked, then `parameter` is a GVariant of the string "grid".

The third parameter `user_data` points to NULL and it is ignored here.

- 3: `g_variant_get_string` gets the string from the GVariant variable.
- 7-13: Sets the child of `scr`.
The function `gtk_scrolled_window_set_child` decreases the reference count of the old child by one.
And it increases the reference count of the new child by one.
- 14-16: Sets the CSS for the buttons.
The background of the clicked button will be silver color and the other button will be white.
- 17: Changes the state of the action.
 
## Activate Signal on GtkListView and GtkGridView

Views (GtkListView and GtkGridView) have an "activate" signal.
It is emitted when an item in the view is double clicked or the enter key is pressed.
You can do anything you like by connecting the "activate" signal to the handler.

The example `list4` launches `tfe` text file editor if the item of the list is a text file.

~~~C
static void
list_activate (GtkListView *list, int position, gpointer user_data) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_list_view_get_model (list)), position));
  launch_tfe_with_file (info);
}

static void
grid_activate (GtkGridView *grid, int position, gpointer user_data) {
  GFileInfo *info = G_FILE_INFO (g_list_model_get_item (G_LIST_MODEL (gtk_grid_view_get_model (grid)), position));
  launch_tfe_with_file (info);
}

... ...
... ...

  g_signal_connect (GTK_LIST_VIEW (list), "activate", G_CALLBACK (list_activate), NULL);
  g_signal_connect (GTK_GRID_VIEW (grid), "activate", G_CALLBACK (grid_activate), NULL);
~~~

The second parameter of each handler is the position of the item (GFileInfo) of the GListModel.
So you can get the item with `g_list_model_get_item` function.

### Content Type and Application Launch

The function `launch_tfe_with_file` gets a file from the GFileInfo instance.
If the file is a text file, it launches `tfe` with the file.

GFileInfo has information about file type.
The file type is like "text/plain", "text/x-csrc" and so on.
This is referred to as the content type.
Content type can be got with `g_file_info_get_content_type` function.

```c
static void
launch_tfe_with_file (GFileInfo *info) {
  GError *err = NULL;
  GFile *file;
  GList *files = NULL;
  const char *content_type;
  const char *text_type = "text/";
  GAppInfo *appinfo;
  int i;

  if (! info)
    return;
  content_type = g_file_info_get_content_type (info);
#ifdef debug
  g_print ("%s\n", content_type);
#endif
  if (! content_type)
    return;
  for (i=0;i<5;++i) { /* compare the first 5 characters */
    if (content_type[i] != text_type[i])
      return;
  }
  appinfo = g_app_info_create_from_commandline ("tfe", "tfe", G_APP_INFO_CREATE_NONE, &err);
  if (err) {
    g_printerr ("%s\n", err->message);
    g_error_free (err);
    return;
  }
  err = NULL;
  file = g_file_new_for_path (g_file_info_get_name (info));
  files = g_list_append (files, file);
  if (! (g_app_info_launch (appinfo, files, NULL, &err))) {
    g_printerr ("%s\n", err->message);
    g_error_free (err);
  }
  g_list_free_full (files, g_object_unref);
  g_object_unref (appinfo);
}
```

- 13: Gets the content type of the file from GFileInfo.
- 14-16: Prints the content type if "debug" is defined.
This is only useful to know a content type of a file.
If you don't want this, delete or uncomment the definition `#define debug 1` at line 6 in the source file.
- 17-22: If there is no content type, or if it doesn't begin with "text/", the function returns.
- 23: Creates GAppInfo object of `tfe` application.
GAppInfo is an interface and the variable `appinfo` points to a GDesktopAppInfo instance.
GAppInfo is a collection of information of applications.
- 32: Launches the application (`tfe`) with an argument `file`.
`g_app_info_launch` has four parameters.
The first parameter is GAppInfo object.
The second parameter is a list of GFile objects.
In this function, only one GFile instance is given to `tfe`, but you can give more arguments.
The third parameter is GAppLaunchContext, but this program gives NULL instead.
The last parameter is the pointer to the pointer to a GError.
- 36: `g_list_free_full` frees the memories used by the list and items.

If your distribution supports GTK 4, using `g_app_info_launch_default_for_uri` is convenient.
The function automatically determines the default application from the file and launches it.
For example, if the file is text, then it launches GNOME text editor with the file.
Such feature comes from desktop.

## Compilation and Execution

The source files are located in [/src/list4](../src/list4/) directory.
To compile and execute list4, type as follows.

~~~
$ cd list4 # or cd src/list4. It depends your current directory.
$ meson setup _build
$ ninja -C _build
$ _build/list4
~~~

Then a file list appears as a list style.
Click on a button on the tool bar so that you can change the style to grid or back to list.
Double click "list4.c" item, then `tfe` text editor runs with the argument "list4.c".
The following is the screenshot.

![Screenshot](/src/images/screenshot_list4.png)

## "gbytes" Property of GtkBuilderListItemFactory

GtkBuilderListItemFactory has "gbytes" property.
The property contains a byte sequence of UI data.
If you use this property, you can put the contents of `factory_list.ui` and `factory_grid.ui` into `list4.ui`.
The following shows a part of the new UI file (`list5.ui`).

~~~xml
  <object class="GtkListView" id="list">
    <property name="model">
      <object class="GtkSingleSelection" id="singleselection">
        <property name="model">
          <object class="GtkDirectoryList" id="directory_list">
            <property name="attributes">standard::name,standard::icon,standard::content-type</property>
          </object>
        </property>
      </object>
    </property>
    <property name="factory">
      <object class="GtkBuilderListItemFactory">
        <property name="bytes"><![CDATA[
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkBox">
        <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
        <property name="spacing">20</property>
        <child>
          <object class="GtkImage">
            <binding name="gicon">
              <closure type="GIcon" function="get_icon">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
        <child>
          <object class="GtkLabel">
            <property name="hexpand">TRUE</property>
            <property name="xalign">0</property>
            <binding name="label">
              <closure type="gchararray" function="get_file_name">
                <lookup name="item">GtkListItem</lookup>
              </closure>
            </binding>
          </object>
        </child>
      </object>
    </property>
  </template>
</interface>
        ]]></property>
      </object>
    </property>
  </object>
~~~

CDATA section begins with "<![CDATA[" and ends with "]]>".
The contents of CDATA section is recognized as a string.
Any character, even if it is a key syntax marker such as '<' or '>', is recognized literally.
Therefore, the text between "<![CDATA[" and "]]>" is inserted to "bytes" property as it is.

This approach reduces the number of UI files.
But, the new UI file is a bit complicated especially for the beginners.
If you feel some difficulty, it is better for you to separate the UI file.

A directory [/src/list5](../src/list5/) includes the UI file above.

Up: [README.md](../README.md),  Prev: [Section 29](sec29.md), Next: [Section 31](sec31.md)
