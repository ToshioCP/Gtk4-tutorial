#!/usr/bin/env ruby
# = Renumber Execution Script
#
# This script executes the renumbering process for section files 
# within the project. It integrates +PathManager+ for directory 
# resolution and the +Renumber+ module for the core logic.
#
# == Usage
#   $ ruby bin/renumber.rb
#
# == Exit Codes
# * 0: Success
# * 1: Failure (Validation error or fatal error)

require_relative '../path_manager'
require PathManager.get_path(:lib, 'lib_renumber')

# Start the renumbering process.
# verbose: true ensures progress messages are printed to $stdout.
if G4T.renumber(verbose: true)
  exit 0
else
  warn "Error: Renumbering failed."
  exit 1
end