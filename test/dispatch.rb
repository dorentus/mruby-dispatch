assert('Dispatch.once') do
  a = 1
  10.times do
    Dispatch.once { a += 1 }
  end
  assert_equal 2, a
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
  q = Dispatch::Queue.main
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

assert('Dispatch::Queue#after') do
  q = Dispatch::Queue.concurrent
  i = 1
  q.after 0.5 do
    i += 1
  end
  assert_equal(1, i)
end

assert('Dispatch::Queue#apply') do
  q = Dispatch::Queue.new "test.dispatch.apply"
  i = 1
  q.apply(11) do |n|
    i += n
  end
  assert_equal(56, i)
end

assert('Dispatch::Queue#async') do
  q = Dispatch::Queue.new "test.dispatch.async"
  i = 1
  q.async do
    i = 2
  end
  assert_equal(1, i)
end

assert('Dispatch::Queue#sync') do
  q = Dispatch::Queue.new "test.dispatch.sync"
  i = 1
  q.sync do
    i = 2
  end
  assert_equal(2, i)
end

assert('Dispatch::Queue#barrier_async') do
  q = Dispatch::Queue.new "test.dispatch.barrier_async"
  i = 1
  q.async do
    i += 1
  end
  q.barrier_async do
    i += 2
  end
  q.async do
    i += 1
  end
  assert_equal(1, i)
end if Dispatch::Queue.method_defined? :barrier_async

assert('Dispatch::Queue#barrier_sync') do
  q = Dispatch::Queue.new "test.dispatch.barrier_sync"
  i = 1
  q.sync do
    i += 1
  end
  q.barrier_sync do
    i += 2
  end
  q.sync do
    i += 1
  end
  assert_equal(5, i)
end if Dispatch::Queue.method_defined? :barrier_sync

assert('Dispatch::Object') do
  assert_true Dispatch::Queue.method_defined? :resume!
  assert_true Dispatch::Queue.method_defined? :suspend!
end
