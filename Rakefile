require 'rake/clean'

require_relative 'lib/lib_sec_file.rb'
require_relative 'lib/lib_src2md.rb'

srcfiles = []
FileList['src/*.src.md'].each do |file|
  srcfiles << Sec_file.new(file)
end
srcfiles = Sec_files.new srcfiles
srcfiles.renum!

mdfilenames = srcfiles.map {|srcfile| "gfm/#{srcfile.to_md}"}
htmlfilenames = srcfiles.map {|srcfile| "html/#{srcfile.to_html}"}
texfilenames = srcfiles.map {|srcfile| "latex/#{srcfile.to_tex}"}

["gfm", "html", "latex"].each do |d|
  if ! Dir.exist?(d)
    Dir.mkdir(d)
  end
end

CLEAN.append(*mdfilenames)
CLEAN << "Readme.md"

# Abstract
abstract=<<'EOS'
This tutorial illustrates how to write C programs with Gtk4 library.
It focuses on beginners so the contents are limited to basic things such as widgets, GObject, signal, menus and build system.
Please refer [Gnome API reference](https://developer.gnome.org/) for further topics.

This tutorial is under development and unstable.
Even though the  examples written in C language have been tested on gtk4 version 4.0,
there might exist bugs.
If you find any bugs, errors or mistakes in the tutorial and C examples,
please let me know.
You can post it to [github issues](https://github.com/ToshioCP/Gtk4-tutorial/issues).
The latest version of the tutorial is located at [Gtk4-tutorial githup repository](https://github.com/ToshioCP/Gtk4-tutorial).
You can read it without download.
EOS

abstract_html_array = abstract.gsub(/\[([^\]]*)\]\(([^\)]*)\)/,"<a href=\"\\2\">\\1</a>").split("\n\n")
abstract_html_array.map! { |s| s.gsub(/[^\n]\z/,"\n").gsub(/\A/,"<p>\n").gsub(/\z/,"</p>\n") } 
abstract_html = abstract_html_array.join

abstract_latex = abstract.gsub(/\[([^\]]*)\]\(([^\)]*)\)/, "\\href{\\2}{\\1}")

# Headers or a tail which is necessary for html files.
header=<<'EOS'
<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

