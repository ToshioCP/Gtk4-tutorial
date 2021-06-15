# GtkMenuButton, accelerators, font, pango and gsettings

Traditional menu structure is fine.
However, Buttons or menu items we often use are not so many.
Some mightn't be clicked at all.
Therefore, it's a good idea to put some frequently used buttons on the toolbar and put the rest of the less frequently used operations into the menu.
Such menu are often connected to GtkMenuButton.

We will restructure tfe text file editor in this section.
It will be more practical.
The buttons are changed to:

- Put open, save and close buttons to the toolbar.
In addition, GtkMenuButton is added to the toolbar.
This button shows a popup menu when clicked on.
Here, popup means widely, including pull-down menu.
- Put new, save as, preference and quit items to the menu under the menu button.

## Signal elements in ui files 

The four buttons are included in the ui file `tfe.ui`.
The difference from prior sections is signal tag.
The following is extracted from `tfe.ui` and it describes the open button.

~~~xml
<object class="GtkButton" id="btno">
  <property name="label">Open</property>
  <signal name="clicked" handler="open_cb" swapped="TRUE" object="nb"></signal>
</object>
~~~

Signal tag specifies the name of the signal, handler and user_data object.
They are the value of name, handler and object attributes.
Swapped attribute has the same meaning as `g_signal_connect_swapped` function.
So, the signal tag above works the same as the function below.

~~~C
g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
~~~

You need to compile the source file with "-WI, --export-dynamic" options.
You can achieve this by adding "export_dynamic: true" argument to executable function in `meson.build`.
And remove static class from the handler.

~~~C
void
open_cb (GtkNotebook *nb) {
  notebook_page_open (nb);
}
~~~
If you add static, the function is in the scope of the file and it can't be seen from outside.
Then the signal tag can't find the function.

## Menu and GkMenuButton

Menus are described in `menu.ui` file.

@@@include
tfe6/menu.ui
@@@

There are four items, "New", "Saveas", "Preference" and "Quit".

- New menu creates a new empty page.
- Saveas menu saves the current page as a new filename.
- Preference menu sets preference items.
This version of `tfe` has only font preference.
- Quit menu quits the application.

These four menus are not used so often.
That's why they are put to the menu behind the menu button.

The menus and the menu button are connected with `gtk_menu_button_set_menu_model` function.
The variable `btnm` below points a GtkMenuButton object.

~~~C
  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/menu.ui");
  menu = G_MENU_MODEL (gtk_builder_get_object (build, "menu"));
  gtk_menu_button_set_menu_model (btnm, menu);
~~~

## Actions and Accelerators

Menus are connected to actions.
Actions are defined with an array and `g_action_map_add_action_entries` function.

~~~C
  const GActionEntry win_entries[] = {
    { "open", open_activated, NULL, NULL, NULL },
    { "save", save_activated, NULL, NULL, NULL },
    { "close", close_activated, NULL, NULL, NULL },
    { "new", new_activated, NULL, NULL, NULL },
    { "saveas", saveas_activated, NULL, NULL, NULL },
    { "pref", pref_activated, NULL, NULL, NULL },
    { "close-all", quit_activated, NULL, NULL, NULL }
  };
  g_action_map_add_action_entries (G_ACTION_MAP (win), win_entries, G_N_ELEMENTS (win_entries), nb);
~~~

There are seven actions, open, save, close, new, saveas, pref and close-all.
But there were only four menus.
New, saveas, pref and close-all actions correspond to new, saveas, preference and quit menu respectively.
The three actions open, save and close doesn't have corresponding menus.
Are thy necessary?
These actions are defined because of accelerators.

Accelerators are a kind of short cut key function.
They are defined with arrays and `gtk_application_set_accels_for_action` function.

~~~C
  struct {
    const char *action;
    const char *accels[2];
  } action_accels[] = {
    { "win.open", { "<Control>o", NULL } },
    { "win.save", { "<Control>s", NULL } },
    { "win.close", { "<Control>w", NULL } },
    { "win.new", { "<Control>n", NULL } },
    { "win.saveas", { "<Shift><Control>s", NULL } },
    { "win.close-all", { "<Control>q", NULL } },
  };

  for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
~~~

This code is a bit complicated.
The array `action-accels[]` is an array of structures.
The structure is:

~~~C
  struct {
    const char *action;
    const char *accels[2];
  }
~~~

The member `action` is a string.
The member `accels` is an array of two strings.
For example,

