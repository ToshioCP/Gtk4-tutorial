# = G4T::Src2Html
#
# This module provides functionality to build HTML output using Pandoc.
# It converts source sections into standalone HTML files with navigation links
# and applies a custom HTML template.

require 'pathname'
require 'fileutils'
require 'open3'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')
require PathManager.get_path(:lib, 'lib_convert')

module G4T

  class << self
    # Converts source files to HTML format using Pandoc.
    #
    # - Generates index.html in the docs directory.
    # - Adds navigation links (Home/Prev/Next) to section files.
    # - Uses assets/template.html for the HTML structure.
    #
    # @raise [RuntimeError] If index.src.md or template.html is missing.
    # @return [void]
    def src2html
      src_dir = PathManager.get_path(:src)
      pn_src_dir = Pathname.new(src_dir)
      index_file = File.join(src_dir, "index.src.md")
      pn_index_file = Pathname(index_file)
      src_files = self.get_src_files(src_dir: src_dir)
      sec_files = src_files[:sections]
      other_src_md_files = src_files[:all].reject{|file| file == index_file || sec_files.include?(file)}.select{|file| file =~ /\.src\.md\z/}
      # Most of the image files are in the '/src/images' directory, but some of them are located in other directories.
      image_files = src_files[:all].select{|file| image?(file)}
      style_file = File.join(src_dir, "style.css")

      docs_dir = PathManager.get_path(:docs) # Output target for HTML
      template_path = File.join(PathManager.get_path(:data), "template.html")

      raise "Error: index.src.md does not exist." unless File.exist?(index_file)
      raise "Error: template.html does not exist." unless File.exist?(template_path)

      Dir.children(docs_dir).each do |child|
        FileUtils.remove_entry_secure(File.join(docs_dir, child))
      end

      # 1. Create index.html (Main portal)
      s = convert(File.read(index_file), "html", index_file)
      s += "\n## Table of contents\n\n"

      sec_files.each do |file|
        header = File.open(file){|f| f.gets}.to_s.sub(/^#* */, "").chomp
        # Link to .html file in the same or nested directory
        target_link = File.basename(file).sub(/\.src\.md\z/, '.html')
        s << "1. [#{header}](#{target_link})\n"
      end
      
      target_index_path = File.join(docs_dir, "index.html")
      pandoc_convert_html(s, target_index_path, template_path, "GTK4 Tutorial")

      # 2. Convert section files (sec<integer>.src.md)
      sec_files.each do |file|
        # Determine output path and run Pandoc
        target_path = File.join(docs_dir, File.basename(file).sub(/\.src\.md\z/, '.html'))

        content = File.read(file)
        content = convert(content, "html", file)

        # Get title from the first line for the <title> tag
        title = content.lines.first&.sub(/^#* */, "")&.chomp || File.basename(target_path)

        i = file.match(/sec(\d+)\.src\.md\z/)[1].to_i
        total = sec_files.size
        
        if i >= 2
          prev_link = "sec#{i-1}.html"
          prev_title = "Section #{i-1}"
        else
          prev_link = prev_title = nil
        end

        if i < total
          next_link = "sec#{i+1}.html"
          next_title = "Section #{i+1}"
        else
          next_link = next_title = nil
        end

        pandoc_convert_html(content, target_path, template_path, title, home_link: "index.html", prev_link: prev_link, next_link: next_link, prev_title: prev_title, next_title: next_title)
      end

      # 3. Convert other .src.md files
      other_src_md_files.each do |file|
        # Determine output path and run Pandoc
        file_rel = Pathname(file).relative_path_from(Pathname(src_dir))
        target_path = File.join(docs_dir, file_rel.to_s.sub(/\.src\.md\z/, '.html'))
        FileUtils.mkdir_p(File.dirname(target_path))

        content = File.read(file)
        content = convert(content, "html", file)

        # Get title from the first line for the <title> tag
        title = content.lines.first&.sub(/^#* */, "")&.chomp || File.basename(target_path)
        pandoc_convert_html(content, target_path, template_path, title, home_link: "index.html")
      end

      # 4. Copy assets (the style file and the images)
      target_style_path = File.join(docs_dir, "style.css")
      write_with_directory(target_style_path, File.read(style_file))
      image_files.each do |image|
        image_rel = Pathname(image).relative_path_from(pn_src_dir)
        target_image_path = File.join(docs_dir, image_rel.to_s)
        FileUtils.mkdir_p(File.dirname(target_image_path))
        File.binwrite(target_image_path, File.binread(image))
      end

      # 5. Create .nojekyll file
      File.write(File.join(docs_dir, ".nojekyll"), "")

    end

    private

    # Runs Pandoc to convert Markdown string to HTML file using a template.
    #
    # @param md_content [String] The markdown content to convert.
    # @param output_path [String] The target HTML file path.
    # @param template [String] Path to the HTML template.
    # @param title [String] Title to pass to Pandoc's metadata.
    # @param home_link [String] link to the home page to pass to Pandoc's metadata.
    # @param prev_link [String] link to the previous page to pass to Pandoc's metadata.
    # @param next_link [String] link to the next page to pass to Pandoc's metadata.
    # @param prev_title [String] Title of the previous page to pass to Pandoc's metadata.
    # @param next_title [String] Title of the next page to pass to Pandoc's metadata.
    def pandoc_convert_html(md_content, output_path, template, title, home_link: nil, prev_link: nil, next_link: nil, prev_title: nil, next_title: nil)
      FileUtils.mkdir_p(File.dirname(output_path))
      
      # Use IO.popen to pipe the markdown string into pandoc
      # --standalone: to use the template
      # --template: specifies the template file
      # --metadata: sets the document title
      cmd = [
        "pandoc",
        "--from", "markdown",
        "--to", "html",
        "--standalone",
        "--template", template,
        "--metadata", "title=#{title}",
        "--output", output_path
      ]
      cmd << "--variable=home_link=#{home_link}" if home_link
      cmd << "--variable=prev_link=#{prev_link}" if prev_link
      cmd << "--variable=next_link=#{next_link}" if next_link
      cmd << "--variable=prev_title=#{prev_title}" if prev_title
      cmd << "--variable=next_title=#{next_title}" if next_title

      _, stderr, status = Open3.capture3(*cmd, stdin_data: md_content)

      unless status.success?
        raise "Pandoc failed:\n#{stderr}"
      end
    end
  end
end