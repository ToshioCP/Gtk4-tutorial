# = LibUtils
#
# A collection of utility methods for file management, validation, and retrieval.
# This module requires PathManager to be loaded.

require 'fileutils'
require_relative '../path_manager'

module G4T
  class << self

    # Validates the sequence and format of section files in the source directory.
    #
    # It checks if files follow 'sec<int>.src.md' and form a perfect sequence starting from 1.
    # Floating point section numbers (e.g., sec1.5.src.md) are not allowed.
    # Raise RuntimeError unless the condition above is satisfied.
    #
    # @param src_dir [String] The absolute path to the directory to check.
    # @return [TrueClass] true
    def sec_files_check(src_dir: PathManager.get_path(:src))
      raise "Directory '#{src_dir}' not found" unless Dir.exist?(src_dir)

      sec_files = Dir.children(src_dir).select { |f| f =~ /\Asec\d+(\.\d+)?\.src\.md\z/ }
      # Check for any floating point numbers (e.g., sec1.1.src.md)
      raise "Decimal section numbers detected. Renumbering suggested" if sec_files.any? { |f| f =~  /\Asec\d+\.\d+\.src\.md\z/ }

      # Extract integers and sort them
      numbers = sec_files.map { |f| f.match(/\Asec(\d+)\.src\.md\z/)[1].to_i }.sort
      # Check if they form a perfect 1, 2, 3... sequence
      raise "Section sequence is broken or does not start at 1" unless numbers == (1..numbers.size).to_a

      true
    end

    private

    # Retrieves files categorized by their role in the project.
    #
    # @param src_dir [String] The absolute path to the source directory.
    # @return [Hash] A hash containing:
    #   * :all   - Array of all files (recursively, excluding directories)
    #   * :sections - Array of sec<int>.src.md sorted by number
    #   * :images - Array of all files under the 'images/' directory
    def get_src_files(src_dir:  PathManager.get_path(:src))
      all_source_files = Dir[File.join(src_dir, "**/*")].reject { |f| File.directory?(f) }
      sections = Dir[File.join(src_dir, "sec*.src.md")]
                .select{|f| File.basename(f) =~ /\Asec\d+\.src\.md\z/}
                .sort_by { |f| File.basename(f)[/\d+/].to_i }
      images = Dir[File.join(src_dir, "images/**/*")].reject { |f| File.directory?(f) }
      {
        all: all_source_files,
        sections: sections,
        images: images
      }
    end

    # Writes content to a file, automatically creating any missing parent directories.
    #
    # This method ensures that the target directory exists by using FileUtils.mkdir_p 
    # before attempting to write the file, preventing Errno::ENOENT errors when 
    # dealing with nested subdirectories.
    #
    # @param path [String] The absolute or relative path to the file to be written.
    # @param content [String] The data to be written into the file.
    # @return [Integer] The number of bytes written.
    def write_with_directory(path, content)
      FileUtils.mkdir_p(File.dirname(path))
      File.write(path, content)
    end

    # Check whether the file is an image.
    # This method only checks the extension matches JPEG, PNG or GIF patterns.
    # For more accurate checking, the 'marcel' Gem is recommended.
    def image?(file)
      # Ignore cases with i option
      file.to_s.match?(/\.(?:jpe?g|png|gif)\z/i)
    end

    # Check whether the file is a source code.
    # This method only checks the extension matches .c, .h, .lex, .ui, .xml and .y.
    def source_code?(file)
      # Ignore cases with i option
      file.to_s.match?(/\.(?:c|h|lex|ui|xml|y)\z/i)
    end
  end
end