require 'rake/clean'

require_relative 'lib/lib_sec_file.rb'
require_relative 'lib/lib_src2md.rb'


srcfiles = []
FileList['src/*.src.md'].each do |file|
  srcfiles << Sec_file.new(file)
end
srcfiles = Sec_files.new srcfiles
srcfiles.renum

mdfilenames = srcfiles.map {|srcfile| srcfile.to_md}

CLEAN.append(*mdfilenames)
CLEAN << "Readme.md"

task default: :md

task md: mdfilenames

0.upto(srcfiles.size - 1) do |i|
  file srcfiles[i].to_md => (srcfiles[i].c_files << srcfiles[i].path) do
    src2md srcfiles[i].path, srcfiles[i].to_md
    if srcfiles.size == 1
      nav = "Up: [Readme.md](#{srcfiles[i].dirname}/Readme.md)\n"
    elsif i == 0
      nav = "Up: [Readme.md](#{srcfiles[i].dirname}/Readme.md),  Next: [Section 2](#{srcfiles[1].path})\n"
    elsif i == srcfiles.size - 1
      nav = "Up: [Readme.md](#{srcfiles[i].dirname}/Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].path})\n"
    else
      nav = "Up: [Readme.md](#{srcfiles[i].dirname}/Readme.md),  Prev: [Section #{i}](#{srcfiles[i-1].path}), Next: [Section #{i+2}](#{srcfiles[i+1].path})\n"
    end
    buf = IO.readlines srcfiles[i].to_md
    buf.insert(0, nav, "")
    buf.append("", nav)
    IO.write srcfiles[i].to_md, buf.join
  end
end

task :md do
  buf = [ "# Gtk4 TUtorial for beginners\n", "\n" ]
  buf << "This tutorial is under development and unstable.\n"
  buf << "You should be careful because there might exists bugs, errors or mistakes.\n"
  buf << "\n"
  0.upto(srcfiles.size-1) do |i|
    h = File.open(srcfiles[i].path) { |file| file.readline }
    h = h.gsub(/^#* */,"").chomp
    buf << "- [#{h}](#{srcfiles[i].to_md})\n"
  end
  File.write("Readme.md", buf.join)
end

task :clean
