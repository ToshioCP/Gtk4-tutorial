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
    # Create a dummy file for testing commands
    File.write(File.join(@src_dir, "hello.txt"), "Hello Shell")
    # Prepare stubs
    original_get_path_method = PathManager.method(:get_path)
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
      directive = "@@@shell\ncat hello.txt\n@@@"
      output = G4T.include_shell(directive)

      # Check if the command prompt is included
      assert_equal "```\n$ cat hello.txt\nHello Shell\n$\n```\n", output
    end
  end

  # Verifies that standard error is also captured.
  def test_include_shell_error_capture
    with_stubbed_paths do
      directive = "@@@shell\nls non_existent_file\n@@@"
      output = G4T.include_shell(directive)

      # Check for typical error messages from the shell (stderr)
      assert_match(/No such file or directory/, output)
    end
  end

  # --- 2. Validation Tests ---

  # Verifies that an invalid directive format returns an empty string.
  def test_invalid_shell_directive
    directive = "@@@shell\nonly_two_lines\n"
    output = G4T.include_shell(directive)

    assert_equal "", output
  end
end