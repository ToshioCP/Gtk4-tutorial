# = Renumber Module
#
# This module provides functionality to re-index section files and maintain 
# internal link consistency. It is designed to work with files named 
# 'sec<num>.src.md' within a designated source directory.
#
# == Key Features
# * **Two-Phase Renaming**: Uses temporary file names to prevent collisions 
#   during index shifts.
# * **Link Integrity**: Automatically updates Markdown links such as 
#   '[Section 1.5](sec1.5.src.md)' to their new integer-based counterparts.

require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_utils')

module G4T
  # Executes the full renumbering and link updating process.
  #
  # The method first validates the current state using +sec_files_check+. If decimal 
  # versions (floats) exist, it collapses them into a sequential integer 
  # sequence (1, 2, 3...).
  #
  # @param verbose [Boolean] Whether to output progress messages to $stdout.
  # @return [Boolean] Returns true if the process completed or no action was needed; 
  #                   returns false if a fatal error (like directory not found) occurred.
  def self.renumber(verbose: true)
    # --- 1. Pre-validation ---
    src_dir = PathManager.get_path(:src)
    begin
      status = self.sec_files_check
    rescue RuntimeError
      if status == :ok
        return true
      end
    end

    raise "Directory '#{src_dir}' not found" unless Dir.exist?(src_dir)

    # --- 2. Extraction and Sorting ---
    # Sort files numerically based on the embedded number (sec1, sec2, sec2.5, sec10...)
    secfiles = Dir.children(src_dir).sort_by { |f| f[/\d+(\.\d+)?/].to_f }

    # --- 3. Build Mapping and Replacement Rules ---
    rename_map = {}
    replace_map = {}

    secfiles.each_with_index do |old_name, idx|
      new_idx = idx + 1
      new_name = "sec#{new_idx}.src.md"
      
      old_num = old_name[/\d+(\.\d+)?/]
      old_link = "[Section #{old_num}](#{old_name})"
      new_link = "[Section #{new_idx}](#{new_name})"

      unless old_name == new_name
        rename_map[old_name] = new_name 
        replace_map[old_link] = new_link
      end
    end

    return true if rename_map.empty?

    # --- 4. Two-Phase File Renaming ---
    puts "Renaming files..." if verbose
    temp_prefix = "temp_"
    
    # Phase A: Move to temporary names to avoid "File already exists" errors
    rename_map.each do |old, new|
      File.rename File.join(src_dir, old), File.join(src_dir, "#{temp_prefix}#{new}")
    end
    # Phase B: Move to final sequential names
    rename_map.each do |old, new|
      File.rename File.join(src_dir, "#{temp_prefix}#{new}"), File.join(src_dir, new)
    end

    # --- 5. Content Link Update ---
    puts "Updating links in files..." if verbose

    # 二重置換を避けるため、正規表現でリンクのパターンを一度に探し、ハッシュで置換する
    # パターン例: [Section 1.5](sec1.5.src.md)
    link_pattern = /\[Section \d+(?:\.\d+)?\]\(sec\d+(?:\.\d+)?\.src\.md\)/

    Dir.children(src_dir).each do |file|
      path = File.join(src_dir, file)
      content = File.read(path)
      
      # gsub にブロックを渡すと、マッチした箇所ごとに「一度だけ」置換を行う
      # これにより、置換済みの文字列が次のループで再置換されるのを防ぐ
      changed = false
      new_content = content.gsub(link_pattern) do |matched|
        if replace_map.key?(matched)
          changed = true
          replace_map[matched]
        else
          matched
        end
      end

      File.write(path, new_content) if changed
    end

    puts "Done! Successfully renumbered #{rename_map.size} files and updated links." if verbose
    true
  end
end