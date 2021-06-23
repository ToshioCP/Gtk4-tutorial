Up: [Readme.md](../Readme.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)

# String and memory management

GtkTextView and GtkTextBuffer have functions that have string parameters or return a string.
The knowledge of strings and memory management is very important for us to understand the functions above.

## String and memory

String is an array of characters that is terminated with '\0'.
String is not a C type such as char, int, float or double.
But the pointer to a character array behaves like a string type of other languages.
So, the pointer is often called string.

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

The array `a` has `char` elements and the size is ten.
The first six elements are 'H', 'e', 'l', 'l', 'o' and '\0'.
This array represents a string "Hello".
The first five elements are character codes that correspond to the characters.
The sixth element is '\0' that is the same as zero.
And it indicates that the string ends there.
The size of the array is 10, so 4 bytes aren't used, but it's OK.
They are just ignored.

The variable 'b' is a pointer to a character.
Because `a` is assigned to `b`, `a` and `b` point the same character ('H').
The variable `a` is defined as an array and it can't be changed.
It always point the top address of the array.
On the other hand, pointers are mutable, so `b` can change itself.
It is possible to write `++b` (`b` is increased by one).

If a pointer is NULL, it points nothing.
So, the pointer is not a string.
Programs with string will include bugs if you aren't careful about NULL pointer.

Another annoying problem is memory that a string is allocated.
There are four cases.

- The string is read only.
- The string is in static memory area.
- The string is in stack.
- The string is in memory allocated from the heap area.

## Read only string

A string literal in C program is surrounded by double quotes.

~~~C
char *s;
s = "Hello"
~~~

"Hello" is a string literal.
A string literal is read only.
In the program above, `s` points the string literal.
So, the following program is illegal.

~~~C
*(s+1) = 'a';
~~~

The result is undefined.
Probably a bad thing will happen, for example, a segmentation fault.

## Strings defined as arrays

If a string is defined as an array, it's in static memory area or stack.
It depends on the class of the array.
If the array's class is `static`, then it's placed in static memory area.
It keeps its value and remains for the life of the program.
This area is writable.

If the array's class is `auto`, then it's placed in stack.
If the array is defined in a function, its default class is `auto`.
The stack area will disappear when the function returns to the caller.
stack is writable.

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

The array `a` is defined externally to functions.
Such variables are placed in static memory area even if the `static` class is left out.
First, the compiler calculates the number of the elements in the right hand side.
It is six.
The compiler allocates six bytes memory in the static memory area and copies the data to the memory.

The array `b` is defined inside the function.
So, its class is `auto`.
The compiler calculates the number of the elements in the string literal.
It is six because the string is zero terminated.
The compiler allocates six bytes memory in the stack and copies the data to the memory.

Both `a` and `b` are writable.

The memory is managed by the executable program.
You don't need to program to allocate or free the memory for `a` and `b`.
The array `a` remains for the life of the program.
The array `b` disappears when the function returns to the caller.

## Strings in the heap area

You can get memory from the heap area and put back the memory to the heap area.
The standard C library provides `malloc` to get memory and `free` to put back memory.
Similarly, GLib provides `g_new` and `g_free`.

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
Conversely, any pointer type can be casted to gpointer. 

~~~C
g_free (s);
/* Frees the memory allocated to s. */

g_free (t);
/* Frees the memory allocated to t. */
~~~

If the argument doesn't point allocated memory, it will cause an error, for example, segmentation fault.

Some Glib functions allocate memory.
For example, `g_strdup` allocates memory and copy a string given as an argument.

~~~C
char *s;
s = g_strdup ("Hello");
g_free (s);
~~~

The string literal "Hello" has 6 bytes because the string has '\0' at the end it.
`g_strdup` gets 6 bytes from the heap area and copies the string to the memory.
`s` is assigned the top address of the memory.
`g_free` returns the memory to the heap area.

`g_strdup` is described in [GLib reference manual](https://developer.gnome.org/glib/stable/glib-String-Utility-Functions.html#g-strdup).
The following is extracted from the reference.

> The returned string should be freed with `g_free()` when no longer needed.

The reference usually describes if the returned value needs to be freed.
If you forget to free the allocated memory, it causes memory leak.

Some GLib functions return a string which mustn't be freed by the caller.

~~~C
const char *
g_quark_to_string (GQuark quark);
~~~

This function returns `const char*` type.
The qualifier `const` means immutable.
Therefore, the characters pointed by the returned value aren't be allowed to change or free.

If a variable is qualified with `const`, the variable can't be assigned except initialization.

~~~C
const int x = 10; /* initialization is OK. */

x = 20; /* This is illegal because x is qualified with const */
~~~


Up: [Readme.md](../Readme.md),  Prev: [Section 5](sec5.md), Next: [Section 7](sec7.md)
