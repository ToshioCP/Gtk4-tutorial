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

mdfilenames = srcfiles.map {|srcfile| "gfm/#{srcfile.to_md}"}
htmlfilenames = srcfiles.map {|srcfile| "html/#{srcfile.to_html}"}
texpathnames = srcfiles.map {|srcfile| "latex/#{srcfile.to_tex}"}
texfilenames = srcfiles.map {|srcfile| srcfile.to_tex}

["gfm", "html", "latex"].each do |d|
  if ! Dir.exist?(d)
    Dir.mkdir(d)
  end
end

CLEAN.append(*mdfilenames)
CLEAN << "Readme.md"

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: ["Readme.md"]

file "Readme.md" => mdfilenames do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  src2md "src/abstract.src.md", "gfm/abstract.md", -1
  File.open("gfm/abstract.md") do |file|
    file.readlines.each do |line|
      buf << line
    end
  end
  File.delete("gfm/abstract.md")
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

task html: ["html/index.html"]

file "html/index.html" => htmlfilenames do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  src2md "src/abstract.src.md", "html/abstract.md", -1
  File.open("html/abstract.md") do |file|
    file.readlines.each do |line|
      buf << line
    end
  end
  File.delete("html/abstract.md")
  buf << "\n"
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](#{srcfiles[i].to_html})\n"
  end
  buf.each do |line|
    line.gsub!(/(\[[^\]]*\])\((sec\d+)\.md\)/,"\\1(\\2.html)")
  end
  File.write("html/index.md", buf.join)
  sh "pandoc -o html/index.html html/index.md"
  File.delete "html/index.md"
  add_head_tail_html "html/index.html"
end

0.upto(srcfiles.size - 1) do |i|
  html_md = "html/#{srcfiles[i].to_md}"
  html_html = "html/#{srcfiles[i].to_html}"
  file html_html => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, html_md, -1
    if srcfiles.size == 1
      nav = "Up: [index.html](index.html)\n"
    elsif i == 0
      nav = "Up: [index.html](index.html),  Next: [Section 2](#{srcfiles[1].to_html})\n"
    elsif i == srcfiles.size - 1
      nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{srcfiles[i-1].to_html})\n"
    else
      nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{srcfiles[i-1].to_html}), Next: [Section #{i+2}](#{srcfiles[i+1].to_html})\n"
    end
    buf = IO.readlines html_md
    buf.insert(0, nav, "\n")
    buf.append("\n", nav)
    buf.each do |line|
      line.gsub!(/(\[[^\]]*\])\((sec\d+)\.md\)/,"\\1(\\2.html)")
    end
    IO.write html_md, buf.join
    sh "pandoc -o #{html_html} #{html_md}"
    File.delete(html_md)
    add_head_tail_html html_html
  end
end

task pdf: "latex" do
  sh "cd latex; pdflatex main.tex"
  sh "cd latex; pdflatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

task latex: ["latex/main.tex"]

file "latex/main.tex" => ["latex/abstract.tex"] + texpathnames do
  gen_main_tex "latex", texfilenames
end

file "latex/abstract.tex" => "src/abstract.src.md" do
  src2md "src/abstract.src.md", "latex/abstract.md", 86
  sh "pandoc -o latex/abstract.tex latex/abstract.md"
  File.delete("latex/abstract.md")
end

0.upto(srcfiles.size - 1) do |i|
  file "latex/#{srcfiles[i].to_tex}" => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, "latex/#{srcfiles[i].to_md}", 86
    sh "pandoc -o latex/#{srcfiles[i].to_tex} latex/#{srcfiles[i].to_md}"
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

