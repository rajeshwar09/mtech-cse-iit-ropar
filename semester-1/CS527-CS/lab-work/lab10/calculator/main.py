import argparse
import importlib
import inspect
import pkgutil
import sys

OPS = {}

def register_core_ops():
  def add(a, b): return a + b
  def sub(a, b): return a - b
  def mul(a, b): return a * b
  def div(a, b): 
    if b == 0:
      raise ZeroDivisionError("Division by zero")
    return a / b

  OPS["add"] = add
  OPS["sub"] = sub
  OPS["mul"] = mul
  OPS["div"] = div


def load_plugins(package_name = "plugins"):
  try:
    package = importlib.import_module(package_name)
  except ModuleNotFoundError:
    return

  for finder, modname, ispkg in pkgutil.iter_modules(package.__path__, package.__name__ + "."):
    module = importlib.import_module(modname)

    if hasattr(module, "register") and callable(module.register):
      module.register(OPS)


def build_parser():
  parser = argparse.ArgumentParser(description="Pluggable calculator. Example: python main.py add 3 5")
  parser.add_argument("op", help="operation to perform (add, sub, mul, div, ...plugins)")
  parser.add_argument("numbers", nargs="*", type=float, help="operands; count depend on op")
  parser.add_argument("--list", action="store_true", help="list available oparations and exit")

  return parser

def check_arity(func, provided_args):
  sig = inspect.signature(func)
  params = [p for p in sig.parameters.values() if p.kind in (p.VAR_POSITIONAL, p.POSITIONAL_OR_KEYWORD) and p.default is p.empty]

  has_varargs = any(p.kind == p.VAR_POSITIONAL for p in sig.parameters.values())

  if has_varargs:
    return True, None
  
  required = len(params)

  return (len(provided_args) == required, required)

def main(argv = None):
  register_core_ops()
  load_plugins()

  parser = build_parser()
  args = parser.parse_args(argv)

  if args.list:
    print("Available operations:")
    for name in sorted(OPS.keys()):
      print("  -", name)
    sys.exit(0)
  
  if args.op not in OPS:
    print(f"Unknown operations: {args.op}")
    print("use --list to see all operations")
    sys.exit(1)
  
  func = OPS[args.op]

  ok, required = check_arity(func, args.numbers)

  if not ok:
    need = required if required is not None else "variable"
    print(f"Arity error: op '{args.op}' needs {need} argument(s).")
    sys.exit(2)
  
  try:
    result = func(*args.numbers)
  except Exception as e:
    print("Error")
    sys.exit(3)
  
  print(result)

if __name__ == "__main__":
  main()

