Up: [Readme.md](../Readme.md),  Prev: [Section 13](sec13.md), Next: [Section 15](sec15.md)

# tfeapplication.c

`tfeapplication.c` includes all the code other than `tfetxtview.c` and `tfenotebook.c`.
It does following things.

- Application support, mainly handling command line arguments.
- Build widgets using ui file.
- Connect button signals and their handlers.
- Manage CSS.

## main

Th function `main` is the first invoked function in C language.
It connects the command line given by the user and GTK application.

~~~C
 1 int
 2 main (int argc, char **argv) {
 3   GtkApplication *app;
 4   int stat;
 5 
 6   app = gtk_application_new ("com.github.ToshioCP.tfe", G_APPLICATION_HANDLES_OPEN);
 7 
 8   g_signal_connect (app, "startup", G_CALLBACK (tfe_startup), NULL);
 9   g_signal_connect (app, "activate", G_CALLBACK (tfe_activate), NULL);
10   g_signal_connect (app, "open", G_CALLBACK (tfe_open), NULL);
11 
12   stat =g_application_run (G_APPLICATION (app), argc, argv);
13   g_object_unref (app);
14   return stat;
15 }
~~~

- 6: Generate GtkApplication object.
- 8-10: Connect "startup", "activate" and "open signals to their handlers.
- 12: Run the application.
- 13-14: release the reference to the application and return the status.

## statup signal handler

Startup signal is emitted just after the application is generated.
What the signal handler needs to do is initialization of the application.

- Build the widgets using ui file.
- Connect button signals and their handlers.
- Set CSS.

The handler is as follows.

~~~C
 1 static void
 2 tfe_startup (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkApplicationWindow *win;
 5   GtkNotebook *nb;
 6   GtkBuilder *build;
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
20   g_signal_connect (btno, "clicked", G_CALLBACK (open_clicked), nb);
21   g_signal_connect (btnn, "clicked", G_CALLBACK (new_clicked), nb);
22   g_signal_connect (btns, "clicked", G_CALLBACK (save_clicked), nb);
23   g_signal_connect (btnc, "clicked", G_CALLBACK (close_clicked), nb);
24   g_object_unref(build);
25 
26 GdkDisplay *display;
27 
28   display = gtk_widget_get_display (GTK_WIDGET (win));
29   GtkCssProvider *provider = gtk_css_provider_new ();
30   gtk_css_provider_load_from_data (provider, "textview {padding: 10px; font-family: monospace; font-size: 12pt;}", -1);
31   gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
32 }
~~~

- 12-15: Build widgets using ui file (resource).
Connect the top window and the application using `gtk_window_set_application`.
- 16-23: Get buttons and connect their signals and handlers.
- 24: Release the reference to GtkBuilder.
- 26-31: Set CSS.
CSS in GTK is similar to CSS in HTML.
You can set margin, border, padding, color, font and so on with CSS.
In this program CSS is in line 30.
It sets padding, font-family and font size of GtkTextView.
- 26-28: GdkDisplay is used to set CSS.
CSS will be explained in the next subsection.

## CSS in GTK

CSS is an abbretiation of Cascading Style Sheet.
It is originally used with HTML to describe the presentation semantics of a document.
You might have found that the widgets in GTK is simialr to the window in a browser.
It implies that CSS can also be apllied to GTK windowing system.

### CSS nodes, selectors

The syntax of CSS is as follws.

    selector { color: yellow; padding-top: 10px; ...}

Every widget has CSS node.
For example GtkTextView has `textview` node.
If you want to set style to GtkTextView, set "textview" to the selector.

    textview {color: yeallow; ...}

Class, ID and some other things can be applied to the selector like Web CSS. Refer GTK4 API reference for further information.

In line 30, the CSS is a string.

    textview {padding: 10px; font-family: monospace; font-size: 12pt;}

- padding is a space between the border and contents.
This space makes the text easier to read.
- font-family is a name of font.
"monospace" is one of the generic family font keywords.
- font-size is set to 12pt.
It is a bit large, but easy on the eyes especially for elderly people.

### GtkStyleContext, GtkCSSProvider and GdkDisplay

GtkStyleContext is an object that stores styling information affecting a widget.
Each widget is connected to the corresponding GtkStyleContext.
You can get the context by `gtk_widget_get_style_context`.

GtkCssProvider is an object which parses CSS in order to style widgets.

To apply your CSS to widgets, you need to add GtkStyleProvider (the interface of GtkCSSProvider) to GtkStyleContext.
However, instead, you can add it to GdkDisplay of the window (usually top level window).

Look at the source file of `startup` handler again.

- 28: The display is obtained by `gtk_widget_get_display`.
- 29: Generate GtkCssProvider.
- 30: Set the CSS into the provider.
- 31: Add the provider to the display.

It is possible to add the provider to the context of GtkTextView instead of GdkDiplay.
To do so, rewrite `tfe_text_view_new`.

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

CSS set to the context takes precedence over the one set to the display.

## activate and open handler

The handler of "activate" and "open" signal are `tfe_activate` and `tfe_open` respectively.
They just generate a new GtkNotebookPage.

