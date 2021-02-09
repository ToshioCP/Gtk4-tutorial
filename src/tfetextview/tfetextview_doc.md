# TfeTextView API reference

TfeTextView -- Child widget of GtkTextView. It holds GFile the contents of GtkTextBuffer correponds to.

## Functions
- GFile *[tfe_text_view_get_file ()](#tfe_text_view_get_file)
- void [tfe_text_view_open ()](#tfe_text_view_open)
- void [tfe_text_view_save ()](#tfe_text_view_save)
- void [tfe_text_view_saveas ()](#tfe_text_view_saveas)
- GtkWidget *[tfe_text_view_new_with_file ()](#tfe_text_view_new_with_file)
- GtkWidget *[tfe_text_view_new ()](#tfe_text_view_new)

## Signals

- void [change-file](#change-file)
- void [open-response](#open-response)

## Types and Values

- [TfeTextView](#tfetextview-1)
- [TfeTextViewClass](#tfetextviewclass)
- [TfeTextViewOpenResponseType](#enum-tfetextviewopenresponsetype)

## Object Hierarchy

~~~
GObject
+--GInitiallyUnowned
   +--GtkWidget
      +--GtkTextView
         +--TfeTextView
~~~

## Includes

~~~
#include <gtk/gtk.h>
~~~

## Description

TfeTextView holds GFile the contents of GtkTextBuffer corresponds to.
File manipulation functions have been added to this object.

## Functions

### tfe_text_view_get_file()

~~~
GFile *
tfe_text_view_get_file (TfeTextView *tv);
~~~

Returns the copy of the GFile in the TfeTextView.

Parameters

- tv: a TfeTextView

### tfe_text_view_open()

~~~
void
tfe_text_view_open (TfeTextView *tv, GtkWidget *win);
~~~

Just shows a GtkFileChooserDialog so that a user can choose a file to read.
This function doesn't do any I/O operations.
They are done by the signal handler connected to the `response` signal emitted by GtkFileChooserDialog.
Therefore the caller can't know the I/O status directly from the function.
Instead, the status is informed by `open-response` signal.
The caller needs to set a handler to this signal in advance.

parameters

- tv: a TfeTextView
- win: the top level window of the TfeTextView

### tfe_text_view_save()

~~~
void
tfe_text_view_save (TfeTextView *tv);
~~~

Saves the content of a TfeTextView to a file.
If `tv` holds a GFile, it is used.
Otherwise, this function calls GtkFileChosserDialog so that a user can choose a file to save.

Parameters

- tv: a TfeTextView

### tfe_text_view_saveas()

~~~
void
tfe_text_view_saveas (TfeTextView *tv);
~~~

Saves the content of a TfeTextView to a file.
This function calls GtkFileChosserDialog so that a user can choose a file to save.

Parameters

- tv: a TfeTextView

### tfe_text_view_new_with_file()

~~~
GtkWidget *
tfe_text_view_new_with_file (GFile *file);
~~~

Creates a new TfeTextView and read the contents of the `file` and set it to the GtkTextBuffer corresponds to the newly created TfeTextView.
Then returns the TfeTextView as GtkWidget.
If I/O error happens, it returns `NULL`.

Parameters

- file: a GFile

Returns

- a new TfeTextView.

### tfe_text_view_new()

~~~
GtkWidget *
tfe_text_view_new (void);
~~~

Creates a new TfeTextView and returns the TfeTextView as GtkWidget.

Returns

- a new TfeTextView.

## Types and Values

### TfeTextView

~~~
typedef struct _TfeTextView TfeTextView
struct _TfeTextView
{
  GtkTextView parent;
  GFile *file;
};
~~~

The members of this structure are not allowed to be accessed by any outer objects.
If you want to obtain GFile, use `tfe_text_view_get_file`.

### TfeTextViewClass

~~~
typedef struct {
  GtkTextViewClass parent_class;
} TfeTextViewClass;
~~~

No member is added because TfeTextView is a final type object.

### enum TfeTextViewOpenResponseType

Predefined values for the response id given by `open-response` signal.

Members:

- TFE_OPEN_RESPONSE_SUCCESS: The file is successfully opened.
- TFE_OPEN_RESPONSE_CANCEL: Reading file is canceled by the user.
- TFE_OPEN_RESPONSE_ERROR: An error happened during the opening or reading process.

## Signals

### change-file

~~~
void
user_function (TfeTextView *tv,
               gpointer user_data)
~~~

Emitted when the GFile in the TfeTextView object is changed.
The signal is emitted when:

- a new file is read
- the contents is saved as a different pathname
- an error occured during I/O operation, and GFile removed as a result.

### open-response

~~~
void
user_function (TfeTextView *tv,
               TfeTextViewOpenResponseType response-id,
               gpointer user_data)
~~~

Emitted after the user called `tfe_text_view_open`.
This signal informs the status of file opening and reading.
