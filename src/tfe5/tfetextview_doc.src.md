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
- [tfe_text_view_set_file ()](#tfe_text_view_set_file)
- [tfe_text_view_read ()](#tfe_text_view_read)
- [tfe_text_view_write ()](#tfe_text_view_write)

## Signals

## API for constructors, instance methods and signals

**constructors**

### tfe_text_view_new()

```
GtkWidget *
tfe_text_view_new (void);
```

Creates a new TfeTextView instance and returns the pointer to it as GtkWidget.

**Return value**

- a new TfeTextView.
The caller of the function takes ownership of the data, and is responsible for freeing it.

### tfe_text_view_new_with_file()

```
GtkWidget *
tfe_text_view_new_with_file (GFile *file, GError **err);
```

Creates a new TfeTextView, reads the contents of the `file` and set it into the GtkTextBuffer associated with the newly created TfeTextView.
Then returns the pointer to the TfeTextView as GtkWidget.
If an error happens, it returns `NULL` and set `err`.

**Parameters**

- file: type: `*GFile`
- err: type: `**GError`.
It can be NULL and if it is NULL the function ignores this argument.
If it is not NULL, `*err` must be NULL.
If there are no errors, the pointer remains unchanged (it remains NULL).
In case of error, the function will set `*err` to a newly allocated GError that contains an error domain, a code, and a message.
The caller will take ownership of the GError, and be responsible for freeing it.

**Return value**

- a new TfeTextView.
The caller of the function takes ownership of the data, and is responsible for freeing it.
If an error, for example a read file error, happens, NULL will be returned.

**Instance methods**

### tfe_text_view_get_file()

```
GFile *
tfe_text_view_get_file (TfeTextView *tv);
```

Returns the copy of the GFile associated with the TfeTextView.
If no file is set, i.e. GFile is NULL, NULL will be returned.

**Parameters**

- tv: type: `*TfeTextView`

**Return value**

- the pointer to the GFile or NULL.
The caller takes the ownership and is responsible for freeing it.

### tfe_text_view_set_file()

```
void
tfe_text_view_set_file (TfeTextView *tv, GFile *file);
```

Set the file associated with the TfeTextView instance.

**Parameters**

- tv: type: `*TfeTextView`
- file: type: `*GFile`
The argument can be NULL.
The function releases the GFile instance in the object if it exists.
Then, it sets file in the object to the argument.
If the argument is NULL, the function just releases the old file instance.

### tfe_text_view_read()

```
gboolean
tfe_text_view_read (TfeTextView *tv, GError **err);
```

Read the content of the file referenced by the GFile in the object.
If the GFile is NULL or an error happens during the read process, it returns FALSE and `*err` is set to a GError.

**parameters**

- tv: type: `*TfeTextView`
- err: type: `**GError`
It can be NULL and if it is NULL the function ignores this argument.
If it is not NULL, `*err` must be NULL.
If there are no errors, the pointer remains unchanged (it remains NULL).
In case of error, the function will set `*err` to a newly allocated GError that contains an error domain, a code, and a message.
The caller will take ownership of the GError, and be responsible for freeing it.

**return value**

- Gboolean. If it successfully reads the file, it returns TRUE, otherwise FALSE

### tfe_text_view_write()

```
gboolean
tfe_text_view_write (TfeTextView *tv, GError **err);
```

Write the content of the object buffer to a file referenced by the GFile in the object.
If the GFile is NULL or an error happens during the write process, it returns FALSE and `*err` is set to a GError.

**Parameters**

- tv: type: `*TfeTextView`
- err: type: `**GError`
It can be NULL and if it is NULL the function ignores this argument.
If it is not NULL, `*err` must be NULL.
If there are no errors, the pointer remains unchanged (it remains NULL).
In case of error, the function will set `*err` to a newly allocated GError that contains an error domain, a code, and a message.
The caller will take ownership of the GError, and be responsible for freeing it.

**return value**

- Gboolean. If it successfully writes the file, it returns TRUE, otherwise FALSE

### Signals

No signal except the ones from the ancestors
