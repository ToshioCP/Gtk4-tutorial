# String and memory management

GtkTextView and GtkTextBuffer have functions that use string parameters or return a string.
The knowledge of strings and memory management is useful to understand how to use these functions.

## String and memory

A String is an array of characters that is terminated with '\0'.
Strings are not a C type such as char, int, float or double,
but exist as a pointer to a character array. They behaves like a string type
which you may be familiar from other languages.
So, this pointer is often called 'a string'.

In the following, `a` and `b` defined as character arrays, and are strings.

~~~C
char a[10], *b;

a[0] = 'H';
a[1] = 'e';
a[2] = 'l';
a[3] = 'l';
a[4] = 'o';
a[5] = '\0';

b = a;
/* *b is 'H' */
/* *(++b) is 'e' */
~~~

The array `a` has `char` elements and the size of ten.
The first six elements are 'H', 'e', 'l', 'l', 'o' and '\0'.
This array represents the string "Hello".
The first five elements are character codes that correspond to the characters.
The sixth element is '\0', which is the same as zero,
and indicates that the string ends there.
The size of the array is 10, so 4 bytes aren't used, but that's OK,
they are just ignored.

The variable 'b' is a pointer to a character.
Because `b` is assigned to be `a`, `a` and `b` point the same character ('H').
The variable `a` is defined as an array and it can't be changed.
It always point the top address of the array.
On the other hand, 'b' is a pointer, which is mutable, so `b` can be change.
It is then possible to write statements like `++b`, which means take the value in b (n address),
increase it by one, and store that back in `b`.

If a pointer is NULL, it points to nothing.
So, the pointer is not a string.
A NULL string on the other hand will be a pointer which points to a location
that contains `\0`, which is a string of length 0 (or "").
Programs that use strings will include bugs if you aren't careful when using NULL pointers.

Another annoying problem is the memory that a string is allocated.
There are four cases:

- The string is read only;
- The string is in static memory area;
- The string is in stack; and
- The string is in memory allocated from the heap area.

## Read only string

A string literal in a C program is surrounded by double quotes and written as the following:

~~~C
char *s;
s = "Hello"
~~~

"Hello" is a string literal, and is stored in program memory.
A string literal is read only.
In the program above, `s` points the string literal.

So, the following program is illegal.

~~~C
*(s+1) = 'a';
~~~

The result is undefined.
Probably a bad thing will happen, for example, a segmentation fault.

NOTE: The memory of the literal string is allocated when the program is
compiled. It is possible to view all the literal strings defined in your program
by using the `string` command.

## Strings defined as arrays

If a string is defined as an array, it's in either stored in the static memory area or stack.
This depends on the class of the array.
If the array's class is `static`, then it's placed in static memory area.
This allocation and memory address is fixed for the life of the program.
This area can be changed and is writable.

If the array's class is `auto`, then it's placed in stack.
If the array is defined inside a function, its default class is `auto`.
The stack area will disappear when the function exits and returns to the caller.
Arrays defined on the stack are writable.

~~~C

static char a[] = {'H', 'e', 'l', 'l', 'o', '\0'};

void
print_strings (void) {
  char b[] = "Hello";

  a[1] = 'a'; /* Because the array is static, it's writable. */
  b[1] = 'a'; /* Because the array is auto, it's writable. */

  printf ("%s\n", a); /* Hallo */
  printf ("%s\n", b); /* Hallo */
}
~~~

The array `a` is defined externally to a function and is global in its scope.
Such variables are placed in static memory area even if the `static` class is left out.
The compiler calculates the number of the elements in the right hand side (six),
and then creates code that allocates six bytes in the static memory area and copies the data to this memory.

The array `b` is defined inside the function
so its class is `auto`.
The compiler calculates the number of the elements in the string literal.
It has six elements as the zero termination character is also included.
The compiler creates code which allocates six bytes memory in the stack and copies the data to the memory.

Both `a` and `b` are writable.

The memory is managed by the executable program.
You don't need your program to allocate or free the memory for `a` and `b`.
The array `a` is created then the program is first run and remains for the life of the program.
The array `b` is created on the stack then the function is called, disappears when the function returns.

## Strings in the heap area

You can also get, use and release memory from the heap area.
The standard C library provides `malloc` to get memory and `free` to put back memory.
GLib provides the functions `g_new` and `g_free` to do the same thing, with support for
some additional Glib functionality.

~~~C
g_new (struct_type, n_struct)
~~~

`g_new` is a macro to allocate memory for an array.

- `struct_type` is the type of the element of the array.
- `n_struct` is the size of the array.
- The return value is a pointer to the array.
Its type is a pointer to `struct_type`.

For example,

~~~C
char *s;
s = g_new (char, 10);
/* s points an array of char. The size of the array is 10. */

struct tuple (int x, int y) *t;
t = g_new (struct tuple, 5);
/* t points an array of struct tuple. */
/* The size of the array is 5. */
~~~

`g_free` frees memory.

~~~C
void
g_free (gpointer mem);
~~~

If `mem` is NULL, `g_free` does nothing.
`gpointer` is a type of general pointer.
It is the same as `void *`.
This pointer can be casted to any pointer type.
Conversely, any pointer type can be casted to `gpointer`.

~~~C
g_free (s);
/* Frees the memory allocated to s. */

g_free (t);
/* Frees the memory allocated to t. */
~~~

If the argument doesn't point allocated memory it will cause an error, specifically, a segmentation fault.

Some Glib functions allocate memory.
For example, `g_strdup` allocates memory and copies a string given as an argument.

~~~C
char *s;
s = g_strdup ("Hello");
g_free (s);
~~~

The string literal "Hello" has 6 bytes because the string has '\0' at the end it.
`g_strdup` gets 6 bytes from the heap area and copies the string to the memory.
`s` is assigned the top address of the memory.
`g_free` returns the memory to the heap area.

`g_strdup` is described in [GLib API Reference](https://docs.gtk.org/glib/func.strdup.html).
The following is extracted from the reference.

> The returned string should be freed with `g_free()` when no longer needed.

The function reference will describe if the returned value needs to be freed.
If you forget to free the allocated memory it will remain allocated.  Repeated use will cause
more memory to be allocated to the program, which will grow over time. This is called a memory leak,
and the only way to address this bug is to close the program (and restart it),
which will automatically release all of the programs memory back to the system.

Some GLib functions return a string which mustn't be freed by the caller.

~~~C
const char *
g_quark_to_string (GQuark quark);
~~~

This function returns `const char*` type.
The qualifier `const` means that the returned value is immutable.
The characters pointed by the returned value aren't be allowed to be changed or freed.

If a variable is qualified with `const`, the variable can't be assigned except during initialization.

~~~C
const int x = 10; /* initialization is OK. */

x = 20; /* This is illegal because x is qualified with const */
~~~
