class Sec_file < String
  def initialize path
    unless path.instance_of?(String)
      raise  "Sec_file class initialization error: The argument is not String type."
    end
    unless File.exist?(path)
      raise  "Sec_file class initialization error: File #{path} is not exist."
    end
    unless path =~ /sec\d+(\.\d+)?\.src\.md$/
      raise  "Sec_file class initialization error: The argment \"#{path}\" doesn't have secXX.src.md form. XX are digits."
    end
    @name = File.basename path
    @dirname = File.dirname path
    super(path)
  end
  def path
    self
  end
  def basename
    @name
  end
  def dirname
    @dirname
  end
  def c_files
    buf = IO.readlines(self)
    files = []
    in_include = false
    buf.each do |line|
      if in_include
        if line == "@@@\n"
          in_include = false
        else
          files << @dirname+"/"+line.match(/^ *(\S*)/)[1]
        end
      elsif line == "@@@include\n"
        in_include = true
      else
        # lines out of @@@include command is thrown away.
      end
    end
    files
  end
  def to_md
    @name.gsub(/\.(src\.md|md|html|tex)$/, ".md")
  end
  def to_html
    @name.gsub(/\.(src\.md|md|html|tex)$/, ".html")
  end
  def to_tex
    @name.gsub(/\.(src\.md|md|html|tex)$/, ".tex")
  end
  def num # the return value is String
    @name.match(/\d+(\.\d+)?/)[0]
  end
  def to_f
    self.num.to_f
  end
  def <=> other
    if other.instance_of?(Sec_file)
      self.to_f <=> other.to_f
    else
      nil
    end
  end
# Note: is_i? indicates the number is integer mathematically. For example, 2.0 is an integer.
# It doesn't mean the class of the number is Integer.
  def is_i?
    self.to_f == self.to_f.floor
  end
  def renum! n
    if n.instance_of?(String)
      n = n.to_i if n =~ /^\d+$/
      n = n.to_f if n =~ /^\d+\.\d+/
    end
    if n.instance_of?(Integer) || n.instance_of?(Float)
      n = n.to_i if n == n.floor
      old = self
      new = self.gsub(/\d+(\.\d+)?\.src\.md$/, "#{n}.src.md")
      if old != new
        File.rename old, new
        self.replace new
        @name = File.basename new
        @dirname = File.dirname new
      end
    end
  end
end

class Sec_files < Array
  def initialize sec_files
    if sec_files.instance_of? Array
      sec_files.each do |sec_file|
        unless sec_file.instance_of? Sec_file
          raise "#{sec_file} is not an instance of Sec_file."
        end
      end
      super(sec_files)
    else
      raise "#{sec_files} is not an array."
    end
  end
  def renum!
    self.sort!
    tbl = []
    n = 1
    self.each do |sec_file|
      tbl << [ sec_file.num, n, sec_file.to_f == n ? true : false ]
      n += 1
    end
    while any_diff?(tbl)
      unless try_renum(tbl)
        break
      end
    end
    if any_diff?(tbl)
      raise "Renumbering failed."
    end
  end

private
  def any_diff? tbl
    tbl.find_index { |row| row[2] == false }
  end
  def try_renum tbl
    changed = false
    (self.size - 1).downto 0 do |i|
      if tbl[i][2] == false
        n = tbl[i][1] # number to substitute
        found = self.find_index { |sec_file| sec_file != self && sec_file.to_f == n }
        unless found # OK to replace
          self[i].renum! n
          tbl[i][2] = true
#         tbl[0] (old number (String) is kept in the array 'tbl')
          changed = true
          self.each do |sec_file|
            buf = IO.readlines sec_file
            buf_n = buf.map { |line| line.gsub(/((S|s)ection *)#{tbl[i][0]}/, "\\1#{n}").gsub(/((S|s)ec *)#{tbl[i][0]}/, "\\1#{n}") }
            IO.write sec_file, buf_n.join
          end
        end
      end
    end
    changed
  end
end
