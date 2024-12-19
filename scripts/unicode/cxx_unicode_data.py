import os
import typing
from fractions import Fraction as fra

PATH_THIS_FILE = __file__
PATH_UCD       = "../../resources/UCD/"
PATH_OUTPUT    = "../../include/colt/unicode/gen/"

def hex_or_none(to_conv: str)->int|None:
  if len(to_conv) == 0:
    return None
  return int(to_conv, 16)

def hex(to_conv: str)->int:
  return int(to_conv, 16)

def fra_or_none(to_conv: str)->fra|None:
  if len(to_conv) == 0:
    return None
  return fra(to_conv)

try:
  # See https://www.unicode.org/reports/tr44/#UnicodeData.txt
  with open(PATH_UCD + 'UnicodeData.txt', 'r') as UnicodeData:
    for line in UnicodeData:
      # skip comments
      line = line.strip()
      if line.startswith('#'):
        continue
      split = [i.strip() for i in line.split(';')]
            
      CODE_POINT : int = hex(split[0]) # The code point number (as int)
      NAME       : str = str(split[1]) # The name (can be empty!)
      CATEGORY   : str = str(split[2]) # See https://www.unicode.org/reports/tr44/#General_Category_Values
      
      DECIMAL : int|None = hex_or_none(split[6]) # [0-9] or empty
      DIGIT   : int|None = hex_or_none(split[7]) # [0-9] or empty
      NUMERIC : fra|None = fra_or_none(split[8]) # [+-][0-9]+(/[0-9]) or empty
      
      SIMPLE_UPPERCASE_MAPPING : int|None = hex_or_none(split[12]) # Uppercase code point number or empty
      SIMPLE_LOWERCASE_MAPPING : int|None = hex_or_none(split[13]) # Lowercase code point number or empty
      SIMPLE_TITLECASE_MAPPING : int|None = hex_or_none(split[14]) or SIMPLE_UPPERCASE_MAPPING # Condition as per (14) Note
except OSError:
  print("Error: Could not open UnicodeData.txt!")
except Exception as E:
  print(f"Error: Exception \"{str(E)}\"")
input()