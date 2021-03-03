# test_lib_src2md.rb
require_relative '../lib/lib_src2md.rb'

# Many kinds of file types are supported.
# .c => C, .h => C, .rb => ruby, Rakefile, => ruby, .xml => xml, .ui => xml, .y => bison, .lex => lex, .build => meson, .md => markdown, Makefile => makefile

sample_c = <<'EOS'
/* This comment is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are. */
#include <stdio.h>

void
printf_something (char *something) {
  printf ("%s\n", something);
}

int
main (int argc, char **argv) {
  printf_something ("Hello world.");
}
EOS

sample_h = <<'EOS'
#include <gtk/gtk.h>
EOS

sample_rb = <<'EOS'
print "Hello ruby.\n"
EOS

rakefile = <<'EOS'
task default: "a.out"
file "a.out" => "sample.c" do
  sh "gcc sample.c"
end
EOS

sample_xml = <<'EOS'
<div class="h1" color="red">Fatal error!!</div>
EOS

sample_ui = <<'EOS'
<object class="GtkWindow" id="win"></object>
EOS

sample_y = <<'EOS'
program: statement | program statement ;
EOS

sample_lex = <<'EOS'
[a-zA-Z0-9]    return ALNUM;
EOS

sample_build = <<'EOS'
project('sampe', 'c')
EOS

sample0_md = <<'EOS'
# Sample text in maridown language
EOS

makefile = <<'EOS'
a.out: sample.c
	cc sample.c
EOS

sample_src_md = <<'EOS'
# Sample.src.md

This .src.md file is made for the test for lib_src2md.rb.

Sample.c with line number is:

@@@include
 sample.c
@@@

The following is also Sample.c with line number.

@@@include -n
 sample.c
@@@

The following is Sample.c without line number.

@@@include -N
 sample.c
@@@

The following prints only `printf_something`.

@@@include
sample.c printf_something
@@@

The following prints `printf_something` and `main`.

@@@include
sample.c printf_something main
@@@

Check info string.

@@@include
sample.c
sample.h
sample.rb
Rakefile
sample.xml
sample.ui
sample.y
sample.lex
sample.build
sample0.md
@@@

Compile sample.c with rake like this:

@@@shell
rake
@@@

@@@shell
echo "This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are."
@@@

The target type is:

@@@if gfm
gfm
@@@elif html
html
@@@elif latex
latex
@@@end

[Relative link](sample.c) will be converted when the target type is gfm or html.
Otherwise (latex) the link will be removed.

Another [relative link](../../Rakefile).

