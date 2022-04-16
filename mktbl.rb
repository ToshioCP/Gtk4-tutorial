require_relative 'lib/lib_mktbl.rb'

src = File.read ARGV[0]
buf = src.partitions(/^@@@table\n.*?@@@\n/m)
buf = buf.map{|chunk| chunk=~/\A@@@table/ ? at_table(chunk) : chunk}.join
File.write file+".bak", src # backup
File.write file, buf
