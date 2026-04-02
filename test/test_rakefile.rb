# frozen_string_literal: true

require 'minitest/autorun'
require 'minitest/mock'
require 'rake'
require 'fileutils'
require_relative '../path_manager'

# Enable metadata recording to capture task descriptions (desc) during testing.
Rake::TaskManager.record_task_metadata = true

# Tests for the project's main Rakefile to ensure tasks are correctly defined
# and their dependencies are properly established.
class TestRakefile < Minitest::Test
  # Sets up a fresh Rake application instance and loads the project Rakefile
  # before each test case.
  def setup
    @rake = Rake::Application.new
    Rake.with_application(@rake) do
      # Load the Rakefile using its absolute path.
      rakefile_path = PathManager.get_path(:root, 'Rakefile')
      load rakefile_path
    end
  end

  # Verifies that all primary tasks are defined with their expected descriptions.
  def test_tasks_existence_and_description
    tasks = {
      'gfm'     => "Build GitHub Flavored Markdown (GFM) and update README.md",
      'html'    => "Build HTML documentation in the /docs directory",
      'pdf'     => "Build a single PDF document in the /pdf directory",
      'clobber' => "Remove the generated PDF directory safely"
    }

    tasks.each do |name, expected_desc|
      task = @rake[name]
      assert_instance_of Rake::Task, task
      assert_equal expected_desc, task.comment
    end
  end

  # Verifies that build tasks correctly depend on the 'common_check' task.
  def test_task_dependencies
    %w[gfm html pdf].each do |name|
      task = @rake[name]
      assert_includes task.prerequisites, 'common_check', "Task #{name} should depend on common_check"
    end
  end

  # Verifies that the default task is set to 'gfm'.
  def test_default_task
    assert_includes @rake['default'].prerequisites, 'gfm'
  end

  # Validates the execution of the 'clobber' task.
  # This ensures the task can be invoked without exceptions even when 
  # the target directory does not exist.
  def test_clobber_task_definition
    # Ensure a clean state for the test environment.
    # Note: For production-level testing, consider mocking the G4T module 
    # to avoid actual file system modifications.
    
    # Assert that the task can be invoked successfully.
    FileUtils.stub(:remove_entry_secure, ->(file){puts file}) do
      out, err = capture_io do
        @rake['clobber'].invoke
      end
      s1 = "No PDF directory found to remove."
      s2 = PathManager.get_path(:pdf)
      assert_match Regexp.union(s1, s2), out
    end
  end
end