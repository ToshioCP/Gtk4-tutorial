Up: [README.md](../README.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)

# Composite Widgets and Alert Dialog

The source files are in the [Gtk4 tutorial GitHub repository](https://github.com/ToshioCP/Gtk4-tutorial).
Download it and see `src/tfe6` directory.

## An Outline of New Tfe Text Editor

Tfe text editor will be restructured.
The program is divided into six parts.

- Main program: the C main function.
- TfeApplication object: It is like GtkApplication but keeps GSettings and CSS Provider.
- TfeWindow object: It is a window with buttons and a notebook.
- TfePref object: A preference dialog.
- TfeAlert object: An alert dialog.
- pdf2css.h and pdf2css.c: Font and CSS utility functions.

This section describes TfeAlert.
Others will be explained in the following sections.

## Composite Widgets

The alert dialog is like this:

![Alert dialog](/src/images/alert.png)

Tfe uses it when a user quits the application or closes a notebook without saving data to files.

The dialog has a title, buttons, an icon and a message.
Therefore, it consists of several widgets.
Such dialog is called a composite widget.

Composite widgets are defined with template XMLs.
The class is built in the class initialization function and the instances are built and disposed by the following functions.

- gtk\_widget\_init\_template
- gtk\_widget\_dispose\_template

TfeAlert is a good example to know composite widgets.
It is defined with the three files.

- tfealert.ui: XML file
- tfealert.h: Header file
- tfealert.c: C program file

## The XML File

A template tag is used in a composite widget XML.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="TfeAlert" parent="GtkWindow">
    <property name="resizable">FALSE</property>
    <property name="modal">TRUE</property>
    <property name="titlebar">
      <object class="GtkHeaderBar">
        <property name="show-title-buttons">FALSE</property>
        <property name="title-widget">
          <object class="GtkLabel" id="lb_title">
            <property name="label">Are you sure?</property>
            <property name="single-line-mode">True</property>
          </object>
        </property>
        <child type="start">
          <object class="GtkButton" id="btn_cancel">
            <property name="label">Cancel</property>
            <style>
              <class name="suggested-action"/>
            </style>
            <signal name="clicked" handler="cancel_cb" swapped="TRUE" object="TfeAlert"></signal>
          </object>
        </child>
        <child type="end">
          <object class="GtkButton" id="btn_accept">
            <property name="label">Close</property>
            <style>
              <class name="destructive-action"/>
            </style>
            <signal name="clicked" handler="accept_cb" swapped="TRUE" object="TfeAlert"></signal>
          </object>
        </child>
      </object>
    </property>
    <child>
      <object class="GtkBox">
        <property name="orientation">GTK_ORIENTATION_HORIZONTAL</property>
        <property name="spacing">12</property>
        <property name="margin-top">12</property>
        <property name="margin-bottom">12</property>
        <property name="margin-start">12</property>
        <property name="margin-end">12</property>
        <child>
          <object class="GtkImage">
            <property name="icon-name">dialog-warning</property>
            <property name="icon-size">GTK_ICON_SIZE_LARGE</property>
          </object>
        </child>
        <child>
          <object class="GtkLabel" id="lb_message">
          </object>
        </child>
      </object>
    </child>
  </template>
</interface>

```

- 3: A template tag defines a composite widget.
The class attribute tells the class name of the composite widget.
The parent attribute tells the parent class of the composite widget.
So, TfeAlert is a child class of GtkWindow.
The parent attribute specifies the parent class of the composite widget.
While it is technically optional in the XML, it is highly recommended to include it.
Providing the parent class allows GtkBuilder to verify the class hierarchy at runtime and improves the readability of the UI file.
- 4-6: Its three properties are defined.
These properties are inherited from GtkWindow.
The titlebar property has a widget for a custom title bar.
The typical widget is GtkHeaderBar.
- 8: If the property "show-title-buttons" is TRUE, the title buttons like close, minimize and maximize are shown.
Otherwise it is not shown.
The TfeAlert object is not resizable.
It is closed when either of the two buttons, cancel or accept, is clicked.
Therefore the title buttons are not necessary and this property is set to FALSE.
- 9-14: The bar contains a title, implemented as a GtkLabel widget.
The default title is "Are you sure?" but it can be replaced by an instance method.
- 15-32: The bar has two buttons, cancel and accept.
The cancel button is on the left so the child tag has `type="start"` attribute.
The accept button is on the right so the child tag has `type="end"` attribute.
The dialog is shown when the user clicked the close button or the quit menu without saving the data.
Therefore, it is safer for the user to click on the cancel button of the alert dialog.
So, the cancel button has a "suggested-action" CSS class.
Ubuntu colors the button green but the color can be blue or other appropriate one defined by the system.
In the same way the accept button has a "destructive-action" CSS class and is colored red.
Two buttons have signals which are defined by the signal tags.
By setting `swapped="TRUE"`, the instance emitting the signal and the user data are swapped.
Consequently, the `cancel_cb` handler receives the `TfeAlert` object as its first parameter."
- 35-54: A horizontal box has an image icon and a label.
- 44-47: The GtkImage widget displays an image.
The "icon-name" property is an icon name in the icon theme.
The theme depends on your system.
You can check it with an icon browser.

~~~
$ gtk4-icon-browser
~~~

The "dialog-warning" icon is something like this.

![dialog-warning icon is like ...](/src/images/dialog_warning.png)

These were created manually.
The real image on the alert dialog is nicer.

It is possible to define the alert widget as a child of GtkDialog.
But GtkDialog is deprecated since GTK version 4.10.
And users should use GtkWindow instead of GtkDialog.

## The Header File

The header file is similar to the one of TfeTextView.

```c
#pragma once

#include <gtk/gtk.h>

#define TFE_TYPE_ALERT tfe_alert_get_type ()
G_DECLARE_FINAL_TYPE (TfeAlert, tfe_alert, TFE, ALERT, GtkWindow)

/* "response" signal id */
enum TfeAlertResponseType
{
  TFE_ALERT_RESPONSE_ACCEPT,
  TFE_ALERT_RESPONSE_CANCEL
};

const char *
tfe_alert_get_title (TfeAlert *alert);

const char *
tfe_alert_get_message (TfeAlert *alert);

const char *
tfe_alert_get_button_label (TfeAlert *alert);

void
tfe_alert_set_title (TfeAlert *alert, const char *title);

void
tfe_alert_set_message (TfeAlert *alert, const char *message);

void
tfe_alert_set_button_label (TfeAlert *alert, const char *btn_label);

GtkWidget *
tfe_alert_new (void);

GtkWidget *
tfe_alert_new_with_data (const char *title, const char *message, const char* btn_label);
```

- 5-6: These two lines are always needed to define a new object.
`TFE_TYPE_ALERT` is the type of TfeAlert object and it is a macro expanded into the call to `tfe_alert_get_type ()`.
G\_DECLARE\_FINAL\_TYPE macro is expanded into:
  - The declaration of the function `tfe_alert_get_type`
  - `TfeAlert` is defined as a typedef of `struct _TfeAlert`, which is defined in the C file.
  - `TFE_ALERT` and `TFE_IS_ALERT` macro is defined as a cast and type check function.
  - `TfeAlertClass` structure is defined as a final class.
- 8-13: The TfeAlert class has a "response" signal.
It has a parameter and the parameter type is defined as a `TfeAlertResponseType` enumerative constant.
- 15-31: Getter and setter methods.
- 33-37: Functions to create an instance.
The function `tfe_alert_new_with_data` is a convenience function, which creates an instance and sets data at once.

## The C File

### Functions for Composite Widgets

The following codes are extracted from `tfealert.c`.

```C
#include <gtk/gtk.h>
#include "tfealert.h"

struct _TfeAlert {
  GtkWindow parent;
  GtkLabel *lb_title;
  GtkLabel *lb_message;
  GtkButton *btn_accept;
  GtkButton *btn_cancel;
};

G_DEFINE_FINAL_TYPE (TfeAlert, tfe_alert, GTK_TYPE_WINDOW);

static void
cancel_cb (TfeAlert *alert) {
  ... ... ...
}

static void
accept_cb (TfeAlert *alert) {
  ... ... ...
}

static void
tfe_alert_dispose (GObject *gobject) { // gobject is actually a TfeAlert instance.
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_ALERT);
  G_OBJECT_CLASS (tfe_alert_parent_class)->dispose (gobject);
}

