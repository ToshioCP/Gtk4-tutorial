# TfeTextView API reference

## Description

TfeTextView is a child object of GtkTextView.
If its contents comes from a file, it holds the pointer to the GFile.
Otherwise, the pointer is NULL.

## Hierarchy

```
GObject -- GInitiallyUnowned -- GtkWidget -- GtkTextView -- TfeTextView
```

## Ancestors

- GtkTextView
- GtkWidget
- GInitiallyUnowned
- GObject

## Constructors

- [tfe_text_view_new ()](#tfe_text_view_new)
- [tfe_text_view_new_with_file ()](#tfe_text_view_new_with_file)

## Instance methods

- [tfe_text_view_get_file ()](#tfe_text_view_get_file)
- [tfe_text_view_open ()](#tfe_text_view_open)
- [tfe_text_view_save ()](#tfe_text_view_save)
- [tfe_text_view_saveas ()](#tfe_text_view_saveas)

## Signals

- [Tfe.TextView::change-file](#change-file)
- [Tfe.TextView::open-response](#open-response)

## API for constructors, instance methods and signals

**constructors**

### tfe_text_view_new()

```
GtkWidget *
tfe_text_view_new (void);
```

Creates a new TfeTextView instance and returns the pointer to it as GtkWidget.
If an error happens, it returns `NULL`.

Return value

- a new TfeTextView.

### tfe_text_view_new_with_file()

```
GtkWidget *
tfe_text_view_new_with_file (GFile *file);
```

Creates a new TfeTextView, reads the contents of the `file` and set it to the GtkTextBuffer corresponds to the newly created TfeTextView.
Then returns the pointer to the TfeTextView as GtkWidget.
If an error happens, it returns `NULL`.

Parameters

- file: a GFile

Return value

- a new TfeTextView.

**Instance methods**

### tfe_text_view_get_file()

```
GFile *
tfe_text_view_get_file (TfeTextView *tv);
```

Returns the copy of the GFile in the TfeTextView.

Parameters

- tv: a TfeTextView

Return value

- the pointer to the GFile

### tfe_text_view_open()

```
void
tfe_text_view_open (TfeTextView *tv, GtkWidget *win);
```

Shows a file chooser dialog so that a user can choose a file to read.
Then, read the file and set the buffer with the contents.
This function doesn't return the I/O status.
Instead, the status is informed by `open-response` signal.
The caller needs to set a handler to this signal in advance.

parameters

- tv: a TfeTextView
- win: the top level window

### tfe_text_view_save()

```
void
tfe_text_view_save (TfeTextView *tv);
```

Saves the contents of the buffer to the file.
If `tv` holds a GFile, it is used.
Otherwise, this function shows a file chooser dialog so that the user can choose a file to save.

Parameters

- tv: a TfeTextView

### tfe_text_view_saveas()

```
void
tfe_text_view_saveas (TfeTextView *tv);
```

Saves the contents of the buffer to a file.
This function shows file chooser dialog so that a user can choose a file to save.

Parameters

- tv: a TfeTextView

**Signals**

### change-file

```
void
user_function (TfeTextView *tv,
               gpointer user_data)
```

Emitted when the GFile in the TfeTextView object is changed.
The signal is emitted when:

- a new file is opened and read
- a user chooses a file with the file chooser dialog and save the contents.

### open-response

```
void
user_function (TfeTextView *tv,
               TfeTextViewOpenResponseType response-id,
               gpointer user_data)
```

Emitted after the user calls `tfe_text_view_open`.
This signal informs the status of file I/O operation.

**Enumerations**

### TfeTextViewOpenResponseType

Predefined values for the response id given by `open-response` signal.

Members:

- TFE_OPEN_RESPONSE_SUCCESS: The file is successfully opened.
- TFE_OPEN_RESPONSE_CANCEL: Reading file is canceled by the user.
- TFE_OPEN_RESPONSE_ERROR: An error happened during the opening or reading process.
