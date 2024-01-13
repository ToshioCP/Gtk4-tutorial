project('tfe', 'c', license : 'GPL-3.0-or-later', meson_version:'>=1.0.1', version: '0.5')

gtkdep = dependency('gtk4')

gnome = import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')
gnome.compile_schemas(depend_files: 'com.github.ToshioCP.tfe.gschema.xml')

sourcefiles = files('main.c', 'tfeapplication.c', 'tfewindow.c', 'tfepref.c', 'tfealert.c', 'pfd2css.c', '../tfetextview/tfetextview.c')

executable(meson.project_name(), sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: true)

schema_dir = get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas/'
install_data('com.github.ToshioCP.tfe.gschema.xml', install_dir: schema_dir)
gnome.post_install (glib_compile_schemas: true)
