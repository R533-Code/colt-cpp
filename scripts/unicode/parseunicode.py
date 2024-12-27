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
    PROPERTY_LIST[NAME]  = NAME
    PROPERTY_LIST[VALUE] = NAME
    for i in range(2, len(split)):
      PROPERTY_LIST[split[i]] = NAME
  return PROPERTY_LIST

@functools.cache
def parse_properties()->list[tuple[CodePointRange, str, str]]:
  """Parses all the properties from `PropList.txt`

  Returns:
      list[tuple[CodePointRange, str, str]]: CodePointRange, Property, Value
  """
  
  PROPERTY_LIST : list[tuple[CodePointRange, str, str]] = []
  REGEX = re.compile("(.*);(.*)# (..).*")  
  for line in colt.lines_of(PATH_UCD + 'PropList.txt'):
    match = REGEX.match(line)
    assert match is not None
    range = CodePointRange.from_str(match.group(1).strip())
    prope = match.group(2).strip()
    value = match.group(3).strip()
    PROPERTY_LIST.append((range, prope, value))
  return PROPERTY_LIST

UNICODE_DATA_PROPERTIES = [
  "Name",
  "General_Category",
  "Canonical_Combining_Class",
  "Bidi_Class",
  "Decomposition_Type", # Decomposition_Mapping
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

@dataclasses.dataclass
class CodePointInfo:
  CODE_POINT : CodePoint
  PROPERTIES : dict[str, str]
  
  def get(self, property):
    return self.PROPERTIES[parse_propertyaliases()[property]]

@functools.cache
def parse_unicodedata()->list[CodePointInfo]:
  """Parses all the code point informations from UnicodeData.txt.
  The function will expand out all the ranges and add the unassigned
  code points. The resulting list is of size `CodePoint.max_code_point() + 1`.

  Returns:
      list[CodePointInfo]: The list of code points
  """
  
  def default_to_true_default(default: str|None):    
    if default is None:
      return default
    CONV = {
      "<code point>" : CodePoint.int_to_hex(CODE_POINT.value),
      "<none>"       : None,
      "NaN"          : None,
      "Unassigned"   : "Cn",
    }
    return CONV.get(default, default)
  @functools.cache
  def default_properties():
    """
    ONE ASSUMPTION MADE BY THIS FUNCTION IS THAT
    THE CODE POINT VALUE DOES NOT CHANGE THE DEFAULT.
    For Unicode 16.0.0 this appears to be true
    """
    ret = dict()
    for j in range(0, len(UNICODE_DATA_PROPERTIES)):
      current_abrv = PROPERTY_ALIAS[UNICODE_DATA_PROPERTIES[j]]
      value = None
      default = PROPERTY_LIST[current_abrv].DEFAULT_VALUE
      if default is not None: # and CodePoint(i) in default[0]:
        value = default_to_true_default(default[1])
      # Lookup abbreviation in property alias
      ret[current_abrv] = value
    return ret
  
  # starts with CodePointInfo(-1) to avoid a bound check inside the
  # parsing loop. This character will be removed when returning
  # the list
  CODE_POINT_LIST : list[CodePointInfo] = [CodePointInfo(CodePoint(-1), dict())]
  PROPERTY_LIST   = parse_propertyvalue()
  PROPERTY_ALIAS  = parse_propertyaliases()
  
  # See https://www.unicode.org/reports/tr44/#UnicodeData.txt
  for line in colt.lines_of(PATH_UCD + 'UnicodeData.txt'):
    split = [i.strip() for i in line.split(';')]
    
    CODE_POINT = CodePoint.from_str(split[0])
    split.pop(0)
    PROPERTIES = dict()
    for i in range(0, len(UNICODE_DATA_PROPERTIES)):
      current_abrv = PROPERTY_ALIAS[UNICODE_DATA_PROPERTIES[i]]
      value = None
      default = PROPERTY_LIST[current_abrv].DEFAULT_VALUE
      if len(split[i]) != 0:
        value = split[i]
      elif (default is not None and CODE_POINT in default[0]):
        value = default_to_true_default(default[1])
      # Lookup abbreviation in property alias
      PROPERTIES[current_abrv] = value
    
    # TODO: Condition as per (14) Note
    
    current = CodePointInfo(CODE_POINT, PROPERTIES)
    # RANGES NEED SPECIAL HANDLING:
    # if a name follows: <.*, First>
    # then the next is : <.*, Last>
    # we need to duplicate the first
    previous_cp = CODE_POINT_LIST[-1].CODE_POINT.value
    if CODE_POINT.value != previous_cp + 1:
      if current.get("Name").endswith(", Last>"):
        CODE_POINT_LIST += [dataclasses.replace(current, CODE_POINT=CodePoint(i + 1)) for i in range(previous_cp, CODE_POINT.value)]
      else:
        CODE_POINT_LIST += [CodePointInfo(CodePoint(i), default_properties()) for i in range(previous_cp + 1, CODE_POINT.value)]
        CODE_POINT_LIST.append(current)
    else:
      CODE_POINT_LIST.append(current)
  # unassigned noncharacters
  while len(CODE_POINT_LIST) <= CodePoint.max_code_point() + 1:
    CODE_POINT_LIST.append(CodePointInfo(len(CODE_POINT_LIST) - 1, CODE_POINT_LIST[-1].PROPERTIES))    
  # pop the CodePointInfo(-1)
  return CODE_POINT_LIST[1:]

def is_assigned_character(x: CodePointInfo)->bool:
  return (x.get("General_Category") != "Cn"
    and x.get("General_Category") != "Co"
    and x.get("General_Category") != "Cs"
  )