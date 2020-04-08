# frozen_string_literal: true

# rubocop:disable RedundantCopDisableDirective, MissingCopEnableDirective, LineTooLong, BlockLength, ParameterLists, MethodLength, AbcSize, IfUnlessModifier, CyclomaticComplexity, LineLength, PerceivedComplexity

require 'fileutils'

namespace_keyword = 'TEST'
namespace_dir = 'test'
out_bin = 'test'

namespace namespace_keyword do
  task :clean do
    puts "==> #{namespace_keyword} => TASK clean"

    Dir.chdir namespace_dir do
      `rm -rf build 2>&1`
    end
  end

  task build: %i[ROOT:build] do
    puts "==> #{namespace_keyword} => TASK build"

    next if Dir.exist?("#{namespace_keyword}/build")

    Dir.chdir namespace_dir do
      FileUtils.mkdir_p 'build'

      puts 'Building test...'
      compiler = 'clang' || Env['CC']
      sources = Dir.glob('src/*.c')
      out_bin_path = "build/#{out_bin}"

      raise 'FAIL: Build failed' unless system "#{compiler} \
      #{sources.join(' ')} -o #{out_bin_path}"

      out_bin_path = Dir.glob("**/*#{out_bin}").map(&File.method(:realpath))[0]
      raise 'FAIL: No bin built' if out_bin_path.nil?
      raise 'FAIL: stripper not built properly' if ENV['STRIPPER_PATH'].nil?

      # Test
      raise 'FAIL: Test failed' unless system "#{ENV['STRIPPER_PATH']} #{out_bin_path}"

      readelf_out = `readelf --sections #{out_bin_path}`
      raise 'FAIL: sections found in test' unless readelf_out.include? 'no sections'

      puts 'SUCCESS: test passed'
    end
  end
end