~~~C
{ "win.open", { "<Control>o", NULL } },
~~~

This is the first element of the array `action_accels`.

- The member `action` is "win.open". This specifies the action "open" belongs to the window object.
- The member `accels` is an array of two strings, "\<Control\>o" and NULL.
The first string specifies a key combination.
Control key and 'o'.
If you keep pressing the control key and push 'o' key, then it activates the action `win.open`.
The second string NULL (or zero) means the end of the list (array).
You can define more than one accelerator keys and the list must ends with NULL (zero).
If you want to do so, the array length needs to be three or more.
The parser recognizes "\<control\>o", "\<Shift\>\<Alt\>F2", "\<Ctrl\>minus" and so on.
If you want to use symbol key like "\<Ctrl\>-", use "\<Ctrl\>minus" instead.
Such relation between lower case and symbol (its character code) is specified in [`gdkkeysyms.h`](https://gitlab.gnome.org/GNOME/gtk/-/blob/master/gdk/gdkkeysyms.h) in the gtk4 source code.

## Saveas handler

TfeTextView has already had a saveas function.
So, only we need to write is the wrapper function in `tfenotebook.c`.

@@@include
tfe6/tfenotebook.c get_current_textview notebook_page_saveas
@@@

The function `get_current_textview` is the same as before.
The function `notebook_page_saveas` simply calls `tfe_text_view_saveas`.

In `tfeapplication.c`, saveas handler just call `notebook_page_saveas`.

@@@include
tfe6/tfeapplication.c saveas_activated
@@@

## Preference and alert dialog

### Preference dialog

Preference dialog xml definition is added to `tfe.ui`.

~~~xml
<object class="GtkDialog" id="pref">
  <property name="title">Preferences</property>
  <property name="resizable">FALSE</property>
  <property name="modal">TRUE</property>
  <property name="transient-for">win</property>
  <child internal-child="content_area">
    <object class="GtkBox" id="content_area">
      <child>
        <object class="GtkBox" id="pref_boxh">
          <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
          <property name="spacing">12</property>
          <property name="margin-start">12</property>
          <property name="margin-end">12</property>
          <property name="margin-top">12</property>
          <property name="margin-bottom">12</property>
          <child>
            <object class="GtkLabel" id="fontlabel">
              <property name="label">Font:</property>
              <property name="xalign">1</property>
            </object>
          </child>
          <child>
            <object class="GtkFontButton" id="fontbtn">
            </object>
          </child>
        </object>
      </child>
    </object>
  </child>
</object>
~~~

- Preference dialog is an independent dialog.
It is not a descendant widget of the top GtkApplicationwindow `win`.
Therefore, There's no child tag of the dialog object.
- There are four properties of the dialog specified.
GtkDialog is a child object (not child widget) of GtkWindow, so it inherits all the properties from GtkWindow.
Title, resizable, modal and transient-for properties are inherited from GtkWindow.
Transient-for specifies a temporary parent window, which the dialog's location is based on.
- internal-child attribute is used in the child tag above.
GtkDialog has a GtkBox child widget.
Its id is "content_area" in `gtkdialog.ui`, which is the ui file of GtkDialog in gtk4 source files.
This box is provided to users to add content widgets in it.
The tag `<child internal-child="content_area">` is put at the top of the contents.
Then you need to specify an object tag and define its class as GtkBox and its id as content_area.
This object is defined in `gtkdialog.ui` but you need to define it again in the child tag.
- In the content area, defines GtkBox, GtkLabel and GtkFontButton.

I want the preference dialog to keep alive during the application lives.
So, it is necessary to catch "close-request" signal from the dialog and stop the signal propagation.
This is accomplished by returning TRUE by the signal handler.

~~~C
pref_close_cb (GtkDialog *pref, gpointer user_data) {
  return TRUE;
}

g_signal_connect (GTK_DIALOG (pref), "close-request", G_CALLBACK (pref_close_cb), NULL);
~~~

Generally, signal emission consists of five stages.

1. Default handler is invoked if the signal's flag is `G_SIGNAL_RUN_FIRST`.
Default handler is set when a signal is registered.
It is different from user signal handler, simply called signal handler, connected by `g_signal_connect`series function.
Default handler can be invoked in either stage 1, 3 or 5.
Most of the default handlers are `G_SIGNAL_RUN_FIRST` or `G_SIGNAL_RUN_LAST`.
2. Signal handlers are invoked, unless it is connected by `g_signal_connect_after`.
3. Default handler is invoked if the signal's flag is `G_SIGNAL_RUN_LAST`.
4. Signal handlers are invoked, if it is connected by `g_signal_connect_after`.
5. Default handler is invoked if the signal's flag is `G_SIGNAL_RUN_CLEANUP`.

In the case of "close-request" signal, the default handler's flag is `G_SIGNAL_RUN_LAST`.
The handler `pref_close_cb` is not connected by `g_signal_connect_after`.
So the number of stages are two.

1. Signal handler `pref_close_cb` is invoked.
2. Default handler is invoked.

And If the user signal handler returns TRUE, then other handlers will be stopped being invoked.
Therefore, the program above prevents the invocation of the default handler and stop the closing process of the dialog.

The following codes are extracted from `tfeapplication.c`.

~~~C
static gulong pref_close_request_handler_id = 0;
static gulong alert_close_request_handler_id = 0;

... ...

static gboolean
dialog_close_cb (GtkDialog *dialog, gpointer user_data) {
  gtk_widget_hide (GTK_WIDGET (dialog));
  return TRUE;
}

... ...

static void
pref_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  gtk_widget_show (GTK_WIDGET (pref));
}

