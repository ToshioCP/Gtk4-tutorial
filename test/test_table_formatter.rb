# = TestTableFormatter (CLI Integration)
#
# This suite validates the command-line interface (CLI) of bin/table_formatter.rb.
# It focuses on argument handling, file I/O, and correct exit statuses using
# PathManager to resolve executable paths and tmpdir for a safe environment.

require 'minitest/autorun'
require 'fileutils'
require 'tmpdir'
require_relative '../path_manager'

class TestTableFormatter < Minitest::Test
  def setup
    @bin_path = PathManager.get_path(:bin, 'table_formatter.rb')
  end

  # --- 1. Argument & Error Handling Tests ---

  def test_no_arguments_error
    output = `ruby #{@bin_path} 2>&1`
    refute $?.success?, "Script should fail without arguments"
    assert_match(/Usage:/, output)
  end

  def test_file_not_found_error
    Dir.mktmpdir do |dir|
      missing_file = File.join(dir, "non_existent.md")
      output = `ruby #{@bin_path} #{missing_file} 2>&1`
      
      refute $?.success?, "Script should fail when file does not exist"
      assert_match(/Error: File not found/, output)
    end
  end

  # --- 2. Functional & Success Case Tests ---

  def test_successful_in_place_formatting
    Dir.mktmpdir do |dir|
      file_path_1 = File.join(dir, "sample1.src.md")
      file_path_2 = File.join(dir, "sample2.src.md")
      
      # --- ファイル1: @@@table タグあり (整形されるべき) ---
      original_content_1 = <<~EOS
        @@@table
        |afternoon|break|
        |:-|-:|
        |1|2|
        @@@end
      EOS
      # 注意: format_table は最後に改行を返すため、期待値も改行で終わらせます
      expected_1 = <<~EOS
        |afternoon|break|
        |:--------|----:|
        |1        |    2|
      EOS
      File.write(file_path_1, original_content_1) # 変数名を修正

      # --- ファイル2: @@@table タグなし (整形されないべき) ---
      original_content_2 = <<~EOS
        |afternoon|break|
        |:-|-:|
        |1|2|
      EOS
      expected_2 = original_content_2 # そのまま残るはず
      File.write(file_path_2, original_content_2) # 変数名を修正

      # Execute script (複数ファイル指定)
      output = `ruby #{@bin_path} #{file_path_1} #{file_path_2} 2>&1`
      
      assert $?.success?, "Script should succeed with valid files"
      assert_match(/Formatted:/, output)

      # Verify contents
      actual_1 = File.read(file_path_1) # 変数名を修正
      assert_equal expected_1, actual_1
      
      actual_2 = File.read(file_path_2)
      assert_equal expected_2, actual_2
    end
  end
end