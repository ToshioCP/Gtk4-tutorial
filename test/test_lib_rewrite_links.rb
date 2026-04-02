require 'minitest/autorun'
require 'pathname'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_rewrite_links')

class TestLibRewriteLinks < Minitest::Test
  def setup
    @src_path = PathManager.get_path(:src)
    @index_path = File.join(@src_path, "index.src.md")
    @sec_path = File.join(@src_path, "sec1.src.md")
  end

  def test_verbatim
    input = <<~EOS
    ````
    ![image](/images/a.png){width=10cm height=5cm}
    ````

    ![image](/images/b.png){width=10cm height=5cm}

    ~~~~~
    ~~~
    ![image](/images/c.png){width=10cm height=5cm}
    ~~~
    ~~~~~
    
        ![image](/images/d.png){width=10cm height=5cm}

    `verbatim code`
    `` `inline code` ``
    EOS

    output = G4T.send(:_split_verbatim_blocks, input)
    assert_equal :fenced, output[0][0]
    assert_equal "````\n![image](/images/a.png){width=10cm height=5cm}\n````\n", output[0][1]
    assert_equal :blank, output[1][0]
    assert_equal "\n", output[1][1]
    assert_equal :other, output[2][0]
    assert_equal "![image](/images/b.png){width=10cm height=5cm}\n", output[2][1]
    assert_equal :blank, output[3][0]
    assert_equal "\n", output[3][1]
    assert_equal :fenced, output[4][0]
    assert_equal "~~~~~\n~~~\n![image](/images/c.png){width=10cm height=5cm}\n~~~\n~~~~~\n", output[4][1]
    assert_equal :blank, output[5][0]
    assert_equal "\n", output[5][1]
    assert_equal :indented, output[6][0]
    assert_equal "    ![image](/images/d.png){width=10cm height=5cm}\n", output[6][1] 
    assert_equal :blank, output[7][0]
    assert_equal "\n", output[7][1]
    assert_equal :other, output[8][0]
    assert_equal "`verbatim code`\n`` `inline code` ``\n", output[8][1]
  end

  def test_image_attribute_removal
    # Size attribute is removed when the type is  GFM
    # Size attribute remains when the type is HTML or PDF
    # Root-relative URL
    input = '![img](/images/a.png){width=50%}'
    assert_equal '![img](/src/images/a.png)', G4T.send(:rewrite_links, input, 'gfm', @sec_path) # Images stay in src directory. URL root is the repository root.
    assert_equal '![img](/images/a.png){width=50%}', G4T.send(:rewrite_links, input, 'html', @sec_path) # Images are copied to docs directory (URL root).
    input = '![img](/images/a.png){width=10cm}'
    assert_equal '![img](/images/a.png){width=10cm}', G4T.send(:rewrite_links, input, 'pdf', @sec_path)

    # Rekative URL
    input = '![img](a.png){width=50%}'
    assert_equal '![img](../src/a.png)', G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_equal '![img](a.png){width=50%}', G4T.send(:rewrite_links, input, 'html', @sec_path)
    input = '![img](images/a.png){width=10cm}'
    assert_match /!\[img\]\(.*\/src\/images\/a.png\){width=10cm}/, G4T.send(:rewrite_links, input, 'pdf', @sec_path)
  end

  def test_root_relative_links_conversion
    input = "![img-2](/images/b.jpg)\n"
    ::ENV['G4T_ENV'] = "production"
    assert_match link2regexp('![img-2](/src/images/b.jpg)'), G4T.send(:rewrite_links, input, 'gfm', @index_path)
    assert_match link2regexp('![img-2](/Gtk4-tutorial/images/b.jpg)'), G4T.send(:rewrite_links, input, 'html', @index_path)
    ::ENV['G4T_ENV'] = "development"
    assert_match link2regexp('![img-2](/src/images/b.jpg)'), G4T.send(:rewrite_links, input, 'gfm', @index_path)
    assert_match link2regexp('![img-2](/images/b.jpg)'), G4T.send(:rewrite_links, input, 'html', @index_path)
  end

  def test_gfm_index_conversion 
    input = "[Section 1](sec1.src.md)\n![img](/images/img/a.png)\n"
    output = G4T.send(:rewrite_links, input, 'gfm', @index_path)
    
    assert_match link2regexp('[Section 1](gfm/sec1.md)'), output
    assert_match link2regexp('![img](/src/images/img/a.png)'), output
  end

  def test_gfm_section_conversion
    input = "[Home](index.src.md)\n[Next](sec2.src.md)\n"
    output = G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    
    assert_match %r{\[Home\]\(\.\./README\.md\)}, output
    assert_match %r{\[Next\]\(sec2\.md\)}, output
  end

  def test_html_conversion
    input = '[Section 1](sec1.src.md)\n![img](/images/img.png){width=50%}'
    output = G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('[Section 1](sec1.html)'), output
    assert_match link2regexp('[img](/images/img.png){width=50%}'), output
  end

  def test_pdf_conversion
    input = 'See [Section 1](sec1.src.md)\n![img](/images/img.png){width=10cm}'
    output = G4T.send(:rewrite_links, input, 'pdf', @sec_path)
    assert_match link2regexp('See [Section 1](sec1.src.md)'), output
    assert_match link2regexp('![img](/images/img.png){width=10cm}'), output
  end

  def test_source_code_conversion
    input = 'See [C program file](/src/misc/example.c)'
    output = G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_match link2regexp('/src/misc/example.c'), output
    input = 'See [C program file](misc/example.c)'
    output = G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('/src/misc/example.c'), output
  end

  private

  # Helper method
  def link2regexp(link)
    Regexp.new(Regexp.escape(link))
  end 
end