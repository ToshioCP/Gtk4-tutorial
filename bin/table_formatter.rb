#!/usr/bin/env ruby

# = Table Formatting Execution Script
#
# This script processes Markdown files to beautify tables wrapped in
# @@@table ... @@@end directives. It overwrites the original files
# with the formatted content.
#
# == Usage
#   $ ./bin/table_formatter.rb file1.src.md file2.src.md ...
#
# == Exit Codes
# * 0: Success
# * 1: Error (File not found or other issues)

require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_table_formatter')

if ARGV.empty?
  warn "Usage: #{$0} <file1.src.md> [file2.src.md ...]"
  exit 1
end

exit_status = 0

ARGV.each do |file_path|
  unless File.exist?(file_path)
    warn "Error: File not found - #{file_path}"
    exit_status = 1
    next
  end

  begin
    content = File.read(file_path)
    # Apply table formatting logic
    formatted_content = G4T.process_tables(content)

    if content == formatted_content
      puts "No changes needed: #{file_path}"
    else
      File.write(file_path, formatted_content)
      puts "Formatted: #{file_path}"
    end
  rescue => e
    warn "Error processing #{file_path}: #{e.message}"
    exit_status = 1
  end
end

exit exit_status