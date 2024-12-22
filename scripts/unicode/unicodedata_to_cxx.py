import parseunicode
import colt
from colt import frac
from colt import CodePoint
from colt import CodePointRange

try:
  # dict of abrv. str to Property
  PROPERTIES = parseunicode.parse_propertyvalue()
  # dict of name str to abrv.
  ALLALIASES = parseunicode.parse_propertyaliases()
  # dict of all
  CODEPOINTS = parseunicode.parse_unicodedata()
  
  # Break down the data as planes
  PLANES = colt.chunk(CODEPOINTS, parseunicode.MAX_PLANE)
  for i, plane in enumerate(PLANES):
    print(f"{i}: {len(plane)}, {len(list(filter(parseunicode.is_assigned_character, plane)))}")
except Exception as E:
  print(f"Error: Exception \"{str(E)}\"")

input("Press Enter to close...")