# frozen_string_literal: true

require "minitest/autorun"
require "minitest/mock"
require "fileutils"

require_relative "../lib/lib_src2pdf"

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
    File.write(File.join(@src_dir, "sec1.md"), "# Sec1\n")
    File.write(File.join(@src_dir, "sec2.md"), "# Sec2\n")

    File.write(File.join(@data_dir, "settings.yml"), "")
    File.write(File.join(@data_dir, "yaml-metadata.yml"), "---\ntitle: Test\n---\n")

    File.write(File.join(@data_dir, "chapters.yml"), <<~YAML)
      - header: "Chap1"
        from: 1
        to: 2
    YAML
    @original_get_path_method = PathManager.method(:get_path)
  end

  def teardown
    FileUtils.remove_entry(@root)
  end

  # Stub PathManager.get_path so the library
  # uses the temporary directory structure.
  def with_stubbed_paths(&block)
    dir_names = {
      root: @root,
      src:  @src_dir,
      docs: @pdf_dir,
      data: @data_dir
    }
    stub_get_path = ->(type, path = "") do
      base_dir = dir_names[type] || @original_get_path_method.call(type)
      File.join(base_dir, path)
    end
    PathManager.stub(:get_path, stub_get_path, &block)
  end

  def test_src2pdf_success
    # ---- PathManager stub ----
    with_stubbed_paths do
      # ---- stub get_src_files ----
      G4T.stub :get_src_files, {
        sections: [
          File.join(@src_dir, "sec1.md"),
          File.join(@src_dir, "sec2.md")
        ],
        all: [
          File.join(@src_dir, "index.src.md"),
          File.join(@src_dir, "sec1.md"),
          File.join(@src_dir, "sec2.md")
        ]
      } do

        # ---- stub convert (identity) ----
        G4T.stub :convert, ->(content, _type) { content } do

          # ---- mock Open3.capture3 ----
          open3_mock = Minitest::Mock.new
          open3_mock.expect :call,
                            ["", "", Minitest::Mock.new.expect(:success?, true)],
                            [String, String, String, String, String],
                            stdin_data: String,
                            chdir: String

          Open3.stub :capture3, open3_mock do
            assert G4T.src2pdf
          end

          open3_mock.verify
        end
      end
    end
  end

  def test_src2pdf_raises_on_pandoc_failure
    with_stubbed_paths do
      G4T.stub :get_src_files, {
        sections: [
          File.join(@src_dir, "sec1.md"),
          File.join(@src_dir, "sec2.md")
        ],
        all: [
          File.join(@src_dir, "index.src.md"),
          File.join(@src_dir, "sec1.md"),
          File.join(@src_dir, "sec2.md")
        ]
      } do

        G4T.stub :convert, ->(content, _type) { content } do
          status_mock = Minitest::Mock.new
          status_mock.expect :success?, false

          Open3.stub :capture3, ["", "pandoc error", status_mock] do
            assert_raises RuntimeError do
              G4T.src2pdf
            end
          end

          status_mock.verify
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
          File.join(@src_dir, "sec1.md"),
          File.join(@src_dir, "sec2.md")
        ],
        all: []
      } do
        G4T.stub :convert, ->(c, _type) { c } do
          assert_raises RuntimeError do
            G4T.src2pdf
          end
        end
      end
    end
  end
end