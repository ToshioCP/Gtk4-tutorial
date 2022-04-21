class Src_file <String
  def initialize path
    check_init path
    super(path)
  end
  def replace path
    check_init path
    super(path)
  end

  def check_init path
    raise  "The argument is not String type." unless path.is_a?(String)
    raise  "File #{path} is not exist." unless File.exist?(path)
    raise  "The argment \"#{path}\" doesn't have .src.md suffix." unless path =~ /\.src\.md$/
    @name = File.basename path, ".src.md"
    @dirname = File.dirname path
  end
  private :check_init

  def path
    self
  end
  def basename
    @name+".src.md"
  end
  def dirname
    @dirname
  end
  def to_md
    @name+".md"
  end
  def to_html
    @name+".html"
  end
  def to_tex
    @name+".tex"
  end
  def c_files
    File.read(self).scan(/^@@@include\n.*?@@@\n/m).map{|s| s.each_line.to_a}.flatten\
                   .reject{|line| line =~ /@@@include\n|@@@\n/}\
                   .map{|line| @dirname+"/"+(line.match(/\S*/)[0])}
  end
end

class Sec_file < Src_file
  def initialize path
    raise  "The argment \"#{path}\" doesn't have secXX.src.md form. (XX is int or float.)" unless path =~ /sec\d+(\.\d+)?\.src\.md$/
    super(path)
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
end

class Sec_files < Array
  def initialize sec_files
    raise "#{sec_files} is not an array." unless sec_files.instance_of? Array
    sec_files.each do |sec_file|
      raise "#{sec_file} is not an instance of Sec_file." unless sec_file.instance_of? Sec_file
    end
    super(sec_files)
  end
  def renum!
    temp = get_temp_name()
    self.sort!
    rename_rule = []
    self.each_with_index do |file, i|
      # rule: sec_file, filename_old, temporary_file, filename_new, number_old, number_new
      # Be careful that the sec_file will change from filename_old to filename_new. String is mutable!
      rename_rule << [file, String.new(file), file+temp, file.gsub(/\d+(\.\d+)?\.src\.md$/,"#{i+1}.src.md"),\
                      file.match(/(\d+(\.\d+)?)\.src\.md$/)[1], (i+1).to_s]
    end
    rename_rule.each do |rule|
      File.rename rule[1], rule[2] if rule[1] != rule[3]
    end
    rename_rule.each do |rule|
      File.rename rule[2], rule[3] if rule[1] != rule[3]
      rule[0].replace rule[3]
    end
    self.each do |file|
      org = File.read(file)
      new = File.read(file)
      rename_rule.each do |rule|
        if rule[1] != rule[3]
          new = new.gsub(/(\[(S|s)ection *)#{rule[4]}\]\(sec#{rule[4]}\.src\.md\)/, "\\1#{rule[4]}](#{rule[2]})")
        end
      end
      rename_rule.each do |rule|
        if rule[1] != rule[3]
          new = new.gsub(/(\[(S|s)ection *)#{rule[4]}\]\(#{rule[2]}\)/, "\\1#{rule[5]}](sec#{rule[5]}.src.md)")
        end
      end
      File.write(file, new) unless new == org
    end
  end

private
  def get_temp_name
    "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
  end

end
