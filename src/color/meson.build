project('color', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','color.gresource.xml')

sourcefiles=files('colorapplication.c', '../tfetextview/tfetextview.c')

executable('color', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true)
