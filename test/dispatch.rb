assert('Dispatch.once') do
  a = 1
  10.times do
    Dispatch.once { a += 1 }
  end
  assert_equal a, 2
end

assert('Dispatch::Queue#to_s with label') do
  label = (0...8).map { (65 + rand(26)).chr }.join
  q = Dispatch::Queue.new label
  assert_equal(label, q.to_s)
end

assert('Dispatch::Queue#to_s without label') do
  q = Dispatch::Queue.new
  assert_equal("", q.to_s)
end
