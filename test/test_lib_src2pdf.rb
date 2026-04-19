# frozen_string_literal: true

require "minitest/autorun"
require "minitest/mock"
require "fileutils"
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_src2pdf')

class TestSrc2Pdf < Minitest::Test
  def setup
    @root = Dir.mktmpdir
    @src_dir = File.join(@root, "src")
    @pdf_dir = File.join(@root, "pdf")
    @data_dir = File.join(@root, "data")

    FileUtils.mkdir_p(@src_dir)
    FileUtils.mkdir_p(@data_dir)

    # fake files
    File.write(File.join(@src_dir, "index.src.md"), "# Preface\n")
    File.write(File.join(@src_dir, "sec1.src.md"), "# Sec1\n")
    File.write(File.join(@src_dir, "sec2.src.md"), "# Sec2\n")

    File.write(File.join(@data_dir, "settings.yml"), "")
    File.write(File.join(@data_dir, "yaml-metadata.yml"), "---\ntitle: Test\n---\n")

    File.write(File.join(@data_dir, "chapters.yml"), <<~YAML)
      - header: "Chap1"
        from: 1
        to: 2
    YAML
    File.write(File.join(@data_dir, "cover.tex"), "This is a cover.\n" )
    @original_get_path_method = PathManager.method(:get_path)
    @mock = Minitest::Mock.new
  end

  def teardown
    FileUtils.rm_rf(@root)
  end

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {
      root: @root,
      src:  @src_dir,
      pdf:  @pdf_dir,
      data: @data_dir
    }
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

  def test_src2pdf_success_or_raise
    # ---- PathManager stub ----
    with_stubbed_paths do
      # ---- stub get_src_files ----
      G4T.stub :get_src_files, {
        sections: [
          File.join(@src_dir, "sec1.src.md"),
          File.join(@src_dir, "sec2.src.md")
        ],
        all: [
          File.join(@src_dir, "index.src.md"),
          File.join(@src_dir, "sec1.src.md"),
          File.join(@src_dir, "sec2.src.md")
        ],
        src_md_files: [
          File.join(@src_dir, "index.src.md"),
          File.join(@src_dir, "sec1.src.md"),
          File.join(@src_dir, "sec2.src.md")
        ]
      } do

        # ---- stub convert (identity) ----
        G4T.stub(:convert, (->(content, _type, src_path=nil){ content })) do
          capture3_result = ->(*args, **kwargs) do
            # Pandoc creates include_before.tex
            file = args.select{|f| f.is_a?(String) && f =~ /.*\.tex$/}
            file.each{|f| File.write(File.join(@pdf_dir, File.basename(f)), "")}
            ["", "", @mock]
          end
          Open3.stub(:capture3, capture3_result) do
            @mock.expect(:success?, true)
            @mock.expect(:success?, true)
            assert G4T.src2pdf
            @mock.verify
            @mock.verify
            @mock.expect(:success?, true)
            @mock.expect(:success?, true)
            assert G4T.src2pdf("latex")
            @mock.verify
            @mock.verify
            assert_raises RuntimeError do
              @mock.expect(:success?, true)
              @mock.expect(:success?, false)
              G4T.src2pdf
              @mock.verify
              @mock.verify
            end
            assert_raises RuntimeError do
              @mock.expect(:success?, true)
              @mock.expect(:success?, false)
              G4T.src2pdf("latex")
              @mock.verify
              @mock.verify
            end
          end
        end
      end
    end
  end

  def test_chapter_mismatch_raises
    File.write(File.join(@data_dir, "chapters.yml"), <<~YAML)
      - header: "Chap1"
        from: 1
        to: 3
    YAML

    with_stubbed_paths do
      G4T.stub :get_src_files, {
        sections: [
          File.join(@src_dir, "sec1.src.md"),
          File.join(@src_dir, "sec2.src.md")
        ],
        all: []
      } do
        G4T.stub :convert, ->(c, _type, src_path=nil) { c } do
          assert_raises RuntimeError do
            G4T.src2pdf
          end
        end
      end
    end
  end
end