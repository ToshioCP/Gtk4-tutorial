require 'rake/clean'
require_relative 'lib/lib_renumber.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_mk_html_template.rb'
require_relative 'lib/lib_change_relative_link.rb'

# Usually, the suffix of xxx.src.md is .md because a suffix/extension is the part after the last period.
# But, we want the Rakefile to recognize that the suffix of xxx.src.md is .src.md.
# So we define some methods for that.

# Redefine String#ext and FileList#ext
class String
  alias :_org_ext :ext
  def ext suffix
    file = self.sub(/\.src\.md\Z/, "")._org_ext(suffix)
  end
end

class FileList
  def ext suffix
    self.map{|fl| fl.ext(suffix)}
  end
end

# get section number from src/secXX.src.md
def get_sec_num(src)
  src.match(/sec(\d+)\.src\.md$/).to_a[1].to_i # no number => 0
end

# get C file names in the .src.md file.
def c_files path
  dir = path.pathmap("%d")
  File.read(path).scan(/^@@@include\n(.*?)@@@\n/m).flatten\
                 .map{|s| s.each_line.to_a}.flatten\
                 .map{|line| "#{dir}/#{(line.match(/^\S*/)[0])}"}
end

# source files
secfiles = FileList['src/sec*.src.md']
renumber(secfiles)
# Some files may be renamed. Redefine sectiles.
secfiles = FileList['src/sec*.src.md']
secfiles.sort!{|f,g| f.match(/\d+\.src\.md$/).to_a[0].to_i <=> g.match(/\d+\.src\.md$/).to_a[0].to_i}
otherfiles = FileList["src/turtle/turtle_doc.src.md",
              "src/tfetextview/tfetextview_doc.src.md",
              "src/Readme_for_developers.src.md"]
srcfiles = secfiles + otherfiles
abstract = "src/abstract.src.md"
# imagesrcfiles are the image files used in the srcfiles.
# They are absolute paths.
imagefiles = srcfiles.map do |file|
    d = file.pathmap("%d")
    s = File.read(file)
    at_if_else(s, "html")\
      .gsub(/^~~~.*?^~~~\n/m,'').gsub(/^    .*\n/,'')\
      .scan(/!\[.*?\]\((.*?)\)/).flatten.sort.uniq\
      .map{|img| File.absolute_path("#{d}/#{img}")}
  end.flatten.sort.uniq
imagefiles = FileList[*imagefiles]

CLEAN.append(FileList["latex/*.tex", "latex/*.aux", "latex/*.log", "latex/*.toc"])
CLOBBER.append("README.md").append(FileList["gfm/*.md"])
CLOBBER.append(FileList["docs/*.html"])
CLOBBER.append(FileList["docs/image/*"])
CLOBBER.append(FileList["latex/*.pdf"])

# tasks

task default: :md
task all: [:md, :html, :pdf]

mdfiles = srcfiles.pathmap("%f").ext(".md").map{|f| "gfm/#{f}"}
task md: %w[README.md] + mdfiles

file "README.md" => [abstract] + secfiles do |t|
  abstract_md = "gfm/"+abstract.pathmap("%f").ext(".md")
  src2md(abstract, "gfm")
  buf = ["# GTK 4 Tutorial for beginners\n\nThe GitHub page of this tutorial is also available. Click [here](https://toshiocp.github.io/Gtk4-tutorial/).\n\n"]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each do |secfile|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{secfile.pathmap('%f').ext(".md")})\n"
  end
  readme_md = buf.join
  readme_md = change_relative_link(readme_md, "gfm", ".")
  File.write(t.name, readme_md)
end

# srcfiles => mdfiles
srcfiles.each do |src|
  dst = "gfm/"+src.pathmap("%f").ext(".md")
  file dst => [src] + c_files(src) + ["gfm"] do |t|
    src2md(t.source, "gfm")
    if secfiles.include?(t.source)
      i = get_sec_num(src)
      if secfiles.size == 1
        nav = "Up: [README.md](../README.md)\n"
      elsif i == 1
        nav = "Up: [README.md](../README.md),  Next: [Section 2](sec2.md)\n"
      elsif i == secfiles.size
        nav = "Up: [README.md](../README.md),  Prev: [Section #{i-1}](sec#{i-1}.md)\n"
      else
        nav = "Up: [README.md](../README.md),  Prev: [Section #{i-1}](sec#{i-1}.md), Next: [Section #{i+1}](sec#{i+1}.md)\n"
      end
      File.write(t.name, nav + "\n" + File.read(t.name) + "\n" + nav)
    end
  end
