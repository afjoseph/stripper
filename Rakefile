# frozen_string_literal: true

# rubocop:disable RedundantCopDisableDirective, MissingCopEnableDirective, LineTooLong, BlockLength, ParameterLists, MethodLength, AbcSize, IfUnlessModifier, CyclomaticComplexity, LineLength, PerceivedComplexity
require 'fileutils'

namespace_keyword = 'ROOT'
out_bin = 'stripper'

task build: %i[ROOT:build]
task clean: %i[ROOT:clean TEST:clean]
task test: %i[ROOT:clean ROOT:build TEST:clean TEST:build]
task default: %i[ROOT:clean ROOT:build]

# Import other Rakefiles
Dir.glob('**/*.rake').each { |r| import r }

namespace namespace_keyword do
  task :clean do
    puts "==> #{namespace_keyword} => TASK clean"

    `rm -rf build 2>&1`
  end

  task :build do
    puts "==> #{namespace_keyword} => TASK build"

    # Skip if we built things already
    if Dir.exist?('build')
      out_bin_path = Dir.glob("**/*#{out_bin}").map(&File.method(:realpath))[0]
      raise 'FAIL: No bin made. Clean task first' if out_bin_path.nil?

      puts "Tool already built in #{out_bin_path}"
      ENV['STRIPPER_PATH'] = out_bin_path
      next
    end

    FileUtils.mkdir_p 'build'

    puts 'Building tool...'
    compiler = 'clang' || Env['CC']
    compile_flags = '' || Env['CFLAGS']
    compile_flags += ' '
    compile_flags += ' -Wall -Wextra -pedantic -Werror -Wshadow'
    compile_flags += ' -g3 -fvisibility=hidden'
    compile_flags += ' -O3'
    linker_flags = '' || Env['LDFLAGS']
    linker_flags += ' -Iinclude/'
    sources = Dir.glob('src/*.c')
    out_bin_path = "build/#{out_bin}"

    raise 'FAIL: Build failed' unless system "#{compiler} \
            #{compile_flags} #{linker_flags} \
            #{sources.join(' ')} -o #{out_bin_path}"

    out_bin_path = Dir.glob("**/*#{out_bin}").map(&File.method(:realpath))[0]
    raise 'FAIL: No bin built' if out_bin_path.nil?

    puts "SUCCESS: tool built in #{out_bin_path}"
    ENV['STRIPPER_PATH'] = out_bin_path
  end
end
