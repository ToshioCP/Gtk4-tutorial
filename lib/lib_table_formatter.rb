# = G4T Table Formatter Module
#
# This module provides functionality to beautify Markdown tables. 
# It aligns columns based on their maximum width while respecting 
# Markdown alignment syntax (:---, :---:, ---:).
#
# The formatter is cautious: it only processes text that strictly 
# matches Markdown table structures to avoid corrupting code blocks 
# or regular text.

module G4T
  class << self

    # Scans the content for @@@table blocks and applies formatting.
    #
    # This is the primary entry point for processing an entire Markdown file.
    # It looks for blocks delimited by @@@table and @@@end.
    #
    # @param content [String] The full content of a Markdown file.
    # @return [String] The content with formatted tables.
    def process_tables(content)
      # The 'm' flag allows '.' to match newlines (multiline match)
      content.gsub(/^@@@table\n(.+?)\n@@@end\n?/m) do
        format_table($1)
      end
    end

    private

    # Formats a Markdown table string to align columns.
    #
    # It identifies the alignment from the second row (separator row) 
    # and pads each cell accordingly using space characters.
    # If the input does not look like a valid table (e.g., missing 
    # a separator row or having 4+ leading spaces), it returns 
    # the original string unchanged.
    #
    # @param table_str [String] The raw Markdown table text.
    # @return [String] The formatted table or original string.
    def format_table(table_str)
      lines = table_str.lines
      return table_str if lines.size < 2

      # --- 1. Validation ---
      # A valid Markdown table must have a separator row as the second line.
      # We allow 0-3 leading spaces. 4+ spaces would be a code block.
      # Pattern: |:---|:---:|---:|
      sep_pattern = /^\s{0,3}\|(?:\s*:?-+:?\s*\|)+\s*$/
      sep_line_idx = 1
      
      unless lines[sep_line_idx] =~ sep_pattern
        return table_str
      end

      # --- 2. Parsing ---
      # Strip each line and remove leading/trailing pipes to get raw cells.
      raw_rows = lines.map do |l|
        l.strip.sub(/^\|/, '').sub(/\|$/, '').split('|').map(&:strip)
      end

      # --- 3. Determine Alignments ---
      alignments = raw_rows[sep_line_idx].map do |cell|
        if cell.start_with?(':') && cell.end_with?(':') then :center
        elsif cell.start_with?(':') then :left
        elsif cell.end_with?(':') then :right
        else :none
        end
      end

      # --- 4. Calculate Column Widths ---
      num_cols = alignments.size
      widths = Array.new(num_cols, 0)

      raw_rows.each do |row|
        row.each_with_index do |cell, i|
          next if i >= num_cols
          widths[i] = [widths[i], cell.length].max
        end
      end

      # --- 5. Reconstruction ---
      formatted_rows = raw_rows.map.with_index do |row, idx|
        if idx == sep_line_idx
          # Regenerate separator row
          cells = row.map.with_index do |_, i|
            case alignments[i]
            when :center then ":" + "-" * (widths[i] - 2) + ":"
            when :left   then ":" + "-" * (widths[i] - 1)
            when :right  then "-" * (widths[i] - 1) + ":"
            else "-" * widths[i] # :none
            end
          end
        else
          # Align content cells using standard String methods
          cells = row.map.with_index do |cell, i|
            case alignments[i]
            when :center then cell.center(widths[i])
            when :right  then cell.rjust(widths[i])
            else cell.ljust(widths[i])
            end
          end
        end
        "|" + cells.join("|") + "|"
      end

      formatted_rows.join("\n") + "\n"
    end
  end
end