import re
import sys

def bump_version(file_path):
    with open(file_path, 'r') as f:
        content = f.read()

    # Regex to match project(name VERSION X.Y.Z)
    # Matches: project(fdnr-vst3 VERSION 0.2.1)
    project_pattern = r'project\(([\w-]+)\s+VERSION\s+(\d+)\.(\d+)\.(\d+)\)'
    
    match = re.search(project_pattern, content)
    if not match:
        print(f"Error: Version pattern not found in {file_path}")
        sys.exit(1)

    project_name = match.group(1)
    major = int(match.group(2))
    minor = int(match.group(3))
    patch = int(match.group(4))

    # Increment patch version
    new_patch = patch + 1
    new_version = f"{major}.{minor}.{new_patch}"
    
    # Replace project version
    new_content = re.sub(
        project_pattern, 
        f'project({project_name} VERSION {new_version})', 
        content
    )

    # Also update JucePlugin_VersionString="0.2.1"
    juce_pattern = r'JucePlugin_VersionString="(\d+)\.(\d+)\.(\d+)"'
    new_content = re.sub(
        juce_pattern,
        f'JucePlugin_VersionString="{new_version}"',
        new_content
    )

    with open(file_path, 'w') as f:
        f.write(new_content)

    print(new_version)

if __name__ == "__main__":
    bump_version('CMakeLists.txt')
