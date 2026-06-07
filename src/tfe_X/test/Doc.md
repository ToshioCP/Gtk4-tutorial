# Document for the test programs

# Compilation

Change your current directory to this "test" directory.

```
$ meson setup _build
$ ninja -C _build
```

## Preference dialog test

```
$ GSETTINGS_SCHEMA_DIR=_build _build/test_pref
Change the font with the font button. Then the new font will be printed out.

Jamrul Italic Semi-Expanded 12
Laksaman Bold 12
Monospace 12
... ... ...
```

Source files:

- meson.build
- com.github.ToshioCP.tfe.gschema.xml (copied from ../com.github.ToshioCP.tfe.gschema.xml)
- main_pref.c
