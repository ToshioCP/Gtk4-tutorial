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

@@@ tfe5/tfeapplication.c main

- 6: Generates GtkApplication object.
- 8-10: Connects "startup", "activate" and "open signals to their handlers.
- 12: Runs the application.
- 13-14: releases the reference to the application and returns the status.

## startup signal handler

Startup signal is emitted just after the application is generated.
What the signal handler needs to do is initialization of the application.

- Builds the widgets using ui file.
- Connects button signals and their handlers.
- Sets CSS.

The handler is as follows.

@@@ tfe5/tfeapplication.c tfe_startup

- 12-15: Builds widgets using ui file (resource).
Connects the top window and the application using `gtk_window_set_application`.
- 16-23: Gets buttons and connects their signals and handlers.
- 24: Releases the reference to GtkBuilder.
- 26-31: Sets CSS.
CSS in GTK is similar to CSS in HTML.
You can set margin, border, padding, color, font and so on with CSS.
In this program CSS is in line 30.
It sets padding, font-family and font size of GtkTextView.
- 26-28: GdkDisplay is used to set CSS.
CSS will be explained in the next subsection.

## CSS in GTK

CSS is an abbreviation of Cascading Style Sheet.
It is originally used with HTML to describe the presentation semantics of a document.
You might have found that the widgets in GTK is similar to the window in a browser.
It implies that CSS can also be applied to GTK windowing system.

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

Class, ID and some other things can be applied to the selector like Web CSS. Refer GTK4 API reference for further information.

In line 30, the CSS is a string.

~~~css
textview {padding: 10px; font-family: monospace; font-size: 12pt;}
~~~

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
- 29: Generates GtkCssProvider.
- 30: Puts the CSS into the provider.
- 31: Adds the provider to the display.

It is possible to add the provider to the context of GtkTextView instead of GdkDiplay.
To do so, rewrite `tfe_text_view_new`.

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

## activate and open handler

The handler of "activate" and "open" signal are `tfe_activate` and `tfe_open` respectively.
They just generate a new GtkNotebookPage.

@@@ tfe5/tfeapplication.c tfe_activate tfe_open

- 1-14: `tfe_activate`.
- 8-10: Gets GtkNotebook object.
- 12-13: Generates a new GtkNotebookPage and show the window.
- 16-33: `tfe_open`.
- 24-26: Gets GtkNotebook object.
- 28-29: Generates GtkNotebookPage with files.
- 30-31: If opening and reading file failed and no GtkNotebookPage has generated, then it generates a empty page.
- 32: Shows the window.

These codes have become really simple thanks to tfenotebook.c and tfetextview.c.

## Primary instance

Only one GApplication instance can be run at a time per session.
The session is a bit difficult concept and also platform-dependent, but roughly speaking, it corresponds to a graphical desktop login.
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

@@@ tfe5/tfeapplication.c open_clicked new_clicked save_clicked close_clicked

`open_clicked`, `new_clicked` and `save_clicked` just call corresponding notebook page functions.
`close_clicked` is a bit complicated.

- 22-25: If there's only one page, we need to close the top level window and quit the application.
First, get the top level window and call `gtk_window_destroy`.
- 26-28: Otherwise, it removes the current page.

## meson.build

@@@ tfe5/meson.build

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
