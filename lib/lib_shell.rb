# = G4T Shell Execution Module
#
# This module provides the functionality to execute shell commands 
# and include both the command line and its output in Markdown.
# It ensures all commands are executed within the 'src' directory.

require_relative '../path_manager'
require 'open3'
require 'shellwords'

module G4T
  class << self

    private
    
    # Processes @@@shell directives to execute commands and import their output.
    # Expects a 3-line block:
    # @@@shell
    # command
    # @@@
    #
    # @param str [String] The 3-line block starting with @@@shell.
    # @return [String] A formatted Markdown fenced code block.
    def include_shell(str)
      lines = str.lines
      unless lines.size == 3 && lines.first.rstrip == "@@@shell" && lines.last.rstrip == "@@@"
        raise "Syntax Error in @@@shell command."
      end
      cmd_line = lines[1].strip
      args = Shellwords.split(cmd_line)

      src_dir = File.expand_path(PathManager.get_path(:src))
      src_prefix = File.join(src_dir, '')
      args.each do |arg|
        next if arg.start_with?('-')
        abs_path = File.expand_path(arg, src_dir)
        unless abs_path == src_dir || abs_path.start_with?(src_prefix)
          raise "Error: Access to path outside 'src' is forbidden: #{arg}"
        end
      end

      # Execute the command inside the src directory
      # This may raise exceptions (e.g., Errno::ENOENT) if the command fails to spawn.
      # Fatal errors will terminate the script as intended.
      result, _status = Open3.capture2e(cmd_line, chdir: src_dir)
      result_str = result.to_s.empty? ? "" : result.chomp + "\n"

      "```\n$ #{cmd_line}\n#{result_str}$\n```\n"
    end
  end
end