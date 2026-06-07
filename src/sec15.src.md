# Completing the Main Program

The file `tfeapplication.c` is the main program of Tfe.
Its main roles are:

- Application initialization, including command-line support (opening files from the terminal).
- Building the user interface using a UI resource file.
- Managing multiple text views with tabs (GtkNotebook).
- Providing signal handlers for buttons and file dialog operations.

## The Function main

The program `tfe` is executed from the command line.

```
$ tfe file1 file2 ...
```

The function `main` is called first.

@@@include
tfe5/tfeapplication.c main
@@@

- 6: Creates a GtkApplication object.
`APPLICATION_ID` is defined as "com.github.ToshioCP.tfe5" before the `main` function.
The second argument `G_APPLICATION_HANDLES_OPEN` is a flag and that means that the application accepts commandline arguments but only pathnames.
- 7-8: Connects the "activate" and "open" signals to their handlers.
- 9: Runs the application.
- 10-11: Releases the application and returns the status.

## Startup Signal Handler

A startup signal is emitted just after the GtkApplication instance is registered.
The purpose of this handler is to initialize the application.
This does not include window initialization, which is handled by the "activate" or "open" signal handlers.

This application does not need a "startup" handler for now, but it will be implemented in a later version.

## Activate and open signal handler

The "activate" and "open" signal handlers are named `app_activate` and `app_open`, respectively.
When the application is launched without any command-line arguments, the "activate" signal is emitted.
Conversely, if arguments are provided, the "open" signal is emitted.

@@@include
tfe5/tfeapplication.c tfe_get_notebook app_activate app_open
@@@

- 1-5: The private function `tfe_get_notebook` returns the notebook instance.
This function is a private utility used by `app_activate` and `app_open`.
This function relies on the assumption that the notebook is the last child of the box.
Therefore, it will no longer work if another widget is added to the box in the future version.
There are several ways to solve this issue, which will be explained in a later section.
- 7-14: `app_activate`.
- 9: The main window instance is obtained via the `get_main_window` function, which will be discussed later.
- 10: The notebook instance `nb` is obtained by calling the function `tfe_get_notebook`.
- 12: A new empty page is created, and finally, the main window is displayed.
The `notebook_page_new_with_file` function will be explained later.
- 16-27: The `app_open` function.
- 22-23: In this for-loop, notebook pages are created with files using the `notebook_page_new_with_file` function.
`files[i]` is the i-th commandline argument.
- 24-25: If no page has been created, maybe because of read error, then it creates an empty page.
- 26: Shows the window.


## The Notebook page building function

The function `notebook_page_new_with_file` builds a new notebook page from a GFile.
If the GFile is NULL, it creates a new empty page.
The function is called by the "activate" and "open" handlers when the application starts.

@@@include
tfe5/tfeapplication.c notebook_page_new_with_file
@@@

- 14-19: Calls `tfe_text_view_new_with_file` to create a new TfeTextView instance.
If it returns NULL, shows the error message through the alert dialog using `tfe_error_alert`.
-20: Gets the GtkLabel instance with the filename.
If no file is assigned to TfeTextView, `tfe_label_from_file` returns NULL.
- 21-22: assigns `tv` to the newly created scrolled window `scr`.
Then, appends the scrolled window and the label to the notebook as a child and tab respectively.
- 23-25: Sets the page property "tab-expand" to TRUE.
- 26: Sets the current page to the new page.

## Primary and Secondary Instances

Only one GApplication instance can be run at a time in a session.
The session is a somewhat complex concept and also platform-dependent, but roughly speaking, it corresponds to a graphical desktop login.
When you use your PC, you probably log in first, and then your desktop appears until you log out.
This is the session.

However, Linux is a multi-process OS and you can run two or more instances of the same application.
Isn't it a contradiction?

When the first instance is launched, it registers itself with its application ID (for example, "com.github.ToshioCP.tfe5").
Just after registration, the "startup" signal is emitted, followed by the "activate" or "open" signal, and finally, the instance's main loop starts.

