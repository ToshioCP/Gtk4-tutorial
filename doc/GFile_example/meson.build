project('gio_samples', 'c')

compiler = meson.get_compiler('c')

giodep = dependency('gio-2.0')

executable('readline', 'readline.c', dependencies: giodep, install: false)
executable('copy', 'copy.c', dependencies: giodep, install: false)
executable('gfile_object', 'gfile_object.c', dependencies: giodep, install: false)

run_command('ruby', '../../src2md.rb', 'readme.src.md', 'readme.md')
