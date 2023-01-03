Up: [README.md](../README.md),  Prev: [Section 14](sec14.md), Next: [Section 16](sec16.md)

# tfeapplication.c

`tfeapplication.c` includes all the code other than `tfetxtview.c` and `tfenotebook.c`.
It does:

- Application support, mainly handling command line arguments.
- Builds widgets using ui file.
- Connects button signals and their handlers.
- Manages CSS.

## main

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
14   stat =g_application_run (G_APPLICATION (app), argc, argv);
15   g_object_unref (app);
16   return stat;
17 }
~~~

- 1: Defines the application id.
It is easy to find the application id, and better than the id is embedded in `gtk_application_new`.
- 8: Creates GtkApplication object.
- 10-12: Connects "startup", "activate" and "open" signals to their handlers.
- 14: Runs the application.
- 15-16: releases the reference to the application and returns the status.

## startup signal handler

Startup signal is emitted just after the GtkApplication instance is initialized.
What the signal handler needs to do is the initialization of the application.

- Builds the widgets using ui file.
- Connects button signals and their handlers.
- Sets CSS.

The handler is as follows.

~~~C
 1 static void
 2 app_startup (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkBuilder *build;
 5   GtkApplicationWindow *win;
 6   GtkNotebook *nb;
 7   GtkButton *btno;
 8   GtkButton *btnn;
 9   GtkButton *btns;
10   GtkButton *btnc;
11 
12   build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfe/tfe.ui");
13   win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (build, "win"));
14   nb = GTK_NOTEBOOK (gtk_builder_get_object (build, "nb"));
15   gtk_window_set_application (GTK_WINDOW (win), app);
16   btno = GTK_BUTTON (gtk_builder_get_object (build, "btno"));
17   btnn = GTK_BUTTON (gtk_builder_get_object (build, "btnn"));
18   btns = GTK_BUTTON (gtk_builder_get_object (build, "btns"));
19   btnc = GTK_BUTTON (gtk_builder_get_object (build, "btnc"));
20   g_signal_connect_swapped (btno, "clicked", G_CALLBACK (open_cb), nb);
21   g_signal_connect_swapped (btnn, "clicked", G_CALLBACK (new_cb), nb);
22   g_signal_connect_swapped (btns, "clicked", G_CALLBACK (save_cb), nb);
23   g_signal_connect_swapped (btnc, "clicked", G_CALLBACK (close_cb), nb);
24   g_object_unref(build);
25 
26 GdkDisplay *display;
27 
28   display = gtk_widget_get_display (GTK_WIDGET (win));
29   GtkCssProvider *provider = gtk_css_provider_new ();
30   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
31   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
32 
33   g_signal_connect (win, "destroy", G_CALLBACK (before_destroy), provider);
34   g_object_unref (provider);
35 }
~~~

- 12-15: Builds widgets using ui file (resource).
Connects the top-level window and the application with `gtk_window_set_application`.
- 16-23: Gets buttons and connects their signals and handlers.
- 24: Releases the reference to GtkBuilder.
- 26-31: Sets CSS.
CSS in Gtk is similar to CSS in HTML.
You can set margin, border, padding, color, font and so on with CSS.
In this program CSS is in line 30.
It sets padding, font-family and font size of GtkTextView.
- 26-28: GdkDisplay is used to set CSS.
CSS will be explained in the next subsection.
- 33: Connects "destroy" signal on the main window and before\_destroy handler.
This handler is explained in the next subsection.
- 34: The provider is useless for the startup handler, so g\_object\_unref(provider) is called.
Note: It doesn't mean the destruction of the provider.
It is referred by the display so the reference count is not zero.

## CSS in Gtk

CSS is an abbreviation of Cascading Style Sheet.
It is originally used with HTML to describe the presentation semantics of a document.
You might have found that the widgets in Gtk is similar to a window in a browser.
It implies that CSS can also be applied to Gtk windowing system.

### CSS nodes, selectors

The syntax of CSS is as follows.

~~~css
selector { color: yellow; padding-top: 10px; ...}
~~~

Every widget has CSS node.
For example GtkTextView has `textview` node.
If you want to set style to GtkTextView, substitute "textview" for the selector.

~~~css
textview {color: yellow; ...}
~~~

