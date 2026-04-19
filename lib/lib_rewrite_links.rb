# frozen_string_literal: true

require 'strscan'
require 'pathname'
require 'yaml'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')

# G4T project namespace.
module G4T
  # Utility module for segmenting text and rewriting internal links.

  class << self

    private
    
    # Rewrites internal links and image attributes based on the target format.
    #
    # @param str [String] The source text content. It is a paragraph in a Markdown text.
    # @param target [String] The build target (gfm, html, or pdf).
    # @param path_from [String] The absolute path of the current source file being processed.
    # @return [String] The processed string.
    def rewrite_links(str, target, path_from)
      segments = split_verbatim_blocks(str)
      segments.map do |seg_type, text|
        if seg_type == :other
          # Parse the text to identify link syntax, and rewrite internal links accordingly.
          sub_segments = lex(text)
          # stat is the state of the link parsing process.
          # 1: before link, or after link
          # 2: in the link text
          # 3: in the link url
          # 4: in the link attributes
          # image_link: indicates whether the link is an image link (true) or a regular link (false). Before determined, it is nil.
          stat = 1
          image_link = nil
          buf = [] # Buffer to store the processed text segments.
          url_buf = [] #necessary for the scope of the variable.
          changable = true
          sub_segments.each do |sub_type, sub_text|
            if stat == 1
              if sub_type == :'!['
                image_link = true
                stat = 2
              elsif sub_type == :'['
                image_link = false
                stat = 2
              end
              buf << sub_text
            elsif stat == 2
              if sub_type == :']('
                stat = 3
                url_buf = []
              end
              buf << sub_text
            elsif stat == 3
              if sub_type == :')' || sub_type == :'){'
                url = url_buf.join
                if changable
                  buf << rewrite_url_path(path_from, url, target, image_link)
                else
                  buf << url
                end
                url_buf = []
                if target =~ /gfm/
                  buf << ")"
                else
                  buf << sub_text
                end
                if sub_type == :')'
                  stat = 1
                  image_link = nil
                else # sub_type == :'){'
                  stat = 4
                end
              else
                if sub_type == :code
                  changable = false
                end
                url_buf << sub_text
              end
            else # stat == 4
              if sub_type == :'}'
                stat = 1
                image_link = nil
                changable = true
              end
              unless target =~ /gfm/
                buf << sub_text
              end
            end
          end
          buf << url_buf.join if url_buf.any?
          buf.join
        else
          text  
        end
      end.join
    end

    # Rewrites *internal* url based on the target format (gfm or html).
    # A url path tells the location in the server.
    # Example: The following shows the relation between urls and paths:
    #   Absolute URL:      https://example.com/a/b/c.html#Section-1  =>  /a/b/c.html
    #   Root-relative URL: /a/b/c.html#Section-1                     =>  /a/b/c.html
    #   Relative URL:      b/c.html#Section-1                        =>  b/c.html
    #
    # @path_from [String] The absolute path of the file including the link.
    # @url_to_path str [String] The path in the destination url in the link.
    # @target [String] The target to convert, gfm or html.
    # @image_link [Boolean] true for image links, false for regular links.
    # @return [String] The rewrited url path that suits for the target.
    def rewrite_url_path(path_from, url_to_path, target, image_link)
      # Base URL is used only in html mode.
      mode = ::ENV['G4T_ENV'] || "development"
      raise "Unexpected G4T_ENV. It must be development or production" unless mode == "development" || mode == "production"
      site = YAML.load_file(PathManager.get_path(:data, "site.yml"))
      base_url = site[mode]["base_url"]
      repository_blob_prefix = site[mode]["repository_url"] + site[mode]["repository_blob_base_url"]
      repository_tree_prefix = site[mode]["repository_url"] + site[mode]["repository_tree_base_url"]

      root_dir = PathManager.get_path(:root)
      src_dir = PathManager.get_path(:src)
      gfm_dir = PathManager.get_path(:gfm)
      index_src_path = PathManager.get_path(:src, "index.src.md")
      gfm_from_root = Pathname(gfm_dir).relative_path_from(Pathname(root_dir)).to_s
      root_from_gfm = Pathname(root_dir).relative_path_from(Pathname(gfm_dir)).to_s
      src_from_root = Pathname(src_dir).relative_path_from(Pathname(root_dir)).to_s
      src_from_gfm = Pathname(src_dir).relative_path_from(Pathname(gfm_dir)).to_s
      gfm_prefix = "/" + gfm_from_root
      src_prefix = "/" + src_from_root

      if url_to_path =~ /\Ahttps?:\/\// || url_to_path =~ /\A#/ || url_to_path == "" # Absolute URL, fragment, or empty URL (Dummy URL)
        url_to_path # No conversion
      elsif url_to_path[0].to_s == '/' # Root-relative URL
        internal_url_check(url_to_path,File.expand_path(url_to_path[1..-1], src_dir), src_dir)
        if target == 'gfm'
          if url_to_path == "/index.src.md"
            "/README.md"
          elsif url_to_path =~ /\.src\.md\z/
            File.join(gfm_prefix, url_to_path.sub(/\.src\.md\z/, ".md"))
          else # non .src.md files, including images
            File.join(src_prefix, url_to_path)
          end
        else # html or pdf, **except fragments**, which points to an ATX heading in the same file
          if target == 'html' && (url_to_path =~ /\.src\.md\z/ || image_link)
            File.join(base_url, url_to_path.sub(/\.src\.md\z/, ".html"))
          elsif target == 'pdf' && url_to_path =~ /\.src\.md\z/
            header = File.open(File.join(src_dir, url_to_path[1..-1])){|f| f.gets}
            "#" + generate_id_pandoc(header)
          elsif target == 'pdf' && image_link
            File.expand_path(url_to_path[1..-1], src_dir) # Remove the leading "/"
          elsif url_to_path =~ /\/\z/ # directory
            File.join(repository_tree_prefix, src_prefix, url_to_path) # tree
          else # non .src..md files
            File.join(repository_blob_prefix, src_prefix, url_to_path) # blob
          end
        end
      else #Relative URL
        internal_url_check(url_to_path, File.expand_path(url_to_path, File.dirname(path_from)), src_dir)
        if target == 'gfm'
          if path_from == index_src_path
            if File.expand_path(url_to_path, File.dirname(path_from)) == index_src_path
              "#"
            elsif url_to_path =~ /\.src\.md\z/
              File.join(gfm_from_root, url_to_path).sub(/\.src\.md$/, ".md")
            else
              File.join(src_from_root, url_to_path)
            end
          else
            url_to_path_abs = File.expand_path(url_to_path, File.dirname(path_from))
            path_from_relative_from_src = Pathname(path_from).relative_path_from(Pathname(src_dir)).to_s
            path_from_after_move = File.join(gfm_dir, path_from_relative_from_src).sub(/\.src\.md\z/, ".md")
            # path_from is a file, not a directory
            path_from_after_move_dir = File.dirname(path_from_after_move)
            if url_to_path_abs == index_src_path
              Pathname(File.join(root_dir, "README.md")).relative_path_from(Pathname(path_from_after_move_dir)).to_s
            elsif url_to_path =~ /\.src\.md\z/
              url_to_path.sub(/\.src\.md$/, ".md")
            else
              r = Pathname(url_to_path_abs).relative_path_from(Pathname(path_from_after_move_dir)).to_s
              (url_to_path[-1] == "/" && r[-1] != "/") ? r + "/" : r
            end
          end
        else # html or pdf, **except fragments**, which points to an ATX heading in the same file
          if  target == 'html' && (url_to_path =~ /\.src\.md\z/ || image_link)
            url_to_path.sub(/\.src\.md/, '.html')
          elsif target == 'pdf' && url_to_path =~ /\.src\.md\z/
            header = File.open(File.expand_path(url_to_path, File.dirname(path_from))){|f| f.gets}
            "#" + generate_id_pandoc(header)
          elsif target == 'pdf' && image_link
            File.expand_path(url_to_path, File.dirname(path_from))
          else
            url_to_path_abs = File.expand_path(url_to_path, File.dirname(path_from))
            url_to_path_root_relative = File.join("/", Pathname(url_to_path_abs).relative_path_from(Pathname(root_dir)).to_s)
            if url_to_path =~ /\/\z/ # directory
              File.join(repository_tree_prefix, url_to_path_root_relative, "/")
            else
              File.join(repository_blob_prefix, url_to_path_root_relative)
            end
          end
        end
      end
    end

    # Check if url_to_path is within the src directory. If not, raise an error.
    def internal_url_check(url_to_path, url_to_path_abs, src_dir)
      url_to_path_abs = Pathname(url_to_path_abs).cleanpath.to_s
      unless url_to_path_abs.start_with?(src_dir)
        raise "Invalid URL: #{url_to_path}. It points outside of the source directory."
      end
    end

    def generate_id_pandoc(header)
      # Remove formatting, etc.
      id = header.gsub(/\[([^\]]+)\]\([^\)]+\)/, '\1') # links
      id = id.gsub(/`([^`]+)`/, '\1')               # inline code
      id = id.gsub(/\*{1,2}([^\*]+)\*{1,2}/, '\1') # bold
      id = id.gsub(/_{1,2}([^_]+)_{1,2}/, '\1') # italic
      id = id.gsub(/\[\^[^\]]+\]/, '') # footnotes
      id = id.gsub(/[^A-Za-z0-9\-_. ]/, '')
      id = id.downcase
      id = id.strip.gsub(/[\s\n]+/, '-')
      id = id.sub(/^[^a-z]+/, '')
      id = "section" if id.empty?
      id
    end

    # Parses the input string and splits it into three types of segments:
    #   1. fenced: fenced code blocks
    #   2. blank: blank lines
    #   3. indented: indented code blocks
    #   4. other: other blocks
    # The order above is important for the parsing process.
    #   1. fenced code blocks can contain blank lines, so, fenced code blocks must be processed before blank lines.
    #   2. Blocks except fenced code blocks are separated by blank lines, so, blank lines process will be the second.
    #   3. All the lines in indented code blocks are indented by at least four spaces, or the block will be treated as a regular text block.
    #   4. Remaining blocks will be other blocks, whic are treated as regular text blocks.
    #
    # @param str [String] The source text to be processed.
    # @return [Array<Array(Symbol, String)>] A list of [type, text] pairs.
    def split_verbatim_blocks(str)
      # Pass 1. -- Fenced code block.
      # An element of `segments` is [type, content].
      # `type` is one of :fenced and :others

      # At least three fence_char (backtick or tilde)
      fence_start = /^[ ]{0,3}(`{3,}|~{3,})/
      indented_lines = /^[ ]{4,}.*(\R|\z)/

      ss = StringScanner.new(str)
      segments = []
      was_blank = true # True if the string pointer at the start of the string or the previous line is blank.
      until ss.eos?
        start_pos = ss.pos
        if was_blank && ss.scan(fence_start)
          # End with a row of fence_char that must be at least as long as the starting row
          # Indented no more than three spaces
          fence_char = ss[1][0]
          fence_char_length = ss[1].length
          fence_end = /^[ ]{0,3}#{Regexp.escape(fence_char)}{#{fence_char_length},}\s*?\R/
          unless ss.scan_until(fence_end) # If the end of the fenced code block is not found, the rest of the string will be treated as a fenced code block.
            ss.terminate
          end
          was_blank = false
          type = :fenced
        elsif ss.scan(/^\s*(\R|\z)/) # blank lines
          was_blank = true
          type = :blank
        elsif was_blank && ss.scan(indented_lines)
          loop do
            while ss.scan(indented_lines) # Move the string pointer until the end of the indented code block.
            end
            end_pos = ss.pos
            while ss.scan(/^\s*(\R|\z)/) # Blank lines can be included between indented code blocks.
            end
            unless ss.check(indented_lines)
              ss.pos = end_pos
              break
            end
          end
          was_blank = false
          type = :indented
        else # other blocks
          if ss.scan_until(/^\s*(\R|\z)/) # Move the string pointer until the end of the blank line.
            ss.pos = ss.pos - ss.matched.bytesize # Move the string pointer back to the start of the blank lines.
          else # If no blank line exists, the endof the string will be treated as the end of the block.
            ss.terminate
          end
          was_blank = false
          type = :other
        end
        segments << [type, ss.string.byteslice(start_pos, ss.pos - start_pos)]
      end
      segments
    end

    # Analyzes text to separate inline code from plain text.
    # This version supports inline code spanning multiple lines.
    # In addition, it does partial lexical analysis of Markdown to identify '[', '](',')' for link syntax and also identifies escape characters.
    #
    # The escape character '\' doesn't work in inline code, so both escape annd inline code need to be analyzed at the same time.   
    #
    # In the returned list, the IDs are not necessarily have special meanings such as the opening of a link.
    # They are determined by the parsing process. They may be plain text in some cases.
    #
    # @param str [String] The text segment to be parsed.
    # @return [Array<Array(Symbol, String)>] A list of [ID, text] pairs. ID is:
    #   :text : plain text
    #   :'![' : the opening symbols of an image link
    #   :'[' : the opening bracket ("["of a link
    #   :'](' : the separator ("](") between link text and URL in a link
    #   :')' : the closing parenthesis (")") of a link
    #   :'){' : the separator between URL and attributes in a link
    #   :'}' : the closing curly brace ("}") of an attribute list
    #   :code : inline code
    def lex(str)
      ss = StringScanner.new(str)
      sub_segments = []
      until ss.eos?
        if ss.scan(/\\/)
          if ss.scan(/[()\[\]{}`~\\]/)
            sub_segments << [:text, "\\" + ss.matched]
          else
            sub_segments << [:text, "\\"]
          end
        elsif ss.scan(/`+/)
          delimiter = ss.matched
          start_pos = ss.pos - delimiter.bytesize
          if ss.scan_until(/(?<=[^`])#{Regexp.escape(delimiter)}(?=[^`]|$|\z)/)
            sub_segments << [:code, ss.string.byteslice(start_pos, ss.pos - start_pos)] # include the backticks
          else
            sub_segments << [:text, delimiter] # If the closing backticks are not found, the backtics are treated as plain text.
          end
        elsif ss.scan(/!\[/)
          sub_segments << [:'![', ss.matched]
        elsif ss.scan(/\[/)
          sub_segments << [:'[', ss.matched]
        elsif ss.scan(/\]\(/)
          sub_segments << [:'](', ss.matched]
        elsif ss.scan(/\)(?!\{)/)
          sub_segments << [:')', ss.matched]
        elsif ss.scan(/\){/)
          sub_segments << [:'){', ss.matched]
        elsif ss.scan(/}/)
          sub_segments << [:'}', ss.matched]
        else
          sub_segments << [:text, ss.getch]
        end
      end
      sub_segments
    end
  end
end