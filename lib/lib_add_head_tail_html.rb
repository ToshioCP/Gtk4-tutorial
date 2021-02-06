# lib_add_head_tail_html.rb
#   add header and tail to body (html)

def add_head_tail_html html_file

head=<<'EOS'
<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

<style type="text/css">
<!--
    body {width: 1080px; margin: 0 auto; font-size: large;}
    h2 {padding: 10px; background-color: #d0f0d0; }
    pre { margin: 10px; padding: 16px 10px 8px 10px; border: 2px solid silver; background-color: ghostwhite; overflow-x:scroll}
-->
</style>

    <title>gtk4 tutorial</title>
</head>
<body>
EOS

tail=<<'EOS'
</body>
</html>
EOS

  body = File.read(html_file)
  File.write(html_file, head+body+tail)
end 
