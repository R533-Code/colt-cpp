import parseunicode
import colt
from colt import frac
from colt import CodePoint
from colt import CodePointRange
import time

def time_fn(fn, *args, **kwargs):
  start = time.time()
  ret = fn(*args, **kwargs)
  end = time.time()
  print(f"{fn.__name__: <24}: {end - start:.2f}s")
  return ret

try:
  # dict of name str to abrv.
  ALLALIASES = time_fn(parseunicode.parse_propertyaliases)
  # dict of abrv. str to Property
  PROPERTIES = time_fn(parseunicode.parse_propertyvalue)
  # list of all properties
  PROPVALUES = time_fn(parseunicode.parse_properties)
  # dict of all
  CODEPOINTS = time_fn(parseunicode.parse_unicodedata)
  
  # Break down the data as planes
  PLANES = colt.chunk(CODEPOINTS, parseunicode.MAX_PLANE)
  for i, plane in enumerate(PLANES):
    print(f"{i}: {len(plane)}, {len(list(filter(parseunicode.is_assigned_character, plane)))}")
except Exception as E:
  print(f"Error: Exception \"{str(E)}\"")

input("Press Enter to close...")