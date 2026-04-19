require 'minitest/autorun'
require 'minitest/mock'
require 'tmpdir'
require 'fileutils'
require 'pathname'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_rewrite_links')

class TestLibRewriteLinks < Minitest::Test
  def setup
    @src_path = PathManager.get_path(:src)
    @index_path = File.join(@src_path, "index.src.md")
    @sec_path = File.join(@src_path, "sec1.src.md")
    @other_path = File.join(@src_path, "sub/other.sec.md")
  end

  def test_verbatim_blocks
    str = <<~EOS
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

    output = G4T.send(:rewrite_links,str, "gfm", File.join(@src_path, "sec1.src.md"))
    assert output.include?("````\n![image](/images/a.png){width=10cm height=5cm}\n````\n")
    assert output.include?("![image](/src/images/b.png)\n")
    assert output.include?("~~~~~\n~~~\n![image](/images/c.png){width=10cm height=5cm}\n~~~\n~~~~~\n")
    assert output.include?("    ![image](/images/d.png){width=10cm height=5cm}\n") 
    assert output.include?("`verbatim code`\n`` `inline code` ``\n")
  end

  def test_conversion_and_inline_code
    assert_equal '![`img`](/src/images/a.png)', G4T.send(:rewrite_links, '![`img`](/images/a.png)', 'gfm', @sec_path)
    assert_equal '![img](`/images/a.png`)', G4T.send(:rewrite_links, '![img](`/images/a.png`)', 'gfm', @sec_path)
    assert_equal '`![img](/src/images/a.png)', G4T.send(:rewrite_links, '`![img](/images/a.png)', 'gfm', @sec_path) # incompete inline code
  end

  def test_absolute_URL_conversion
    assert_equal '[Absolute URL](https://example.com/a/b/c.html)', G4T.send(:rewrite_links, '[Absolute URL](https://example.com/a/b/c.html)', 'gfm', @sec_path)
    assert_equal '[Absolute URL](https://example.com/a/b/c.html)', G4T.send(:rewrite_links, '[Absolute URL](https://example.com/a/b/c.html)', 'html', @sec_path)
    assert_equal '[Absolute URL](https://example.com/a/b/c.html)', G4T.send(:rewrite_links, '[Absolute URL](https://example.com/a/b/c.html)', 'pdf', @sec_path)
  end

  def test_fragment_URL_conversion
    input = "See [Headers and Links](#headers-and-links)\n"
    assert_equal "See [Headers and Links](#headers-and-links)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_equal "See [Headers and Links](#headers-and-links)\n", G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_equal "See [Headers and Links](#headers-and-links)\n", G4T.send(:rewrite_links, input, 'pdf', @sec_path)
  end

  def test_URL_within_src_directory
    input = "[Out of src](../../../../etc/passwd)\n"
    assert_raises RuntimeError do
      G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    end
  end

  def test_image_url_conversion
    # Size attribute is removed when the type is  GFM
    # Size attribute remains when the type is HTML or PDF
    # Root-relative URL
    input = '![img](/images/a.png){width=50%}'
    assert_equal '![img](/src/images/a.png)', G4T.send(:rewrite_links, input, 'gfm', @sec_path) # Images stay in src directory. URL root is the repository root.
    assert_equal '![img](/images/a.png){width=50%}', G4T.send(:rewrite_links, input, 'html', @sec_path) # Images are copied to docs directory (URL root).
    input = '![img](/images/a.png){width=10cm}'
    # The URL is converted to an absolute file path.
    assert_match %r|!\[img\]\(/.*/src/images/a\.png\)\{width=10cm\}|, G4T.send(:rewrite_links, input, 'pdf', @sec_path)

    # Rekative URL
    input = '![img](images/a.png){width=50%}'
    assert_equal '![img](../src/images/a.png)', G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_equal '![img](images/a.png){width=50%}', G4T.send(:rewrite_links, input, 'html', @sec_path)
    input = '![img](images/a.png){width=10cm}'
    # The URL is converted to an absolute file path.
    assert_match %r|!\[img\]\(.*/src/images/a.png\)\{width=10cm\}|, G4T.send(:rewrite_links, input, 'pdf', @sec_path)
  end

  def test_html_conversion_when_production_and_development
    input = "![img-2](/images/b.jpg)\n"
    ::ENV['G4T_ENV'] = "production"
    assert_match link2regexp('![img-2](/Gtk4-tutorial/images/b.jpg)'), G4T.send(:rewrite_links, input, 'html', @index_path)
    ::ENV['G4T_ENV'] = "development"
    assert_match link2regexp('![img-2](/images/b.jpg)'), G4T.send(:rewrite_links, input, 'html', @index_path)
  end

  # Root-relative and relative conversion
  def test_gfm_link_conversion 
    # Root-relative
    input = "[Home](/index.src.md)\n[Section 2](/sec2.src.md)\n"
    assert_equal("[Home](/README.md)\n[Section 2](/gfm/sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path))
    assert_equal("[Home](/README.md)\n[Section 2](/gfm/sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path))
    assert_equal("[Home](/README.md)\n[Section 2](/gfm/sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path))

    input = "[Other](/sub/other.src.md)\n"
    assert_equal("[Other](/gfm/sub/other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path))
    assert_equal("[Other](/gfm/sub/other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path))
    assert_equal("[Other](/gfm/sub/other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path))

    input = "[C file](/misc/pr1.c)\n[directory](/misc/)\n"
    assert_equal "[C file](/src/misc/pr1.c)\n[directory](/src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path)
    assert_equal "[C file](/src/misc/pr1.c)\n[directory](/src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_equal "[C file](/src/misc/pr1.c)\n[directory](/src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path)

    # Relative
    input = "[Home](index.src.md)\n[Section 2](sec2.src.md)\n"
    assert_equal("[Home](#)\n[Section 2](gfm/sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path))
    assert_equal("[Home](../README.md)\n[Section 2](sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path))
    input = "[Home](../index.src.md)\n[Section 2](../sec2.src.md)\n"
    assert_equal("[Home](../../README.md)\n[Section 2](../sec2.md)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path))

    input = "[Other](sub/other.src.md)\n"
    assert_equal("[Other](gfm/sub/other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path))
    assert_equal("[Other](sub/other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path))
    input = "[Other](other.src.md)\n"
    assert_equal("[Other](other.md)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path))

    input = "[C file](misc/pr1.c)\n[directory](misc/)\n"
    assert_equal "[C file](src/misc/pr1.c)\n[directory](src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @index_path)
    assert_equal "[C file](../src/misc/pr1.c)\n[directory](../src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    input = "[C file](../misc/pr1.c)\n[directory](../misc/)\n"
    assert_equal "[C file](../../src/misc/pr1.c)\n[directory](../../src/misc/)\n", G4T.send(:rewrite_links, input, 'gfm', @other_path)
  end

  def test_html_relative_conversion
    input = "[Section 1](sec1.src.md)\n"
    assert_equal "[Section 1](sec1.html)\n", G4T.send(:rewrite_links, input, 'html', @sec_path)
  end

  def test_pdf_link_conversion
    input = "See [Section 1](#introduction)\n"
    assert_equal "See [Section 1](#introduction)\n", G4T.send(:rewrite_links, input, 'pdf', @sec_path)
    @temp_root = Dir.mktmpdir
    @src_dir = File.join(@temp_root, "src")
    @original_get_path_method = PathManager.method(:get_path)
    sec_path = File.join(@src_dir, "sec1.src.md")
    FileUtils.mkdir_p(@src_dir)
    tem_file = File.join(@src_dir, "temp.src.md")
    File.write(tem_file, "# Headers and Links\n")
    with_stubbed_paths do
      assert_equal "See [temp](#headers-and-links)\n", G4T.send(:rewrite_links, "See [temp](/temp.src.md)\n", 'pdf', sec_path)
      assert_equal "See [temp](#headers-and-links)\n", G4T.send(:rewrite_links, "See [temp](temp.src.md)\n", 'pdf', sec_path)
    end
    FileUtils.rm_rf(@temp_root)
  end

  def test_source_code_conversion
    input = 'See [C program file](/misc/example.c)'
    assert_match link2regexp('/src/misc/example.c'), G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/blob/main/src/misc/example.c'), G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/blob/main/src/misc/example.c'), G4T.send(:rewrite_links, input, 'pdf', @sec_path)
    input = 'See [C program file](misc/example.c)'
    assert_match link2regexp('../src/misc/example.c'), G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/blob/main/src/misc/example.c'), G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/blob/main/src/misc/example.c'), G4T.send(:rewrite_links, input, 'pdf', @sec_path)
    input = 'See the directory[misc](/misc/)'
    assert_match link2regexp('/src/misc/'), G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/misc/'), G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/misc/'), G4T.send(:rewrite_links, input, 'pdf', @sec_path)
    input = 'See the directory[misc](misc/)'
    assert_match link2regexp('../src/misc/'), G4T.send(:rewrite_links, input, 'gfm', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/misc/'), G4T.send(:rewrite_links, input, 'html', @sec_path)
    assert_match link2regexp('https://github.com/ToshioCP/Gtk4-tutorial/tree/main/src/misc/'), G4T.send(:rewrite_links, input, 'pdf', @sec_path)
  end

  private

  # Helper method
  def link2regexp(link)
    Regexp.new(Regexp.escape(link))
  end 

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {
      root: @tmp_root,
      src:  @src_dir,
    }
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

end