static void
tfe_alert_init (TfeAlert *alert) {
  gtk_widget_init_template (GTK_WIDGET (alert));
}

static void
tfe_alert_class_init (TfeAlertClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_alert_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_title);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_message);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_cancel);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), cancel_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), accept_cb);
  ... ... ...
}

GtkWidget *
tfe_alert_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, NULL));
}
```

- The macro `G_DEFINE_FINAL_TYPE` is available since GLib version 2.70.
It is used only for a final type class.
You can use `G_DEFINE_TYPE` macro instead.
They are expanded into:
  - The declaration of the functions `tfe_alert_init` and `tfe_alert_class_init`.
They are defined in the following part of the C program.
  - The definition of the variable `tfe_alert_parent_class`.
  - The definition of the function `tfe_alert_get_type`.
- The names of the members of `_TfeAlert`, which are `lb_title`, `lb_message`, `btn_accept` and `btn_cancel`,
must be the same as the id attribute in the XML file `tfealert.ui`.
- The function `tfe_alert_class_init` initializes the composite widget class.
  - The function `gtk_widget_class_set_template_from_resource` sets the template of the class.
The template is built from the XML resource "tfealert.ui".
At this moment no instance is created.
It just makes the class recognize the structure of the object.
That’s why the top level tag is not object but template in the XML file.
  - The function macro `gtk_widget_class_bind_template_child` connects the member of TfeAlert and the object class in the template.
So, for example, you can access to `lb_title` GtkLabel instance via `alert->lb_title` where `alert` is an instance of TfeAlert class.
  - The function `gtk_widget_class_bind_template_callback` connects the callback function and the `handler` attribute of the signal tag in the XML.
For example, the "clicked" signal on the cancel button has a handler named "cancel\_cb" in the signal tag.
And the function `cancel_cb` exists in the C file above.
These two are connected so when the signal is emitted the function `cancel_cb` is called.
You can add `static` storage class to the callback function thanks to this connection.
- The function `tfe_alert_init` initializes the newly created instance.
You need to call `gtk_widget_init_template` to create and initialize the child widgets in the template.
- The function `tfe_alert_dispose` releases objects.
The function `gtk_widget_dispose_template` clears the template children.
- The function `tfe_alert_new` creates the composite widget TfeAlert instance.
It creates not only TfeAlert itself but also all the child widgets that the composite widget has.

### Other Functions

The following is the full codes of `tfealert.c`.

```c
#include <gtk/gtk.h>
#include "tfealert.h"

