require 'rake/clean'
require 'fileutils'
require_relative 'lib/lib_renumber.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_mk_html_template.rb'

def pair array1, array2
  n = [array1.size, array2.size].max
  (0...n).map{|i| [array1[i], array2[i], i]}
end
def s2md file
  "#{File.basename(file,'.src.md')}.md"
end
def s2html file
  "#{File.basename(file,'.src.md')}.html"
end
def s2tex file
  "#{File.basename(file,'.src.md')}.tex"
end
def c_files path
  dir = File.dirname(path)
  File.read(path).scan(/^@@@include\n(.*?)@@@\n/m).flatten\
                 .map{|s| s.each_line.to_a}.flatten\
                 .map{|line| "#{dir}/#{(line.match(/^\S*/)[0])}"}
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
imagesrcfiles = srcfiles.map do |file|
    d = File.dirname(file)
    File.read(file)\
        .gsub(/^    .*\n/,'').gsub(/^~~~.*?^~~~\n/m,'')\
        .scan(/!\[.*?\]\((.*?)\)/).flatten.uniq
        .map{|img| File.absolute_path("#{d}/#{img}")}
  end
imagesrcfiles = imagesrcfiles.flatten.sort.uniq

# docs is a directory for html files.
html_dir = 'docs'

# target files
mdfiles = srcfiles.map{|file| "gfm/#{s2md(file)}"}
htmlfiles = srcfiles.map {|file| "#{html_dir}/#{s2html(file)}"}
sectexfiles = secfiles.map {|file| "latex/#{s2tex(file)}"}
othertexfiles = otherfiles.map {|file| "latex/#{s2tex(file)}"}
texfiles = sectexfiles + othertexfiles
abstract_md = "gfm/#{s2md(abstract)}"
abstract_tex = "latex/#{s2tex(abstract)}"
htmlimagefiles = imagesrcfiles.map{|file| "#{html_dir}/image/#{File.basename(file)}"}

["gfm", html_dir, "latex"].each{|d| Dir.mkdir(d) unless Dir.exist?(d)}

CLEAN.append(FileList["latex/*.tex", "latex/*.aux", "latex/*.log", "latex/*.toc"])
CLOBBER.append("Readme.md").append(*mdfiles)
CLOBBER.append(FileList["docs/*.html"])
CLOBBER.append(FileList["docs/image/*"])
CLOBBER.append(FileList["latex/*.pdf"])

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: %w[Readme.md] + mdfiles

file "Readme.md" => [abstract] + secfiles do
  src2md abstract, abstract_md, "gfm"
  buf = ["# Gtk4 Tutorial for beginners\n\nThe github page of this tutorial is also available. Click [here](https://toshiocp.github.io/Gtk4-tutorial/).\n\n"]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](gfm/#{s2md(secfile)})\n"
  end
  readme_md = buf.join
  readme_md.sub!(/\[How to build Gtk4 Tutorial\]\(Readme_for_developers.md\)/,'[How to build Gtk4 Tutorial](gfm/Readme_for_developers.md)')
  File.write("Readme.md", readme_md)
end

# srcfiles => mdfiles
pair(srcfiles, mdfiles).each do |src, dst, i|
  file dst => [src] + c_files(src) do
    src2md src, dst, "gfm"
    if src =~ /sec\d+\.src\.md$/
      if secfiles.size == 1
        nav = "Up: [Readme.md](../Readme.md)\n"
      elsif i == 0
        nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](#{s2md(secfiles[1])})\n"
      elsif i == secfiles.size - 1
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{s2md(secfiles[i-1])})\n"
      else
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{s2md(secfiles[i-1])}), Next: [Section #{i+2}](#{s2md(secfiles[i+1])})\n"
      end
      File.write(dst, nav + "\n" + File.read(dst) + "\n" + nav)
    end
  end
end

task html: %W[#{html_dir}/index.html] + htmlfiles + htmlimagefiles

file "#{html_dir}/index.html" => [abstract] + secfiles do
  abstract_html_md = "#{html_dir}/#{s2md(abstract)}"
  src2md abstract, abstract_html_md, "html"
  buf = [ "# Gtk4 Tutorial for beginners\n\n" ]\
        + File.readlines(abstract_html_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_html_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{s2html(secfile)})\n"
  end
  buf << "\nThis website uses [Bootstrap](https://getbootstrap.jp/)."
  File.write("#{html_dir}/index.md", buf.join)
  mk_html_template(nil, nil, nil)
  sh "pandoc -s --template=docs/template.html --metadata=title:\"Gtk4 tutorial\" -o #{html_dir}/index.html #{html_dir}/index.md"
  File.delete "#{html_dir}/index.md"
  File.delete "docs/template.html"
end

pair(srcfiles, htmlfiles).each do |src, dst, i|
  file dst => [src] + c_files(src) do
    html_md = "#{html_dir}/#{s2md(src)}"
    src2md src, html_md, "html"
    if src =~ /sec\d+\.src\.md$/
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

pair(imagesrcfiles, htmlimagefiles).each do |src, dst, i|
  file dst => src do
    cp src, dst
  end
end

task pdf: %w[latex/main.tex] do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

file "latex/main.tex" => [abstract_tex] + texfiles do
  gen_main_tex "latex", s2tex(abstract), sectexfiles, othertexfiles
end

file abstract_tex => abstract do
  abstract_tex_md = "latex/#{s2md(abstract)}"
  src2md abstract, abstract_tex_md, "latex"
  sh "pandoc --listings -o #{abstract_tex} #{abstract_tex_md}"
  File.delete(abstract_tex_md)
end

pair(srcfiles, texfiles).each do |src, dst, i|
  file dst => [src] + c_files(src) do
    tex_md = "latex/#{s2md(src)}"
    src2md src, tex_md, "latex"
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
