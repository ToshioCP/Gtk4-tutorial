project('list4', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','list4.gresource.xml')

sourcefiles=files('list4.c')

executable('list4', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)

