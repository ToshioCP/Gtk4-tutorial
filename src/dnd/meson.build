project('dnd', 'c')

gtkdep = dependency('gtk4')

gnome = import('gnome')
resources = gnome.compile_resources('resources','dnd.gresource.xml')

executable(meson.project_name(), 'dnd.c', resources, dependencies: gtkdep, export_dynamic: true, install: false)
