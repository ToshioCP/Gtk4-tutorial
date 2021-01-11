#!/bin/sh
exec ruby -x "$0" "$@"
#!ruby

# src2md.rb

require_relative 'lib/lib_src2md.rb'

def usage
  $stderr.print "Usage: ruby srcd2md.rb src.md_file md_file\n"
end

if ARGV.size != 2
  usage
  exit 1
end

srcmd = ARGV[0]
md = ARGV[1]
src2md srcmd, md, 80

