#!/bin/sh
exec ruby -x "$0" "$@"
#!ruby

# src2md.rb

require_relative 'lib/lib_src2md.rb'

def usage
  $stderr.print "Usage: ruby srcd2md.rb XXXX.src.md type\n"
  $stderr.print "       XXXX.src.md is a .src.md file, which is like sec1.src.md.\n"
  $stderr.print "       type is gfm (default), html or latex.\n"
  $stderr.print "src2md.rb converts XXXX.src.md into (gfm|docs|latex)/XXXX.md.\n"
end

if ARGV.size == 1
  src2md ARGV[0], "gfm"
elsif ARGV.size == 2
  src2md ARGV[0], ARGV[1]
else
  usage
  exit 1
end
