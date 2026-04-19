# = TestLibTableFormatter
#
# This suite validates the G4T.format_table and G4T.process_tables methods.
# It ensures that Markdown tables are correctly beautified with proper
# column alignment and that original content is preserved when the
# input is not a valid table.

require 'minitest/autorun'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_table_formatter')

class TestLibfTableFormatter < Minitest::Test

  # Tests that columns are correctly aligned based on the maximum width
  # of all rows, including the separator row itself.
  def test_format_table_alignment
    input = <<~EOS
      |Header|Small|Very Long Column Header|
      |:---|:---:|---:|
      |a|b|c|
      |long value|x|y|
    EOS

    # The width of the second column should be at least 5 (length of ':---:')
    # The width of the third column should be at least 23 (length of 'Very Long Column Header')
    expected = <<~EOS
      |Header    |Small|Very Long Column Header|
      |:---------|:---:|----------------------:|
      |a         |  b  |                      c|
      |long value|  x  |                      y|
    EOS

    assert_equal expected, G4T.send(:format_table, input)
  end

  # Tests the case where the separator row is the longest element.
  # This ensures the new logic of including the separator in width calculations works.
  def test_width_determined_by_separator
    input = <<~EOS
      |A|B|
      |:-------:|---|
      |1|2|
    EOS

    expected = <<~EOS
      |    A    |B  |
      |:-------:|---|
      |    1    |2  |
    EOS

    assert_equal expected, G4T.send(:format_table, input)
  end

  # Tests that non-table content or invalid structures (like 4+ spaces)
  # are returned as-is.
  def test_validation_gatekeeper
    # Case 1: No separator row
    invalid_table = "|a|b|\n|c|d|\n"
    assert_equal invalid_table, G4T.send(:format_table, invalid_table)

    # Case 2: Indented with 4 spaces (Code Block)
    indented_table = "    |a|b|\n    |-|-|\n    |1|2|\n"
    assert_equal indented_table, G4T.send(:format_table, indented_table)
  end

  # Tests the @@@table directive processing.
  def test_process_tables_directive
    input = <<~EOS
      Introduction text.
      @@@table
      |ID|Name|
      |--|--|
      |1|Toshio|
      @@@end
      Conclusion.
    EOS

    output = G4T.process_tables(input)
    expected = <<~EOS
      Introduction text.
      |ID|Name  |
      |--|------|
      |1 |Toshio|
      Conclusion.
    EOS
    
    # Verify markers are removed and table is formatted
    assert_equal expected, output
  end

  # Tests that multiple @@@table blocks in one file are all processed.
  def test_multiple_directives_in_file
    input = <<~EOS
      @@@table
      |a|
      |-|
      |1|
      @@@end
      Between
      @@@table
      |b|
      |-|
      |2|
      @@@end
    EOS

    output = G4T.process_tables(input)
    expected = <<~EOS
      |a|
      |-|
      |1|
      Between
      |b|
      |-|
      |2|
    EOS

    assert_equal expected, output
  end
end