import dataclasses
from fractions import Fraction as frac
from typing_extensions import Self
from typing import Callable
from typing import Any
import pickle
import re
import gc

CP_HEX_REGEX = "(?:[0-9a-fA-F]{4,6})"
CODE_POINT_FROM_STR = re.compile(f"^({CP_HEX_REGEX})$")
CODE_POINT_RANGE_FROM_STR = re.compile(f"^({CP_HEX_REGEX})(?:..({CP_HEX_REGEX}))?$")

@dataclasses.dataclass(order=True)
class CodePoint:
  value: int
  
  def int_to_hex(value: int)->str:
    """Converts a code point to its hex string

    Args:
        value (int): The code point value

    Returns:
        str: Hex string of the code point
    """
    if value <= 0xFFFF:
      return f"{value:04X}"
    if value <= 0xFFFFF:
      return f"{value:05X}"
    return f"{value:06X}"
  
  def max_code_point()->int:
    """Returns the max value of a code point (inclusive)

    Returns:
        int: The max value of the code point
    """
    return 0x10FFFF
  
  def from_str(value: str)->Self:
    """Converts a hex unicode code point to its value

    Args:
        value (str): The hex string

    Returns:
        Self: The new CodePoint
    """
    value = value.strip()
    match = CODE_POINT_FROM_STR.match(value)
    if match is None:
      raise ValueError(f"Could not convert '{value}' to a CodePoint!")
    return CodePoint(int(value, 16))
  
  def __init__(self: Self, value: int):
    """Constructor

    Args:
        value (int): The code point value

    Raises:
        ValueError: If code point > 0x10FFFF
    """
    if value > CodePoint.max_code_point():
      raise ValueError(f"Invalid code point: {CodePoint.int_to_hex(value)} > {CodePoint.int_to_hex(CodePoint.max_code_point())}!")
    self.value = value
  
  def __str__(self: Self)->str:
    """Converts CodePoint to a string

    Returns:
        str: The string representation of CodePointRange
    """
    return CodePoint.int_to_hex(self.value)

@dataclasses.dataclass
class CodePointRange:
  begin: CodePoint
  end: CodePoint
  
  def __str__(self: Self)->str:
    """Converts CodePointRange to a string

    Returns:
        str: The string representation of CodePointRange
    """
    if self.end != self.begin:
      return f"{CodePoint.int_to_hex(self.begin)}..{CodePoint.int_to_hex(self.end)}"
    return f"{CodePoint.int_to_hex(self.begin)}"
  
  def __init__(self: Self, begin: CodePoint, end: CodePoint):
    """Constructor

    Args:
        begin (CodePoint): The beginning of the range
        end (CodePoint): The end of the range (inclusive)

    Raises:
        ValueError: If begin > end
    """
    assert isinstance(begin, CodePoint) and isinstance(end, CodePoint)
    if begin > end:
      raise ValueError(f"Invalid range: {CodePoint.int_to_hex(begin)} > {CodePoint.int_to_hex(end)}!")
    self.begin = begin
    self.end = end
  
  def __contains__(self, key: CodePoint):
    return self.begin <= key and key <= self.end
  
  def from_str(value: str)->Self:
    """Converts a string of the form [0-9A-F]+(..[0-9A-F]+)? to the range it represent

    Args:
        value (str): The hex string

    Returns:
        Self: The new CodePoint
    """
    match = CODE_POINT_RANGE_FROM_STR.match(value)
    if match is None:
      raise ValueError(f"Could not convert '{value}' to a CodePointRange!")
    if match.group(2) is None:
      tmp = CodePoint(int(match.group(1), 16))
      return CodePointRange(tmp, tmp)
    return CodePointRange(
      CodePoint(int(match.group(1), 16)),
      CodePoint(int(match.group(2), 16))
    )

hexint = lambda x: int(x, 16)

def none_or(fn: Callable[[str], Any], value: str)->Any|None:
  """If 'value' is empty, returns None, else tries to convert it using 'fn'

  Args:
      fn (Callable[[str], Any]): String to object conversion function
      value (str): The string to convert

  Returns:
      Any|None: None if value is empty else fn(value)
  """
  value = value.strip()
  if len(value) == 0:
    return None
  return fn(value)

def chunk(l: list, n: int):
  """Chunks a list into an iterator of list of n elements

  Args:
      l (list): The list to chunk
      n (int): The size of the chunk

  Yields:
      iterator: produces a list of n elements
  """
  d, r = divmod(len(l), n)
  for i in range(n):
    si = (d+1)*(i if i < r else r) + d*(0 if i < r else i - r)
    yield l[si:si+(d+1 if i < r else d)]

def lines_of(path: str, filter_comments: bool = True, filter_at_comments: bool = True):
  with open(path, 'r') as file:
    for line in file:
      line = line.strip()
      if len(line) == 0:
        continue
      if line.startswith("# @") and not filter_at_comments:
        yield line
      elif line.startswith("#") and filter_comments:
        continue
      else:
        yield line