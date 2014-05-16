require File.expand_path('../mrblib/version.rb', __FILE__)

MRuby::Gem::Specification.new('mruby-dispatch') do |spec|
  spec.license = 'MIT'
  spec.authors = 'ZHANG Yi'
  spec.version = Dispatch::VERSION
  spec.linker.libraries += %w(dispatch BlocksRuntime) unless RUBY_PLATFORM =~ /darwin/
  spec.cc.flags << '-fblocks'
end