[Absolute link](https://github.com/ToshioCP) is kept as it is.

Image link.
If the target type is gfm or html, the size will be removed.
Otherwise (latex) it remains.

![Screenshot of the box](../../image/screenshot_lb4.png){width=6.3cm height=5.325cm}
EOS

sample_md_gfm = <<EOS
# Sample.src.md

This .src.md file is made for the test for lib_src2md.rb.

Sample.c with line number is:

~~~C
#{i=1;sample_c.lines.each {|line| line.replace(sprintf("%#2d %s", i, line)); i+=1}.join}~~~

The following is also Sample.c with line number.

~~~C
#{i=1;sample_c.lines.each {|line| line.replace(sprintf("%#2d %s", i, line)); i+=1}.join}~~~

The following is Sample.c without line number.

~~~C
#{sample_c}~~~

The following prints only `printf_something`.

~~~C
1 void
2 printf_something (char *something) {
3   printf ("%s\\n", something);
4 }
~~~

The following prints `printf_something` and `main`.

~~~C
1 void
2 printf_something (char *something) {
3   printf ("%s\\n", something);
4 }
5 
6 int
7 main (int argc, char **argv) {
8   printf_something ("Hello world.");
9 }
~~~

Check info string.

~~~C
#{i=1;sample_c.lines.each {|line| line.replace(sprintf("%#2d %s", i, line)); i+=1}.join}~~~
~~~C
#{i=1;sample_h.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~ruby
#{i=1;sample_rb.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~ruby
#{i=1;rakefile.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~xml
#{i=1;sample_xml.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~xml
#{i=1;sample_ui.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~bison
#{i=1;sample_y.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~lex
#{i=1;sample_lex.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~meson
#{i=1;sample_build.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~
~~~markdown
#{i=1;sample0_md.lines.each {|line| line.replace(sprintf("%#1d %s", i, line)); i+=1}.join}~~~

Compile sample.c with rake like this:

~~~
$ rake
~~~

~~~
$ echo "This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are."
This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are.
~~~

The target type is:

gfm

[Relative link](../temp/sample.c) will be converted when the target type is gfm or html.
Otherwise (latex) the link will be removed.

Another [relative link](../../Rakefile).

[Absolute link](https://github.com/ToshioCP) is kept as it is.

Image link.
If the target type is gfm or html, the size will be removed.
Otherwise (latex) it remains.

![Screenshot of the box](../../image/screenshot_lb4.png)
EOS

sample_md_html = <<EOS
# Sample.src.md

This .src.md file is made for the test for lib_src2md.rb.

Sample.c with line number is:

~~~{.C .numberLines}
#{sample_c}~~~

The following is also Sample.c with line number.

~~~{.C .numberLines}
#{sample_c}~~~

The following is Sample.c without line number.

~~~{.C}
#{sample_c}~~~

The following prints only `printf_something`.

~~~{.C .numberLines}
void
printf_something (char *something) {
  printf ("%s\\n", something);
}
~~~

The following prints `printf_something` and `main`.

~~~{.C .numberLines}
void
printf_something (char *something) {
  printf ("%s\\n", something);
}

int
main (int argc, char **argv) {
  printf_something ("Hello world.");
}
~~~

Check info string.

~~~{.C .numberLines}
#{sample_c}~~~
~~~{.C .numberLines}
#{sample_h}~~~
~~~{.ruby .numberLines}
#{sample_rb}~~~
~~~{.ruby .numberLines}
#{rakefile}~~~
~~~{.xml .numberLines}
#{sample_xml}~~~
~~~{.xml .numberLines}
#{sample_ui}~~~
~~~{.bison .numberLines}
#{sample_y}~~~
~~~{.lex .numberLines}
#{sample_lex}~~~
~~~{.numberLines}
#{sample_build}~~~
~~~{.markdown .numberLines}
#{sample0_md}~~~

Compile sample.c with rake like this:

~~~
$ rake
~~~

~~~
$ echo "This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are."
This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are.
~~~

The target type is:

html

[Relative link](../temp/sample.c) will be converted when the target type is gfm or html.
Otherwise (latex) the link will be removed.

Another [relative link](../../Rakefile).

[Absolute link](https://github.com/ToshioCP) is kept as it is.

Image link.
If the target type is gfm or html, the size will be removed.
Otherwise (latex) it remains.

![Screenshot of the box](../../image/screenshot_lb4.png)
EOS

sample_md_latex = <<EOS
# Sample.src.md

This .src.md file is made for the test for lib_src2md.rb.

Sample.c with line number is:

~~~{.C .numberLines}
#{sample_c}~~~

The following is also Sample.c with line number.

~~~{.C .numberLines}
#{sample_c}~~~

The following is Sample.c without line number.

~~~{.C}
#{sample_c}~~~

The following prints only `printf_something`.

~~~{.C .numberLines}
void
printf_something (char *something) {
  printf ("%s\\n", something);
}
~~~

The following prints `printf_something` and `main`.

~~~{.C .numberLines}
void
printf_something (char *something) {
  printf ("%s\\n", something);
}

int
main (int argc, char **argv) {
  printf_something ("Hello world.");
}
~~~

Check info string.

~~~{.C .numberLines}
#{sample_c}~~~
~~~{.C .numberLines}
#{sample_h}~~~
~~~{.ruby .numberLines}
#{sample_rb}~~~
~~~{.ruby .numberLines}
#{rakefile}~~~
~~~{.xml .numberLines}
#{sample_xml}~~~
~~~{.xml .numberLines}
#{sample_ui}~~~
~~~{.numberLines}
#{sample_y}~~~
~~~{.numberLines}
#{sample_lex}~~~
~~~{.numberLines}
#{sample_build}~~~
~~~{.numberLines}
#{sample0_md}~~~

Compile sample.c with rake like this:

~~~
$ rake
~~~

~~~
$ echo "This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are."
This text is very long, longer than 100. It must be folded if this file is converted to latex. Because latex doesn't care about verbatim lines. The lines are printed as they are.
~~~

The target type is:

latex

[Relative link](../temp/sample.c) will be converted when the target type is gfm or html.
Otherwise (latex) the link will be removed.

Another [relative link](../../Rakefile).

[Absolute link](https://github.com/ToshioCP) is kept as it is.

Image link.
If the target type is gfm or html, the size will be removed.
Otherwise (latex) it remains.

![Screenshot of the box](../../image/screenshot_lb4.png){width=6.3cm height=5.325cm}
EOS

# -------------------
#  Test goes here.
# -------------------

files = [
  [sample_c, "sample.c", "C"],
  [sample_h, "sample.h", "C"],
  [sample_rb, "sample.rb", "ruby"],
  [rakefile, "Rakefile", "ruby"],
  [sample_xml, "sample.xml", "xml"],
  [sample_ui, "sample.ui", "xml"],
  [sample_y, "sample.y", "bison"],
  [sample_lex, "sample.lex", "lex"],
  [sample_build, "sample.build", "meson"],
  [sample0_md, "sample0.md", "markdown"],
  [makefile, "Makefile", "makefile"],
  [sample_src_md, "sample.src.md", nil]
]

Dir.mkdir "temp" unless Dir.exist? "temp"
files.each do |f|
  File.write "temp/#{f[1]}", f[0]
end

# --- test lang
files.each do |f|
  if f[2] && lang("temp/#{f[1]}", "gfm") != f[2]
    print " lang(\"temp/#{f[1]}\") != #{f[2]} in gfm\n"
  end
  if f[2] && lang("temp/#{f[1]}", "pandoc") != f[2]
    print " lang(\"temp/#{f[1]}\") != \"\" in pandoc\n" unless f[2] == "meson" && lang("temp/#{f[1]}", "pandoc") == ""
  end
end

# --- test change_rel_link
# general relative link
s = "[sample.c](temp/sample.c)"
t = change_rel_link s, "test", "gfm"
if t != "[sample.c](../test/temp/sample.c)"
  print "Relative link change according to base directory change didn't work.\n"
  print "  Base directory test => gtm\n"
  print "  Original link  => #{s}\n"
  print "  Relative link temp/sample.c => #{t}\n"
  print "  ** Correct new link must be [sample.c](../test/temp/sample.c) **\n"
end
# link to a section
#   srcdir/secXX.src.md is converted to dstdir/secXX.md.
#   Therefore,  secXX.src.md must be changed tp secXX.md.
s = "[Section 3](sec3.src.md)"
t = change_rel_link s, "test", "gfm"
if t != "[Section 3](sec3.md)"
  print "Relative link change according to base directory change didn't work.\n"
  print "  Base directory test => gtm\n"
  print "  Original link  => #{s}\n"
  print "  Relative link temp/sample.c => #{t}\n"
  print "  ** Correct new link must be [Section 2](sec3) **\n"
end

# --- test src2md
dst_dirs = ["gfm", "html", "latex"]
dst_dirs.each do |d|
  Dir.mkdir d unless Dir.exist? d
  src2md "temp/sample.src.md", "#{d}/sample.md"
  dst_md = File.read "#{d}/sample.md"
  if d == "gfm"
    print "Gfm result didn't match !!\n" if dst_md != sample_md_gfm
#File.write "tmp.txt", sample_md_gfm
#system "diff", "#{d}/sample.md", "tmp.txt"
#File.delete "tmp.txt"
  elsif d == "html"
    if dst_md != sample_md_html
      print "Html result didn't match !!\n"
#File.write "tmp.txt", sample_md_html
#system "diff", "#{d}/sample.md", "tmp.txt"
#File.delete "tmp.txt"
    end
  elsif d == "latex"
    sample_md_latex.gsub!(/\[((R|r)elative link)\]\([^)]+\)/, "\\1")
    if dst_md != sample_md_latex
      print "Latex result didn't match !!\n"
#File.write "tmp.txt", sample_md_latex
#system "diff", "#{d}/sample.md", "tmp.txt"
#File.delete "tmp.txt"
    end
  else
   print "Unexpected error.\n"
  end
end

# --- clean the temorary files
dst_dirs.each do |d|
  if Dir.exist? d
    pathname = "#{d}/sample.md"
    File.delete pathname if File.exist? pathname
    Dir.rmdir d if Dir.empty? d
  end
end
system "rm", "-rf", "temp"
