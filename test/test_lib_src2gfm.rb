# = TestLibSrc2Md
#
# Unit tests for G4T.src2gfm.
# This test verifies the conversion logic, navigation generation,
# and directory structure preservation.

require 'minitest/autorun'
require 'minitest/mock'
require 'fileutils'
require 'tmpdir'
require 'pathname'
require_relative '../lib/lib_src2gfm'
require_relative '../path_manager'

class TestLibSrc2Md < Minitest::Test
  def setup
    # Create a temporary directory structure for testing
    @root = Dir.mktmpdir("g4t_root")
    @src_dir = File.join(@root, "src")
    @gfm_dir = File.join(@root, "gfm")
    Dir.mkdir(@src_dir)
    Dir.mkdir(@gfm_dir)

    # Setup sample files
    setup_test_files

    # Prepare stubs
    @original_get_path_method = PathManager.method(:get_path)
  end

  def teardown
    FileUtils.remove_entry @root
  end

  def setup_test_files
    # 1. index.src.md
    File.write(File.join(@src_dir, "index.src.md"), "# Project Index\nWelcome.")

    # 2. Three section files
    File.write(File.join(@src_dir, "sec1.src.md"), "# Section 1\nContent 1")
    File.write(File.join(@src_dir, "sec2.src.md"), "# Section 2\nContent 2")
    File.write(File.join(@src_dir, "sec3.src.md"), "# Section 3\nContent 3")

    # 3. Other .src.md and asset files
    File.write(File.join(@src_dir, "extra.src.md"), "# Extra\nExtra content")
    FileUtils.touch(File.join(@src_dir, "diagram.png"))

    # 4. Subdirectory with a file
    sub_dir = File.join(@src_dir, "sub")
    Dir.mkdir(sub_dir)
    File.write(File.join(sub_dir, "nested.src.md"), "# Nested\nNested content")
  end

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {
      root: @root,
      src: @src_dir,
      gfm: @gfm_dir
    }
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

  def test_src2gfm_full_process
    # Execute the conversion
    # We use stub again to ensure the code inside src2gfm sees the test paths
    with_stubbed_paths do
      G4T.src2gfm
    end

    # Verification: README.md in root
    readme_path = File.join(@root, "README.md")
    assert File.exist?(readme_path), "README.md should be created in root"
    readme_content = File.read(readme_path)
    assert_match(/## Table of contents/, readme_content)
    assert_match(/\[Section 1\]\(gfm\/sec1\.md\)/, readme_content)
    assert_match(/\[Section 2\]\(gfm\/sec2\.md\)/, readme_content)
    assert_match(/\[Section 3\]\(gfm\/sec3\.md\)/, readme_content)

    # Verification: Navigation in sec2.md (Middle section)
    sec2_path = File.join(@gfm_dir, "sec2.md")
    assert File.exist?(sec2_path)
    sec2_content = File.read(sec2_path)
    assert_match(/Up: \[README.md\]\(\.\.\/README\.md\),  Prev: \[Section 1\]\(sec1\.md\), Next: \[Section 3\]\(sec3\.md\)/, sec2_content)

    # Verification: Navigation in sec1.md (First section)
    sec1_content = File.read(File.join(@gfm_dir, "sec1.md"))
    assert_match(/Next: \[Section 2\]\(sec2\.md\)/, sec1_content)
    refute_match(/Prev:/, sec1_content)

    # Verification: Navigation in sec3.md (Last section)
    sec3_content = File.read(File.join(@gfm_dir, "sec3.md"))
    assert_match(/Prev: \[Section 2\]\(sec2\.md\)/, sec3_content)
    refute_match(/Next:/, sec3_content)

    # Verification: Non-section markdown
    assert File.exist?(File.join(@gfm_dir, "extra.md"))
    
    # Verification: Asset file copy (or "conversion" to .md if you treat all as .src.md)
    # Note: In your current script, if it's not .src.md, it's still "read" and saved.
    assert File.exist?(File.join(@gfm_dir, "diagram.png"))

    # Verification: Subdirectory structure preservation
    assert File.exist?(File.join(@gfm_dir, "sub", "nested.md"))
  end

  def test_src2gfm_raises_error_if_no_index
    File.delete(File.join(@src_dir, "index.src.md"))
    
    with_stubbed_paths do
      assert_raises(RuntimeError) { G4T.src2gfm }
    end
  end
end