assert('Dispatch') do
  assert_equal Dispatch.class, Module
end

assert('Dispatch.once') do
  a = 1
  10.times do
    Dispatch.once { a += 1 }
  end
  assert_equal a, 2
end
