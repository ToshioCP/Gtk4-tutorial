project('csv', 'c')

glibdep = dependency('glib-2.0', version: '>=2.68.0')
gtkdep = dependency('gtk4', version: '>=4.8')

gnome=import('gnome')
resources = gnome.compile_resources('resources','listeditor.gresource.xml')

#sourcefiles=files('listeditor.c', 'tcsvapplication.c')

executable('listeditor', ['listeditor.c'], resources, dependencies: gtkdep, export_dynamic: true, install: false)
