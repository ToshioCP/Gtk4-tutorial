require_relative '../lib/lib_mktbl.rb'

file = ARGV[0]
old = File.readlines file
in_stat = false
new = []
changed = false
tmp = []
old.each do |line|
  if in_stat
    if line == "@@@\n"
      in_stat = false
      new += mktbl tmp
    else
      tmp << line
    end
  elsif line == "@@@table\n"
    changed = true
    in_stat = true
    tmp = []
  else
    new << line
  end
end
new.each do |line|
  if line[-1] != "\n"
    line.sub!(/\z/,"\n")
  end
end
exit unless changed
File.write file+".bak", old.join
File.write file, new.join