... ...

/* ----- quit application ----- */
void
tfe_application_quit (GtkWindow *win) {
  if (pref_close_request_handler_id > 0)
    g_signal_handler_disconnect (pref, pref_close_request_handler_id);
  if (alert_close_request_handler_id > 0)
    g_signal_handler_disconnect (alert, alert_close_request_handler_id);
  g_clear_object (&settings);
  gtk_window_destroy (GTK_WINDOW (alert));
  gtk_window_destroy (GTK_WINDOW (pref));
  gtk_window_destroy (win);
}

... ...

static void
tfe_startup (GApplication *application) {

  ... ...

  pref = GTK_DIALOG (gtk_builder_get_object (build, "pref"));
  pref_close_request_handler_id = g_signal_connect (GTK_DIALOG (pref), "close-request", G_CALLBACK (dialog_close_cb), NULL);

  ... ... 
}
~~~

The function `tfe_application_quit` destroys top level windows and quits the application.
It first disconnects the handlers from the signal "close-request".

### Alert dialog

If a user closes a page which hasn't been saved, it is advisable to show an alert to confirm it.
Alert dialog is used in this application for such a situation.

~~~xml
  <object class="GtkDialog" id="alert">
    <property name="title">Are you sure?</property>
    <property name="resizable">FALSE</property>
    <property name="modal">TRUE</property>
    <property name="transient-for">win</property>
    <child internal-child="content_area">
      <object class="GtkBox">
        <child>
          <object class="GtkBox">
            <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
            <property name="spacing">12</property>
            <property name="margin-start">12</property>
            <property name="margin-end">12</property>
            <property name="margin-top">12</property>
            <property name="margin-bottom">12</property>
            <child>
              <object class="GtkImage">
                <property name="icon-name">dialog-warning</property>
                <property name="icon-size">GTK_ICON_SIZE_LARGE</property>
              </object>
            </child>
            <child>
              <object class="GtkLabel" id="lb_alert">
              </object>
            </child>
          </object>
        </child>
      </object>
    </child>
    <child type="action">
      <object class="GtkButton" id="btn_cancel">
        <property name="label">Cancel</property>
      </object>
    </child>
    <child type="action">
      <object class="GtkButton" id="btn_accept">
        <property name="label">Close</property>
      </object>
    </child>
    <action-widgets>
      <action-widget response="cancel" default="true">btn_cancel</action-widget>
      <action-widget response="accept">btn_accept</action-widget>
    </action-widgets>
    <signal name="response" handler="alert_response_cb" swapped="NO" object="nb"></signal>
  </object>
~~~

This ui file describes the alert dialog.
Some part are the same as preference dialog.
There are two objects in the content area, GtkImage and GtkLabel.

GtkImage shows an image.
The image can comes from files, resources, icon theme and so on.
The image above displays an icon from the current icon theme.
You can see icons in the theme by `gtk4-icon-browser`.

~~~
$ gtk4-icon-browser
~~~

The icon named "dialog-warning" is something like this.

![dialog-warning icon is like ...](../image/dialog_warning.png){width=4.19cm height=1.62cm}

These are made by my hand.
The real image on the alert dialog is nicer.

