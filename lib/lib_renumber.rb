def renumber secfiles
  temp_name = get_temp_name()
  secfiles.sort!{|f,g| f.match(/\d+(\.\d+)?\.src\.md$/).to_a[0].to_f <=> g.match(/\d+(\.\d+)?\.src\.md$/).to_a[0].to_f}
  rename_rule = []
  secfiles.each_with_index do |file, i|
    # rule: filename_old, temporary_file, filename_new
    rename_rule << [file, file+temp_name, file.gsub(/\d+(\.\d+)?\.src\.md$/,"#{i+1}.src.md")]
  end
  rename_rule.each do |rule|
    File.rename rule[0], rule[1] if rule[0] != rule[2]
  end
  rename_rule.each do |rule|
    File.rename rule[1], rule[2] if rule[0] != rule[2]
  end
  rename_rule.each do |rule|
    src = File.read(rule[2])
    changed = false
    rename_rule.each do |rule|
      old, temp, new = rule
      unless old == new
        old_n = /\d+(\.\d+)?/.match(old).to_a[0]
        new_n = /\d+(\.\d+)?/.match(new).to_a[0]
        src = src.gsub(/(\[(S|s)ection *)#{old_n}\]\(sec#{old_n}\.src\.md\)/, "\\1#{new_n}](sec#{new_n})")
        changed = true
      end
    end
    File.write(rule[2], src) if changed
  end
end
def get_temp_name
  "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
end
