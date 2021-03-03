# mktbl.rb -- make table easy to read

def usage
  print "ruby mktbl.rb file\n"
  exit
end

def get_sep line
  unless line.instance_of?(String) && line =~ /^\|(:?-+:?\|)+$/
    raise "Ilegal parameter"
  end
  line = line.chomp.sub(/^\|/,"")
  a = []
  while line.length >= 1
    if line =~ /^(:?-+:?)\|(.*)$/
      pat = $1
      line = $2
      if pat[0] == ":" && pat[-1] == ":"
        a << :c
      elsif pat[0] == ":"
        a << :l
      elsif pat[-1] == ":"
        a << :r
      else #default
        a << :l
      end
    else
      raise "Unexpected error. line is \"#{line}\""
    end
  end
  a
end

def line2cells line
  unless line.instance_of?(String)
    raise "Argument must be String"
  end
  unless line.length < 1000
    raise "String too long"
  end
  a = []
  line = line.chomp.sub(/^\s*\|/,"").sub(/\s*$/, "")
  i = 0
  while i < line.length
    cell = ""
    while i < line.length && (c = line[i]) != "|"
      if c != "\\" && c != "|"
        cell << c
        i += 1
      elsif i+1 < line.length && line[i+1] == "|"
        cell << "\\|"
        i += 2
      else
        cell << "\\"
        i += 1
      end
    end
    unless i < line.length
      raise "String format error in #{line}."
    end
    cell = cell.sub(/^\s*/,"").sub(/\s*$/, "")
    a << cell
    i += 1
  end
  return a
end

def get_tbl buf
  t = []
  buf.each do |line|
    t << line2cells(line)
  end
  n = t[0].size
  t.each do |a|
    raise "Each row must have the same number of columns." unless a.size == n
  end
  t
end

def get_width tbl, sep
  n = sep.size
  tbl.each do |a|
    raise "Each row must have the same number of columns." unless a.size == n
  end
  width = []
  0.upto(n-1) do |i|
    max = 0
    tbl.each do |a|
      max = max < a[i].length ? a[i].length : max
    end
    if sep[i] == :l || sep[i] == :r
      max = max < 2 ? 2 : max
    elsif sep[i] == :c
      max = max < 3 ? 3 : max
    else
      raise "Separater format error."
    end
    width << max
  end
  width
end

def mk_sep_l sep, width
  raise "Size of sep and width is different." unless sep.size == width.size
  n = sep.size
  line = "|"
  0.upto(n-1) do |i|
    if sep[i] == :l
      line << ":"+"-"*(width[i]-1)+ "|"
    elsif sep[i] == :r
      line << "-"*(width[i]-1) + ":|"
    elsif sep[i] == :c
      line << ":" + "-"*(width[i]-2) + ":|"
    else
      raise "Separater format error."
    end
  end
  line
end

def mk_tbl_l tbl, sep, width
  n = width.size
  raise "mk_tbl_l: Each row must have the same number of columns." unless sep.size == n
  tbl.each do |a|
    raise "mk_tbl_l: Each row must have the same number of columns." unless a.size == n
    0.upto(n-1) do |i|
      raise "mk_tbl_l: table data [#{i}] has wider width than expected (#{width[i]})." if a[i].size > width[i]
    end
  end
  tbl_l = []
  tbl.each do |row|
    line = "|"
    0.upto(n-1) do |i|
      space = width[i]-row[i].length
      if sep[i] == :l
        line << row[i] + " "*space + "|"
      elsif sep[i] == :r
        line << " "*space + row[i] + "|"
      elsif sep[i] == :c
        left = space / 2
        right = space - left
        line << " "*left + row[i] + " "*right + "|"
      else
        raise "mk_tbl_l: Separater format error."
      end
    end
    tbl_l << line
  end
  tbl_l
end