~~~C
 1 static void
 2 tfe_activate (GApplication *application) {
 3   GtkApplication *app = GTK_APPLICATION (application);
 4   GtkWidget *win;
 5   GtkWidget *boxv;
 6   GtkNotebook *nb;
 7 
 8   win = GTK_WIDGET (gtk_application_get_active_window (app));
 9   boxv = gtk_window_get_child (GTK_WINDOW (win));
10   nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
11 
12   notebook_page_new (nb);
13   gtk_widget_show (GTK_WIDGET (win));
14 }
15 
16 static void
17 tfe_open (GApplication *application, GFile ** files, gint n_files, const gchar *hint) {
18   GtkApplication *app = GTK_APPLICATION (application);
19   GtkWidget *win;
20   GtkWidget *boxv;
21   GtkNotebook *nb;
22   int i;
23 
24   win = GTK_WIDGET (gtk_application_get_active_window (app));
25   boxv = gtk_window_get_child (GTK_WINDOW (win));
26   nb = GTK_NOTEBOOK (gtk_widget_get_last_child (boxv));
27 
28   for (i = 0; i < n_files; i++)
29     notebook_page_new_with_file (nb, files[i]);
30   if (gtk_notebook_get_n_pages (nb) == 0)
31     notebook_page_new (nb);
32   gtk_widget_show (win);
33 }
~~~

- 1-14: `tfe_activate`.
- 8-10: Get GtkNotebook object.
- 12-13: Generate a new GtkNotebookPage and show the window.
- 16-33: `tfe_open`.
- 24-26: Get GtkNotebook object.
- 28-29: Generate GtkNotebookPage with files.
- 30-31: If opening and reading file failed and no GtkNotebookPage has generated, then generate a empty page.
- 32: Show the window.

These codes have become really simple thanks to tfenotebook.c and tfetextview.c.

## Primary instance

Only one GApplication instance can be run at a time per session.
The session is a bit diffcult concept and also platform-dependent, but roughly speaking, it corresponds to a graphical desktop login.
When you use your PC, you probably login first, then your desktop appears until you log off.
This is the session.

However, linux is multi process OS and you can run two or more instances of the same application.
Isn't it a contradiction?

When first instance is launched, then it register itself with its application ID (for example, `com.github.ToshioCP.tfe`).
Just after the registration, startup signal is emitted, then activate or open signal is emitted and the instance's main loop runs.
I wrote "startup signal is emitted just after the application is generated" in the prior subsection.
More precisely, it is emitted just after the registration.

If another instance which has the same application ID is invoked after that, it also tries to register itself.
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
 2 open_clicked (GtkWidget *btno, GtkNotebook *nb) {
 3   notebook_page_open (nb);
 4 }
 5 
 6 static void
 7 new_clicked (GtkWidget *btnn, GtkNotebook *nb) {
 8   notebook_page_new (nb);
 9 }
10 
11 static void
12 save_clicked (GtkWidget *btns, GtkNotebook *nb) {
13   notebook_page_save (nb);
14 }
15 
16 static void
17 close_clicked (GtkWidget *btnc, GtkNotebook *nb) {
18   GtkWidget *win;
19   GtkWidget *boxv;
20   gint i;
21 
22   if (gtk_notebook_get_n_pages (nb) == 1) {
23     boxv = gtk_widget_get_parent (GTK_WIDGET (nb));
24     win = gtk_widget_get_parent (boxv);
25     gtk_window_destroy (GTK_WINDOW (win));
26   } else {
27     i = gtk_notebook_get_current_page (nb);
28     gtk_notebook_remove_page (GTK_NOTEBOOK (nb), i);
29   }
30 }
~~~

`open_clicked`, `new_clicked` and `save_clicked` just call corresponding notebook page functions.
`close_clicked` is a bit complicated.

- 22-25: If there's only one page, we need to close the top level window and quit the application.
First, get the top level window and call `gtk_window_destroy`.
- 26-28: Otherwise, it removes the current page.

## meson.build

~~~meson
 1 project('tfe', 'c')
 2 
 3 gtkdep = dependency('gtk4')
 4 
 5 gnome=import('gnome')
 6 resources = gnome.compile_resources('resources','tfe.gresource.xml')
 7 
 8 sourcefiles=files('tfeapplication.c', 'tfenotebook.c', 'tfetextview.c')
 9 
10 executable('tfe', sourcefiles, resources, dependencies: gtkdep)
~~~

In this file, just the source file names are modified.

## source files

The [source files](https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/tfe5) of the text editor `tfe` will be shown in the next section.

You can download the files.
There are two options.

- Use git and clone.
- Run your browser and open the [top page](https://github.com/ToshioCP/Gtk4-tutorial). Then click on "Code" button and click "Download ZIP" in the popup menu.
After that, unzip the archive file.

If you use git, run the terminal and type the following.

    $ git clone https://github.com/ToshioCP/Gtk4-tutorial.git

The source files are under `/src/tfe5` directory.

Up: [Readme.md](../Readme.md),  Prev: [Section 13](sec13.md), Next: [Section 15](sec15.md)
