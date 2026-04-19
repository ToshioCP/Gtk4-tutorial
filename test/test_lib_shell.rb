# = TestShellInclude
#
# This suite validates the G4T.include_shell method.
# It ensures that shell commands are executed in the correct directory,
# and their outputs (including stderr) are formatted as plain code blocks.

require 'minitest/autorun'
require 'minitest/mock'
require 'fileutils'
require 'tmpdir'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_shell')

class TestShellInclude < Minitest::Test
  # Sets up a temporary directory for command execution tests.
  def setup
    @src_dir = Dir.mktmpdir
    @original_get_path_method = PathManager.method(:get_path)
  end

  # Removes the temporary directory after tests.
  def teardown
    FileUtils.rm_rf(@src_dir)
  end

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {src:  @src_dir}
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

  # --- 1. Execution and Content Tests ---

  # Verifies that the command is executed and its output is captured with a prompt.
  def test_include_shell_execution_output
    with_stubbed_paths do
      # Create a dummy file for testing commands
      File.write(File.join(@src_dir, "hello.txt"), "Hello Shell")
      directive = "@@@shell\ncat hello.txt\n@@@"
      assert_equal "```\n$ cat hello.txt\nHello Shell\n$\n```\n", G4T.send(:include_shell, directive)
    end
  end

  # Syntax error in the @@@shell command
  def test_invalid_shell_directive
    with_stubbed_paths do
      directive = "@@@shell\nonly_two_lines\n"
      assert_raise(RuntimeError) do
        G4T.send(:include_shell, directive)
      end
    end
  end

  # Bad path error
  def test_invalid_shell_directive
    with_stubbed_paths do
      directive = "@@@shell\nonly_two_lines\n"
      assert_raises(RuntimeError) do
        G4T.send(:include_shell, directive)
      end
    end
  end

  #Error: Access to the  outside of 'src'
  def test_include_shell_path_error
    with_stubbed_paths do
      directive = "@@@shell\nls ../gfm\n@@@"
      assert_raises(RuntimeError) do
        G4T.send(:include_shell, directive)
      end
    end
  end


end