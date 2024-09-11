import os
import sys
import subprocess
import shutil

if sys.platform != "linux":
  print("Error: this script is only meant to be ran on Linux!")
  exit(1)
if "scripts" not in os.getcwd():
  os.chdir(os.path.dirname(__file__))

def get_alias_path(alias):
  """Returns the path that represents an alias

  Args:
      alias (str): The alias name

  Returns:
      str|None: The path or None if the alias does not exist
  """
  sp = subprocess.Popen(["/bin/bash", "-i", "-c", f"alias '{alias}'"],
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  a, _ = sp.communicate()
  a = str(a)
  if a.count('=') == 0:
    return None
  return a.split('=')[-1].replace('\'', '').replace('"', '').replace('\\n', '')

ARM_GCC = get_alias_path("arm-gcc")
ARM_GXX = get_alias_path("arm-g++")
if ARM_GCC is None:
  print("Error: Could not find 'arm-gcc' alias!")
  exit(1)
if ARM_GXX is None:
  print("Error: Could not find 'arm-g++' alias!")
  exit(1)

ARM_DIR = os.path.dirname(ARM_GCC)
os.environ["CC"] = ARM_GCC
os.environ["CXX"] = ARM_GXX
os.environ['PATH'] += os.pathsep + ARM_DIR

if not os.path.isfile("../build/__generated_py"):
  shutil.rmtree('../build', ignore_errors=True)
  os.mkdir("../build")
  SAVE = os.getcwd()
  os.chdir("../build")
  sp = subprocess.Popen(["/bin/bash", "-i", "-c", f"cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../resources/toolchain-arm64.cmake .."])
  if sp.wait() != 0:
    print("Error: Problem configuring CMake!")
    exit(1)
  open("__generated_py", "w").close()
  os.chdir(SAVE)

SAVE = os.getcwd()
os.chdir("../build")
sp = subprocess.Popen(["/bin/bash", "-i", "-c", f"make -j$nproc"])
if sp.wait() != 0:
  print("Error: Compilation errors!")
  exit(1)
os.chdir(SAVE)