struct _TfeAlert {
  GtkWindow parent;
  GtkLabel *lb_title;
  GtkLabel *lb_message;
  GtkButton *btn_accept;
  GtkButton *btn_cancel;
};

G_DEFINE_FINAL_TYPE (TfeAlert, tfe_alert, GTK_TYPE_WINDOW);

enum {
  RESPONSE,
  NUMBER_OF_SIGNALS
};

static guint tfe_alert_signals[NUMBER_OF_SIGNALS];

static void
cancel_cb (TfeAlert *alert) {
  g_signal_emit (alert, tfe_alert_signals[RESPONSE], 0, TFE_ALERT_RESPONSE_CANCEL);
  gtk_window_destroy (GTK_WINDOW (alert));
}

static void
accept_cb (TfeAlert *alert) {
  g_signal_emit (alert, tfe_alert_signals[RESPONSE], 0, TFE_ALERT_RESPONSE_ACCEPT);
  gtk_window_destroy (GTK_WINDOW (alert));
}

const char *
tfe_alert_get_title (TfeAlert *alert) {
  return gtk_label_get_text (alert->lb_title);
}

const char *
tfe_alert_get_message (TfeAlert *alert) {
    return gtk_label_get_text (alert->lb_message);
}

const char *
tfe_alert_get_button_label (TfeAlert *alert) {
  return gtk_button_get_label (alert->btn_accept);
}

void
tfe_alert_set_title (TfeAlert *alert, const char *title) {
  gtk_label_set_text (alert->lb_title, title);
}

void
tfe_alert_set_message (TfeAlert *alert, const char *message) {
  gtk_label_set_text (alert->lb_message, message);
}

void
tfe_alert_set_button_label (TfeAlert *alert, const char *btn_label) {
  gtk_button_set_label (alert->btn_accept, btn_label);
}

static void
tfe_alert_dispose (GObject *gobject) { // gobject is actually a TfeAlert instance.
  gtk_widget_dispose_template (GTK_WIDGET (gobject), TFE_TYPE_ALERT);
  G_OBJECT_CLASS (tfe_alert_parent_class)->dispose (gobject);
}

static void
tfe_alert_init (TfeAlert *alert) {
  gtk_widget_init_template (GTK_WIDGET (alert));
}

