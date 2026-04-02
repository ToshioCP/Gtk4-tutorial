# = G4T Preprocessor Module
#
# Provides a simple branch-selection preprocessor for @@@if...@@@end blocks.
# This module identifies the first branch that matches the target and returns
# its content exclusively.

require 'stringio'
require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')

module G4T
  class << self
    # Processes a single @@@if block by selecting the matching section.
    #
    # This method expects a block that starts with @@@if and ends with @@@end.
    # It sequentially evaluates @@@if and @@@elif conditions. If a match is found,
    # it returns that section's content and ignores all subsequent branches.
    #
    # @param block_str [String] The block starting with @@@if and ending with @@@end.
    # @param target [String] The current build target (e.g., 'html', 'gfm').
    # @return [String] The content of the matching branch, or an empty string if none match.
    def preprocess_block(block_str, target)
      StringIO.open(block_str, 'r') do |io|
        # First line must be @@@if
        s = io.gets
        return "" unless s =~ /^@@@if\s+(.+)/
        condition = $1
        status, block_in_clause, s = _get_block_in_if_elsif_clause(io, condition, target)
        return block_in_clause if status

        while (s =~ /^@@@elif\s+(.+)/)
          condition = $1
          status, block_in_clause, s = _get_block_in_if_elsif_clause(io, condition, target)
          return block_in_clause if status
        end

        if s =~ /^@@@else/
          content = []
          while (s = io.gets)
            if s =~ /^@@@end/
              return content.join
            else
              content << s
            end
          end
        end

        if s =~ /^@@@end/
          return "" # no condition matched
        else
          raise "@@@if-elif-else-end syntax error"
        end
      end
    end

    private

    # Private method only for preprocess_block method.
    #
    # @param io [StringIO] The StringIO object.
    # @param condition [String] The condition in the @@@if or @@@elif directive.
    # @param target [String] The target (gfm, html or pdf).
    # @return [Array] [The result of the matching, content of the matching branch, input string].
    def _get_block_in_if_elsif_clause(io, condition, target)
      content = []
      while (s = io.gets)
        if s =~ /^@@@(elif|else|end)/
          return [true, content.join, s] if _match_condition?(condition, target)
          break
        else
          content << s
        end
      end
      [false, "", s]
    end

    # Checks if the build target matches the condition (supports OR logic with '|').
    #
    # @param condition [String] The condition string from the directive (e.g., 'html|gfm').
    # @param target [String] The current build target.
    # @return [Boolean] True if the target matches any part of the condition.
    # @private
    def _match_condition?(condition, target)
      condition.split('|').map(&:strip).include?(target)
    end
  end
end