project('tfe', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','tfe.gresource.xml')

# Main application
sourcefiles=files('tfeapplication.c', 'tfetextview.c')
executable('tfe', sourcefiles, resources, dependencies: gtkdep)

# Test executable for tfetextview
test_sourcefiles = files('test/test_tfetextview.c', 'tfetextview.c')
test_exe = executable('test_tfetextview', test_sourcefiles, dependencies: gtkdep)

# Test registration
test('tfetextview_test', test_exe)