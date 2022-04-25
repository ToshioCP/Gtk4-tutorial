require 'fileutils'
require 'pathname'
include FileUtils

class String
  def partitions pattern
    a = []
    b = self.partition(pattern)
    until b[1] == ""
      a += [b[0],b[1]]
      b = b[2].partition(pattern)
    end
    a += [b[0]]
  end
end

def change_relative_link src, old_dir, new_dir
  p_new_dir = Pathname.new(new_dir)
  buf = src.partitions(/^~~~.*?^~~~\n/m)
  buf = buf.map{|chunk| chunk =~ /\A~~~.*?^~~~\n\z/m ? chunk : chunk.partitions(/(^    .*\n)+/)}.flatten
  # buf = buf.inject([]){|b,e| b.append(*e)}
  buf = buf.map do |chunk|
    if (chunk =~ /\A~~~.*?^~~~\n\z/m || chunk =~ /\A(^    .*\n)+\z/)
      chunk
    else
      # change inline codes (`...`) to escape char ("\e"=0x1B) in the link change procedure temporarily.
      # This avoids the influence of the change in the inline codes.
      # So, .src.md files must not include escape code (0x1B).
      codes = chunk.scan(/`.*?`/)
      chunk = chunk.gsub(/`.*?`/,"\e")
      b = chunk.partitions(/\[.*?\]\(.*?\)/)
      b = b.map do |c|
        m = c.match(/(\[.*?\])\((.*?)\)/)
        if m == nil
          c
        else
          name = m[1]
          target = m[2]
          if target.include?("\e") || target =~ /^(http|\/)/
            c
          else
            n_target = Pathname.new("#{old_dir}/#{target}").relative_path_from(p_new_dir).to_s
            "#{name}(#{n_target})"
          end
        end
      end
      a = b.join.split("\e")
      i = 0
      codes.inject([a[0]]){|b, code| i+=1; b.append(code, a[i])}.join
    end
  end
  buf.join
end
