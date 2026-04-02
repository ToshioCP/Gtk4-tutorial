# The PathManager module provides a centralized way to resolve absolute paths 
# within the project. It handles the mapping between logical directory keys 
# (like :src or :bin) and their physical locations on the file system.
#
# By using this module, you can avoid hardcoding relative paths like '../src' 
# throughout your application.

require 'pathname'

module PathManager
  # The absolute path to the project root directory.
  ROOT = __dir__.freeze

  # A map of logical directory keys to their respective folder names.
  DIR_NAMES = {
    root: '.',
    bin:  'bin',
    data: 'data',
    docs: 'docs',
    gfm:  'gfm',
    lib:  'lib',
    pdf: 'pdf',
    src:  'src',
    test: 'test'
  }.freeze

  # Returns the absolute path for a given directory key and an optional subpath.
  #
  # @param key [Symbol] The directory key (e.g., :root, :src, :bin, :lib, :test).
  # @param subpath [String] An optional file or subdirectory path to append.
  # @return [String] The full absolute path to the requested resource.
  # @raise [RuntimeError] If the provided key is not defined in DIR_NAMES
  #   or if the resolved path escapes the project root.
  def self.get_path(key, subpath = "")
    dir_name = DIR_NAMES[key] || raise(ArgumentError, "Unknown directory key: #{key}")

    base = File.join(ROOT, dir_name)
    target = Pathname(base).join(subpath.to_s).cleanpath.to_s
    unless target == ROOT || target.start_with?(ROOT + File::SEPARATOR)
      raise "Accessing #{target} not allowed"
    end

    target
  end
end