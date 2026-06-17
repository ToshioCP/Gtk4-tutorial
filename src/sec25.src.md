# GtkFontDialogButton and CSS Updates

## The Preference Dialog

When you click "Preferences" menu on the new TFE application, the following preference dialog appears.

@@@if pdf
![Preference dialog](/images/pref_dialog.png){width=4cm height=1.6cm}
@@@else
![Preference dialog](/images/pref_dialog.png)
@@@end

A preference dialog allows users to customize settings, such as fonts.
Nowadays, it is often called a "Settings" dialog.

This dialog contains only one bUtton: a `GtkFontDialogButton`.
You can add more widgets to this dialog, but this simple layout is enough for our first version.

Clicking this button opens `GtkFontDialog` to select a font.

@@@if pdf
![Font dialog](/images/fontdialog.png){width=6.27cm height=7.38cm}
@@@else
![Font dialog](/images/fontdialog.png)
@@@end

When the user selects a font and clicks the "Select" button, the application's font updates.

Note that `GtkFontDialogButton` and `GtkFontDialog` are new widgets introduced in GTK 4.10.
They replace the older `GtkFontButton` and `GtkFontChooserDialog`, which were deprecated in version 4.10.

### The Composite Widget

This preference dialog contains a `GtkBox`, a `GtkLabel`, and a `GtkFontDialogButton`, and is defined as a composite widget.
Here is the template UI file for `TfePref`:

@@@include
tfe11/tfepref.ui
@@@

- The `<template>` tag specifies that this is a composite widget.
- The `class` attribute is set to "TfePref" and the `parent` attribute to GtkWindow.
This makes `TfePref` a child class of GtkWindow.
- It sets three properties: `title`, `resizable`, and `modal`.
When `modal` is set to `TRUE`, the user cannot interact with the parent window while this dialog is open.
- Inside, there is a horizontal `GtkBox` containing a `GtkLabel` and a `GtkFontDialogButton`.

### The Header File

The `tfepref.h` file defines the types and declares the public function.

@@@include
tfe11/tfepref.h
@@@

- 5: Defines the `TFE_TYPE_PREF` macro, which is replaced by `tfe_pref_get_type()`.
- 6: Uses the `G_DECLARE_FINAL_TYPE` macro to automatically generate standard boilerplate code.
- 8-9: Declares the public function `tfe_pref_new()`, which creates a new `TfePref` instance.
It takes a GtkApplication argument, which will be assigned to "application" property.

### The C Source File for the Composite Widget

The following code is the `tfepref.c` file.

@@@include
tfe11/tfepref.c
@@@

- 5-9: The `_TfePref` structure includes a `font_dialog_btn` member.
This holds a pointer to the `GtkFontDialogButton` instance defined in the XML file (`tfepref.ui`).
This member name must exactly match the `id` attribute in the XML.
- 11: The `G_DEFINE_FINAL_TYPE` macro expands into standard boilerplate code.
- 13-18: `font_dialog_btn_notify_cb()` is the notify signal handler for the font dialog button's `font-desc` property.
This signal is emitted whenever the property changes.
The handler retrieves the `font-desc` property (which is a `PangoFontDescription`) and passes it to our `tfe_style_manager_set_font()` style manager function.
Because `tfe_style_manager_set_font()` not only converts this data into a CSS string but also applies it to the `GdkDisplay`, simply calling this function instantly updates the screen's style.
- 20-26: The dispose handler uses `gtk_widget_dispose_template()` to destroy the instance built from the template.
- 28-32: The instance initialization function uses `gtk_widget_init_template()` to build the instance from the template.
It also connects the notify signal for the `font-desc` property to our callback handler.
- 34-39: `tfe_pref_class_init()` loads the template and binds the `font_dialog_btn` instance variable to the UI file.
- 41-44: The constructor `tfe_pref_new()` creates the new instance using `g_object_new()`.
The argument `application` is assigned to the "application" property.

### Handling it in the Application

The preference dialog opens when the user clicks "Preferences" in the application menu.
Therefore, we need to add code to generate and display the dialog inside the application's action handler.

@@@include
tfe11/tfeapplication.c pref_activated
@@@

The logic here is very simple:
it creates the preference dialog, sets the main window as its transient parent window, and shows the dialog.
The user closes the dialog by clicking the window's close (X) button.
Once closed, the dialog instance is destroyed.

## Compile and Run

The source code is located in the [src/tfe11](tfe11/) directory.

Before compiling, you need to make a few changes:

- Change the Application ID in `main.c` to "com.github.ToshioCP.tfe11".
- Change the prefix in `tfe.gresource.xml` to "/com/github/ToshioCP/tfe11".
Be sure to update the UI file loading prefix in `tfewindow.c` to match.
- Add `tfepref.c` to the source files list in `meson.build`.

Move your current directory to `src/tfe11` and type the following commands:

```
$ meson setup _build
$ ninja -C _build
$ _build/tfe
```

Select "Preferences" from the menu to open the dialog.
Click the font button to open the font chooser, pick a font and size, and the chooser will close, returning you to the preference dialog.
Now, click the close (X) button at the top right to close the preference dialog.
You should see that the text view in the main window is now using your selected font.

This setting remains active while the program is running, but once you exit the application, all settings are lost.
The next time you launch the app, it will revert to the default font.

To save these settings so they persist across restarts, the best practice in GTK is to use GSettings.
In the next section, we will wrap up the TFE project by covering how to implement GSettings.
