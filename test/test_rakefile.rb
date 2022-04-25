# test_lib_sec_file.rb
require 'minitest/autorun'
require 'fileutils'
# require_relative "../Rakefile"


class Test_rakefile < Minitest::Test
  include FileUtils
  def dirname(file)
    File.dirname(file)
  end
  def test_rakefile
    rakefile = File.read("../Rakefile")
    c_files = rakefile.match(/^def c_files.*?^end\n/m)[0]
    eval c_files
    src = <<~'EOS'
    ## meson.build

    @@@include
    tfe5/meson.build
    @@@

    ## tfe.gresource.xml

    @@@include
    tfe5/tfe.gresource.xml
    @@@

    ## tfe.ui

    @@@include
    tfe5/tfe.ui
    @@@

    ## tfe.h

    @@@include
    tfe5/tfe.h
    @@@

    ## tfeapplication.c

    @@@include
    tfe5/tfeapplication.c app_startup app_open
    @@@
    EOS
    temp_dir = get_temp_name()
    Dir.mkdir temp_dir unless Dir.exist? temp_dir
    path = "#{temp_dir}/secXX.src.md"
    File.write(path, src)
    expected = ["tfe5/meson.build", "tfe5/tfe.gresource.xml", "tfe5/tfe.ui", "tfe5/tfe.h", "tfe5/tfeapplication.c"]\
               .map{|f| "#{temp_dir}/#{f}"}
    actual = c_files(path)
    remove_entry_secure(temp_dir)
    assert_equal expected.sort, actual.sort
  end
  def get_temp_name
    "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
  end
end
