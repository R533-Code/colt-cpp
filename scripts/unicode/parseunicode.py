import os
import dataclasses
import re
import functools
import colt
from colt import frac
from colt import CodePoint
from colt import CodePointRange

PATH_THIS_FILE = __file__
PATH_UCD       = "../../resources/UCD/"
PATH_OUTPUT    = "../../include/colt/unicode/gen/"
MAX_PLANE      = 17

# The working directory must be the folder containing
# the script
os.chdir(os.path.dirname(PATH_THIS_FILE))

@dataclasses.dataclass
class CodePointInfo:
  CODE_POINT : CodePoint
  NAME       : str = ""
  CATEGORY   : str = "Cn"
  
  DECIMAL :  int|None = None
  DIGIT   :  int|None = None
  NUMERIC : frac|None = None
  
  SIMPLE_UPPERCASE_MAPPING : CodePoint|None = None
  SIMPLE_LOWERCASE_MAPPING : CodePoint|None = None
  SIMPLE_TITLECASE_MAPPING : CodePoint|None = None

@dataclasses.dataclass
class Property:
  NAME     : str
  FULLNAME : str
  KEYS     : dict[str, str]
  DEFAULT_VALUE : tuple[CodePointRange, str]|None = None

@functools.cache
def parse_propertyvalue()->dict[str, Property]:
  """Parses all the properties information from `PropertyValueAliases.txt`.

  Returns:
      dict[str, Property]: The abbreviation name to property
  """
  PROPERTY_LIST : dict[str, Property] = dict()
  CURRENT_PROPERTY = "" # used by @ comments
  for line in colt.lines_of(PATH_UCD + 'PropertyValueAliases.txt', False, False):
    # TODO: handle @ comments
    if line.startswith("# @missing: "):
      split = [i.strip() for i in line[12:].split(';')]
      RANGE = CodePointRange.from_str(split[0])
      VALUE = split[2]
      PROPERTY_LIST[CURRENT_PROPERTY].DEFAULT_VALUE = (RANGE, VALUE)
      continue
    elif line.startswith("#"):
      # name (abrv)
      match = re.match("# ([_a-zA-Z0-9]+) \\(([_a-zA-Z0-9]+)\\)", line)
      if not match is None:
        CURRENT_PROPERTY = match.group(2)
        PROPERTY_LIST[match.group(2)] = Property(match.group(2), match.group(1), dict())
      continue      
  
    split = [i.strip() for i in line.split(';')]
    NAME = split[0]
    VALUE = split[1]
    
    current_dict = {VALUE: VALUE}
    for i in range(2, len(split)): # add aliases
      current_dict[split[i]] = VALUE
    if NAME in PROPERTY_LIST:
      PROPERTY_LIST[NAME].KEYS = {**PROPERTY_LIST[NAME].KEYS, **current_dict}
    else:
      current = Property(NAME=NAME, KEYS=current_dict)
      PROPERTY_LIST[NAME] = current
  
  return PROPERTY_LIST

@functools.cache
def parse_propertyaliases()->dict[str, str]:
  """Parses all the properties aliases from `PropertyAliases.txt`

  Returns:
      dict[str, str]: Dict of names to abbreviation
  """
  PROPERTY_LIST : dict[str, Property] = dict()
  for line in colt.lines_of(PATH_UCD + 'PropertyAliases.txt'):
    split = [i.strip() for i in line.split(';')]
    NAME = split[0]
    VALUE = split[1]    
    PROPERTY_LIST[NAME] = NAME
    PROPERTY_LIST[VALUE] = NAME
    for i in range(2, len(split)):
      PROPERTY_LIST[split[i]] = NAME
  return PROPERTY_LIST

