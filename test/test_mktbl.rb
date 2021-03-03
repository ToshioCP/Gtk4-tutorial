# test_mktbl.rb
include Math

mktbl = "../src/mktbl.rb"
raise "No such file #{mktbl}." unless File.exist? mktbl

tbl = <<EOS
This is a test file for \"mktbl.rb\".
The following is a trigonometry table from 0 to 10 degrees.

@@@table
|angle|sine|cosine|tangent|
|-:|:-|:-|:-|
|0|#{sin 0}|#{cos 0}|#{tan 0}|
|1|#{sin 1*PI/180}|#{cos 1*PI/180}|#{tan 1*PI/180}|
|2|#{sin 2*PI/180}|#{cos 2*PI/180}|#{tan 2*PI/180}|
|3|#{sin 3*PI/180}|#{cos 3*PI/180}|#{tan 3*PI/180}|
|4|#{sin 4*PI/180}|#{cos 4*PI/180}|#{tan 4*PI/180}|
|5|#{sin 5*PI/180}|#{cos 5*PI/180}|#{tan 5*PI/180}|
|6|#{sin 6*PI/180}|#{cos 6*PI/180}|#{tan 6*PI/180}|
|7|#{sin 7*PI/180}|#{cos 7*PI/180}|#{tan 7*PI/180}|
|8|#{sin 8*PI/180}|#{cos 8*PI/180}|#{tan 8*PI/180}|
|9|#{sin 9*PI/180}|#{cos 9*PI/180}|#{tan 9*PI/180}|
|10|#{sin 10*PI/180}|#{cos 10*PI/180}|#{tan 10*PI/180}|
@@@

The table is made by Math module.
EOS

tbl_expected = <<'EOS'
This is a test file for "mktbl.rb".
The following is a trigonometry table from 0 to 10 degrees.

|angle|sine               |cosine            |tangent             |
|----:|:------------------|:-----------------|:-------------------|
|    0|0.0                |1.0               |0.0                 |
|    1|0.01745240643728351|0.9998476951563913|0.017455064928217585|
|    2|0.03489949670250097|0.9993908270190958|0.03492076949174773 |
|    3|0.05233595624294383|0.9986295347545738|0.052407779283041196|
|    4|0.0697564737441253 |0.9975640502598242|0.06992681194351041 |
|    5|0.08715574274765817|0.9961946980917455|0.08748866352592401 |
|    6|0.10452846326765346|0.9945218953682733|0.10510423526567646 |
|    7|0.12186934340514748|0.992546151641322 |0.1227845609029046  |
|    8|0.13917310096006544|0.9902680687415704|0.14054083470239145 |
|    9|0.15643446504023087|0.9876883405951378|0.15838444032453627 |
|   10|0.17364817766693033|0.984807753012208 |0.17632698070846498 |

The table is made by Math module.
EOS

File.write "test.txt", tbl

system "ruby", mktbl, "test.txt"

tbl_generated = File.read "test.txt"
raise "test_mktbl: Generated file is different from expected one." unless tbl_generated == tbl_expected
tbl_backup = File.read "test.txt.bak"
raise "test_mktbl: Bakup file is different from the original one." unless tbl_backup == tbl
File.delete "test.txt"
File.delete "test.txt.bak"


