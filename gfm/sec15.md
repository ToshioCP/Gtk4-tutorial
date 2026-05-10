Up: [README.md](../README.md),  Prev: [Section 14](sec14.md), Next: [Section 16](sec16.md)

# Tfe Main Program

The file `tfeapplication.c` is a main program of Tfe.
It includes all the code other than `tfetextview.c` and `tfenotebook.c`.
It does:

- Application support, mainly handling command line arguments.
- Builds widgets using UI files.
- Connects button signals and their handlers.
- Manages CSS.

## The Function main

The function `main` is the first invoked function in C language.
It connects the command line given by the user and Gtk application.

~~~C
 1 #define APPLICATION_ID "com.github.ToshioCP.tfe"
 2
 3 int
 4 main (int argc, char **argv) {
 5   GtkApplication *app;
 6   int stat;
 7
 8   app = gtk_application_new (APPLICATION_ID, G_APPLICATION_HANDLES_OPEN);
 9
10   g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
11   g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
12   g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
13
14   stat = g_application_run (G_APPLICATION (app), argc, argv);
15   g_object_unref (app);
16   return stat;
17 }
~~~

- 1: Defines the application id.
Thanks to the `#define` directive, it is easy to find the application id.
- 8: Creates GtkApplication object.
- 10-12: Connects "startup", "activate" and "open" signals to their handlers.
- 14: Runs the application.
- 15-16: Releases the reference to the application and returns the status.

## Startup Signal Handler

A startup signal is emitted just after the GtkApplication instance is initialized.
The handler initializes the whole application which includes not only GtkApplication instance but also widgets and some other objects.

- Builds the widgets using a UI file.
- Connects button signals and their handlers.
- Sets CSS.

The handler is as follows.

```c
static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkBuilder *build;
  GtkApplicationWindow *win;
  GtkNotebook *nb;
  GtkButton *btno;
  GtkButton *btnn;
  GtkButton *btns;
  GtkButton *btnc;

  build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
  win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
  nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
  gtk_window_set_application (GTK_WINDOW (win), app);
  btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
  btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
  btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
  btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
  g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
  g_signal_connect_swapped (btnn, "clicked", G_CALLBACK (new_cb), nb);
  g_signal_connect_swapped (btns, "clicked", G_CALLBACK (save_cb), nb);
  g_signal_connect_swapped (btnc, "clicked", G_CALLBACK (close_cb), nb);
  g_object_unref(build);

GdkDisplay *display;

  display = gdk_display_get_default ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_string (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}");
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_signal_connect (win, "destroy", G_CALLBACK (before_destroy), provider);
  g_object_unref (provider);
}
```

- 12-15: Builds widgets using a UI resource.
Connects the top-level window and the application with `gtk_window_set_application`.
- 16-23: Gets buttons and connects their signals and handlers.
The macro `g_signal_connect_swapped` connects a signal and handler like `g_signal_connect`.
The difference is that `g_signal_connect_swapped` swaps the user data for the object.
For example, the macro on line 20 swaps `nb` for `btno`.
So, the handler expects that the first argument is `nb` instead of `btno`.
- 24: Releases the reference to GtkBuilder.
- 26-31: Sets CSS.
CSS in Gtk is similar to CSS in HTML.
You can set margin, border, padding, color, font and so on with CSS.
In this program, CSS is on line 30.
It sets padding, font-family and font size of GtkTextView.
CSS will be explained in the next subsection.
- 26-28: GdkDisplay is used to set CSS.
The default GdkDisplay object can be obtained with the function `gdk_display_get_default`.
This function needs to be called after the application is registered.
- 33: Connects "destroy" signal on the main window and before\_destroy handler.
This handler is explained in the next subsection.
- 34: The provider is useless for the startup handler, so it is released.
Note: It doesn't mean the destruction of the provider.
It is referred by the display so the reference count is not zero.

## CSS in Gtk

CSS is an abbreviation of Cascading Style Sheet.
It is originally used with HTML to describe the presentation semantics of a document.
You might have found that widgets in Gtk is similar to elements in HTML.
It shows that CSS can be applied to Gtk windowing system, too.

### CSS Nodes and Selectors

The syntax of CSS is as follows.

~~~css
selector { color: yellow; padding-top: 10px; ...}
~~~

Every widget has CSS node.
For example, GtkTextView has `textview` node.
If you want to set style to GtkTextView, substitute "textview" for `selector` above.

~~~css
textview {color: yellow; ...}
~~~

