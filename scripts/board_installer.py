import os
from pathlib import Path
from shutil import copyfile

# Prepare paths
core_dir = ".platformio"
script_dir = Path(__file__).resolve().parent
json_x3_file = Path(script_dir, "roboboard_x3.json")
json_x4_file = Path(script_dir, "roboboard_x4.json")
# Validate if json files exist
if not json_x3_file.is_file():
    exit("Error: Can't find roboboard_x3.json file in " + str(json_x3_file))
if not json_x4_file.is_file():
    exit("Error: Can't find roboboard_x4.json file in " + str(json_x4_file))
# Find path to PlatformIO core directory
platformio_path = Path(Path.home(), core_dir, "b")
# Acquire PlatformIO core directory from environment variable (if not found)
if not platformio_path.is_dir():
    env_path = os.environ["PLATFORMIO_PATH"].split(";" if os.name == 'nt' else ":")
    path = [i for i in env_path if core_dir in i]
    if len(path) == 0:
        exit("Error: Failed to locate .platformio directory for board installation.")
    split_path = path[0].partition(core_dir)
    platformio_path = Path(split_path[0]+split_path[1])
    if not platformio_path.is_dir():
        exit("Error: Failed to locate .platformio directory for board installation in " + str(platformio_path))

# Create .platformio/boards directory
boards_path = Path(platformio_path, "boards")
boards_path.mkdir(parents=False, exist_ok=True)
# Prepare paths
path_json_x3 = Path(boards_path, "roboboard_x3.json")
path_json_x4 = Path(boards_path, "roboboard_x4.json")
# Copy files
copyfile(json_x3_file, path_json_x3)
copyfile(json_x4_file, path_json_x4)
# End
print("Totem boards installed:")
print(str(path_json_x3))
print(str(path_json_x4))