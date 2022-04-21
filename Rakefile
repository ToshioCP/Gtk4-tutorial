require 'rake/clean'
require 'fileutils'
require_relative 'lib/lib_src_file.rb'
require_relative 'lib/lib_src2md.rb'
require_relative 'lib/lib_gen_main_tex.rb'
require_relative 'lib/lib_mk_html_template.rb'

secfiles = Sec_files.new(FileList['src/sec*.src.md'].to_a.map{|file| Sec_file.new(file)})
secfiles.renum!
otherfiles = ["src/turtle/turtle_doc.src.md",
              "src/tfetextview/tfetextview_doc.src.md",
              "src/Readme_for_developers.src.md"].map {|file| Src_file.new file}
srcfiles = secfiles + otherfiles
abstract = Src_file.new "src/abstract.src.md"

# docs is a directory for html files.
html_dir = 'docs'
mdfiles = srcfiles.map {|file| "gfm/" + file.to_md}
htmlfiles = srcfiles.map {|file| "#{html_dir}/" + file.to_html}
sectexfiles = secfiles.map {|file| "latex/" + file.to_tex}
othertexfiles = otherfiles.map {|file| "latex/" + file.to_tex}
texfiles = sectexfiles + othertexfiles
abstract_tex = "latex/"+abstract.to_tex

["gfm", html_dir, "latex"].each{|d| Dir.mkdir(d) unless Dir.exist?(d)}

CLEAN.append(FileList["latex/*.tex", "latex/*.aux", "latex/*.log", "latex/*.toc"])
CLOBBER.append("Readme.md").append(*mdfiles)
CLOBBER.append(FileList["docs/*.html"])
CLOBBER.append(FileList["latex/*.pdf"])

def pair array1, array2
  n = [array1.size, array2.size].max
  (0...n).map{|i| [array1[i], array2[i], i]}
end

# tasks

task default: :md
task all: [:md, :html, :pdf]

task md: %w[Readme.md] + mdfiles

file "Readme.md" => [abstract] + secfiles do
  src2md abstract, abstract.to_md, "gfm"
  buf = ["# Gtk4 Tutorial for beginners\n\nThe github page of this tutorial is also available. Click [here](https://toshiocp.github.io/Gtk4-tutorial/).\n\n"]\
        + File.readlines(abstract.to_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract.to_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile.path){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](gfm/#{secfile.to_md})\n"
  end
  File.write("Readme.md", buf.join)
end

# srcfiles => mdfiles
pair(srcfiles, mdfiles).each do |src, dst, i|
  file dst => [src] + src.c_files do
    src2md src, dst, "gfm"
    if src.instance_of? Sec_file
      if secfiles.size == 1
        nav = "Up: [Readme.md](../Readme.md)\n"
      elsif i == 0
        nav = "Up: [Readme.md](../Readme.md),  Next: [Section 2](#{secfiles[1].to_md})\n"
      elsif i == secfiles.size - 1
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{secfiles[i-1].to_md})\n"
      else
        nav = "Up: [Readme.md](../Readme.md),  Prev: [Section #{i}](#{secfiles[i-1].to_md}), Next: [Section #{i+2}](#{secfiles[i+1].to_md})\n"
      end
      File.write(dst, nav + "\n" + File.read(dst) + "\n" + nav)
    end
  end
end

task html: %W[#{html_dir}/index.html] + htmlfiles

file "#{html_dir}/index.html" => [abstract] + secfiles do
  abstract_md = "#{html_dir}/#{abstract.to_md}"
  src2md abstract, abstract_md, "html"
  buf = [ "# Gtk4 Tutorial for beginners\n\n" ]\
        + File.readlines(abstract_md)\
        + ["\n## Table of contents\n\n"]
  File.delete(abstract_md)
  secfiles.each_with_index do |secfile, i|
    h = File.open(secfile.path){|file| file.readline}.sub(/^#* */,"").chomp
    buf << "1. [#{h}](#{secfile.to_html})\n"
  end
  buf << "\nThis website uses [Bootstrap](https://getbootstrap.jp/)."
  File.write("#{html_dir}/index.md", buf.join)
  mk_html_template(nil, nil, nil)
  sh "pandoc -s --template=docs/template.html --metadata=title:\"Gtk4 tutorial\" -o #{html_dir}/index.html #{html_dir}/index.md"
  File.delete "#{html_dir}/index.md"
  File.delete "docs/template.html"
end

pair(srcfiles, htmlfiles).each do |src, dst, i|
  file dst => [src] + src.c_files do
    html_md = "#{html_dir}/#{src.to_md}"
    src2md src, html_md, "html"
    if src.instance_of? Sec_file
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

task pdf: %w[latex/main.tex] do
  sh "cd latex; lualatex main.tex"
  sh "cd latex; lualatex main.tex"
  sh "mv latex/main.pdf latex/gtk4_tutorial.pdf"
end

file "latex/main.tex" => [abstract_tex] + texfiles do
  gen_main_tex "latex", abstract.to_tex, sectexfiles, othertexfiles
end

file abstract_tex => abstract do
  abstract_md = "latex/"+abstract.to_md
  src2md abstract, abstract_md, "latex"
  sh "pandoc --listings -o #{abstract_tex} #{abstract_md}"
  File.delete(abstract_md)
end

pair(srcfiles, texfiles).each do |src, dst, i|
  file dst => [src] + src.c_files do
    tex_md = "latex/#{src.to_md}"
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
