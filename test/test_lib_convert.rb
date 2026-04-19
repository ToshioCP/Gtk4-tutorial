# = TestLibConvert
#
# Unit tests for G4T.convert.
# This test verifies the two-pass conversion logic:
# 1. @@@if...@@@end (Preprocessing)
# 2. @@@include/@@@shell (Resource Integration)

require 'minitest/autorun'
require 'minitest/mock'
require 'stringio'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_convert')

class TestLibConvert < Minitest::Test

  # Test Pass 1: Conditional Preprocessing
  def test_convert_pass1_preprocessing
    src = <<~SRC
      Start
      @@@if html
      HTML Only
      @@@else
      Other
      @@@end
      End
    SRC

    # Case: target is 'html'
    result_html = G4T.send(:convert, src, "html")
    assert_match(/Start\nHTML Only\nEnd/, result_html)
    refute_match(/Other/, result_html)

    # Case: target is 'gfm'
    result_gfm = G4T.send(:convert, src, "gfm")
    assert_match(/Start\nOther\nEnd/, result_gfm)
    refute_match(/HTML Only/, result_gfm)
  end

  # Test Pass 2: @@@include and @@@shell directives
  def test_convert_pass2_integration
    src = <<~SRC
      Before Include
      @@@include
      sample.c
      @@@
      After Include
      @@@shell
      date
      @@@
    SRC

    # Stub the inclusion methods to return predictable strings
    G4T.stub :include_source, "[SOURCE_CONTENT]\n" do
      G4T.stub :include_shell, "[SHELL_OUTPUT]\n" do
        result = G4T.send(:convert, src, "html")
        
        assert_match(/Before Include\n\[SOURCE_CONTENT\]\nAfter Include/, result)
        assert_match(/\[SHELL_OUTPUT\]/, result)
      end
    end
  end

  # Test two-pass combination
  # Ensures @@@include inside a matched @@@if is processed,
  # while @@@include inside an unmatched @@@if is ignored.
  def test_convert_combined_passes
    src = <<~SRC
      @@@if html
      @@@include
      web.html
      @@@
      @@@else
      @@@include
      print.tex
      @@@
      @@@end
    SRC

    # Track which file is requested
    G4T.stub :include_source, ->(line, target) { "CONTENT_OF_#{line.split("\n")[1]}\n" } do
      result = G4T.send(:convert, src, "html")
      assert_equal "CONTENT_OF_web.html\n", result
      refute_match(/print\.tex/, result)
    end
  end

  # Test the robustness of Pass 2's three-line directive parsing
  def test_convert_pass2_robustness_with_missing_end_mark
    # Missing @@@ mark for the second directive
    src = <<~SRC
      @@@include
      file1.c
      @@@
      @@@shell
      ls
      Not an end mark
    SRC

    # @@@shell block includes the last line because of no end mark '@@@'.
    # So the last line will disapper.
    G4T.stub :include_source, "FILE1\n" do
      G4T.stub :include_shell, "SHELL1\n" do
        result = G4T.send(:convert, src, "html")
        assert_match(/FILE1/, result)
        # The shell logic should still fire or be handled based on s3 rollback
        assert_match(/SHELL1/, result)
      end
    end
  end

  
  #
  # split_directive_blocks
  #

  def test_split_directive_blocks_plain_text_only
    input = "hello\nworld\n"
    result = G4T.send(:split_directive_blocks, input)

    assert_equal ["hello\nworld\n"], result
  end

  def test_split_directive_blocks_with_if_block
    input = <<~TEXT
      before
      @@@if condition
      inside 1
      @@@elif condition
      inside 2
      @@@else
      inside 3
      @@@end
      after
    TEXT

    result = G4T.send(:split_directive_blocks, input)
    assert_equal 3, result.size
    assert_match(/^before/, result[0])
    assert_match(/^@@@if/, result[1])
    assert_match(/inside\s3/, result[1])
    assert_match(/^after/, result[2])
  end

  def test_split_directive_blocks_with_include_block
    input = <<~TEXT
      @@@shell
      ls
      @@@
      @@@include
      file.c
      @@@
      after
    TEXT

    result = G4T.send(:split_directive_blocks, input)

    assert_equal 3, result.size
    assert_match(/^@@@shell/, result[0])
    assert_match(/^file\.c/, result[1])
    assert_match(/^after/, result[2])
  end
end