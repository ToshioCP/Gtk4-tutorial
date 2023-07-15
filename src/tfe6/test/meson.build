project('tfe_test', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','../tfe.gresource.xml', source_dir: '..')
gnome.compile_schemas(build_by_default: true, depend_files: 'com.github.ToshioCP.tfe.gschema.xml')

executable('test_pref', ['test_pref.c', '../tfepref.c'], resources, dependencies: gtkdep, export_dynamic: true, install: false)

executable('test_pfd2css', ['test_pfd2css.c', '../pfd2css.c'], dependencies: gtkdep, export_dynamic: true, install: false)

executable('test_alert', ['test_alert.c', '../tfealert.c'], resources, dependencies: gtkdep, export_dynamic: true, install: false)

executable('test_window_ui', 'test_window_ui.c', resources, dependencies: gtkdep, export_dynamic: true, install: false)

sourcefiles_test_window=files('test_window.c', '../tfewindow.c', '../tfepref.c', '../tfealert.c', '../../tfetextview/tfetextview.c')
executable('test_window', sourcefiles_test_window, resources, dependencies: gtkdep, export_dynamic: true, install: false)
