MRuby::Build.new do |conf|
  toolchain :clang
  conf.gembox 'default'
  conf.gem '../mruby-dispatch'
end