Class, ID and some other things can be applied to the selector like Web CSS.
Refer to [GTK 4 API Reference -- CSS in Gtk](https://docs.gtk.org/gtk4/css-overview.html) for further information.

In line 30, the CSS is a string.

~~~css
textview {padding: 10px; font-family: monospace; font-size: 12pt;}
~~~

- padding is a space between the border and contents.
This space makes the textview easier to read.
- font-family is a name of font.
"monospace" is one of the generic family font keywords.
- font-size is set to 12pt.

### GtkStyleContext, GtkCSSProvider and GdkDisplay

GtkStyleContext is an object that stores styling information affecting a widget.
Each widget is connected to the corresponding GtkStyleContext.
You can get the context by `gtk_widget_get_style_context`.

GtkCssProvider is an object which parses CSS in order to style widgets.

To apply your CSS to widgets, you need to add GtkStyleProvider (the interface of GtkCSSProvider) to GtkStyleContext.
However, instead, you can add it to GdkDisplay of the window (usually top-level window).

Look at the source file of `startup` handler again.

- 28: The display is obtained by `gtk_widget_get_display`.
- 29: Creates a GtkCssProvider instance.
- 30: Puts the CSS into the provider.
- 31: Adds the provider to the display.
The last argument of `gtk_style_context_add_provider_for_display` is the priority of the style provider.
`GTK_STYLE_PROVIDER_PRIORITY_APPLICATION` is a priority for application-specific style information.
`GTK_STYLE_PROVIDER_PRIORITY_USER` is also often used and it is the highest priority.
So, `GTK_STYLE_PROVIDER_PRIORITY_USER` is often used to a specific widget.

It is possible to add the provider to the context of GtkTextView instead of GdkDiplay.
To do so, rewrite `tfe_text_view_new`.
First, get the GtkStyleContext object of a TfeTextView object.
Then adds the CSS provider to the context.

~~~C
GtkWidget *
tfe_text_view_new (void) {
  GtkWidget *tv;

  tv = gtk_widget_new (TFE_TYPE_TEXT_VIEW, NULL);

  GtkStyleContext *context;

  context = gtk_widget_get_style_context (GTK_WIDGET (tv));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
  gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  return tv;
}
~~~

CSS in the context takes precedence over CSS in the display.

~~~C
1 static void
2 before_destroy (GtkWidget *win, GtkCssProvider *provider) {
3   GdkDisplay *display = gtk_widget_get_display (GTK_WIDGET (win));
4   gtk_style_context_remove_provider_for_display (display, GTK_STYLE_PROVIDER (provider));
5 }
~~~

When a widget is destroyed, or more precisely during its dispose process, a "destroy" signal is emitted.
The "before\_destroy" handler connects to the signal on the main window.
(See the program list of app\_startup.)
So, it is called when the window is destroyed.

The handler removes the CSS provider from the GdkDisplay.

## activate and open handler

The handler of "activate" and "open" signal are `app_activate` and `app_open` respectively.
They just create a new GtkNotebookPage.

~~~C
 1 static void
 2 app_activate (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
 5   GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
 6   GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
 7 
 8   notebook_page_new (nb);
 9   gtk_window_present (GTK_WINDOW (win));
10 }
11 
12 static void
13 app_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
14   GtkApplication *app = GTK_APPLICATION (application);
15   GtkWidget *win = GTK_WIDGET (gtk_application_get_active_window (app));
16   GtkWidget *boxv = gtk_window_get_child (GTK_WINDOW (win));
17   GtkNotebook *nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
18   int i;
19 
20   for (i = 0; i < n_files; i++)
21     notebook_page_new_with_file (nb, files[i]);
22   if (gtk_notebook_get_n_pages (nb) == 0)
23     notebook_page_new (nb);
24   gtk_window_present (GTK_WINDOW (win));
25 }
~~~

- 1-10: `app_activate`.
- 8-10: Creates a new page and shows the window.
- 12-25: `app_open`.
- 20-21: Creates notebook pages with files.
- 22-23: If no page has created, maybe because of read error, then it creates an empty page.
- 24: Shows the window.

These codes have become really simple thanks to tfenotebook.c and tfetextview.c.

## Primary instance

Only one GApplication instance can be run at a time per session.
The session is a bit difficult concept and also platform-dependent, but roughly speaking, it corresponds to a graphical desktop login.
When you use your PC, you probably login first, then your desktop appears until you log off.
This is the session.

However, Linux is multi process OS and you can run two or more instances of the same application.
Isn't it a contradiction?

When first instance is launched, then it registers itself with its application ID (for example, `com.github.ToshioCP.tfe`).
Just after the registration, startup signal is emitted, then activate or open signal is emitted and the instance's main loop runs.
I wrote "startup signal is emitted just after the application instance is initialized" in the prior subsection.
More precisely, it is emitted just after the registration.

If another instance which has the same application ID is invoked, it also tries to register itself.
Because this is the second instance, the registration of the ID has already done, so it fails.
Because of the failure startup signal isn't emitted.
After that, activate or open signal is emitted in the primary instance, not the second instance.
The primary instance receives the signal and its handler is invoked.
On the other hand, the second instance doesn't receive the signal and it immediately quits.

Try to run two instances in a row.

    $ ./_build/tfe &
    [1] 84453
    $ ./build/tfe tfeapplication.c
    $

First, the primary instance opens a window.
Then, after the second instance is run, a new notebook page with the contents of `tfeapplication.c` appears in the primary instance's window.
This is because the open signal is emitted in the primary instance.
The second instance immediately quits so shell prompt soon appears.

## a series of handlers correspond to the button signals

~~~C
 1 static void
 2 open_cb (GtkNotebook *nb) {
 3   notebook_page_open (nb);
 4 }
 5 
 6 static void
 7 new_cb (GtkNotebook *nb) {
 8   notebook_page_new (nb);
 9 }
10 
11 static void
12 save_cb (GtkNotebook *nb) {
13   notebook_page_save (nb);
14 }
15 
16 static void
17 close_cb (GtkNotebook *nb) {
18   notebook_page_close (GTK_NOTEBOOK (nb));
19 }
~~~

`open_cb`, `new_cb`, `save_cb` and `close_cb` just call corresponding notebook page functions.

## meson.build

~~~meson
 1 project('tfe', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 
 8 sourcefiles=files('tfeapplication.c', 'tfenotebook.c', '../tfetextview/tfetextview.c')
 9 
10 executable('tfe', sourcefiles, resources, dependencies: gtkdep)
~~~

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
