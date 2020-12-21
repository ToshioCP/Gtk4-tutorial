class Sec_file < String
  def initialize path
    if path.instance_of?(String) && File.exist?(path)
      @name = File.basename path
      @dirname = File.dirname path
      unless @name =~ /^sec\d+(\.\d+)?\.(src\.md|md|html|tex)$/
        raise "Sec_file class initialization error: #{path} is not Sec_file object name."
      end
      super(path)
    else
      raise  "Sec_file class initialization error: file #{path} is not exist."
    end
  end
  def type
    @name.match(/\.(src\.md|md|html|tex)$/)[1]
  end
  def path
    self
  end
  def name
    @name
  end
  def dirname
    @dirname
  end
  def c_files
    if self.type != "src.md"
      return []
    else
      buf = IO.readlines(self)
      files = []
      buf.each do |line|
        if line =~ /^@@@ (\S+)/
          files << @dirname+"/"+$1
        end
      end
    files
    end
  end
  def to_srcmd
    @name.gsub(/\.(src\.md|md|html|tex)$/, ".src.md")
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
  def num
    @name.match(/\d+(\.\d+)?/)[0].to_f
  end
  def <=> other
    if other.instance_of?(Sec_file)
      self.num <=> other.num
    else
      nil
    end
  end
  def is_i?
    self.num == self.num.floor
  end
  def renum n
    if n.instance_of?(String)
      n = n.to_i if n =~ /^\d+$/
      n = n.to_f if n =~ /^\d+\.\d+/
    end
    if n.instance_of?(Integer) || n.instance_of?(Float)
      old = self
      new = self.gsub(/\d+(\.\d+)?(\.(src\.md|md|html|tex)$)/, "#{n}\\2")
      File.rename old, new
      self.replace new
      @name = File.basename self
      @dirname = File.dirname self
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
  def renum
    self.sort!
    tbl = []
    n = 1.0
    self.each do |sec_file|
      tbl << [ sec_file.num, n, sec_file.num == n ? true : false ]
      n += 1.0
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
    diff = false
    tbl.each do |t|
      diff = true if t[2] == false
    end
    diff
  end
  def try_renum tbl
    changed = false
    (self.size - 1).downto 0 do |i|
      if tbl[i][2] == false
        n = tbl[i][1] # number to substitute
        found = false
        tbl.each do |t|
          if t[0] == n
            found = true
            break
          end
        end
        unless found # OK to replace
          self[i].renum n
          tbl[i][0] = n.to_f
          tbl[i][2] = true
          changed = true
        end
      end
    end
    changed
  end
end
