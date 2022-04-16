require 'minitest/autorun'
require 'fileutils'
require 'diff/lcs'
require_relative "../lib/lib_gen_main_tex.rb"

class Test_lib_src_file < Minitest::Test
  include FileUtils

  def test_gen_main_tex
    expected_main_tex = <<~'EOS'
    \documentclass[a4paper]{article}
    \include{helper.tex}
    \title{Gtk4 tutorial for beginners}
    \author{Toshio Sekiya}
    \date{}
    \begin{document}
    \maketitle
    \begin{center}
    \textbf{abstract}
    \end{center}
    \input{abstract.tex}
    \newpage
    \tableofcontents
    \newpage
      \input{sec1.tex}
      \input{sec2.tex}
      \input{sec3.tex}
    \newpage
    \appendix
      \input{appendix1.tex}
      \input{appendix2.tex}
    \end{document}
    EOS
    expected_helper_tex = <<~'EOS'
    % Options for packages loaded elsewhere
    \PassOptionsToPackage{unicode}{hyperref}
    \PassOptionsToPackage{hyphens}{url}
    %
    \usepackage{lmodern}
    \usepackage{amssymb,amsmath}
    \usepackage{ifxetex,ifluatex}
    \ifnum 0\ifxetex 1\fi\ifluatex 1\fi=0 % if pdftex
      \usepackage[T1]{fontenc}
      \usepackage[utf8]{inputenc}
      \usepackage{textcomp} % provide euro and other symbols
    \else % if luatex or xetex
      \usepackage{unicode-math}
      \defaultfontfeatures{Scale=MatchLowercase}
      \defaultfontfeatures[\rmfamily]{Ligatures=TeX,Scale=1}
    \fi
    % Use upquote if available, for straight quotes in verbatim environments
    \IfFileExists{upquote.sty}{\usepackage{upquote}}{}
    \IfFileExists{microtype.sty}{% use microtype if available
      \usepackage[]{microtype}
      \UseMicrotypeSet[protrusion]{basicmath} % disable protrusion for tt fonts
    }{}
    \makeatletter
    \@ifundefined{KOMAClassName}{% if non-KOMA class
      \IfFileExists{parskip.sty}{%
        \usepackage{parskip}
      }{% else
        \setlength{\parindent}{0pt}
        \setlength{\parskip}{6pt plus 2pt minus 1pt}}
    }{% if KOMA class
      \KOMAoptions{parskip=half}}
    \makeatother
    \usepackage{xcolor}
    \IfFileExists{xurl.sty}{\usepackage{xurl}}{} % add URL line breaks if available
    \IfFileExists{bookmark.sty}{\usepackage{bookmark}}{\usepackage{hyperref}}
    \hypersetup{
      hidelinks,
      pdfcreator={LaTeX via pandoc}}
    \urlstyle{same} % disable monospaced font for URLs
    \usepackage{listings}
    \newcommand{\passthrough}[1]{#1}
    \lstset{defaultdialect=[5.3]Lua}
    \lstset{defaultdialect=[x86masm]Assembler}
    \usepackage{longtable,booktabs}
    % Correct order of tables after \paragraph or \subparagraph
    \usepackage{etoolbox}
    \makeatletter
    \patchcmd\longtable{\par}{\if@noskipsec\mbox{}\fi\par}{}{}
    \makeatother
    % Allow footnotes in longtable head/foot
    \IfFileExists{footnotehyper.sty}{\usepackage{footnotehyper}}{\usepackage{footnote}}
    \makesavenoteenv{longtable}
    \setlength{\emergencystretch}{3em} % prevent overfull lines
    \providecommand{\tightlist}{%
      \setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}}
    \usepackage[margin=2.4cm]{geometry}
    \usepackage{graphicx}
    \lstdefinelanguage[]{turtle}{
      keywords={pu, pd, pw, fd, tr, bc, fc, if, rt, rs, dp},
      comment=[l]\#
    }
    [keywords, comments]
    \lstset {
      extendedchars=true,
      basicstyle=\small\ttfamily,
      keywordstyle=\color{red},
      commentstyle=\color{gray},
      stringstyle=\color{blue},
      breaklines=true,
      breakatwhitespace=true
    }
    EOS
    abstractfile = "abstract.tex"
    texfiles = ["sec1.tex", "sec2.tex", "sec3.tex"]
    appendixfiles = ["appendix1.tex", "appendix2.tex"]
    directory = "temp_"+Time.now.to_f.to_s.gsub(/\./,'')
    Dir.mkdir(directory) unless Dir.exist?(directory)
    gen_main_tex(directory, abstractfile, texfiles, appendixfiles)
    actual_main_tex = File.read("#{directory}/main.tex")
    actual_helper_tex = File.read("#{directory}/helper.tex")
    remove_entry_secure(directory)
    # If you want to see the difference
    # Diff::LCS.diff(expected_main_tex.each_line.to_a, actual_main_tex.each_line.to_a).each do |array|
    #   array.each do |change|
    #     print "#{change.action}  #{change.position.to_s.chomp}: #{change.element.to_s.chomp}\n"
    #   end
    # end
    # Diff::LCS.diff(expected_helper_tex.each_line.to_a, actual_helper_tex.each_line.to_a).each do |array|
    #   array.each do |change|
    #     print "#{change.action}  #{change.position.to_s.chomp}: #{change.element.to_s.chomp}\n"
    #   end
    # end
    assert_equal expected_main_tex, actual_main_tex
    assert_equal expected_helper_tex, actual_helper_tex
  end
end
