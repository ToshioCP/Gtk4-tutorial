require 'minitest/autorun'
require 'fileutils'
require 'diff/lcs'
require_relative '../lib/lib_change_relative_link.rb'

class Test_lib_change_relative_link < Minitest::Test
  include FileUtils

  def test_change_link
    src = <<~'EOS'
      For further information, see [Section 1](sec1.src.md).
      The file will be moved to [Section 1](../gfm/sec1.md)
      There's a [document](../doc/document.src.md).
      ![image](../image/image.png){width=9.0cm height=6.0cm}
      See [Github repository](https://github.com/ToshioCP).
    EOS
    expected = <<~'EOS'
      For further information, see [Section 1](../src/sec1.src.md).
      The file will be moved to [Section 1](sec1.md)
      There's a [document](../doc/document.src.md).
      ![image](../image/image.png){width=9.0cm height=6.0cm}
      See [Github repository](https://github.com/ToshioCP).
    EOS
    actual = change_relative_link(src, "src", "gfm")
    assert_equal expected, actual
  end
end
