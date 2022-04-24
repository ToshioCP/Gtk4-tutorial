# test_lib_sec_file.rb
require 'minitest/autorun'
require 'fileutils'
require_relative "../lib/lib_renumber.rb"


class Test_lib_renumber < Minitest::Test
  include FileUtils
  def test_renumber
    table = [
      ["sec4.src.md", "sample5\n", "sec5.src.md"],
      ["sec0.5.src.md", "sample2\n", "sec2.src.md"],
      ["sec1.5.src.md", "sample4\n", "sec4.src.md"],
      ["sec0.src.md", "sample1\n", "sec1.src.md"],
      ["sec5.5.src.md", "sample6\n", "sec6.src.md"],
      ["sec1.src.md", "sample3\n", "sec3.src.md"]
    ]
    temp_dir = get_temp_name()
    Dir.mkdir temp_dir unless Dir.exist? temp_dir
    table = table.map{|t| ["#{temp_dir}/#{t[0]}", t[1], "#{temp_dir}/#{t[2]}"]}
    table.each{|t| File.write(t[0], t[1])}
    files = table.map{|t| t[0]}
    renumber files
    table.each do |t|
      t << File.read(t[2])
    end
    remove_entry_secure(temp_dir)
    table.each do |t|
      assert_equal t[1], t[3]
    end
  end
  def get_temp_name
    "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
  end
end
