# = G4T Source Include Module
#
# This module provides functionality to include external source files 
# into Markdown as fenced code blocks. It supports:
# - Extracting specific C functions based on function names.
# - Outputting GFM (GitHub Flavored Markdown) or Pandoc-style Markdown.
# - Automatic line numbering for Pandoc (HTML/PDF) outputs.

require 'yaml'
require_relative '../path_manager'

module G4T

  class << self

    private
    
    # Processes @@@include directives to import source code.
    #
    # @param str [String] The 3-line block starting with @@@include.
    # @param target [String] The output type: 'gfm', 'html', or 'pdf'.
    # @return [String] A formatted Markdown fenced code block.
    def include_source(str, target)
      lines = str.lines
      return "" unless lines.size == 3 # Basic validation
      return "" unless lines.first.strip == "@@@include" && lines.last.strip == "@@@"
      return "" unless ['gfm', 'html', 'pdf'].include?(target) # Valid target

      src_dir = PathManager.get_path(:src)

      # Second line contains: "pathname [func1 func2 ...]"
      # The pathname is the relative path to the source files.
      config = lines[1].strip.split(/\s+/)
      relative_pathname = config[0]
      requested_functions = config[1..-1].to_a
      file_path = File.join(src_dir, relative_pathname)
      return "\n" unless File.exist?(file_path)

      raw_src = File.read(file_path)
      ext = File.extname(file_path).downcase
      lang = detect_language(ext, target)

      # 1. C-specific function extraction
      processed_src = if ext == '.c' && requested_functions.any?
                        extract_c_functions(raw_src, requested_functions)
                      else
                        raw_src
                      end

      # 2. Formatting the fenced code block
      format_code_block(processed_src, lang, target)
    end


    # Detects the language identifier (info string) based on extension/filename and target.
    #
    # @param ext [String] The file extension (e.g., '.c') or filename (e.g., 'meson.build').
    # @param target [String] The output type: 'gfm', 'html', or 'pdf'.
    # @return [String] The mapped language identifier or an empty string.
    def detect_language(ext, target)
      ext_yaml_path = PathManager.get_path(:data, 'ext.yml')
      @ext_data = File.exist?(ext_yaml_path) ? YAML.load_file(ext_yaml_path) : {}
      # Map target 'html' or 'pdf' to the YAML key 'pandoc_markdown'
      category = target =~ /html|pdf/ ? "pandoc_markdown" : target
      # Return the mapped value, or an empty string if not found
      @ext_data[category].to_h[ext] || ""
    end

    # Extracts specific functions from C source using a simplified regex.
    # Matches: [return_type] func_name(args) { ... }
    def extract_c_functions(src, function_names)
      lines = src.lines
      functions = []
      function_names.each do |func|
        i = lines.find_index{|line| line =~ /^#{func}\b/}
        next unless i # not found
        i -= 1 if i > 0
        j = lines[i..-1].find_index{|line| line =~ /^}/}
        next unless j # not found
        j = i + j
        functions << lines.slice(i..j).join
      end
      functions.join("\n")
    end

    # Formats the fence based on the target system (GFM vs Pandoc).
    def format_code_block(src, lang, type)
      src = src.chomp + "\n" # Ensure clean ending
      
      if type == 'gfm'
        # GFM: standard triple backticks (no line numbers as per spec)
        "```#{lang}\n#{src}```\n"
      else
        # Pandoc (HTML/PDF): attributes in curly braces
        # Adds .numberLines automatically for Pandoc targets.
        attr = []
        attr << ".#{lang}" unless lang.empty?
        attr << ".numberLines"
        "``` {#{attr.join(' ')}}\n#{src}```\n"
      end
    end
  end
end