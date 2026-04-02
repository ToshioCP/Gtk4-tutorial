# = TestPathManager
#
# This suite validates the PathMangager.get_path method.
# It ensures that the absolute paths are correctly created.
# We intentionally hardcode "/" as the separator to avoid
# reproducing the same logic as the implementation.
#
# ** This test doesn't work on Windows. **

require 'minitest/autorun'
require_relative '../path_manager'

class TestPathManager < Minitest::Test
  def setup
    @test_dir = __dir__
    @root_dir = File.expand_path("..", @test_dir)
  end

  def teardown
  end

  def test_get_path
    dir_names = {
    root: '.',
    bin:  'bin',
    data: 'data',
    docs: 'docs',
    gfm:  'gfm',
    lib:  'lib',
    pdf: 'pdf',
    src:  'src',
    test: 'test'
    }
    dir_names.each do |name, relative_path|
      if relative_path == "."
        absolute_path = @root_dir
      else
        absolute_path = @root_dir + "/" + relative_path
      end
      assert_equal absolute_path, PathManager.get_path(name)
    end
    lib_script = 'lib_example.rb'
    assert_equal @root_dir + "/lib/#{lib_script}", PathManager.get_path(:lib, lib_script)
  end

  def test_check_path_traversal
    assert_raises RuntimeError do
      PathManager.get_path(:root, "..")
    end
    assert_raises RuntimeError do
      PathManager.get_path(:lib, "/etc/passwd")
    end
  end
end