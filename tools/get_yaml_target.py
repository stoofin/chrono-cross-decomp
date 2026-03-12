# From https://github.com/Vatuu/silent-hill-decomp/tree/master

import yaml
import os
import sys

def strip_prefix(path: str, prefix: str) -> str:
    path_norm = os.path.normpath(path)
    prefix_norm = os.path.normpath(prefix)

    # Ensure we only strip whole-path prefixes
    if path_norm == prefix_norm:
        return ""
    if path_norm.startswith(prefix_norm + os.sep):
        return path_norm[len(prefix_norm) + 1:]
    return path_norm

def get_yaml_target(yaml_file):
    yaml_path = os.path.join(os.getcwd(), yaml_file)

    with open(yaml_path, 'r') as file:
        data = yaml.safe_load(file)
        target_path = data.get('options', {}).get('target_path')
        if not target_path:
            return "target_path not found"

        # Strip known roots in priority order
        for prefix in (
            "disc/extracted",
            "rom",
        ):
            stripped = strip_prefix(target_path, prefix)
            # If we stripped something (or the path was exactly the prefix), accept it
            if stripped != os.path.normpath(target_path):
                target_path = stripped
                break

        return target_path

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python tools/get_yaml_target.py configs/<config_file>.yaml")
        sys.exit(1)

    print(get_yaml_target(sys.argv[1]))
