project('kernel',
    [ 'c', 'cpp' ],
    license: [ 'GPLv2' ],
    default_options: [ 'c_std=c11', 'cpp_std=c++2a', 'b_asneeded=false', 'b_lundef=false'],
    version: '1.0.0'
)

c_args = [ '--target=riscv64', '-mno-relax' ]
cpp_args = [ ] + c_args
link_args = [ '--target=riscv64', '-ffreestanding', '-nodefaultlibs', '-fuse-ld=lld', '-T../linker.ld' ]

# Make sure the project gets cross compiled
assert(meson.is_cross_build(), 'This project can only be cross-compiled. Make sure to call meson with the "--cross-file=asgard" option!')


# Source files and include directories
source_files = [
    'source/main.cpp',
]

include_dirs = include_directories('include')


# Executable building
elf = executable(
    'kernel.elf',
    source_files,
    native: false,
    c_args: c_args,
    cpp_args: cpp_args,
    link_args: link_args,
    name_prefix: '',
    dependencies: [ ],
    include_directories: include_dirs
)

dep = declare_dependency(include_directories: include_dirs, link_with: elf)