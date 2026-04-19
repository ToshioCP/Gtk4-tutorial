# = TestLibRenumber
#
# This test suite validates the +G4T.renumber+ function, ensuring both file system 
# integrity and content consistency. 
#
# == Test Coverage
# * **Filename Renaming**: Verifies that old decimal/irregular names are 
#   removed and replaced by sequential integer names.
# * **Content Integrity**: Ensures that all Markdown links within a file 
#   (multiple occurrences) are updated in a single pass to prevent 
#   double-replacement bugs.
# * **Full Match**: Validates the entire content of the processed files 
#   against expected strings to ensure no data loss or corruption.

require 'minitest/autorun'
require 'minitest/mock'
require 'fileutils'
require 'tmpdir'

require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_renumber')

class TestLibRenumber < Minitest::Test
  # Sets up a temporary directory to act as a mock source for each test.
  def setup
    @src_dir = Dir.mktmpdir("test_src_")
    @original_get_path_method = PathManager.method(:get_path)
  end

  # Removes the temporary directory after each test execution.
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

  # Validates the full renumbering system including file renaming and link updates.
  # It simulates a complex scenario with floats, gaps, and multiple cross-links 
  # per file to ensure the 'single-pass replacement' logic works correctly.
  def test_lib_renumber
    # --- 1. Preparation ---
    # Create files with 3 links each, including sequences that could trigger 
    # double-replacement bugs if not handled correctly (e.g., 1.5 -> 2, 2 -> 3).
    files_to_create = {
      "sec1.src.md"   => "[Section 1.5](sec1.5.src.md), [Section 2](sec2.src.md), [Section 10](sec10.src.md)",
      "sec1.2.src.md" => "Nothing to be changed.",
      "sec1.5.src.md" => "[Section 1](sec1.src.md), [Section 2](sec2.src.md), [Section 2](sec2.src.md)",
      "sec2.src.md"   => "[Section 1](sec1.src.md), [Section 1.5](sec1.5.src.md), [Section 10](sec10.src.md)",
      "sec10.src.md"  => "[Section 1](sec1.src.md), [Section 1.5](sec1.5.src.md), [Section 2](sec2.src.md)"
    }
    files_to_create.each { |name, content| create_file(name, content) }

    # --- 2. Execution ---
    # Stub PathManager to point :src to our temporary test directory.
    # Expected mapping:
    # sec1   (1)   -> sec1.src.md
    # sec1.2 (1.5) -> sec2.src.md
    # sec1.5 (1.5) -> sec3.src.md
    # sec2   (2)   -> sec4.src.md
    # sec10  (10)  -> sec5.src.md
    with_stubbed_paths do
      G4T.renumber(verbose: false)
    end

    # --- 3. Filename Verification ---
    # Verify that the directory contains exactly the expected sequential filenames.
    expected_files = ["sec1.src.md", "sec2.src.md", "sec3.src.md", "sec4.src.md", "sec5.src.md"].sort
    actual_files = Dir.children(@src_dir).sort

    assert_equal expected_files, actual_files

    # --- 4. Content Verification ---
    # Check if each file's content matches the expected re-indexed links exactly.
    expected_contents = {
      "sec1.src.md" => "[Section 3](sec3.src.md), [Section 4](sec4.src.md), [Section 5](sec5.src.md)",
      "sec2.src.md" => "Nothing to be changed.",
      "sec3.src.md" => "[Section 1](sec1.src.md), [Section 4](sec4.src.md), [Section 4](sec4.src.md)",
      "sec4.src.md" => "[Section 1](sec1.src.md), [Section 3](sec3.src.md), [Section 5](sec5.src.md)",
      "sec5.src.md" => "[Section 1](sec1.src.md), [Section 3](sec3.src.md), [Section 4](sec4.src.md)"
    }

    expected_contents.each do |filename, expected_text|
      actual_content = File.read(File.join(@src_dir, filename))
      assert_equal expected_text, actual_content
    end
  end

  private

  # Helper method to write content to a file in the mock source directory.
  # @param name [String] The filename.
  # @param content [String] The content to write.
  def create_file(name, content)
    File.write(File.join(@src_dir, name), content)
  end
end