UNICODE_DATA_PROPERTIES = [
  "_CODEPOINT",
  "Name",
  "General_Category",
  "Canonical_Combining_Class",
  "Bidi_Class",
  "Decomposition_Type|Decomposition_Mapping",
  "Numeric_Type",
  "Numeric_Value",
  "Numeric_Value",
  "Bidi_Mirrored",
  "Unicode_1_Name",
  "ISO_Comment",
  "Simple_Uppercase_Mapping",
  "Simple_Lowercase_Mapping",
  "Simple_Titlecase_Mapping",
]

@functools.cache
def parse_unicodedata()->list[CodePointInfo]:
  """Parses all the code point informations from UnicodeData.txt.
  The function will expand out all the ranges and add the unassigned
  code points. The resulting list is of size `CodePoint.max_code_point() + 1`.

  Returns:
      list[CodePointInfo]: The list of code points
  """
  
  # starts with CodePointInfo(-1) to avoid a bound check inside the
  # parsing loop. This character will be removed when returning
  # the list
  CODE_POINT_LIST : list[CodePointInfo] = [CodePointInfo(CodePoint(-1))]
  PROPERTY_LIST   = parse_propertyvalue()
  PROPERTY_ALIAS  = parse_propertyaliases()
    
  # See https://www.unicode.org/reports/tr44/#UnicodeData.txt
  for line in colt.lines_of(PATH_UCD + 'UnicodeData.txt'):
    split = [i.strip() for i in line.split(';')]
    
    CODE_POINT = CodePoint.from_str(split[0])
    NAME       : str = str(split[1])    # The name (can be empty!)
    CATEGORY   : str = str(split[2])    # See https://www.unicode.org/reports/tr44/#General_Category_Values        
    
    DECIMAL :  int|None = colt.none_or(int, split[6])  # [0-9] or empty
    DIGIT   :  int|None = colt.none_or(int, split[7])  # [0-9] or empty
    NUMERIC : frac|None = colt.none_or(frac, split[8]) # [+-][0-9]+(/[0-9]) or empty
    
    SIMPLE_UPPERCASE_MAPPING = colt.none_or(CodePoint.from_str, split[12]) # Uppercase code point number or empty
    SIMPLE_LOWERCASE_MAPPING = colt.none_or(CodePoint.from_str, split[13]) # Lowercase code point number or empty
    SIMPLE_TITLECASE_MAPPING = colt.none_or(CodePoint.from_str, split[14]) or SIMPLE_UPPERCASE_MAPPING # Condition as per (14) Note
    
    current = CodePointInfo(
      CODE_POINT, NAME, CATEGORY, DECIMAL, DIGIT, NUMERIC,
      SIMPLE_UPPERCASE_MAPPING, SIMPLE_LOWERCASE_MAPPING,
      SIMPLE_TITLECASE_MAPPING)
    # RANGES NEED SPECIAL HANDLING:
    # if a name follows: <.*, First>
    # then the next is : <.*, Last>
    # we need to duplicate the first
    previous_cp = CODE_POINT_LIST[-1].CODE_POINT.value
    if CODE_POINT.value != previous_cp + 1:
      if NAME.endswith(", Last>"):
        CODE_POINT_LIST += [dataclasses.replace(current, CODE_POINT=CodePoint(i + 1)) for i in range(previous_cp, CODE_POINT.value)]
      else:
        CODE_POINT_LIST += [CodePointInfo(CodePoint(i)) for i in range(previous_cp + 1, CODE_POINT.value)]
        CODE_POINT_LIST.append(current)
    else:
      CODE_POINT_LIST.append(current)
  # unassigned noncharacters
  while len(CODE_POINT_LIST) <= CodePoint.max_code_point() + 1:
    CODE_POINT_LIST.append(CodePointInfo(len(CODE_POINT_LIST) - 1))    
  # pop the CodePointInfo(-1)
  return CODE_POINT_LIST[1:]

def is_assigned_character(x: CodePointInfo)->bool:
  return (x.CATEGORY != "Cn"
    and x.CATEGORY != "Co"
    and x.CATEGORY != "Cs"
  )