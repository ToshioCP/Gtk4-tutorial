project('list5', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','column.gresource.xml')

sourcefiles=files('column.c')

executable('column', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)

