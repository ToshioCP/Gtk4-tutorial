require 'fileutils'
include FileUtils

def cp_images srcmdfiles, dst_dir
  mkdir_p(dst_dir) unless Dir.exist?(dst_dir)
  images = srcmdfiles.map do |file|
    d = File.dirname(file)
    f = File.read(file)
    f = f.gsub(/^    .*\n/,'')
    f = f.gsub(/^~~~.*?^~~~\n/m,'')
    imgs = f.scan(/!\[.*?\]\((.*?)\)/).flatten.uniq
    imgs.map{|img| File.absolute_path("#{d}/#{img}")}
  end
  images = images.flatten.sort.uniq
  images.each do |src|
    dst = "#{dst_dir}/#{File.basename(src)}"
    cp(src, dst)
  end
end
