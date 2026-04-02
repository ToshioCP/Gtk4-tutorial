# = G4T PDF Generation Module
#
# This module handles the conversion of Markdown content to a PDF document
# using Pandoc and LuaLaTeX. It supports external templates and custom
# LaTeX packages for high-quality typesetting.

require 'open3'
require 'yaml'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_convert')
require PathManager.get_path(:lib, 'lib_utils')

module G4T
  # Converts Markdown string to a PDF file.
  def self.src2pdf
    src_dir = PathManager.get_path(:src)
    pdf_path = PathManager.get_path(:pdf)
    Dir.mkdir(pdf_path) unless Dir.exist?(pdf_path)
    target_path = File.join(pdf_path, "Gtk4-tutorial.pdf")
    settings_path = PathManager.get_path(:data, "settings.yml")
    yaml_metadata_path = PathManager.get_path(:data, "yaml-metadata.yml")
    chapters = YAML.load_file(PathManager.get_path(:data, "chapters.yml"))
    src_files = self.get_src_files(src_dir: src_dir)
    section_files = src_files[:sections]

    # Pandoc arguments:
    cmd = [
      "pandoc",
      "--defaults=#{settings_path}",
      "--resource-path=#{src_dir}",
      "-o",
      target_path
    ]

    # Construct source
    s = []
    # YAML metadata
    s << File.read(yaml_metadata_path)
    # Preface
    s << "\n\\frontmatter\n\n"
    index_src_md = File.join(src_dir, "index.src.md")
    s << self.convert(File.read(index_src_md), "pdf")
    # Table of contents
    s << "\n\\tableofcontents\n\n\\mainmatter\n\n"
    # section files
    raise "chapters.yml has different number of sections" unless chapters[-1]['to'] == section_files.size
    chapters.each do |chapter|
      s << "\\chapter{#{chapter['header']}}\n\n"
      r = Range.new(chapter['from'].to_i, chapter['to'].to_i)
      r.each do |n|
        content = File.read(section_files[n - 1])
        content = self.convert(content, "pdf")
        s << content + "\n"
      end
    end
    # Other files for appendices
    other_src_md_files = src_files[:all].reject{|f| f == index_src_md }.reject{|f| src_files[:sections].include?(f)}
    unless other_src_md_files.empty?
      s << "\\appendix\n\n"
      other_src_md_files.each do |file|
        content = File.read(file)
        content = self.convert(content, "pdf")
        s << content + "\n"
      end
    end

    # result = [stdout, stderr, status]
    _, stderr, status = Open3.capture3(*cmd, stdin_data: s.join("\n"), chdir: pdf_path)
    raise "Pandoc PDF Error: #{stderr}" unless status.success?

    true
  end
end