require 'rake/clean'

require_relative 'lib/lib_sec_file.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_add_head_tail_html.rb'

srcfiles = []
FileList['src/sec*.src.md'].each do |file|
  srcfiles << Sec_file.new(file)
end
srcfiles = Sec_files.new srcfiles
srcfiles.renum!

mdpathnames   = srcfiles.map {|srcfile| "gfm/#{srcfile.to_md}"}
htmlpathnames = srcfiles.map {|srcfile| "html/#{srcfile.to_html}"}
texpathnames  = srcfiles.map {|srcfile| "latex/#{srcfile.to_tex}"}
texfilenames  = srcfiles.map {|srcfile| srcfile.to_tex}

["gfm", "html", "latex"].each do |d|
  if ! Dir.exist?(d)
    Dir.mkdir(d)
  end
end

CLEAN.append(*mdpathnames)
CLEAN << "Readme.md"

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: ["Readme.md", "src/turtle/turtle_doc.md"]

file "Readme.md" => mdpathnames+["src/abstract.src.md"] do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  src2md "src/abstract.src.md", "abstract.md"
  buf += File.readlines("abstract.md")
  File.delete("abstract.md")
  buf.append("\n", "## Table of contents\n", "\n")
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](gfm/#{srcfiles[i].to_md})\n"
  end
  File.write("Readme.md", buf.join)
end

0.upto(srcfiles.size - 1) do |i|
  file "gfm/#{srcfiles[i].to_md}" => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, "gfm/#{srcfiles[i].to_md}"
    if srcfiles.size == 1
      nav = "Up: [Readme.md](../Readme.md)\n"
    elsif i == 0
      nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](#{srcfiles[1].to_md})\n"
    elsif i == srcfiles.size - 1
      nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].to_md})\n"
    else
      nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].to_md}), Next: [Section #{i+2}](#{srcfiles[i+1].to_md})\n"
    end
    buf = File.readlines "gfm/#{srcfiles[i].to_md}"
    buf.insert(0, nav, "\n")
    buf.append("\n", nav)
    File.write "gfm/#{srcfiles[i].to_md}", buf.join
  end
end

file "src/turtle/turtle_doc.md" => "src/turtle/turtle_doc.src.md" do
  src2md "src/turtle/turtle_doc.src.md", "src/turtle/turtle_doc.md"
end

task html: ["html/index.html", "html/tfetextview_doc.html", "html/turtle_doc.html", "html/Readme_for_developers.html"]

file "html/index.html" => htmlpathnames+["src/abstract.src.md"] do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  src2md "src/abstract.src.md", "html/abstract.md"
  buf += File.readlines("html/abstract.md")
  File.delete("html/abstract.md")
  buf.append("\n", "## Table of contents\n", "\n")
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](#{srcfiles[i].to_html})\n"
  end
  buf.each do |line|
    line.gsub!(/doc\/Readme_for_developers.md/,"html/Readme_for_developers.html")
    line.gsub!(/(\[[^\]]*\])\((.+)\.md\)/,"\\1(\\2.html)")
  end
  File.write("html/index.md", buf.join)
  sh "pandoc -o html/index.html html/index.md"
  File.delete "html/index.md"
  add_head_tail_html "html/index.html"
end

file "html/tfetextview_doc.html" => "src/tfetextview/tfetextview_doc.md" do
  sh "pandoc -o html/tfetextview_doc.html src/tfetextview/tfetextview_doc.md"
  add_head_tail_html "html/tfetextview_doc.html"
end

file "html/turtle_doc.html" => "src/turtle/turtle_doc.src.md" do
  src2md "src/turtle/turtle_doc.src.md", "html/turtle_doc.md"
  sh "pandoc -o html/turtle_doc.html html/turtle_doc.md"
  File.delete "html/turtle_doc.md"
  add_head_tail_html "html/turtle_doc.html"
end

file "html/Readme_for_developers.html" => "doc/Readme_for_developers.md" do
  sh "pandoc -o html/Readme_for_developers.html doc/Readme_for_developers.md"
  add_head_tail_html "html/Readme_for_developers.html"
end

0.upto(srcfiles.size - 1) do |i|
  html_md = "html/#{srcfiles[i].to_md}"
  html_html = "html/#{srcfiles[i].to_html}"
  file html_html => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, html_md
    if srcfiles.size == 1
      nav = "Up: [index.html](index.html)\n"
    elsif i == 0
      nav = "Up: [index.html](index.html),  Next: [Section 2](#{srcfiles[1].to_html})\n"
    elsif i == srcfiles.size - 1
      nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{srcfiles[i-1].to_html})\n"
    else
      nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{srcfiles[i-1].to_html}), Next: [Section #{i+2}](#{srcfiles[i+1].to_html})\n"
    end
    buf = File.readlines html_md
    buf.insert(0, nav, "\n")
    buf.append("\n", nav)
    buf.each do |line|
      line.gsub!(/(\[[^\]]*\])\((.+)\.md\)/,"\\1(\\2.html)")
    end
    File.write html_md, buf.join
    sh "pandoc -o #{html_html} #{html_md}"
    File.delete(html_md)
    add_head_tail_html html_html
  end
end

task pdf: "latex" do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

task latex: ["latex/main.tex"]

file "latex/main.tex" => ["latex/abstract.tex", "latex/tfetextview_doc.tex", "latex/turtle_doc.tex"] + texpathnames do
  gen_main_tex "latex", texfilenames, ["tfetextview_doc.tex", "turtle_doc.tex"]
end

file "latex/abstract.tex" => "src/abstract.src.md" do
  src2md "src/abstract.src.md", "latex/abstract.md"
  sh "pandoc --listings -o latex/abstract.tex latex/abstract.md"
  File.delete("latex/abstract.md")
end

file "latex/tfetextview_doc.tex" => "src/tfetextview/tfetextview_doc.md" do
  sh "pandoc --listings -o latex/tfetextview_doc.tex src/tfetextview/tfetextview_doc.md"
end

file "latex/turtle_doc.tex" => "src/turtle/turtle_doc.src.md" do
  src2md "src/turtle/turtle_doc.src.md", "latex/turtle_doc.md"
  sh "pandoc --listings -o latex/turtle_doc.tex latex/turtle_doc.md"
  File.delete("latex/turtle_doc.md")
end

0.upto(srcfiles.size - 1) do |i|
  latex_md = "latex/#{srcfiles[i].to_md}"
  latex_tex = "latex/#{srcfiles[i].to_tex}"
  file latex_tex => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, latex_md
    sh "pandoc --listings -o #{latex_tex} #{latex_md}"
    File.delete(latex_md)
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