The GtkLabel `lb_alert` has no text yet.
An alert message will be inserted by the program later.

There are two child tags which have "action" type.
They are button objects located in the action area.
Action-widgets tag describes the actions of the buttons.
Btn\_cancel button emits response signal with cancel response (GTK_RESPONSE_CANCEL) if it is clicked on.
Btn\_accept button emits response signal with accept response (GTK_RESPONSE_ACCEPT) if it is clicked on.
The response signal is connected to `alert_response_cb` handler.

The alert dialog keeps alive while the application lives.
The "close-request" signal is stopped by the handler `dialog_close_cb` like the preference dialog.

## Close and quit handlers

If a user closes a page or quits the application without saving the contents, the application alerts.

~~~C
static gboolean is_quit;

... ...

void
close_cb (GtkNotebook *nb) {
  is_quit = false;
  if (has_saved (GTK_NOTEBOOK (nb)))
    notebook_page_close (GTK_NOTEBOOK (nb));
  else {
    gtk_label_set_text (lb_alert, "Contents aren't saved yet.\nAre you sure to close?");
    gtk_button_set_label (close_btn_close, "Close");
    gtk_widget_show (GTK_WIDGET (alert));
  }
}

... ...

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  close_cb (GTK_NOTEBOOK (nb));
}

... ...

void
alert_response_cb (GtkDialog *alert, int response_id, gpointer user_data) {
  GtkNotebook *nb = GTK_NOTEBOOK (user_data);
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  gtk_widget_hide (GTK_WIDGET (alert));
  if (response_id == GTK_RESPONSE_ACCEPT) {
    if (is_quit)
      tfe_application_quit (GTK_WINDOW (win));
    else
      notebook_page_close (nb);
  }
}

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer nb) {
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW);

  is_quit = true;
  if (has_saved_all (GTK_NOTEBOOK (nb)))
    tfe_application_quit (GTK_WINDOW (win));
  else {
    gtk_label_set_text (lb_alert, "Contents aren't saved yet.\nAre you sure to quit?");
    gtk_button_set_label (close_btn_close, "Quit");
    gtk_widget_show (GTK_WIDGET (alert));
  }
}

static void
tfe_startup (GApplication *application) {

  ... ...

  alert = GTK_DIALOG (gtk_builder_get_object (build, "alert"));
  alert_close_request_handler_id = g_signal_connect (GTK_DIALOG (alert), "close-request", G_CALLBACK (dialog_close_cb), NULL);
  lb_alert = GTK_LABEL (gtk_builder_get_object (build, "lb_alert"));
  btn_accept = GTK_BUTTON (gtk_builder_get_object (build, "btn_accept"));

  ... ...

}
~~~

The static variable `is_quit` is true when user tries to quit the application and false otherwise.
When user presses "Ctrl-w", `close_activated` handler is invoked.
It just calls `close_cb`.
When user clicks on the close button, `close_cb` handler is invoked.

The handler sets `is_quit` to false.
The function `has_saved` returns true if the current page has been saved.
If it is true, it calls `notebook_page_close` to close the current page.
Otherwise, it sets the text in the label and button, then shows the alert dialog.

The response signal of the dialog is connected to the handler `alert_response_cb`.
It hides the dialog first.
Then check the response\_id.
If it is `GTK_RESPONSE_ACCEPT`, which means user clicked on the close button, then closes the current page.

When user press "Ctrl-q" or clicked on the quit menu, then `quit_activated` handler is invoked.
The handler sets `is_quit` to true.
The function `has_saved_all` returns true if all the pages have been saved.
If it is true, it calls `tfe_application_quit` to quit the application.
Otherwise, it sets the text in the label and button, then shows the alert dialog.

Now `alert_response_cb` works similar but it calls `tfe_application_quit`instead of `notebook_page_close`.

The static variables `alert`, `lb_alert` and `btn_accept` are set in the startup handler.
And the signal "close-request" and `dialog_close_cb` handler are connected.

@@@include
tfe6/tfenotebook.c has_saved has_saved_all
@@@

- 1-14: `has_saved` function.
- 10: The function `gtk_text_buffer_get_modified` returns true if the content of the buffer has been modified since the modified flag had set false.
The flag is set to false when the buffer is generated.
It is reset to false when it is replaced with a new contents from a file or it is saved to a file.
- 11-13: This function returns true if the contents of the current page has been saved and no modification has been made.
If it returns false, then the user tries to close the current page without saving the modified contents.
- 16-33: `has_saved_all` function.
This function is similar to `has_saved` function.
It returns true if all the pages have been saved.
It returns false if at least one page has been modified since it last had been saved.

