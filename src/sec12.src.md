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

- 6: Generate GtkApplication object.
- 8-10: Connect "startup", "activate" and "open signals to their handlers.
- 12: Run the application.
- 13-14: release the reference to the application and return the status.

## statup signal handler

"startup" signal is emitted just after the application is generated.
What the signal handler needs to do is initialization of the application.

- Build the widgets using ui file.
- Connect button signals and their handlers.
- Set CSS.

The handler is as follows.

@@@ tfe5/tfeapplication.c tfe_startup

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

To apply your CSS to wodgets, you need to add GtkStyleProvider (the interface of GtkCSSProvider) to GtkStyleContext.
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

@@@ tfe5/tfeapplication.c tfe_activate tfe_open

- 1-14: `tfe_activate`.
- 8-10: Get GtkNotebook object.
- 12-13: Generate a new GtkNotebookPage and show the window.
- 16-33: `tfe_open`.
- 24-26: Get GtkNotebook object.
- 28-29: Generate GtkNotebookPage with files.
- 30-31: If opening and reading file failed and no GtkNotebookPage has generated, then generate a empty page.
- 32: Show the window.

These codes have become really simple thanks to tfenotebook.c and tfetextview.c.

## a series of handlers correspond to the button signals

@@@ tfe5/tfeapplication.c open_clicked new_clicked save_clicked close_clicked

`open_clicked`, `new_clicked` and `save_clicked` just call corresponding notebook page functions.
`close_clicked` is a bit complicated.

- 22-25: If there's only one page, closing the last page is considered that it also close the top level window and quit the application.
Therefore, it gets the top level window and call `gtk_window_destroy`.
- 26-28: Otherwise, it removes the current page.

## meson.build

@@@ tfe5/meson.build

This file is just modified the source file names.

