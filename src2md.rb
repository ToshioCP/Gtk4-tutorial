#!/bin/sh
exec ruby -x "$0" "$@"
#!ruby

# src2md.rb

require_relative 'lib/lib_src2md.rb'

def usage
  $stderr.print "Usage: ruby srcd2md.rb src.md_file md_file width\n"
  $stderr.print "    The width is used to fold lines in indented or fenced code blocks.\n"
  $stderr.print "    If the width is negative, no lines are folded.\n"
end

if ARGV.size != 3
  usage
  exit 1
end

srcmd = ARGV[0]
md = ARGV[1]
width = ARGV[2]
src2md srcmd, md, width.to_i

