# Signals

## Signals

In GTK programming, each object is capsulated.
And it is not recommended to use global variables because they tend to make the program complicated.
So, we need something to communicate between objects.
There are two ways to do so.

- Functions.
For example, `tb = gtk_text_view_get_buffer (tv)`.
The function caller requests `tv`, which is a GtkTextView object, to give back `tb`, which is a GtkTextBuffer object connected to `tv`.
- Signals.
For example, `activate` signal on GApplication object.
When the application is activated, the signal is emitted.
Then the handler, which has been connected to the signal, is invoked.

The caller of the function or the handler connected to the signal is usually outside of the object.
One of the difference between these two is that the object is active or passive.
In functions the object passively responds to the caller.
In signals the object actively sends a signal to the handler.

GObject signals are registered, connected and emitted.

1. Signals are registered with the object type on which they can be emitted.
The registration is done usually when the class is initialized.
2. Signals are connected to handlers by `g_connect_signal` or its family functions.
3. When Signals are emitted, the connected handlers are invoked.

Step one and three are done in the object on which the signal belongs.
Step two is usually done outside the object.

## Signal registration

In TfeTextView, two signals are registered.

- "change-file" signal.
This signal is emitted when `tv->file` is changed.
- "open-response" signal.
`tfe_text_view_open` function is not able to return the status because it uses GtkFileChooserDialog.
This signal is emitted instead of the return value of the function.

A static variable or array is used to store the signal ID.
A static array is used to register two or more signals.

~~~C
enum {
  CHANGE_FILE,
  OPEN_RESPONSE,
  NUMBER_OF_SIGNALS
};

static guint tfe_text_view_signals[NUMBER_OF_SIGNALS];
~~~

Signal registration codes are written in the class initialization function.

@@@include
tfetextview/tfetextview.c tfe_text_view_class_init
@@@

- 6-15: Registers "change-file" signal.
`g_signal_newv` function is used.
This signal has no default handler (object method handler).
You usually don't need to set a default handler in final type object.
If you need it, put the closure of the handler in line 9.
- The return value of `g_signal_newv` is the signal id.
The type of signal id is guint, which is the same as unsigned int.
It is used when the signal is emitted.
- 16-26: Registers "open-response" signal.
This signal has a parameter.
- 25: Number of the parameter.
"open-response" signal has one parameter.
- 26: An array of types of parameters.
The array `param_types` is defined in line 16.
It has one element, which is `G_TYPE_INT`.
`G_TYPE_INT` is a type of integer.
Such fundamental types are described in [GObject API reference](https://developer.gnome.org/gobject/stable/gobject-Type-Information.html).

The handlers are declared as follows.

~~~C
void change_file_handler (TfeTextView *tv, gpointer user_data);
void open_response_handler (TfeTextView *tv, guint parameter, gpointer user_data);
~~~

- Because "change-file" signal doesn't have parameter, the handler's parameters are a TfeTextView object and user data.
- Because "open-response" signal has one parameter, the handler's parameters are a TfeTextView object, the signal's parameter and a user data.
- `tv` is the object instance on which the signal is emitted.
- `user_data` comes from the fourth argument of `g_signal_connect`.
- `parameter` comes from the fourth argument of `g_signal_emit`.

The values of the parameter is defined in `tfetextview.h` because they are public.

~~~C
/* "open-response" signal response */
enum
{
  TFE_OPEN_RESPONSE_SUCCESS,
  TFE_OPEN_RESPONSE_CANCEL,
  TFE_OPEN_RESPONSE_ERROR
};
~~~

- The parameter is set to `TFE_OPEN_RESPONSE_SUCCESS` when `tfe_text_view_open` successfully has opened a file and loaded it.
- The parameter is set to `TFE_OPEN_RESPONSE_CANCEL` when the user has canceled to open a file.
- The parameter is set to `TFE_OPEN_RESPONSE_ERROR` when error has occurred.
 
## Signal connection

A signal and a handler are connected by the function `g_signal_connect`.
There are some similar functions like `g_signal_connect_after`, `g_signal_connect_swapped` and so on.
However, `g_signal_connect` is the most common function.
The signals "change-file" is connected to a callback function outside of TfeTextView object.
In the same way, the signals "open-response" is connected to a callback function outside of TfeTextView object.
Those callback functions are defined by users.

In the program tfe, callback functions are defined in `tfenotebook.c`.
And their names are `file_changed` and `open_response`.
They will be explained later.

~~~C
g_signal_connect (GTK_TEXT_VIEW (tv), "change-file", G_CALLBACK (file_changed), nb);

g_signal_connect (TFE_TEXT_VIEW (tv), "open-response", G_CALLBACK (open_response), nb);
~~~

## Signal emission

Signals are emitted on the object.
The type of the object is the second argument of `g_signal_newv`.
The relationship between the signal and object (type) is made up when the signal is generated.

A function `g_signal_emit` is used to emit the signal.
The following lines are extracted from `tfetextview.c`.
Each line is quoted from a different line.

~~~C
g_signal_emit (tv, tfe_text_view_signals[CHANGE_FILE], 0);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_SUCCESS);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_CANCEL);
g_signal_emit (tv, tfe_text_view_signals[OPEN_RESPONSE], 0, TFE_OPEN_RESPONSE_ERROR);
~~~

- The first argument is the object on which the signal is emitted.
- The second argument is the signal id.
- The third argument is the detail of the signal.
"change-file" signal and "open-response" signal doesn't have details and the argument is zero when no details.
- "change-file" signal doesn't have parameter, so there's no fourth parameter.
- "open-response" signal has one parameter.
The fourth parameter is the parameter.

