require_relative 'lib/lib_src2md.rb'

src = File.read ARGV[0]
buf = src.partitions(/^@@@table\n.*?@@@\n/m)
dst = buf.map{|chunk| chunk=~/\A@@@table/ ? at_table(chunk) : chunk}.join
File.write ARGV[0]+".bak", src # backup
File.write ARGV[0], dst