end

htmlfiles =  srcfiles.pathmap("%f").ext(".html").map{|f| "docs/#{f}"}
htmlimagefiles = imagefiles.pathmap("%f").map{|f| "docs/image/#{f}"}

task html: %W[docs/index.html docs/.nojekyll] + htmlfiles + htmlimagefiles

file "docs/index.html" => [abstract, "docs"] + secfiles do
  abstract_md = "docs/"+abstract.pathmap("%f").ext(".md")
    src2md(abstract, "html")
  buf = [ "# GTK 4 Tutorial for beginners\n\n" ]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each do |secfile|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{secfile.pathmap('%f').ext('.html')})\n"
  end
  buf << "\nThis website uses [Bootstrap](https://getbootstrap.jp/)."
  File.write("docs/index.md", buf.join)
  mk_html_template(nil, nil, nil)
  sh "pandoc -s --template=docs/template.html --metadata=title:\"GTK 4 tutorial\" -o docs/index.html docs/index.md"
  File.delete "docs/index.md"
  File.delete "docs/template.html"
end

file "docs/.nojekyll" => "docs" do |t|
  touch t.name
end

srcfiles.each do |src|
  dst = "docs/"+src.pathmap("%f").ext(".html")
  file dst => [src, "docs"] + c_files(src) do |t|
    html_md = "docs/"+src.pathmap("%f").ext(".md")
    src2md(src, "html")
    if secfiles.include?(t.source)
      i = get_sec_num(src)
      if secfiles.size == 1
        mk_html_template("index.html", nil, nil)
      elsif i == 1
        mk_html_template("index.html", nil, "sec2.html")
      elsif i == secfiles.size
        mk_html_template("index.html", "sec#{i-1}.html", nil)
      else
        mk_html_template("index.html", "sec#{i-1}.html", "sec#{i+1}.html")
      end
    else
      mk_html_template("index.html", nil, nil)
    end
    sh "pandoc -s --template=docs/template.html --metadata=title:\"GTK 4 tutorial\" -o #{t.name} #{html_md}"
    File.delete(html_md)
    File.delete "docs/template.html"
  end
end

imagefiles.each do |src|
  dst = "docs/image/"+src.pathmap("%f")
  file dst => [src, "docs/image"] do
    cp src, dst
  end
end

task pdf: %w[latex/main.tex] do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

texfiles =  srcfiles.pathmap("%f").ext(".tex").map{|f| "latex/#{f}"}
sec_texfiles = secfiles.pathmap("%f").ext(".tex").map{|f| "latex/#{f}"}
other_texfiles = otherfiles.pathmap("%f").ext(".tex").map{|f| "latex/#{f}"}
abstract_tex = "latex/"+abstract.pathmap("%f").ext(".tex")

file "latex/main.tex" => [abstract_tex] + texfiles do
  gen_main_tex "latex", abstract_tex, sec_texfiles, other_texfiles
end

file abstract_tex => abstract do |t|
  abstract_md = "latex/"+abstract.pathmap("%f").ext(".md")
  src2md(abstract, "latex")
  sh "pandoc --listings -o #{t.name} #{abstract_md}"
  File.delete(abstract_md)
end

srcfiles.each do |src|
  dst = "latex/"+src.pathmap("%f").ext(".tex")
  file dst => [src] + c_files(src) do |t|
    tex_md = "latex/"+src.pathmap("%f").ext(".md")
    src2md(src, "latex")
    if src == "src/Readme_for_developers.src.md"
      sh "pandoc -o #{t.name} #{tex_md}"
    else
      sh "pandoc --listings -o #{t.name} #{tex_md}"
    end
    File.delete(tex_md)
  end
end

["gfm", "docs", "docs/image", "latex"].each do |d|
  directory d
end

task :clean
task :clobber
task :cleangfm do
  rm 'README.md'
  remove_entry_secure("gfm")
end
task :cleanhtml do
  remove_entry_secure("docs")
end
task :cleanlatex do
  remove_entry_secure("latex")
end
