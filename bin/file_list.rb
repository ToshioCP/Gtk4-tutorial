# = G4T File List Utility
#
# This script provides various ways to list files within a source directory.
# It can output full pathnames, unique filenames, or unique extensions.

require 'pathname'
require_relative '../path_manager'

# Returns an array of all file pathnames under the given directory.
#
# @param src_dir [String] The directory to search.
# @return [Array<String>] Sorted list of all file paths.
def collect_pathnames(src_dir)
  unless Dir.exist?(src_dir)
    warn "Error: Directory '#{src_dir}' not found."
    return []
  end
  # Use sort to ensure consistent output
  Dir.glob(File.join(src_dir, "**", "*")).select { |f| File.file?(f) }.sort
end

# Returns a sorted array of unique filenames from the given directory.
#
# @param src_dir [String] The directory to search.
# @return [Array<String>] Sorted unique filenames.
def collect_filenames(src_dir)
  files = collect_pathnames(src_dir).map { |path| File.basename(path) }
  files.uniq.sort
end

# Returns a sorted array of unique extensions (or filenames if no extension).
#
# @param src_dir [String] The directory to search.
# @return [Array<String>] Sorted unique extensions or filenames.
def collect_extensions(src_dir)
  files = collect_pathnames(src_dir).map { |path| File.basename(path) }
  extensions = files.map do |file|
    ext = File.extname(file)
    ext.empty? ? file : ext
  end
  extensions.uniq.sort
end

# Displays the usage information for this script.
def usage
  default_src = PathManager.get_path(:src)
  puts "Usage: ruby file_list.rb [options] [src_directory]"
  puts "Options:"
  puts "  -p, --path       List all file pathnames"
  puts "  -f, --file       List unique filenames"
  puts "  -e, --extension  List unique extensions (or filenames if none)"
  puts "  -h, --help       Show this help message"
  puts "\nDefault src_directory: '#{default_src}'"
  puts "If no options are provided, it defaults to listing filenames (-f)."
end

# --- Argument Processing ---

if __FILE__ == $0
  option = ARGV.find { |arg| arg.start_with?("-") }
  src_dir = ARGV.reject { |arg| arg.start_with?("-") }.first
  src_dir ||= PathManager.get_path(:src)

  if option == "-h" || option == "--help"
    usage
    exit
  end

  # Determine which list to generate based on the option
  case option
  when "-p", "--path"
    list = collect_pathnames(src_dir)
  when "-f", "--file"
    list = collect_filenames(src_dir)
  when "-e", "--extension"
    list = collect_extensions(src_dir)
  when nil
    # Default behavior when no option is given
    list = collect_filenames(src_dir)
  else
    warn "Unknown option: #{option}"
    usage
    exit 1
  end

  # Output the result
  list.each { |item| puts item }
end