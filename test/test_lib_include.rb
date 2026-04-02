# = TestIncludeSource
#
# This suite validates the G4T.include_source method.
# It ensures that external source files are correctly imported, 
# C functions are accurately extracted, and the output format 
# respects the target type (gfm vs html/pdf).

require 'minitest/autorun'
require 'minitest/mock'
require 'fileutils'
require 'tmpdir'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_include')

class TestIncludeSource < Minitest::Test
  # Sets up a temporary directory with dummy C and Ruby source files.
  def setup
    @src_dir = Dir.mktmpdir
    # To prevent infinite recursion, we store the original method as a Method object.
    # The stubbed lambda will delegate non-src path requests to this original method.
    @original_get_path_method = PathManager.method(:get_path)
    
    # C source code for testing function extraction
    @c_file_content = <<~C
      #include <stdio.h>

      void
      hello() {
          printf("hello\\n");
      }

      int
      add(int a, int b) {
          return a + b;
      }

      void
      extra() {
          // not requested
      }
    C
    File.write(File.join(@src_dir, "test.c"), @c_file_content)

    # C source code in a subdirectory
    Dir.mkdir(File.join(@src_dir, "sub_directory"))
    File.write(File.join(@src_dir, "sub_directory", "test.c"), @c_file_content)

    # Ruby source code for testing standard includes
    @rb_file_content = "puts 'hello world'\n"
    File.write(File.join(@src_dir, "test.rb"), @rb_file_content)
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

  # --- 1. C Function Extraction Tests ---

  def test_extract_specific_c_functions
    with_stubbed_paths do
      # Verifies that only requested C functions are extracted.
      directive = "@@@include\ntest.c hello add\n@@@"
      output = G4T.send(:include_source, directive, 'html')
      assert_match(/``` {.c .numberLines}/, output)
      # Check if requested functions are included
      assert_match( /void/, output)
      assert_match( /hello\(\)/, output)
      assert_match( /int/, output)
      assert_match( /add\(int a, int b\)/, output)
      # Check if non-requested function is excluded
      refute_match( /extra\(\)/, output)

      # Verifies the C source code in the sub directory is extracted.
      directive = "@@@include\nsub_directory/test.c\n@@@"
      output = G4T.send(:include_source, directive, 'html')
      assert_match(/``` {.c .numberLines}/, output)
      # Check if all the functions are included
      assert_match( /hello\(\)/, output)
      assert_match( /add\(int a, int b\)/, output)
      assert_match( /extra\(\)/, output)
    end
  end

  # --- 2. Format Switching Tests (GFM vs Pandoc) ---

  # Verifies GFM output format (standard backticks, no line numbers).
  def test_format_output_for_gfm
    with_stubbed_paths do
      directive = "@@@include\ntest.rb\n@@@"
      output = G4T.send(:include_source, directive, 'gfm')

      # Check GFM style
      assert_match(/^```ruby/, output)
      refute_match(/\.numberLines/, output)
      assert_match(/puts 'hello world'/, output)
    end
  end

  # Verifies Pandoc output format (curly brace attributes and line numbers).
  def test_format_output_for_pandoc
    with_stubbed_paths do
      directive = "@@@include\ntest.rb\n@@@"
      output = G4T.send(:include_source, directive, 'pdf')

      # Check Pandoc style attributes
      assert_match(/``` {.ruby .numberLines}/, output)
    end
  end

  # --- 3. Error Handling Tests ---

  # Verifies that a missing file returns a single newline.
  def test_file_not_found_returns_newline
    with_stubbed_paths do
      directive = "@@@include\nmissing.c\n@@@"
      output = G4T.send(:include_source, directive, 'html')

      assert_equal "\n", output
    end
  end

  # Verifies that an invalid directive format returns an empty string.
  def test_invalid_directive_format
    directive = "@@@include\nonly_two_lines\n"
    output = G4T.send(:include_source, directive, 'html')

    assert_equal "", output
  end
end