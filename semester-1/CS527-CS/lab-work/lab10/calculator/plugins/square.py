def register(ops):
  def square(a):
    return a * a
  
  ops["square"] = square