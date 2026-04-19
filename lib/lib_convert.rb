# = G4T Convert Module
#
# This module provides a two-pass conversion process for source files:
# 1. Preprocessing: Handles conditional content via @@@if...@@@end blocks.
# 2. Integration: Processes @@@include and @@@shell directives simultaneously.

require 'stringio'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_preprocess')
require PathManager.get_path(:lib, 'lib_include')
require PathManager.get_path(:lib, 'lib_shell')
require PathManager.get_path(:lib, 'lib_rewrite_links')

module G4T
  class << self
    # Directive patterns used by split_directive_blocks
    DIRECTIVE_START_IF = /\A@@@if\s/
    DIRECTIVE_START_INCLUDE = /\A@@@include\s*$/
    DIRECTIVE_START_SHELL = /\A@@@shell\s*$/
    DIRECTIVE_START = Regexp.union(DIRECTIVE_START_IF, DIRECTIVE_START_INCLUDE, DIRECTIVE_START_SHELL)
    DIRECTIVE_END_IF   = /^@@@end\s*$/
    DIRECTIVE_END_OTHER   = /^@@@\s*$/

    private_constant :DIRECTIVE_START_IF, :DIRECTIVE_START_INCLUDE, :DIRECTIVE_START_SHELL, :DIRECTIVE_START
    private_constant :DIRECTIVE_END_IF, :DIRECTIVE_END_OTHER

    private
    # Converts the input string based on the specified target.
    #
    # @param str [String] The raw source content.
    # @param target [String] The build target (e.g., 'html', 'gfm', 'pdf').
    # @return [String] The fully converted content.
    def convert(str, target, src_path = nil)
      # Pass 1: Conditional Preprocessing (@@@if...@@@end)
      blocks = split_directive_blocks(str)
      buf = []
      blocks.each do |block|
        if block =~ DIRECTIVE_START_IF
          buf << self.preprocess_block(block, target)
        else
          buf << block
        end
      end
      str = buf.join
      # Pass 2: Source Inclusion and Shell Execution
      # Processes three-line directives:
      #   1: @@@include or @@@shell
      #   2: filename or command
      #   3: @@@end (closing mark)
      # Processes plain-text blocks (outside of directive blocks)
      # Rewrite links:
      #   1: gfm type  => [Section 1](sec1.md)
      #   2: html type => [Section 1](sec1.html)
      #   3: pdf type  => Section 1
      # Note: These rewrites are applied except within verbatim blocks.
      blocks = split_directive_blocks(str)
      buf = []
      blocks.each do |block|
        if block =~ DIRECTIVE_START_INCLUDE
          buf << include_source(block, target)
        elsif block =~ DIRECTIVE_START_SHELL
          buf << include_shell(block)
        else
          buf << rewrite_links(block, target, src_path)
        end
      end
      buf.join
    end

    # Parses the argument string and splits it into a sequence of command blocks and plain text.
    #
    # This method identifies @@@ commands (such as if/include/shell) and @@@end.
    # Returns an ordered array of String blocks.
    # Each block is either:
    #   - a directive block starting with @@@
    #   - or plain text between directives.
    #
    # The result forms net (recursive) array.
    # For example, "@@@if gfm\n@@@include\nfile.c\n@@@end\n@@@end" will be converted to:
    # ["@@@if gfm", ["@@@include\nfile.c\n@@@end], "@@@end"]
    # This means @@@end corresponds to the last @@@if, @@@include or @@@shell before. 
    #
    # @param str [String] The source text to be processed.
    # @return [Array<String>] An ordered list of parsed segments.
    def split_directive_blocks(str)
      array_blocks = []
      block = []
      block_end_mark = nil # keep alive anywhere in the block below
      status = :out
      str.each_line do |s|
        if status == :out && s =~ DIRECTIVE_START
          block_end_mark = s =~ DIRECTIVE_START_IF ? DIRECTIVE_END_IF : DIRECTIVE_END_OTHER
          array_blocks << block.join unless block.empty?
          block = [s]
          status = :in
        elsif status == :in && s =~ block_end_mark
          block << s
          array_blocks << block.join unless block.empty?
          block = []
          status = :out
        else
          block << s
        end
      end
      array_blocks << block.join unless block.empty?
      array_blocks
    end
  end
end