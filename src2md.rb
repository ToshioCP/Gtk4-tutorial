#!/bin/sh
exec ruby -x "$0" "$@"
#!ruby

# src2md.rb

require_relative 'lib/lib_src2md.rb'

def usage
  $stderr.print "Usage: ruby srcd2md.rb src.md_file md_file type\n"
  $stderr.print "       type is gfm (default), html or latex.\n"
end

if ARGV.size == 2
  src2md ARGV[0], ARGV[1], "gfm"
elsif ARGV.size == 3
  src2md ARGV[0], ARGV[1], ARGV[2]
else
  usage
  exit 1
end