If another instance with the same application ID is launched, it also tries to register itself.
Because this is the second instance, the registration of the ID has already been done, so it fails.
Because of the failure, the "startup" signal isn't emitted.
After that, the "activate" or "open" signal is emitted in the primary instance, not in the second instance.
The primary instance receives the signal and its handler is invoked.
On the other hand, the second instance doesn't receive the signal and immediately quits.

### Building a Single Window

The `activate` and `open` signal handlers are responsible for creating and displaying the window.
However, because these handlers can be called multiple times, creating a window unconditionally would result in multiple main windows.
Our TFE application is designed to have only one main window, using notebook pages to manage multiple files.
Therefore, the handlers must follow this logic:

- If the application already has a window, use it.
- If it does not, create a new one.

Since the window is built from a UI file, creating it also involves instantiating child widgets and connecting button signals to their handlers.
The `get_main_window` helper function takes care of all these steps.

@@@include
tfe5/tfeapplication.c get_main_window
@@@

- 4: An application can generally have multiple main windows.
These are stored in a list, which can be retrieved using the `gtk_application_get_windows` function.
The return type is a `GList *` pointer.
A `GList` structure contains `data` (pointing to the window in this case), `next`, and `prev` pointers, forming a doubly linked list.
If the list is empty, the function returns `NULL`.
For more details, refer to the [GLib Documentation - Doubly Linked Lists](https://docs.gtk.org/glib/data-structures.html#doubly-linked-lists) and [GList](https://docs.gtk.org/glib/struct.List.html).
- 14-15: If `windows` is not `NULL`, the application already has a window.
The function simply returns this existing window (`windows->data`).
- 17-32: Otherwise, it uses `GtkBuilder` to generate all the widgets, registers the new window with the application, connects the "clicked" signals to their handlers, and finally returns the window.

### Running and Observing Primary and Secondary Instances

For compiling, see "Build and Execute the Program" subsection below.

Try running two instances in a row:

```
$ ./_build/tfe & ./_build/tfe tfeapplication.c
$
```

First, the primary instance opens a window.
Then, after the second instance is run, a new notebook page with the contents of `tfeapplication.c` appears in the primary instance's window.
This is because the "open" signal is emitted in the primary instance.
The second instance immediately quits, so the shell prompt soon appears.

## New and CLose Button Signal Handlers

Open and save button handlers were explained in the previous section.
Now we will explore the remaining handlers: the new and close handlers

@@@include
tfe5/tfeapplication.c new_cb close_cb
@@@

- 1-4: Handles the "clicked" signal for the "New" button.
The `new_cb` function simply calls `notebook_page_new_with_file` to create a new, empty page.
- 6-12: Handles the "clicked" signal for the "Close" button.
If two or more pages exist, it removes the current page.
If it is the last remaining page, the program destroys the window, which consequently quits the application.

## meson.build

@@@include
tfe5/meson.build
@@@

- 1-10: In this file, just the source file names are modified from the prior version.
- 12-17: These lines are for a test program for `tfetextview.c`.
Test is not covered by this tutorial.
If you want to know Glib test framework, See [the GLib documentation](https://docs.gtk.org/glib/testing.html). 

## Source Files

You can download the files from the [repository](https://github.com/ToshioCP/Gtk4-tutorial).

The source files are under the [/src/tfe5](tfe5) directory.

## Build and Execute the Program

You can build and execute the program using Meson and Ninja.
To install the applications, type:

```
$ sudo apt install meson ninja-build
```

Change your current directory to `Gtk4-tutorial/src/tfe5` and type the following to build the program.

```
$ meson setup _build
$ ninja -C _build
```

To execute the program, type:

```
$ _build/tfe
```

You can add pathnames as arguments.
A window will appear, containing four buttons and an editing area.

If you want to run the test program for `tfetextview.c`, type:

```
$ meson test -C _build
ninja: Entering directory `/home/username/Gtk4-tutorial/src/tfe5/_build'
ninja: no work to do.
1/1 tfetextview_test        OK              0.53s

Ok:                 1   
Expected Fail:      0   
Fail:               0   
Unexpected Pass:    0   
Skipped:            0   
Timeout:            0   

Full log written to /home/username/Gtk4-tutorial/src/tfe5/_build/meson-logs/testlog.txt
```