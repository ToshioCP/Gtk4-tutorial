project('exp', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','exp.gresource.xml')

sourcefiles=files('exp.c')

executable('exp', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)

