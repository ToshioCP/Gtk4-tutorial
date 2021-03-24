project('list5', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','list5.gresource.xml')

sourcefiles=files('list5.c')

executable('list5', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)

