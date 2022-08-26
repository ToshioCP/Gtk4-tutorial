require 'rake/clean'
require 'fileutils'
require_relative 'lib/lib_renumber.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_mk_html_template.rb'
require_relative 'lib/lib_change_relative_link.rb'

def basename(file, suffix=nil)
  suffix ? File.basename(file, suffix) : File.basename(file)
end
def dirname(file)
  File.dirname(file)
end
# Get dstination file(s) from source file(s).
def mdst src
  "gfm/#{basename(src, ".src.md")}.md"
end
def hdst src
  "docs/#{basename(src, ".src.md")}.html"
end
def tdst src
  "latex/#{basename(src, ".src.md")}.tex"
end
def idst src
  "docs/image/#{basename(src)}"
end
def mdsts srcs
  srcs.map{|src| mdst(src)}
end
def hdsts srcs
  srcs.map{|src| hdst(src)}
end
def tdsts srcs
  srcs.map{|src| tdst(src)}
end
def idsts(srcs)
  srcs.map{|src| idst(src)}
end

def to_md file
  basename(file).sub(/\..*$/,'.md')
end
def to_html file
  basename(file).sub(/\..*$/,'.html')
end
# get C file names in the .src.md file.
def c_files path
  dir = dirname(path)
  File.read(path).scan(/^@@@include\n(.*?)@@@\n/m).flatten\
                 .map{|s| s.each_line.to_a}.flatten\
                 .map{|line| "#{dir}/#{(line.match(/^\S*/)[0])}"}
end
def is_secfile?(file)
  file =~ /sec\d+\.src\.md$/
end

# source files
secfiles = FileList['src/sec*.src.md']
renumber(secfiles)
otherfiles = ["src/turtle/turtle_doc.src.md",
              "src/tfetextview/tfetextview_doc.src.md",
              "src/Readme_for_developers.src.md"]
srcfiles = secfiles + otherfiles
abstract = "src/abstract.src.md"
# imagesrcfiles are the image files used in the srcfiles.
# They are absolute paths.
imagefiles = srcfiles.map do |file|
    d = dirname(file)
    File.read(file)\
        .gsub(/^~~~.*?^~~~\n/m,'').gsub(/^    .*\n/,'')\
        .scan(/!\[.*?\]\((.*?)\)/).flatten.sort.uniq\
        .map{|img| File.absolute_path("#{d}/#{img}")}
  end.flatten.sort.uniq

["gfm", "docs", "latex"].each{|d| Dir.mkdir(d) unless Dir.exist?(d)}
Dir.mkdir('docs/image') unless Dir.exist?('docs/image')

CLEAN.append(FileList["latex/*.tex", "latex/*.aux", "latex/*.log", "latex/*.toc"])
CLOBBER.append("Readme.md").append(FileList["gfm/*.md"])
CLOBBER.append(FileList["docs/*.html"])
CLOBBER.append(FileList["docs/image/*"])
CLOBBER.append(FileList["latex/*.pdf"])

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: %w[Readme.md] + mdsts(srcfiles)

file "Readme.md" => [abstract] + secfiles do
  abstract_md = mdst(abstract)
  src2md(abstract, "gfm")
  buf = ["# Gtk4 Tutorial for beginners\n\nThe github page of this tutorial is also available. Click [here](https://toshiocp.github.io/Gtk4-tutorial/).\n\n"]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{to_md(secfile)})\n"
  end
  readme_md = buf.join
  readme_md = change_relative_link(readme_md, "gfm", ".")
  File.write("Readme.md", readme_md)
end

# srcfiles => mdfiles
srcfiles.each_with_index do |src, i|
  dst = mdst(src)
  file dst => [src] + c_files(src) do
    src2md(src, "gfm")
    if is_secfile?(src)
      if secfiles.size == 1
        nav = "Up: [Readme.md](../Readme.md)\n"
      elsif i == 0
        nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](sec2.md)\n"
      elsif i == secfiles.size - 1
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](sec#{i}.md)\n"
      else
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](sec#{i}.md), Next: [Section #{i+2}](sec#{i+2}.md)\n"
      end
      File.write(dst, nav + "\n" + File.read(dst) + "\n" + nav)
    end
  end
end

task html: %W[docs/index.html docs/.nojekyll] + hdsts(srcfiles) + idsts(imagefiles)

file "docs/index.html" => [abstract] + secfiles do
  abstract_md = "docs/#{to_md(abstract)}"
  src2md(abstract, "html")
  buf = [ "# Gtk4 Tutorial for beginners\n\n" ]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{to_html(secfile)})\n"
  end
  buf << "\nThis website uses [Bootstrap](https://getbootstrap.jp/)."
  File.write("docs/index.md", buf.join)
  mk_html_template(nil, nil, nil)
  sh "pandoc -s --template=docs/template.html --metadata=title:\"Gtk4 tutorial\" -o docs/index.html docs/index.md"
  File.delete "docs/index.md"
  File.delete "docs/template.html"
end

file "docs/.nojekyll" do |t|
  touch t.name
end

srcfiles.each_with_index do |src, i|
  dst = hdst(src)
  file dst => [src] + c_files(src) do
    html_md = "docs/#{to_md(src)}"
    src2md(src, "html")
    if is_secfile?(src)
      if secfiles.size == 1
        mk_html_template("index.html", nil, nil)
      elsif i == 0
        mk_html_template("index.html", nil, "sec2.html")
      elsif i == secfiles.size - 1
        mk_html_template("index.html", "sec#{i}.html", nil)
      else
        mk_html_template("index.html", "sec#{i}.html", "sec#{i+2}.html")
      end
    else
      mk_html_template("index.html", nil, nil)
    end
    sh "pandoc -s --template=docs/template.html --metadata=title:\"Gtk4 tutorial\" -o #{dst} #{html_md}"
    File.delete(html_md)
    File.delete "docs/template.html"
  end
end

imagefiles.each do |src|
  dst = idst(src)
  file dst => src do
    cp src, dst
  end
end

task pdf: %w[latex/main.tex] do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

file "latex/main.tex" => [tdst(abstract)] + tdsts(srcfiles) do
  gen_main_tex "latex", tdst(abstract), tdsts(secfiles), tdsts(otherfiles)
end

file tdst(abstract) => abstract do
  abstract_md = "latex/#{to_md(abstract)}"
  src2md(abstract, "latex")
  sh "pandoc --listings -o #{tdst(abstract)} #{abstract_md}"
  File.delete(abstract_md)
end

srcfiles.each do |src|
  dst = tdst(src)
  file dst => [src] + c_files(src) do
    tex_md = "latex/#{to_md(src)}"
    src2md(src, "latex")
    if src == "src/Readme_for_developers.src.md"
      sh "pandoc -o #{dst} #{tex_md}"
    else
      sh "pandoc --listings -o #{dst} #{tex_md}"
    end
    File.delete(tex_md)
  end
end

task :clean
task :clobber
task :cleangfm do
  remove_entry_secure("gfm")
end
task :cleanhtml do
  remove_entry_secure("docs")
end
task :cleanlatex do
  remove_entry_secure("latex")
end
