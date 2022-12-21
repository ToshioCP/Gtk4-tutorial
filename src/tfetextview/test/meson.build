project ('test', 'c')

gtk4dep = dependency ('gtk4')

sourcefiles_open = ['test_open.c', '../tfetextview.c']
executable ('test_open', sourcefiles_open, dependencies: gtk4dep, install: false)

sourcefiles_save = ['test_save.c', '../tfetextview.c']
executable ('test_save', sourcefiles_save, dependencies: gtk4dep, install: false)
