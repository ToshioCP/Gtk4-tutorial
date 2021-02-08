# test_lib_sec_file.rb
require_relative "../lib/lib_sec_file.rb"

# Sample files for the test
sample_c = <<'EOS'
#include <stdio.h>

int
main (int argc, char **argv) {
  printf ("Hello world.\n");
}
EOS

sec1_text = <<'EOS'
This is a test file.
The sorce of `sample.c` is:

@@@include
sample.c main
@@@

It is the simplest C program.
EOS

sec2_text = <<'EOS'
To compile the C source file `sample.c`, type:

~~~
$ gcc sample.c
~~~

Then executable file `a.out` is generated.

@@@shell
ls
@@@

To execute it, type:

~~~
$ ./a.out
~~~

The source code is in [Section 1](sec1.src.md)
EOS

sec05_text = <<'EOS'
Prerequisite

- Linux OS like Ubuntu or Debian.
- gcc
EOS

files = [
  ["temp/sample.c", sample_c],
  ["temp/sec1.src.md", sec1_text],
  ["temp/sec2.src.md", sec2_text],
  ["temp/sec0.5.src.md", sec05_text]
]

# Generate sample file
unless Dir.exist? "temp"
  Dir.mkdir "temp"
end
files.each do |f|
  File.write f[0], f[1]
end

# Test Sec_file
print "****** Sec_file class test ******\n"
src_sec05 = Sec_file.new "temp/sec0.5.src.md"
src_sec1 = Sec_file.new "temp/sec1.src.md"
src_sec2 = Sec_file.new "temp/sec2.src.md"
test_items = [
  ["path", "\"temp/sec1.src.md\""],
  ["basename", "\"sec1.src.md\""],
  ["dirname", "\"temp\""],
  ["c_files", "[ \"temp/sample.c\" ]"],
  ["to_md", "\"sec1.md\""],
  ["to_html", "\"sec1.html\""],
  ["to_tex", "\"sec1.tex\""],
  ["num", "\"1\""],
  ["to_f", "1.0"],
  ["<=> src_sec05", "1"],
  ["<=> src_sec1", "0"],
  ["<=> src_sec2", "-1"],
  ["is_i?", "true"]
]
test_items.each do |item|
  if eval("src_sec1.#{item[0]} != #{item[1]}")
    print "src_sec1.#{item[0]} != #{item[1]}\n"
    print " ....  src_sec1.#{item[0]} is #{eval("src_sec1.#{item[0]}")}\n"
  end
end
if src_sec05.is_i? != false
  print "src_sec05.is_i? != false\n"
end
src_sec2.renum! 3
print "\n\"src_sec2.renum! 3\" is invoked.\n\n"
if src_sec2.path != "temp/sec3.src.md"
  print "temp/sec2.src.md is renumbered to 3, but the name didn't changed to sec3.src.md\n"
  print " ....  The name is #{src_sec2.path})}\n"
end
unless File.exist? "temp/sec3.src.md"
  print "temp/sec3/src/md doesn't exist.\n"
end
if src_sec2.basename != "sec3.src.md"
  print "The new basename isn't sec3.src.md.\n"
  print " ....  It is #{src_sec2.basename}.\n"
end
if src_sec2.dirname != "temp"
  print "The new dirname isn't temp.\n"
  print " ....  It is #{src_sec2.dirname}.\n"
end

# Test Sec_files
print "****** Sec_files class test ******\n"
sec_files = Sec_files.new [src_sec05, src_sec1, src_sec2]
sec_files.renum!
print "\n\"sec_files.renum!\" is invoked.\n\n"
filenames = sec_files.map { |sec_file| sec_file.path }
if filenames != ["temp/sec1.src.md", "temp/sec2.src.md", "temp/sec3.src.md"]
  print "Renumbering failed.\n"
  print "The filenames are:\n"
  p filenames
end
s_line = /The source code is in \[Section \d\]\(sec\d.src.md\)/
buf = File.readlines src_sec2.path
i = buf.find_index {|l| l =~ s_line }
if i
  line = buf[i]
else
  print "The line \"The source code is in [Section \\d](sec\\d.src.md)\" didn't find in src_sec2.\n"
end
if line != "The source code is in [Section 2](sec2.src.md)\n"
  print "\"Section 1\" didn't change to \"Section 2\", or \"sec1\" didn't change to \"sec2\".\n"
  print " ....  The line is #{line.chomp}.\n"
end
system "rm", "-rf", "temp"
