# = TestLibSrc2Html
#
# Unit tests for G4T.src2html.
#
# These tests verify the full HTML build pipeline:
#
# * Reading source Markdown files
# * Running Pandoc conversion
# * Applying the HTML template
# * Generating navigation links
# * Creating index.html
# * Copying assets such as CSS and images
#
# The tests run inside a temporary directory so that
# the real project files are not affected.

require 'minitest/autorun'
require 'minitest/mock'
require 'fileutils'
require 'tmpdir'
require 'pathname'

require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_src2html')

class TestLibSrc2Html < Minitest::Test

  # Prepare a temporary directory structure.
  #
  # Structure:
  #
  #   tmp/
  #     src/
  #       index.src.md
  #       sec1.src.md
  #       sec2.src.md
  #       sec3.src.md
  #       style.css
  #       images/sample.png
  #
  #     docs/
  #
  #     data/template.html
  #
  # PathManager.get_path is stubbed so the library
  # works inside this temporary structure.
  def setup
    @tmp_root = Dir.mktmpdir("g4t_html_test")

    @src_dir  = File.join(@tmp_root, "src")
    @docs_dir = File.join(@tmp_root, "docs")
    @data_dir = File.join(@tmp_root, "data")

    FileUtils.mkdir_p(@src_dir)
    FileUtils.mkdir_p(@docs_dir)
    FileUtils.mkdir_p(@data_dir)

    setup_template
    setup_image_extensions_yaml
    setup_site_env_yaml
    setup_test_files

    @original_get_path_method = PathManager.method(:get_path)
  end

  # Remove the temporary directory after the test.
  def teardown
    FileUtils.rm_rf(@tmp_root)
  end


  # ------------------------------------------------------------
  # Test preparation helpers
  # ------------------------------------------------------------

  # Create a minimal Pandoc HTML template.
  def setup_template
    template = <<~HTML
    <html>
    <head>
      <title>$title$</title>
    </head>
    <body>
      <ul>
      $if(home_link)$
      <li><a href="$home_link$">Home</a></li>
      $endif$
      $if(prev_link)$
      <li><a href="$prev_link$">Prev: $prev_title$</a></li>
      $endif$
      $if(next_link)$
      <li><a href="$next_link$">Next: $next_title$</a></li>
      $endif$
      </ul>
    $body$
    </body>
    </html>
    HTML

    @template_path = File.join(@data_dir, "template.html")
    File.write(@template_path, template)
  end

  def setup_image_extensions_yaml
    image_extensions = <<~YAML
    # Extensions for images
    - .jpg
    - .jpeg
    - .png
    - .pdf
    YAML
    @image_extensions_path = File.join(@data_dir, "image_extensions.yml")
    File.write(@image_extensions_path, image_extensions)
  end

  # Create a mode definition YAML file.
  def setup_site_env_yaml
    site_env = <<~YAML
      production:
        base_url: /Gtk4-tutorial
      development:
        base_url: ""
    YAML

    File.write(File.join(@data_dir, "site.yml"), site_env)
  end

  # Create source files used for testing.
  def setup_test_files
    File.write(File.join(@src_dir, "index.src.md"),
      "# Index Page\nWelcome to HTML build.")

    File.write(File.join(@src_dir, "sec1.src.md"),
      "# Section 1\nContent 1")

    File.write(File.join(@src_dir, "sec2.src.md"),
      "# Section 2\nContent 2")

    File.write(File.join(@src_dir, "sec3.src.md"),
      "# Section 3\nContent 3")

    File.write(File.join(@src_dir, "other.src.md"),
      "# Other File\nContent other")

      File.write(File.join(@src_dir, "style.css"),
      "body { font-family: sans-serif; }")

    FileUtils.mkdir_p(File.join(@src_dir, "images"))
    File.binwrite(File.join(@src_dir, "images/sample.png"), "PNGDATA")
  end

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {
      root: @tmp_root,
      src:  @src_dir,
      docs: @docs_dir,
      data: @data_dir
    }
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

  # ------------------------------------------------------------
  # Tests
  # ------------------------------------------------------------

  # Verify that the full build process runs successfully.
  #
  # Checks:
  # * index.html is generated
  # * section HTML files exist
  # * navigation links are created
  # * assets are copied
  def test_src2html_full_process
    with_stubbed_paths do
      G4T.src2html
    end

    verify_index_html
    verify_section_files
    verify_navigation_links
    verify_assets
  end


  # Verify that an error is raised when template.html is missing.
  def test_src2html_raises_error_if_template_missing
    File.delete(@template_path)

    with_stubbed_paths do
      assert_raises(RuntimeError) { G4T.src2html }
    end
  end


  # ------------------------------------------------------------
  # Verification helpers
  # ------------------------------------------------------------

  # Verify index.html generation.
  def verify_index_html
    index_path = File.join(@docs_dir, "index.html")

    assert File.exist?(index_path),
      "index.html should be generated"

    html = File.read(index_path)

    assert_match(/Table of contents/, html)
    assert_match(/sec1.html/, html)
    assert_match(/sec2.html/, html)
  end


  # Verify section HTML files exist.
  def verify_section_files
    %w[sec1 sec2 sec3].each do |name|
      path = File.join(@docs_dir, "#{name}.html")

      assert File.exist?(path),
        "#{name}.html should be generated"
    end
  end


  # Verify navigation links between sections.
  def verify_navigation_links
    html = File.read(File.join(@docs_dir, "sec2.html"))

    assert_match(/index.html/, html)
    assert_match(/sec1.html/, html)
    assert_match(/sec3.html/, html)
  end


  # Verify asset copying (CSS and images).
  def verify_assets
    assert File.exist?(File.join(@docs_dir, "style.css")),
      "style.css should be copied"

    assert File.exist?(File.join(@docs_dir, "images/sample.png")),
      "image files should be copied"
  end

end