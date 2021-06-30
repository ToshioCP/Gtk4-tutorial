require 'rake/clean'

require_relative 'lib/lib_src_file.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_add_head_tail_html.rb'

secfiles = []
FileList['src/sec*.src.md'].each do |file|
  secfiles << Sec_file.new(file)
end
secfiles = Sec_files.new secfiles
secfiles.renum!

abstract = Src_file.new "src/abstract.src.md"

otherfiles = ["src/turtle/turtle_doc.src.md",
              "src/tfetextview/tfetextview_doc.src.md",
              "src/Readme_for_developers.src.md"].map {|file| Src_file.new file}
srcfiles = secfiles + otherfiles

file_table = srcfiles.map do |srcfile|
  [
    srcfile,
    "gfm/" + srcfile.to_md,
    "html/" + srcfile.to_html,
    "latex/" + srcfile.to_tex
  ]
end

mdfiles = srcfiles.map {|file| "gfm/" + file.to_md}
htmlfiles = srcfiles.map {|file| "html/" + file.to_html}
sectexfiles = secfiles.map {|file| "latex/" + file.to_tex}
othertexfiles = otherfiles.map {|file| "latex/" + file.to_tex}
texfiles = srcfiles.map {|file| "latex/" + file.to_tex}

["gfm", "html", "latex"].each do |d|
  if ! Dir.exist?(d)
    Dir.mkdir(d)
  end
end

CLEAN.append(*mdfiles)
CLEAN << "Readme.md"

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: %w[Readme.md] + mdfiles

file "Readme.md" => [abstract] + secfiles do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  src2md abstract, abstract.to_md, file_table, "gfm"
  buf += File.readlines(abstract.to_md)
  File.delete(abstract.to_md)
  buf.append("\n", "## Table of contents\n", "\n")
  0.upto(secfiles.size-1) do |i|
    h = File.open(secfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](gfm/#{secfiles[i].to_md})\n"
  end
  File.write("Readme.md", buf.join)
end

file_table.each do |tbl|
  file tbl[1] => [tbl[0]] + tbl[0].c_files do
    src2md tbl[0], tbl[1], file_table, "gfm"
    if tbl[0].instance_of? Sec_file
      i = tbl[0].num.to_i - 1
      if secfiles.size == 1
        nav = "Up: [Readme.md](../Readme.md)\n"
      elsif i == 0
        nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](#{secfiles[1].to_md})\n"
      elsif i == secfiles.size - 1
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{secfiles[i-1].to_md})\n"
      else
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{secfiles[i-1].to_md}), Next: [Section #{i+2}](#{secfiles[i+1].to_md})\n"
      end
      buf = [nav, "\n"]
      buf += File.readlines tbl[1]
      buf.append("\n", nav)
      File.write tbl[1], buf.join
    end
  end
end

task html: ["html/index.html"] + htmlfiles

file "html/index.html" => [abstract] + secfiles do
  buf = [ "# Gtk4 Tutorial for beginners\n", "\n" ]
  abstract_md = "html/#{abstract.to_md}"
  src2md abstract, abstract_md, file_table, "html"
  buf += File.readlines(abstract_md)
  File.delete(abstract_md)
  buf.append("\n", "## Table of contents\n", "\n")
  0.upto(secfiles.size-1) do |i|
    h = File.open(secfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "1. [#{h}](#{secfiles[i].to_html})\n"
  end
  File.write("html/index.md", buf.join)
  sh "pandoc -o html/index.html html/index.md"
  File.delete "html/index.md"
  add_head_tail_html "html/index.html"
end

file_table.each do |tbl|
  file tbl[2] => [tbl[0]] + tbl[0].c_files do
    html_md = "html/" + tbl[0].to_md
    src2md tbl[0], html_md, file_table, "html"
    if tbl[0].instance_of? Sec_file
      i = tbl[0].num.to_i - 1 # 0 based index
      if secfiles.size == 1
        nav = "Up: [index.html](index.html)\n"
      elsif i == 0
        nav = "Up: [index.html](index.html),  Next: [Section 2](#{secfiles[1].to_html})\n"
      elsif i == secfiles.size - 1
        nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{secfiles[i-1].to_html})\n"
      else
        nav = "Up: [index.html](index.html),  Prev: [Section #{i}](#{secfiles[i-1].to_html}), Next: [Section #{i+2}](#{secfiles[i+1].to_html})\n"
      end
      buf = [nav, "\n"]
      buf += File.readlines html_md
      buf.append("\n", nav)
      File.write html_md, buf.join
    end
    sh "pandoc -o #{tbl[2]} #{html_md}"
    File.delete(html_md)
    add_head_tail_html tbl[2]
  end
end

task pdf: "latex" do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

task latex: ["latex/main.tex"]

file "latex/main.tex" => ["latex/abstract.tex"]+texfiles do
  gen_main_tex "latex", sectexfiles, othertexfiles
end

abstract_tex = "latex/"+abstract.to_tex
file abstract_tex => abstract do
  abstract_md = "latex/"+abstract.to_md
  src2md abstract, abstract_md, file_table, "latex"
  sh "pandoc --listings -o #{abstract_tex} #{abstract_md}"
  File.delete(abstract_md)
end

file_table.each do |tbl|
  file tbl[3] => [tbl[0]] + tbl[0].c_files do
    tex_md = "latex/" + tbl[0].to_md
    src2md tbl[0], tex_md, file_table, "latex"
    sh "pandoc --listings -o #{tbl[3]} #{tex_md}"
    File.delete(tex_md)
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

