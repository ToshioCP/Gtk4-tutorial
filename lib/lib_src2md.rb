# lib_src2md.rb

def src2md srcmd, md
  src_buf = IO.readlines srcmd
  src_dir = File.dirname srcmd
  md_buf = []
  comflag = false
  src_buf.each do |line|
    if comflag
      if line == "$$$\n"
        comflag = false
      else
        md_buf << "    $ "+line
        `cd #{src_dir}; #{line.chomp}`.each_line do |l|
          md_buf << l.gsub(/^/,"    ")
        end
      end
    elsif line == "$$$\n"
      comflag = true
    elsif line =~ /^@@@\s+(\S+)\s*(.*)\s*$/
      c_file = $1
      c_functions = $2.split(" ")
      if c_file =~ /^\// # absolute path
        c_file_buf = IO.readlines(c_file)
      else #relative path
        c_file_buf = IO.readlines(src_dir+"/"+c_file)
      end
      if c_functions.empty? # no functions are specified
        tmp_buf = c_file_buf
      else
        tmp_buf = []
        spc = false
        c_functions.each do |c_function|
          from = c_file_buf.find_index { |line| line =~ /^#{c_function} *\(/ }
          if ! from
            warn "ERROR!!! --- Didn't find #{func} in #{filename}. ---"
            break
          end
          to = from
          while to < c_file_buf.size do
            if c_file_buf[to] == "}\n"
              break
            end
            to += 1
          end
          n = from-1
          if spc
            tmp_buf << "\n"
          else
            spc = true
          end
          while n <= to do
            tmp_buf << c_file_buf[n]
            n += 1
          end
        end
      end
      width = tmp_buf.size.to_s.length
      n = 1
      tmp_buf.each do |l|
        md_buf << sprintf("    %#{width}d %s", n, l)
        n += 1
      end
    else
      md_buf << line
    end
  end
  IO.write(md,md_buf.join)
end

