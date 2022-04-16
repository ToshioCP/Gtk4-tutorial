# test_lib_sec_file.rb
require 'minitest/autorun'
require 'fileutils'
require_relative "../lib/lib_src_file.rb"

module Prepare_test
  def files_srcfile
    src_text = <<~'EOS'
    This is a source file.
    EOS

    sample_c = <<~'EOS'
    #include <stdio.h>

    int
    main (int argc, char **argv) {
      printf ("Hello world.\n");
    }
    EOS

    sec1_text = <<~'EOS'
    This is a test file.
    The sorce of `sample.c` is:

    @@@include
    sample.c main
    @@@

    It is the simplest C program.
    EOS

    sec2_text = <<~'EOS'
    To compile the C source file `sample.c`, type:

    ~~~
    $ gcc sample.c
    ~~~

    Then executable file `a.out` is generated.

    @@@shell
    ls
    @@@

    To execute it, type:

    ~~~
    $ ./a.out
    ~~~

    The source code is in [Section 1](sec1.src.md)
    EOS

    sec05_text = <<~'EOS'
    Prerequisite

    - Linux OS like Ubuntu or Debian.
    - gcc
    EOS

    [
      ["srcfile.src.md", src_text],
      ["sample.c", sample_c],
      ["sec1.src.md", sec1_text],
      ["sec2.src.md", sec2_text],
      ["sec0.5.src.md", sec05_text]
    ]
  end
end

class Test_lib_src_file < Minitest::Test
  include FileUtils
  include Prepare_test
  def setup
    # Create sample file
    @temp = get_temp_name()
    Dir.mkdir @temp unless Dir.exist? @temp
    files_srcfile().each do |f|
      File.write "#{@temp}/#{f[0]}", f[1]
    end
  end
  def teardown
    remove_entry_secure(@temp)
  end
  def test_src_file
    src = Src_file.new "#{@temp}/srcfile.src.md"
    test_items = [
      ["path", "#{@temp}/srcfile.src.md"],
      ["basename", "srcfile.src.md"],
      ["dirname", "#{@temp}"],
      ["to_md", "srcfile.md"],
      ["to_html", "srcfile.html"],
      ["to_tex", "srcfile.tex"],
    ]
    test_items.each do |item|
      assert_equal item[1], src.public_send(item[0].to_sym)
    end
  end
  def test_sec_file
    src_sec05 = Sec_file.new "#{@temp}/sec0.5.src.md"
    src_sec1 = Sec_file.new "#{@temp}/sec1.src.md"
    src_sec2 = Sec_file.new "#{@temp}/sec2.src.md"
    test_items = [
      ["path", "#{@temp}/sec1.src.md"],
      ["basename", "sec1.src.md"],
      ["dirname", "#{@temp}"],
      ["c_files", [ "#{@temp}/sample.c" ]],
      ["to_md", "sec1.md"],
      ["to_html", "sec1.html"],
      ["to_tex", "sec1.tex"],
      ["num", "1"],
      ["to_f", 1.0],
      ["<=>", src_sec05, 1],
      ["<=>", src_sec1, 0],
      ["<=>", src_sec2, -1],
      ["is_i?", true]
    ]
    test_items.each do |item|
      if item.size == 2
        assert_equal item[1], src_sec1.public_send(item[0].to_sym)
      else
        assert_equal item[2], src_sec1.public_send(item[0].to_sym, item[1])
      end
    end
    refute src_sec05.is_i?
  end
  def test_sec_files
    temp_renum = "temp_renum"+get_temp_name()
    unless Dir.exist? temp_renum
      Dir.mkdir temp_renum
    end
    files_srcfile().each do |f|
      File.write "#{temp_renum}/#{f[0]}", f[1]
    end
    src_sec05 = Sec_file.new "#{temp_renum}/sec0.5.src.md"
    src_sec1 = Sec_file.new "#{temp_renum}/sec1.src.md"
    src_sec2 = Sec_file.new "#{temp_renum}/sec2.src.md"
    sec_files = Sec_files.new [src_sec05, src_sec1, src_sec2]
    sec_files.renum!
    line_actual = File.read(src_sec2).match(/The source code is in \[Section \d\]\(sec\d.src.md\)\n/).to_a[0]
    remove_entry_secure(temp_renum)
    assert_equal ["#{temp_renum}/sec1.src.md", "#{temp_renum}/sec2.src.md", "#{temp_renum}/sec3.src.md"], sec_files
    assert_equal "The source code is in [Section 2](sec2.src.md)\n", line_actual
  end
private
  def get_temp_name
    "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
  end
end