<style type="text/css">
<!--
    body {width: 1080px; margin: 0 auto; font-size: large;}
    h2 {padding: 10px; background-color: #d0f0d0; }
    pre { margin: 10px; padding: 16px 10px 8px 10px; border: 2px solid silver; background-color: ghostwhite; overflow-x:scroll}
-->
</style>

    <title>gtk4 tutorial</title>
</head>
<body>
EOS

tail=<<'EOS'
</body>
</html>
EOS

file_index =<<'EOS'
<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

<style type="text/css">
<!--
    body {width: 1080px; margin: 0px auto; font-size: large;}
    h1 {padding: 10px 20px 10px 20px; background-color: #e0f0f0}
    li {margin: 10px 0px 10px 0px; font-size: x-large; list-style: decimal}
-->
</style>

    <title>gtk4 tutorial</title>
</head>
<body>
<h1>Gtk4 Tutorial for beginners</h1>
@@@ abstract
<ul>
EOS

file_index.gsub!(/@@@ abstract\n/, abstract_html)

# Preamble for latex files.

main = <<'EOS'
\documentclass[a4paper]{book}
\include{helper.tex}
\begin{document}
\frontmatter
\begin{titlepage}

\begin{center}
\begin{tikzpicture}
  \node at (0,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (70pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (140pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (210pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (280pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (350pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
\end{tikzpicture}
\end{center}

\vspace{4cm}
\begin{center}
{\Huge Gtk4 tutorial for beginners}
\end{center}
\vspace{1cm}
\begin{center}
{\huge Toshio Sekiya}
\end{center}
%\vspace{6.5cm}
%\begin{center}
%{\Large Organization}
%\end{center}
%\begin{center}
%{\Large Department}
%\end{center}
%\vspace{3cm}
\vspace{10cm}

\begin{center}
\begin{tikzpicture}
  \node at (0,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (70pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (140pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (210pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (280pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
  \node at (350pt,0) {\includegraphics[width=100pt]{../image/gecko.png}};
\end{tikzpicture}
\end{center}

\end{titlepage}
@@@ abstract
\tableofcontents
\mainmatter
EOS

main.gsub!(/@@@ abstract\n/, abstract_latex)

helper = <<'EOS'
\usepackage[pdftex]{graphicx}
\usepackage[colorlinks=true,linkcolor=black]{hyperref}
\usepackage[margin=2.4cm]{geometry}
\usepackage{tikz}
\providecommand{\tightlist}{%
  \setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}}
EOS
# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: ["Readme.md"]

file "Readme.md" => mdfilenames do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  buf << abstract
  buf << "\n"
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](gfm/#{srcfiles[i].to_md})\n"
  end
  File.write("Readme.md", buf.join)
end

0.upto(srcfiles.size - 1) do |i|
  file "gfm/#{srcfiles[i].to_md}" => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, "gfm/#{srcfiles[i].to_md}", -1
    if srcfiles.size == 1
      nav = "Up: [Readme.md](../Readme.md)\n"
    elsif i == 0
      nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](#{srcfiles[1].to_md})\n"
    elsif i == srcfiles.size - 1
      nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].to_md})\n"
    else
      nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].to_md}), Next: [Section #{i+2}](#{srcfiles[i+1].to_md})\n"
    end
    buf = IO.readlines "gfm/#{srcfiles[i].to_md}"
    buf.insert(0, nav, "\n")
    buf.append("\n", nav)
    IO.write "gfm/#{srcfiles[i].to_md}", buf.join
  end
end

task html: htmlfilenames+["html/index.html"]

file "html/index.html" do
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    file_index += "<li> <a href=\"#{srcfiles[i].to_html}\">#{h}</a> </li>\n"
  end
  file_index += ("</ul>\n" + tail)
  IO.write("html/index.html",file_index)
end

0.upto(srcfiles.size - 1) do |i|
  file "html/#{srcfiles[i].to_html}" => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, "html/#{srcfiles[i].to_md}", -1
    buf = IO.readlines "html/"+srcfiles[i].to_md
    buf.each do |line|
      line.gsub!(/(\[[^\]]*\])\((sec\d+)\.md\)/,"\\1(\\2.html)")
    end
    IO.write "html/"+srcfiles[i].to_md, buf.join
    sh "pandoc -o html/#{srcfiles[i].to_html} html/#{srcfiles[i].to_md}"
    File.delete("html/#{srcfiles[i].to_md}")
    if srcfiles.size == 1
      nav = "Up: <a href=\"index.html\">index.html</a>\n"
    elsif i == 0
      nav = "Up: <a href=\"index.html\">index.html</a>,  "
      nav += "Next: <a href=\"sec2.html\">Section 2</a>\n"
    elsif i == srcfiles.size - 1
      nav = "Up: <a href=\"index.html\">index.html</a>,  "
      nav += "Prev: <a href=\"#{srcfiles[i-1].to_html}\">Section #{i}</a>\n"
    else
      nav = "Up: <a href=\"index.html\">index.html</a>,  "
      nav += "Prev: <a href=\"#{srcfiles[i-1].to_html}\">Section #{i}</a>, "
      nav += "Next: <a href=\"#{srcfiles[i+1].to_html}\">Section #{i+2}</a>\n"
    end
    buf = IO.readlines "html/"+srcfiles[i].to_html
    buf.insert(0, header, nav, "\n")
    buf.append("\n", nav, "\n", tail)
    IO.write "html/"+srcfiles[i].to_html, buf.join
  end
end

task pdf: "latex" do
  sh "cd latex; pdflatex main.tex"
  sh "cd latex; pdflatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

task latex: texfilenames+["latex/main.tex"]

file "latex/main.tex" do
  0.upto(srcfiles.size-1) do |i|
    main += "  \\input{#{srcfiles[i].to_tex}}\n"
  end
  main += "\\end{document}\n"
  IO.write("latex/main.tex", main)
  IO.write("latex/helper.tex", helper)
end

0.upto(srcfiles.size - 1) do |i|
  file "latex/#{srcfiles[i].to_tex}" => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, "latex/#{srcfiles[i].to_md}", 80
    sh "pandoc -o latex/#{srcfiles[i].to_tex} --top-level-division=chapter latex/#{srcfiles[i].to_md}"
    File.delete("latex/#{srcfiles[i].to_md}")
  end
end

task :clean
task :cleanhtml do
  if Dir.exist?("html") && (! Dir.empty?("html"))
    sh "rm html/*"
  end
end
task :cleanlatex do
  if Dir.exist?("latex") && (! Dir.empty?("latex"))
    sh "rm latex/*"
  end
end
task cleanall: [:clean, :cleanhtml, :cleanlatex]

