# = Rakefile for Gtk4-tutorial Project (G4T)
#
# This Rakefile automates the conversion of Markdown source files into 
# multiple formats: GFM (GitHub Flavored Markdown), HTML, and PDF.
# It ensures data integrity by validating section numbering before execution.

require "webrick"
require_relative 'path_manager'
require PathManager.get_path(:lib, 'lib_utils')
require PathManager.get_path(:lib, 'lib_src2gfm')
require PathManager.get_path(:lib, 'lib_src2html')
require PathManager.get_path(:lib, 'lib_src2pdf')

# == Internal Helper Tasks

# Validates that source section files follow a strict natural number sequence (1, 2, 3...).
# If decimal numbers are found or the sequence is broken, it suggests renumbering and exits.
task :common_check do
  begin
    # (1) Check if section files are numbered sequentially starting from 1.
    G4T.sec_files_check
  rescue => e
    # If validation fails, provide a suggestion to the user and terminate.
    puts "Error: #{e.message}"
    puts "Suggested action: Run 'ruby bin/renumber.rb' to fix the file indices."
    exit 1
  end
end

# == Primary Build Tasks

desc "Build GitHub Flavored Markdown (GFM) and update README.md"
# (2) Calls G4T.src2gfm to convert .src.md files to .md in the /gfm directory.
# (3) Converts index.src.md to README.md in the project root.
task :gfm => :common_check do
  puts "Target: GFM (GitHub Flavored Markdown)"
  G4T.src2gfm
  puts "Conversion to GFM completed successfully."
end

desc "Build HTML documentation in the /docs directory"
# (2) Calls G4T.src2html to generate standalone HTML files from source.
# (3) Converts index.src.md to docs/index.html.
task :html => :common_check do
  puts "Target: HTML documentation"
  G4T.src2html
  puts "Conversion to HTML completed successfully."
end

desc "Launch a local web server to preview the HTML documentation"
# (1) Ensures HTML files are built before starting the server.
# (2) Uses Ruby's 'un' library to launch a thin HTTP server on port 8000.
task :serve do
  puts "Press Ctrl+C to stop the server."
  server = WEBrick::HTTPServer.new(
    Port: 8000,
    DocumentRoot: "docs"
  )
  trap("INT") { server.shutdown }
  server.start
end

desc "Build a single PDF document in the /pdf directory"
# (2) Calls G4T.src2pdf to generate Gtk4-tutorial.pdf using Pandoc and LuaLaTeX.
# (3) Intermediate content is handled via pipes, so no temporary files are left behind.
task :pdf => :common_check do
  puts "Target: PDF document"
  G4T.src2pdf
  puts "PDF generation completed successfully."
end

# == Maintenance Tasks

desc "Remove the generated PDF directory safely"
task :clobber do
  pdf_path = PathManager.get_path(:pdf)
  if Dir.exist?(pdf_path)
    puts "Removing directory safely: #{pdf_path}"
    # Use remove_entry_secure for enhanced security over rm_rf
    FileUtils.remove_entry_secure(pdf_path)
  else
    puts "No PDF directory found to remove."
  end
end

# Run all the tasks.
task :all => [:gfm, :html, :pdf]

# Default task:
task :default => :gfm