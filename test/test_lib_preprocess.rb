# = TestLibPreprocess
#
# Unit tests for G4T.preprocess_block.
# This test verifies that the conditional branching (if, elif, else)
# correctly selects the appropriate content based on the target.

require 'minitest/autorun'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_preprocess')

class TestLibPreprocess < Minitest::Test
  # Test basic @@@if ... @@@end structure
  def test_basic_if_match
    block = <<~BLOCK
      @@@if html
      Inside HTML
      @@@end
    BLOCK
    assert_equal "Inside HTML\n", G4T.preprocess_block(block, "html")
    assert_equal "", G4T.preprocess_block(block, "gfm")
  end

  # Test if-else structure
  def test_if_else_structure
    block = <<~BLOCK
      @@@if html
      HTML Content
      @@@else
      Other Content
      @@@end
    BLOCK
    assert_equal "HTML Content\n", G4T.preprocess_block(block, "html")
    assert_equal "Other Content\n", G4T.preprocess_block(block, "gfm")
  end

  # Test if-elif-else structure
  def test_full_chain_structure
    block = <<~BLOCK
      @@@if html
      HTML
      @@@elif gfm
      GFM
      @@@elif pdf
      PDF
      @@@else
      ELSE
      @@@end
    BLOCK
    assert_equal "HTML\n", G4T.preprocess_block(block, "html")
    assert_equal "GFM\n", G4T.preprocess_block(block, "gfm")
    assert_equal "PDF\n", G4T.preprocess_block(block, "pdf")
    assert_equal "ELSE\n", G4T.preprocess_block(block, "latex")
  end

  # Test multi-target conditions using '|'
  def test_multi_target_condition
    block = <<~BLOCK
      @@@if html | gfm
      Web Content
      @@@else
      Print Content
      @@@end
    BLOCK
    assert_equal "Web Content\n", G4T.preprocess_block(block, "html")
    assert_equal "Web Content\n", G4T.preprocess_block(block, "gfm")
    assert_equal "Print Content\n", G4T.preprocess_block(block, "pdf")
  end

  # Test behavior when no match is found and no @@@else exists
  def test_no_match_without_else
    block = <<~BLOCK
      @@@if html
      Only HTML
      @@@end
    BLOCK
    assert_equal "", G4T.preprocess_block(block, "gfm")
  end

  # Test potential syntax error (starting without @@@if)
  def test_invalid_start_returns_empty
    block = "Just text without if"
    assert_equal "", G4T.preprocess_block(block, "html")
  end
end