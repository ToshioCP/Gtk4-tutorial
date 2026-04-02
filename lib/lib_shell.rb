# = G4T Shell Execution Module
#
# This module provides the functionality to execute shell commands 
# and include both the command line and its output in Markdown.

require_relative '../path_manager'

module G4T
  # Processes @@@shell directives to execute commands and import their output.
  #
  # @param str [String] The 3-line block starting with @@@shell.
  # @return [String] A formatted Markdown fenced code block (no info string).
  def self.include_shell(str)
    lines = str.lines
    return "" unless lines.size == 3
    return "" unless lines.first.strip == "@@@shell" && lines.last.strip == "@@@"

    src_dir = PathManager.get_path(:src)
    cmd = lines[1].strip
    
    # Execute the command and capture stdout & stderr in the source directory
    result = `cd #{src_dir} && #{cmd} 2>&1`.chomp+"\n"
    
    # Merge command prompt ($) and the result
    content = "$ #{cmd}\n#{result}$\n"
    
    # Return a clean fenced code block without an info string (no syntax highlighting)
    "```\n#{content}```\n"
  end
end