Class, ID and some other things can be applied to the selector like Web CSS.
Refer to [GTK 4 API Reference -- CSS in Gtk](https://docs.gtk.org/gtk4/css-overview.html) for further information.

The codes of the startup handler has a CSS string on line 30.

~~~css
textview {padding: 10px; font-family: monospace; font-size: 12pt;}
~~~

- Padding is a space between the border and contents.
This space makes the textview easier to read.
- font-family is a name of font.
The font name "monospace" is one of the generic family font keywords.
- Font-size is set to 12pt.

### GtkStyleContext, GtkCssProvider and GdkDisplay

GtkStyleContext is deprecated since version 4.10.
But two functions `gtk_style_context_add_provider_for_display` and `gtk_style_context_remove_provider_for_display` are not deprecated.
They add or remove a css provider object to the GdkDisplay object.

GtkCssProvider is an object which parses CSS for style widgets.

To apply your CSS to widgets, you need to add GtkStyleProvider (the interface of GtkCssProvider) to the GdkDisplay object.
You can get the default display object with the function `gdk_display_get_default`.
The returned object is owned by the function and you don't have its ownership.
So, you don't need to care about releasing it.

Look at the source file of the `startup` handler again.

- 28: The display is obtained by `gdk_display_get_default`.
- 29: Creates a GtkCssProvider instance.
- 30: Puts the CSS into the provider.
- 31: Adds the provider to the display.
The last argument of `gtk_style_context_add_provider_for_display` is the priority of the style provider.
`GTK_STYLE_PROVIDER_PRIORITY_APPLICATION` is a priority for application-specific style information.
Refer to [GTK 4 Reference --- Constants](https://docs.gtk.org/gtk4/index.html#constants) for more information.
You can find other constants, which have "STYLE\_PROVIDER\_PRIORITY\_XXXX" pattern names.

```c
static void
before_destroy (GtkWidget *win, GtkCssProvider *provider) {
  GdkDisplay *display = gdk_display_get_default ();
  gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
}
```

When a widget is destroyed, or more precisely during its disposing process, a "destroy" signal is emitted.
The "before\_destroy" handler connects to the signal on the main window.
(See the program list of app\_startup.)
So, it is called when the window is destroyed.

The handler removes the CSS provider from the GdkDisplay.

Note: CSS providers are removed automatically when the application quits.
So, even if the handler `before_destroy` is removed, the application works.

## Activate and open signal handler

The handlers of "activate" and "open" signals are `app_activate` and `app_open` respectively.
They just create a new GtkNotebookPage.

```c
static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));

  notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}

static void
app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
  GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
  GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
  int i;

  for (i = 0; i < n_files; i++)
    notebook_page_new_with_file (nb, files[i]);
  if (gtk_notebook_get_n_pages (nb) == 0)
    notebook_page_new (nb);
  gtk_window_present (GTK_WINDOW (win));
}
```

- 1-10: `app_activate`.
- 6: In this program, we traverse the widget hierarchy for the sake of simplicity.
However, the most robust design is to extend GtkApplicationWindow and include a pointer to the notebook in its instance structure (e.g., win->nb).
- 8-10: Creates a new page and shows the window.
- 12-25: `app_open`.
- 20-21: Creates notebook pages with files.
- 22-23: If no page has been created, maybe because of read error, then it creates an empty page.
- 24: Shows the window.

These codes have become really simple thanks to tfenotebook.c and tfetextview.c.

## A primary instance

Only one GApplication instance can be run at a time in a session.
The session is a bit difficult concept and also platform-dependent, but roughly speaking, it corresponds to a graphical desktop login.
When you use your PC, you probably login first, then your desktop appears until you log off.
This is the session.

However, Linux is multi process OS and you can run two or more instances of the same application.
Isn't it a contradiction?

When first instance is launched, then it registers itself with its application ID (for example, `com.github.ToshioCP.tfe`).
Just after the registration, the startup signal is emitted, then activate or open signal is emitted and the instance's main loop runs.
I wrote "a startup signal is emitted just after the application instance is initialized" in the prior subsection.
More precisely, it is emitted after the registration.

If another instance which has the same application ID is launched, it also tries to register itself.
Because this is the second instance, the registration of the ID has already been done, so it fails.
Because of the failure, a startup signal isn't emitted.
After that, activate or open signal is emitted in the primary instance, not on the second instance.
The primary instance receives the signal and its handler is invoked.
On the other hand, the second instance doesn't receive the signal and it immediately quits.

Try running two instances in a row.

    $ ./_build/tfe &
    [1] 84453
    $ ./build/tfe tfeapplication.c
    $

First, the primary instance opens a window.
Then, after the second instance is run, a new notebook page with the contents of `tfeapplication.c` appears in the primary instance's window.
This is because the open signal is emitted in the primary instance.
The second instance immediately quits so shell prompt soon appears.

## A series of handlers corresponds to the button signals

```c
static void
open_cb (GtkNotebook *nb) {
  notebook_page_open (nb);
}

static void
new_cb (GtkNotebook *nb) {
  notebook_page_new (nb);
}

static void
save_cb (GtkNotebook *nb) {
  notebook_page_save (nb);
}

static void
close_cb (GtkNotebook *nb) {
  notebook_page_close (GTK_NOTEBOOK (nb));
}
```

`open_cb`, `new_cb`, `save_cb` and `close_cb` just call corresponding notebook page functions.

## meson.build

```
project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

sourcefiles=files('tfeapplication.c', 'tfenotebook.c', '../tfetextview/tfetextview.c')

executable('tfe', sourcefiles, resources, dependencies: gtkdep)
```

In this file, just the source file names are modified from the prior version.

## source files

You can download the files from the [repository](https://github.com/ToshioCP/Gtk4-tutorial).
There are two options.

- Use git and clone.
- Run your browser and open the [top page](https://github.com/ToshioCP/Gtk4-tutorial). Then click on "Code" button and click "Download ZIP" in the popup menu.
After that, unzip the archive file.

If you use git, run the terminal and type the following.

    $ git clone https://github.com/ToshioCP/Gtk4-tutorial.git

The source files are under [/src/tfe5](../src/tfe5) directory.

Up: [README.md](../README.md),  Prev: [Section 14](sec14.md), Next: [Section 16](sec16.md)