## Notebook page tab

If you have some pages and edit them together, you might be confused which file needs to be saved.
Common file editors changes the tab when the contents are modified.
GtkTextBuffer provides "modified-changed" signal to notify the modification.

~~~C
static void
notebook_page_build (GtkNotebook *nb, GtkWidget *tv, char *filename) {
  ... ...
  g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed_cb), NULL);
  g_signal_connect (tb, "modified-changed", G_CALLBACK (modified_changed_cb), tv);
}
~~~

When a page is built, connect "change-file" and "modified-changed" signals to `file_changed_cb` and `modified_changed_cb` handlers respectively.

@@@include
tfe6/tfenotebook.c file_changed_cb modified_changed_cb
@@@

- 1-20: `file_changed_cb` handler.
- 9-10: If the signal emits during the page is being built, it is possible that `tv` isn't a descendant of `nb`.
That is, there's no page corresponds to `tv`.
Then, it isn't necessary to change the name of the tab because no tab exists.
- 13-15: If `file` is GFile, then it gets the filename and unrefs `file`.
- 16-17: Otherwise, `file` is probably NULL and it assigns "Untitled" related name to `filename`
- 18-19: Generates GtkLabel with `filename` and sets the tab of the page with the GtkLabel.
- 22-40: `modified_changed_cb` handler.
- 31-32: If `tv` isn't a descendant of `nb`, then nothing needs to be done.
- 33-35: If the content is modified, then it gets the text of the tab and adds asterisk at the beginning of the text.
- 36-37: Sets the tab with the asterisk prepended text.
- 38-39: Otherwise the modified bit is off.
It is because content is saved.
It calls `file_changed_cb` and resets the filename, that means it leaves out the asterisk.

## Font

### GtkFontButton and GtkFontChooser

The GtkFontButton is a button which displays the current font.
It opens a font chooser dialog if a user clicked on the button.
A user can change the font (family, style, weight and size) with the dialog.
Then the button keeps the new font and displays it.

The button and its signal "font-set" is initialized in the application startup process.

~~~C
static void
font_set_cb (GtkFontButton *fontbtn, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW (user_data);
  PangoFontDescription *pango_font_desc;

  pango_font_desc = gtk_font_chooser_get_font_desc (GTK_FONT_CHOOSER (fontbtn));
  set_font_for_display_with_pango_font_desc (win, pango_font_desc);
}

static void
tfe_startup (GApplication *application) {

  ... ...

  fontbtn = GTK_FONT_BUTTON (gtk_builder_get_object (build, "fontbtn"));
  g_signal_connect (fontbtn, "font-set", G_CALLBACK (font_set_cb), win);

  ... ...

}
~~~

In the startup handler, set the variable `fontbtn` to point the GtkFontButton object.
Then connect the "font-set" signal to `font_set_cb` handler.
The signal "font-set" is emitted when the user selects a font.

GtkFontChooser is an interface implemented by GtkFontButton.
The function `gtk_font_chooser_get_font_desc` gets the PangoFontDescription of the currently selected font.

