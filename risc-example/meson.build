project('riscv-example',
    [ 'c', 'cpp' ],
    license: [ 'GPLv2' ],
    default_options: [ 'c_std=c11', 'cpp_std=c++2a', 'b_asneeded=false', 'b_lundef=false'],
    version: '1.0.0'
)

c_args = [ '--target=riscv64' ]
cpp_args = [ ] + c_args

# Make sure the project gets cross compiled
    assert(meson.is_cross_build(), 'This project can only be cross-compiled. Make sure to call meson with the "--cross-file=asgard" option!')


kernel = subproject('kern').get_variable('dep')