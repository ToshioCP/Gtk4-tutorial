# = G4T PDF Generation Module
#
# This module handles the conversion of Markdown content to a PDF document
# using Pandoc and LuaLaTeX. It supports external templates and custom
# LaTeX packages for high-quality typesetting.

require 'open3'
require 'yaml'
require 'fileutils'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_convert')
require PathManager.get_path(:lib, 'lib_utils')

module G4T
  class << self

    # Converts Markdown string to a PDF file.
    def src2pdf(target = "pdf")
      src_dir = PathManager.get_path(:src)
      data_dir = PathManager.get_path(:data)
      pdf_dir = PathManager.get_path(:pdf)
      src_files = get_src_files(src_dir: src_dir)
      sec_files = src_files[:sections].to_a
      index_src_md = File.join(src_dir, "index.src.md")
      other_src_md_files = src_files[:src_md_files].to_a.reject{|file| file == index_src_md || sec_files.include?(file)}
      FileUtils.rm_rf(pdf_dir) if File.exist?(pdf_dir)
      FileUtils.mkdir_p(pdf_dir)
      yaml_metadata_path = File.join(data_dir, "yaml-metadata.yml")
      chapters = YAML.load_file(File.join(data_dir, "chapters.yml"))
      if target == "latex"
        settings_path = File.join(data_dir, "settings_latex.yml")
        output_format = "--to=latex"
        target_path = File.join(pdf_dir, "Gtk4-tutorial.tex")
      else
        settings_path = File.join(data_dir, "settings.yml")
        output_format = "--to=pdf"
        target_path = File.join(pdf_dir, "Gtk4-tutorial.pdf")
      end

      # Pandoc arguments:
      cmd = [
        "pandoc",
        "--defaults=#{settings_path}",
        "--resource-path=#{src_dir}",
        output_format,
        "-o",
        target_path
      ]

      # Construct source
      s = []
      # YAML metadata
      s << File.read(yaml_metadata_path)
      # Preface
      # result = [stdout, stderr, status]
      File.write(File.join(pdf_dir, "cover.tex"), File.read(File.join(data_dir, "cover.tex")))
      File.write(File.join(pdf_dir, "include-before.md"), self.convert(File.read(index_src_md), "pdf", index_src_md))
      _, stderr, status = Open3.capture3("pandoc", File.join(pdf_dir, "include-before.md"), "-o", File.join(pdf_dir, "include-before.tex"), chdir: pdf_dir)
      raise "Pandoc Error in Preface: #{stderr}" unless status.success?
      # section files
      raise "chapters.yml has different number of sections" unless chapters[-1]['to'] == sec_files.size
      chapters.each do |chapter|
        s << "\\chapter{#{chapter['header']}}\n\n"
        r = Range.new(chapter['from'].to_i, chapter['to'].to_i)
        r.each do |n|
          content = File.read(sec_files[n - 1])
          content = self.convert(content, "pdf", sec_files[n - 1])
          s << content + "\n"
        end
      end
      # Other files for appendices
      unless other_src_md_files.empty?
        s << "\\appendix\n\n\\chapter{Appendix}\n\n"
        other_src_md_files.each do |file|
          content = File.read(file)
          content = self.convert(content, "pdf", file)
          s << content + "\n"
        end
      end

      # result = [stdout, stderr, status]
      _, stderr, status = Open3.capture3(*cmd, stdin_data: s.join("\n"), chdir: pdf_dir)
      raise "Pandoc PDF Error: #{stderr}" unless status.success?
      File.delete(File.join(pdf_dir, "include-before.md"))
      File.delete(File.join(pdf_dir, "include-before.tex"))
      File.delete(File.join(pdf_dir, "cover.tex"))

      true
    end
  end
end