def mktbl buf
  raise "The data not a table." unless buf.size >= 3 # table must have header, separator, body
  sep = get_sep buf[1] # Sep is an array contains ALIGNMENT (LEFT, CENTER or RIGHT) information of each column.
  buf.delete_at 1
  tbl = get_tbl buf # Tbl is a two dimensional array of the table. Each element is a cell of the table.
  width = get_width tbl, sep # Width is an array. Each element is the maximum width of the cells in the column corresponds to the element.
  sep_l = mk_sep_l sep, width # Make a text of the separator. Each column in the text has a width given by the second argument.
  tbl_l = mk_tbl_l tbl, sep, width # Make an array of texts. Each column in the text has a width given by the second argument.
  tbl_l.insert(1, sep_l)
  tbl_l
end

# ----  test  ----
def test_sep
  s = "|:-:|:---|---:|-|\n"
  sep = get_sep s
  raise "test_sep: Error." unless sep == [:c, :l, :r, :l]
end

def test_tbl
  t = "| a | bc|def | gh i  | \\|kl|\n"
  tbl = line2cells t
  raise "test_tbl: Error." unless tbl == ["a", "bc", "def", "gh i", "\\|kl"]
end

def test_get_tbl
  buf = [ "||book|author|price|\n",
          "| 1 | Calculus | T. Takagi | \\3,600 |\n",
          "|2|Algebra|S. Lang|$50|\n"
        ]
  tbl = get_tbl buf
  tbl_expected = [["", "book", "author", "price"],
                  ["1", "Calculus", "T. Takagi", "\\3,600"],
                  ["2", "Algebra", "S. Lang", "$50"]
                 ]
  raise "test_get_tbl: Error" unless tbl == tbl_expected
end

def test_get_width
  tbl = [["", "book", "author", "price"],
         ["1", "Calculus", "T. Takagi", "\\3,600"],
         ["2", "Algebra", "S. Lang", "$50"]
        ]
  sep = [:c, :c, :c, :c]
  width = get_width tbl, sep
  width_expected = [3, 8, 9, 6]
  raise "test_get_width: Error" unless width == width_expected
end

def test_mk_sep_l
  sep = [:l, :c, :r, :c]
  width = [3, 8, 9, 6]
  sep_l = mk_sep_l sep, width
  sep_l_expected = "|:--|:------:|--------:|:----:|"
  raise "test_mk_sep_l: Error" unless sep_l == sep_l_expected
end

def test_mk_tbl_l
  sep = [:l, :c, :r, :c]
  width = [3, 8, 9, 6]
  tbl = [["", "book", "author", "price"],
       ["1", "Calculus", "T. Takagi", "\\3,600"],
       ["2", "Algebra", "S. Lang", "$50"]
      ]
  tbl_l = mk_tbl_l tbl, sep, width
  tbl_l_expected = ["|   |  book  |   author|price |",
                    "|1  |Calculus|T. Takagi|\\3,600|",
                    "|2  |Algebra |  S. Lang| $50  |"
                   ]
  raise "test_mk_tbl_l: Error" unless tbl_l == tbl_l_expected
end

def test_mktbl
  buf = <<'EOS'.split("\n")
| |token kind | yylval.ID | yylval.NUM |
|:-:|:-|-:|:-:|
|1|ID|distance| |
|2|=|||
|3|NUM||100|
|4|FD|||
|5|ID|distance||
|6|*|||
|7|NUM||2|
EOS

  tbl_l = mktbl buf
  
  tbl_l_expected = <<'EOS'.split("\n")
|   |token kind|yylval.ID|yylval.NUM|
|:-:|:---------|--------:|:--------:|
| 1 |ID        | distance|          |
| 2 |=         |         |          |
| 3 |NUM       |         |   100    |
| 4 |FD        |         |          |
| 5 |ID        | distance|          |
| 6 |*         |         |          |
| 7 |NUM       |         |    2     |
EOS
  raise "test_mktbl: Error" unless tbl_l == tbl_l_expected
end

def test
  test_sep
  test_tbl
  test_get_tbl
  test_get_width
  test_mk_sep_l
  test_mk_tbl_l
  test_mktbl
end

