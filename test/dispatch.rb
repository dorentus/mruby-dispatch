assert('Dispatch.once') do
  a = 1
  10.times do
    Dispatch.once { a += 1 }
  end
  assert_equal a, 2
end

assert('Dispatch::Queue#to_s [1]') do
  label = (0...8).map { (65 + rand(26)).chr }.join
  q = Dispatch::Queue.new label
  assert_equal(label, q.to_s)
end

assert('Dispatch::Queue#to_s [2]') do
  q = Dispatch::Queue.new
  assert_equal("", q.to_s)
end

assert('Dispatch::Queue.current') do
  q = Dispatch::Queue.current
  assert_equal(Dispatch::Queue, q.class);
end

assert('Dispatch::Queue.main') do
  q = Dispatch::Queue.current
  assert_equal(Dispatch::Queue, q.class);
end

assert('Dispatch::Queue.concurrent [1]') do
  %w(high default low background).each do |priority|
    q = Dispatch::Queue.concurrent priority.to_sym
    assert_equal(Dispatch::Queue, q.class)
  end
end

assert('Dispatch::Queue.concurrent [2]') do
  q1 = Dispatch::Queue.concurrent :default
  q2 = Dispatch::Queue.concurrent
  assert_equal(q1.to_s, q2.to_s)
end
