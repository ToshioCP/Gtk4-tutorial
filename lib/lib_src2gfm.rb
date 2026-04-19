# = G4T::Src2Gfm
#
# This module provides the functionality to convert source files into 
# GitHub Flavored Markdown (GFM). It handles index page generation, 
# section navigation, and hierarchical directory structures.

require 'fileutils'
require 'pathname'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')
require PathManager.get_path(:lib, 'lib_convert')

module G4T

  class << self

    # Converts source files in the src directory to GFM format.
    # 
    # - Generates README.md in the root directory with a Table of Contents.
    # - Converts secXX.src.md files with added navigation links.
    # - Copies or converts other files preserving the directory structure.
    #
    # @return [void]
    def src2gfm
      src_dir = PathManager.get_path(:src)
      gfm_dir = PathManager.get_path(:gfm)
      root_dir = PathManager.get_path(:root)

      # clean up the gfm directory before writing new files
      FileUtils.rm_rf(gfm_dir) if File.exist?(gfm_dir)
      FileUtils.mkdir_p(gfm_dir)

      # Retrieve categorized source files using the utility method.
      src_files = get_src_files(src_dir: src_dir)
      section_files = src_files[:sections].to_a

      # 1. Create README.md from index.src.md
      index_src_md = File.join(src_dir, "index.src.md")
      raise "Error: index.src.md not exist." unless File.exist?(index_src_md)
      
      s = convert(File.read(index_src_md), "gfm", index_src_md)
      s += "\n## Table of contents\n\n"

      pn_src_dir = Pathname.new(src_dir)
      pn_root_dir = Pathname.new(root_dir)
      
      section_files.each do |file|
        # Extract the header in the first line for the table of Contents.
        header = File.open(file){|f| f.readline}.sub(/^#* */, "").chomp
        source_rel = Pathname.new(file).relative_path_from(pn_src_dir)
        target_path = File.join(gfm_dir, source_rel.to_s.sub(/\.src\.md\z/, '.md'))
        target_link = Pathname.new(target_path).relative_path_from(pn_root_dir)
        
        s << "1. [#{header}](#{target_link})\n"
      end
      File.write(File.join(root_dir, "README.md"), s)

      # 2. Convert all .src.md files except index.src.md
      src_md_files = src_files[:src_md_files].to_a.reject {|f| f == index_src_md }
      
      src_md_files.each do |file|
        content = File.read(file)
        content = convert(content, "gfm", file).chomp + "\n"
        # Add navigation bars to section files.
        if section_files.include?(file)
          i = file.match(/sec(\d+)\.src\.md\z/)[1].to_i
          total = section_files.size
          
          if total == 1
            nav = "Up: [README.md](../README.md)\n"
          elsif i == 1
            nav = "Up: [README.md](../README.md),  Next: [Section 2](sec2.md)\n"
          elsif i == total
            nav = "Up: [README.md](../README.md),  Prev: [Section #{i-1}](sec#{i-1}.md)\n"
          else
            nav = "Up: [README.md](../README.md),  Prev: [Section #{i-1}](sec#{i-1}.md), Next: [Section #{i+1}](sec#{i+1}.md)\n"
          end
          content = "#{nav}\n#{content}\n#{nav}"
        end
        
        # Determine the target path and ensure the directory exists before writing.
        source_rel = Pathname.new(file).relative_path_from(pn_src_dir)
        # Replace extension.
        target_filename = source_rel.to_s.sub(/\.src\.md\z/, '.md')
        target_path = File.join(gfm_dir, target_filename)
        
        self.write_with_directory(target_path, content)
      end
    end
  end
end