Another function `gtk_font_chooser_get_font` returns a font name which includes family, style, weight and size.
I thought it might be able to be applied to tfe editor.
The font name can be used to the `font` property of GtkTextTag as it is.
But it can't be used to the CSS without converting the string to fit.
CSS is appropriate to change the font of entire text in all the buffers.
I think GtkTextTag is less appropriate.
If you know a good solution, please post it to [issue](https://github.com/ToshioCP/Gtk4-tutorial/issues) and let me know.

It takes many codes to set the CSS from the PangoFontDescription so the task is left to the function `set_font_for_display_with_pango_font_desc`.

### CSS and Pango

A new file `css.c` is made for functions related to CSS.

@@@include
tfe6/css.c
@@@

- 3-11: `set_css_for_display`.
This function sets CSS for GdkDisplay.
The content of the function is the same as the part of startup handler in the previous version of `tfeapplication.c`.
- 13-20: `set_font_for_display`.
This function sets CSS with font-family, font-style, font-weight and font-size.
  - font-family is a name of a font. For example, sans-serif, monospace, Helvetica and "Noto Sans" are font-family.
It is recommended to quote font family names that contains white space, digits, or punctuation characters other than hyphens.
  - font-style is one of normal, italic and oblique.
  - font-weight specifies the thickness of a font.
It is normal or bold.
It can be specified with a number between 100 and 900.
Normal is the same as 400.
Bold is 700.
  - font-size specifies the size of a font.
Small, medium, large and 12pt are font-size.
- 17: Makes CSS text.
The function `g_strdup_printf` generates a new string with printf-like formatting.
- 22-91: `set_font_for_display_with_pango_font_desc`.
This function takes out font-family, font-style, font-weight and font-size from the PangoFontDescription object and calls `set_font`for_display`.
- 31: Gets the font-family of `pango_font_desc`.
- 32-46: Gets the font-style of `pango_font_desc`.
The functions `pango_font_description_get_style` returns an enumerated value.
- 47-88: Gets the font-weight of `pango_font_desc`.
The function `pango_font_description_get_weight` returns an enumerated value.
They corresponds to the numbers from 100 to 900.
- 89: Gets the font-size of `pango_font_desc`.
The function `pango_font_description_get_size` returns the size of a font.
The unit of this size is (1/PANGO\_SCALE)pt.
If the font size is 10pt, the function returns 10*PANGO\_SCALE.
PANGO\_SCALE is defined as 1024.
Therefore, 10*PANGO\_SCALE is 10240.
- 90: calls `set_font_for_display` to set CSS for the GdkDisplay.

## GSettings

We want to maintain the font data after the application quits.
There are some ways to implement it.

- Make a configuration file.
For example, a text file "~/.config/tfe/font.cfg" keeps font information.
- Use GSettings object.
The basic idea of GSettings are similar to configuration file.
Configuration information data is put into a database file.

The coding with GSettings object is simple and easy.
However, it is a bit hard to understand the concept.
This subsection describes the concept first and then how to program it.

### GSettings schema

GSettings schema describes a set of keys, value types and some other information.
GSettings object uses this schema and it writes/reads the value of a key to/from the right place in the database.

- A schema has an id.
The id must be unique.
We often use the same string as application id, but schema id and application id are different.
You can use different name from application id.
Schema id is a string delimited by periods.
For example, "com.github.ToshioCP.tfe" is a correct schema id.
- A schema usually has a path.
The path is a location in the database.
Each key is stored under the path.
For example, if a key `font` is defined with a path `/com/github/ToshioCP/tfe/`, the key's location in the database is `/com/github/ToshioCP/tfe/font`.
Path is a string begins with and ends with a slash (`/`).
And it is delimited by slashes.
- GSettings save information as key-value style.
Key is a string begins with lower case characters followed by lower case, digit or dash (`-`) and ends with lower case or digit.
No consecutive dashes are allowed.
Values can be any type.
GSettings stores values as GVariant type, which may contain, for example, integer, double, boolean, string or complex types like an array.
The type of values needs to be defined in the schema.
- A default value needs to be set for each key.
- A summery and description can be set for each key optionally.

Schemas are described in an XML format.
For example,

@@@include
tfe6/com.github.ToshioCP.tfe.gschema.xml
@@@

- 4: The type attribute is "s".
It is [GVariant format string](https://developer.gnome.org/glib/stable/gvariant-format-strings.html).
Other common types are:
  - "b": gboolean
  - "i": gint32.
  - "d": double
Further information is in the website `GVariant format string` above.

### gsettings

First, let's try `gsetting` application.
It is a configuration tool for GSettings.

~~~
$ gsettings help
Usage:
  gsettings --version
  gsettings [--schemadir SCHEMADIR] COMMAND [ARGS?]

Commands:
  help                      Show this information
  list-schemas              List installed schemas
  list-relocatable-schemas  List relocatable schemas
  list-keys                 List keys in a schema
  list-children             List children of a schema
  list-recursively          List keys and values, recursively
  range                     Queries the range of a key
  describe                  Queries the description of a key
  get                       Get the value of a key
  set                       Set the value of a key
  reset                     Reset the value of a key
  reset-recursively         Reset all values in a given schema
  writable                  Check if a key is writable
  monitor                   Watch for changes

Use "gsettings help COMMAND" to get detailed help.
~~~

List schemas.

~~~
$ gsettings list-schemas
org.gnome.rhythmbox.podcast
ca.desrt.dconf-editor.Demo.Empty
org.gnome.gedit.preferences.ui
org.gnome.evolution-data-server.calendar
org.gnome.rhythmbox.plugins.generic-player

... ...

~~~

Each line is an id of a schema.
Each schema has a key-value configuration data.
You can see them with list-recursively command.
Let's look at the keys and values of `org.gnome.calculator` schema.

~~~
$ gsettings list-recursively org.gnome.calculator
org.gnome.calculator source-currency ''
org.gnome.calculator source-units 'degree'
org.gnome.calculator button-mode 'basic'
org.gnome.calculator target-currency ''
org.gnome.calculator base 10
org.gnome.calculator angle-units 'degrees'
org.gnome.calculator word-size 64
org.gnome.calculator accuracy 9
org.gnome.calculator show-thousands false
org.gnome.calculator window-position (122, 77)
org.gnome.calculator refresh-interval 604800
org.gnome.calculator target-units 'radian'
org.gnome.calculator precision 2000
org.gnome.calculator number-format 'automatic'
org.gnome.calculator show-zeroes false
~~~

This schema is used by Gnome Calculator.
Run the calculator and change the mode, then check the schema again.

~~~
$ gnome-calculator
~~~

![gnome-calculator basic mode](../image/gnome_calculator_basic.png){width=5.34cm height=5.97cm}


Then, change the mode to advanced and quit.

![gnome-calculator advanced mode](../image/gnome_calculator_advanced.png){width=10.74cm height=7.14cm}

Run gsettongs and check whether the value of `button-mode` changes.

~~~
$ gsettings list-recursively org.gnome.calculator

... ...

org.gnome.calculator button-mode 'advanced'

... ...

~~~

Now we know that Gnome Calculator used gsettings and it sets `button-mode` key to "advanced" which is the current mode.
The value remains even the calculator quits.
So when the calculator is run again, it will appear as an advanced mode calculator.

### glib-compile-schemas

GSettings schemas are specified with XML format.
The XML schema files must have the filename extension `.gschema.xml`.
The following is the XML schema file for the application `tfe`.

@@@include
tfe6/com.github.ToshioCP.tfe.gschema.xml
@@@

The filename is "com.github.ToshioCP.tfe.gschema.xml".
Schema XML filenames are usually the schema id followed by ".gschema.xml" suffix.
You can use different name from schema id, but it is not recommended.

- 2: The top level element is `<schemalist>`.
- 3: schema tag has `path` and `id` attributes.
A path determines where the settings are stored in the conceptual global tree of settings.
An id identifies the schema.
- 4: Key tag has two attributes.
Name is the name of the key.
Type is the type of the value of the key and specified with [GVariant format string](https://developer.gnome.org/glib/stable/gvariant-format-strings.html).
- 5: default value of the key `font` is `Monospace 12`.
- 6: Summery and description elements describes the key.
They are optional, but it is recommended to add them in the XML file.

The XML file is compiled by glib-compile-schemas.
When compiling, `glib-compile-schemas` compiles all the XML files which have ".gschema.xml" file extension in the directory given as an argument.
It converts the XML file into a binary file `gschemas.compiled`.
Suppose the XML file above is under `tfe6`directory.

~~~
$ glib-compile-schemas tfe6
~~~

Then, `gschemas.compiled` is generated under `tfe6`.
When you test your application, set `GSETTINGS_SCHEMA_DIR` so that GSettings objet can find `gschemas.compiled`.

~~~
$ GSETTINGS_SCHEMA_DIR=(the directory gschemas.compiled is located):$GSETTINGS_SCHEMA_DIR (your application name)
~~~

This is because GSettings object searches `GSETTINGS_SCHEMA_DIR` for `gschemas.compiled`. 

GSettings object looks for this file by the following process.

- It searches `glib-2.0/schemas` subdirectories of all the directories specified in the environment cariable `XDG_DATA_DIRS`.
Most common directory is `/usr/share/glib-2.0/schemas`. 
- If `GSETTINGS_SCHEMA_DIR` environment variable is defined, it searches all the directories specified in the variable.
`GSETTINGS_SCHEMA_DIR` can specify multiple directories delimited by colon (:).

In the directories above, all the `.gschema.xml` files are stored.
Therefore, when you install your application, follow the instruction below to install your schemas.

1. Make `.gschema.xml` file.
2. Copy it to one of the directories above. For example, `/usr/share/glib-2.0/schemas`.
3. Run `glib-compile-schemas` on the directory above.

### Meson.build

Meson provides `gnome.compile_schemas` method to compile XML file in the build directory.
This is used to test the application.
Write the following to the `meson.build` file.

@@@if gfm
~~~meson
@@@else
~~~
@@@end
gnome.compile_schemas(build_by_default: true, depend_files: 'com.github.ToshioCP.tfe.gschema.xml')
~~~

- `build_by_default`: If it is true, the target will be build by default.
- `depend_files`: XML files to be compiled.

In the example above, this method runs `glib-compile-schemas` to generate `gschemas.compiled` from the XML file `com.github.ToshioCP.tfe.gschema.xml`.
The file `gschemas.compiled` is located under the build directory.
If you run meson as `meson _build` and ninja as `ninja -C _build`, then it is under `_build` directory.

After compilation, you can test your application like this:

~~~
$ GSETTINGS_SCHEMA_DIR=_build:$GSETTINGS_SCHEMA_DIR _build/tfe
~~~

### GSettings object and g\_settings\_bind

Write gsettings related codes to `tfeapplication.c'.

~~~C
... ...
static GSettings *settings;
... ...

void
tfe_application_quit (GtkWindow *win) {
  ... ...
  g_clear_object (&settings);
  ... ...
}

static void
tfe_startup (GApplication *application) {
  ... ...
  settings = g_settings_new ("com.github.ToshioCP.tfe");
  g_settings_bind (settings, "font", fontbtn, "font", G_SETTINGS_BIND_DEFAULT);
  ... ...
}
~~~

Static variable `settings` keeps a pointer to GSettings instance.
Before application quits, the application releases the GSettings instance.
The function `g_clear_object` is used.

Startup handler creates GSettings instance with the schema id "com.github.ToshioCP.tfe" and assigns the pointer to `settings`.
The function `g_settings_bind` connects the settings keys (key and value) and the "font" property of `fontbtn`.
Then the two values will be always the same.
If one value changes then the other will automatically change.

You need to make an effort to understand GSettings concept, but coding is very simple.
Just generate a GSettings object and bind it to a property of an object.

## Installation

It is a good idea to install your application in `$HOME/local/bin` directory if you have installed gtk4 under the instruction in Section 2.
Then you need to put `--prefix=$HOME/local` option to meson like this.

~~~
$ meson --prefix=$HOME/local _build
~~~

Modify `meson.build` abd add install option and set it true in executable function.

@@@if gfm
~~~meson
@@@else
~~~
@@@end
executable('tfe', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: true)
~~~

You can install your application by:

~~~
$ ninja -C _build install
~~~

However, you need to do one more thing.
Copy your XML file to `$HOME/local/share/glib-2.0/schemas/`, which is specified in `GSETTINGS_SCHEMA_DIR` environment variable,
and run `glib-compile-schemas` on that directory.

@@@if gfm
~~~meson
@@@else
~~~
@@@end
schema_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas/'
install_data('com.github.ToshioCP.tfe.gschema.xml', install_dir: schema_dir)
~~~

- get_option: This function returns the value of build options.
The default value of the option 'prefix' is "/usr/local", but it is "\$HOME/local" because we have run meson with prefix option.
The default value of the option 'datadir' is "share".
The operator '/' connects the strings with '/' separator.
So, `$HOME/local/share/glib-2.0/schemas` is assigned to the varable `schema_dir`.
- install_data: This function installs the data to the install directory.

Meson can runs a post compile script.

@@@if gfm
~~~meson
@@@else
~~~
@@@end
meson.add_install_script('glib-compile-schemas', schema_dir)
~~~

This method runs 'glib-compile-schemas' with an argument `schema_dir`.
The following is `meson.build`.

@@@include
tfe6/meson.build
@@@

Source files of `tfe` is under [src/tfe6](tfe6) directory.
Copy them to your temporary directory and try to compile and install.

~~~
$ meson --prefix=$HOME/local _build
$ ninja -C _build
$ GSETTINGS_SCHEMA_DIR=_build:$GSETTINGS_SCHEMA_DIR _build/tfe
$ ninja -C _build install
$ tfe
$ ls $HOME/local/bin
... ...
... tfe
... ...
$ ls $HOME/local/share/glib-2.0/schemas
com.github.ToshioCP.tfe.gschema.xml
gschema.dtd
gschemas.compiled
... ...
~~~

The screenshot is as follows.

![tfe6](../image/tfe6.png){width=9.06cm height=6.615cm}

