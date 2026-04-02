# frozen_string_literal: true

# = Unit Tests for G4T::LibUtils
#
# == Overview
# This file contains the Minitest suite for the +G4T::LibUtils+ module.
# It focuses on validating internal helper methods for:
# * File sequence checking (sec1, sec2, etc.)
# * Directory and file discovery
# * Robust file writing with automatic directory creation
# * Text parsing and directive block splitting
#
# == Usage
# Run this test file directly from the terminal:
#   ruby test/lib_utils_test.rb

require 'minitest/autorun'
require 'tmpdir'
require 'fileutils'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')

module G4T
  # Test suite for LibUtils internal helper methods.
  # This class ensures that file handling, sequence validation, and 
  # text parsing logic work as expected.
  class LibUtilsTest < Minitest::Test

    #
    # sec_files_check
    #

    def test_sec_files_check_valid_sequence
      Dir.mktmpdir do |dir|
        File.write(File.join(dir, "sec1.src.md"), "")
        File.write(File.join(dir, "sec2.src.md"), "")
        File.write(File.join(dir, "sec3.src.md"), "")

        assert G4T.send(:sec_files_check, src_dir: dir)
      end
    end

    def testsec_files_check_missing_number
      Dir.mktmpdir do |dir|
        File.write(File.join(dir, "sec1.src.md"), "")
        File.write(File.join(dir, "sec3.src.md"), "")

        assert_raises(RuntimeError) do
          G4T.send(:sec_files_check, src_dir: dir)
        end
      end
    end

    def test_sec_files_check_decimal_number
      Dir.mktmpdir do |dir|
        File.write(File.join(dir, "sec1.src.md"), "")
        File.write(File.join(dir, "sec1.5.src.md"), "")

        assert_raises(RuntimeError) do
          G4T.send(:sec_files_check, src_dir: dir)
        end
      end
    end

    def test_sec_files_check_directory_not_found
      assert_raises(RuntimeError) do
        G4T.send(:sec_files_check, src_dir: "/no/such/dir")
      end
    end


    #
    # get_src_files
    #

    def test_get_src_files_returns_all_and_sections
      Dir.mktmpdir do |dir|
        File.write(File.join(dir, "sec2.src.md"), "")
        File.write(File.join(dir, "sec1.src.md"), "")
        File.write(File.join(dir, "other.txt"), "")

        [%w(sub nested.src.md), %w(images image.png)].each do |sub, file|
          subdir = File.join(dir, sub)
          Dir.mkdir(subdir)
          File.write(File.join(subdir, file), "")
        end

        result = G4T.send(:get_src_files, src_dir: dir)

        assert_includes result[:all], File.join(dir, "sec1.src.md")
        assert_includes result[:all], File.join(dir, "sub", "nested.src.md")
        assert_includes result[:all], File.join(dir, "images", "image.png")

        assert_equal [
          File.join(dir, "sec1.src.md"),
          File.join(dir, "sec2.src.md")
        ], result[:sections]

        assert_equal [File.join(dir, "images/image.png")], result[:images]
      end
    end


    #
    # write_with_directory
    #

    def test_write_with_directory_creates_parent_dirs
      Dir.mktmpdir do |dir|
        path = File.join(dir, "a/b/c.txt")

        bytes = G4T.send(:write_with_directory, path, "hello")

        assert_equal 5, bytes
        assert File.exist?(path)
        assert_equal "hello", File.read(path)
      end
    end

    def test_image?
      assert G4T.send(:image?, "image.JPEG")
      assert G4T.send(:image?, "image.JPG")
      assert G4T.send(:image?, "image.jpeg")
      assert G4T.send(:image?, "image.jpg")
      assert G4T.send(:image?, "image.PNG")
      assert G4T.send(:image?, "image.png")
      assert G4T.send(:image?, "image.GIF")
      assert G4T.send(:image?, "image.gif")
      refute G4T.send(:image?, "sec2.src.md")
    end

    def test_source_code?
      assert G4T.send(:source_code?, "sample.c")
      assert G4T.send(:source_code?, "sample.h")
      assert G4T.send(:source_code?, "sample.lex")
      assert G4T.send(:source_code?, "sample.ui")
      assert G4T.send(:source_code?, "sample.xml")
      assert G4T.send(:source_code?, "sample.y")
    end
  end
end