static void
tfe_alert_class_init (TfeAlertClass *class) {
  G_OBJECT_CLASS (class)->dispose = tfe_alert_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/ToshioCP/tfe/tfealert.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_title);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, lb_message);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_accept);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), TfeAlert, btn_cancel);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), cancel_cb);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), accept_cb);

  tfe_alert_signals[RESPONSE] = g_signal_new ("response",
                                G_TYPE_FROM_CLASS (class),
                                G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                0 /* class offset */,
                                NULL /* accumulator */,
                                NULL /* accumulator data */,
                                NULL /* C marshaller */,
                                G_TYPE_NONE /* return_type */,
                                1     /* n_params */,
                                G_TYPE_INT
                                );
}

GtkWidget *
tfe_alert_new (void) {
  return GTK_WIDGET (g_object_new (TFE_TYPE_ALERT, NULL));
}

GtkWidget *
tfe_alert_new_with_data (const char *title, const char *message, const char* btn_label) {
  GtkWidget *alert = tfe_alert_new ();
  tfe_alert_set_title (TFE_ALERT (alert), title);
  tfe_alert_set_message (TFE_ALERT (alert), message);
  tfe_alert_set_button_label (TFE_ALERT (alert), btn_label);
  return alert;
}
```

The function `tfe_alert_new_with_data` is used more often than `tfe_alert_new` to create a new instance.
It creates the instance and sets three data at the same time.
The following is the common process when you use the TfeAlert class.

- Call `tfe_alert_new_with_data` and create an instance.
- Call `gtk_window_set_transient_for` to set the transient parent window.
- Call `gtk_window_present` to show the TfeAlert dialog.
- Connect "response" signal and a handler.
- The user clicks on the cancel or accept button.
Then the dialog emits the "response" signal and destroy itself.
- The user catches the signal and do something.

The rest of the program is:

- 14-19: An array for a signal id. You can use a variable instead of an array because the class has only one signal.
But using an array is a common way.
- 21-31: Signal handlers. They emits the "response" signal and destroy the instance itself.
- 33-61: Getters and setters.
- 85-95: Creates the "response" signal.
- 103-110: A convenience function `tfe_alert_new_with_data` creates an instance and sets labels.

## An Example

There's an example in the `src/tfe6/example` directory.
It shows how to use TfeAlert.
The program is `src/example/ex_alert.c`.

```c
#include <gtk/gtk.h>
#include "../tfealert.h"

static void
alert_response_cb (TfeAlert *alert, int response, gpointer user_data) {
  if (response == TFE_ALERT_RESPONSE_ACCEPT)
    g_print ("%s\n", tfe_alert_get_button_label (alert));
  else if (response == TFE_ALERT_RESPONSE_CANCEL)
    g_print ("Cancel\n");
  else
    g_print ("Unexpected error\n");
}

static void
app_activate (GApplication *application) {
  GtkWidget *alert;
  char *title, *message, *btn_label;

  title = "Example for TfeAlert"; message = "Click on Cancel or Accept button"; btn_label = "Accept";
  alert = tfe_alert_new_with_data (title, message, btn_label);
  g_signal_connect (TFE_ALERT (alert), "response", G_CALLBACK (alert_response_cb), NULL);
  gtk_window_set_application (GTK_WINDOW (alert), GTK_APPLICATION (application));
  gtk_window_present (GTK_WINDOW (alert));
}

static void
app_startup (GApplication *application) {
}

#define APPLICATION_ID "com.github.ToshioCP.example_tfe_alert"

int
main (int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  stat = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return stat;
}
```

The "activate" signal handler `app_activate` initializes the alert dialog.

- A TfeAlert instance is created.
- Its "response" signal is connected to the handler `alert_response_cb`.
- TfeAlert class is a sub class of GtkWindow so it can be a top level window that is connected to an application instance.
The function `gtk_window_set_application` does that.
- The dialog is shown.

A user clicks on either the cancel button or the accept button.
Then, the "response" signal is emitted and the dialog is destroyed.
The signal handler `alert_response_cb` checks the response and prints "Accept" or "Cancel".
If an error happens, it prints "Unexpected error".

You can compile it with meson and ninja.

```
$ cd src/tfe6/example
$ meson setup _build
$ ninja -C _build
$ _build/ex_alert
Accept #<= if you clicked on the accept button
```

Up: [README.md](../README.md),  Prev: [Section 19](sec19.md), Next: [Section 21](sec21.md)
