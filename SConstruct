#!/usr/bin/env python
import os
import sys

# Read command-line options
opts = Variables([], ARGUMENTS)

# Define our options
opts.Add(EnumVariable("target", "Compilation target", "template_debug", ["editor", "template_release", "template_debug"]))
opts.Add(EnumVariable("platform", "Compilation platform", "", ["", "linux", "windows", "macos"]))
opts.Add(EnumVariable("arch", "CPU architecture", "x86_64", ["x86_32", "x86_64", "arm32", "arm64"]))
opts.Add(BoolVariable("use_llvm", "Use the LLVM compiler", False))
opts.Add(BoolVariable("use_mingw", "Use MinGW compiler (Windows)", False))
opts.Add(PathVariable("target_path", "The path where the library is installed", "addons/gd2ts/bin/", PathVariable.PathAccept))
opts.Add(PathVariable("target_name", "The library name", "libgd2ts", PathVariable.PathAccept))

# Create environment
env = Environment(variables=opts)

# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - LINKFLAGS are for linking flags

# Require C++17
env.Append(CXXFLAGS=["-std=c++17"])

# Platform-specific settings
if env["platform"] == "":
    env["platform"] = sys.platform

if env["platform"] == "linux" or env["platform"] == "linux2":
    env["platform"] = "linux"
    env.Append(CCFLAGS=["-fPIC"])
    env.Append(LINKFLAGS=["-Wl,-rpath,'$$ORIGIN'"])
    if env["target"] == "template_debug":
        env.Append(CCFLAGS=["-g3", "-Og"])
    else:
        env.Append(CCFLAGS=["-O3"])

elif env["platform"] == "windows" or env["platform"] == "win32":
    env["platform"] = "windows"
    env.Append(CXXFLAGS=["/EHsc", "/vmg"])
    if env["target"] == "template_debug":
        env.Append(CCFLAGS=["/DEBUG", "/Od"])
    else:
        env.Append(CCFLAGS=["/O2"])

elif env["platform"] == "darwin" or env["platform"] == "osx":
    env["platform"] = "macos"
    env.Append(CCFLAGS=["-arch", "x86_64", "-arch", "arm64"])
    env.Append(LINKFLAGS=["-arch", "x86_64", "-arch", "arm64"])
    if env["target"] == "template_debug":
        env.Append(CCFLAGS=["-g", "-O0"])
    else:
        env.Append(CCFLAGS=["-O3"])

# Use LLVM if requested
if env["use_llvm"]:
    env["CC"] = "clang"
    env["CXX"] = "clang++"

# Setup godot-cpp
env.Append(CPPPATH=["godot-cpp/gdextension/", "godot-cpp/include/", "godot-cpp/gen/include/"])
env.Append(LIBPATH=["godot-cpp/bin/"])

# Get godot-cpp library name
# Note: Prebuilt binaries only include release versions
godot_cpp_lib = "libgodot-cpp"
if env["platform"] == "linux":
    godot_cpp_lib += ".linux"
elif env["platform"] == "windows":
    godot_cpp_lib += ".windows"
elif env["platform"] == "macos":
    godot_cpp_lib += ".macos"

# Always use template_release for prebuilt binaries
godot_cpp_lib += ".template_release.{}".format(env["arch"])

if env["platform"] == "windows":
    godot_cpp_lib += ".lib"
else:
    godot_cpp_lib += ".a"

env.Append(LIBS=[godot_cpp_lib])

# Add our source path
env.Append(CPPPATH=["src/"])

# Setup tree-sitter core library
tree_sitter_core_path = "tree-sitter-core"
env.Append(CPPPATH=[tree_sitter_core_path + "/lib/include"])

# Setup tree-sitter-gdscript
tree_sitter_path = "tree-sitter-gdscript"
env.Append(CPPPATH=[
    tree_sitter_path + "/src",
    tree_sitter_path + "/src/tree_sitter"
])

# Gather all source files
sources = Glob("src/*.cpp")
sources += Glob("src/parser/*.cpp")
sources += Glob("src/transformer/*.cpp")
sources += Glob("src/generator/*.cpp")
sources += Glob("src/utils/*.cpp")

# Add tree-sitter core library sources (excluding lib.c which is an amalgamation)
import os
tree_sitter_sources = []
for src_file in Glob(tree_sitter_core_path + "/lib/src/*.c"):
    if not str(src_file).endswith("lib.c"):
        tree_sitter_sources.append(src_file)
sources += tree_sitter_sources

# Add tree-sitter-gdscript sources
sources += [
    tree_sitter_path + "/src/parser.c",
    tree_sitter_path + "/src/scanner.c"
]

# Build the library
library_path = "{}/{}".format(env["target_path"], env["platform"])
os.makedirs(library_path, exist_ok=True)

library_name = "{}.{}.{}.{}".format(
    env["target_name"],
    env["platform"],
    env["target"],
    env["arch"]
)

if env["platform"] == "linux":
    library_name += ".so"
elif env["platform"] == "windows":
    library_name += ".dll"
elif env["platform"] == "macos":
    library_name += ".dylib"

library = env.SharedLibrary(
    "{}/{}".format(library_path, library_name),
    source=sources
)

Default(library)

# Generate help text
Help(opts.GenerateHelpText(env))
