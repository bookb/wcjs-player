{
  'variables': {
      'pro_root': '<(module_root_dir)', #设置工程根目录，<(root)为应用在脚本中GYP_DEFINES中的root变量
  },
  'targets': [{
    'target_name': 'player_bindings',
    'sources': [
      'src/*.cpp'
    ],
    'include_dirs': [
      'deps',
      'deps/libvlc_wrapper/libvlc-sdk/include'
    ],
    'dependencies': [
     "deps/libvlc_wrapper/libvlc.gyp:player"
    ],
    'cflags!': [ '-fno-exceptions' ],
    'cflags_cc!': [ '-fno-exceptions' ],
    'xcode_settings': {
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      'CLANG_CXX_LIBRARY': 'libc++',
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
    },
    'msvs_settings': {
      'VCCLCompilerTool': { 'ExceptionHandling': 1 },
    },
    'conditions': [
      ['OS=="win"', {
        'sources': [
            'src/dlopen/win32-dlfcn.cc'
        ],
        'conditions': [
            ['target_arch=="ia32"', {
                 'libraries': ['<(pro_root)/deps/libvlc_wrapper/libvlc-sdk/lib/msvc/libvlc.lib']
              }, { # target_arch=="x64"
                 'libraries': ['<(pro_root)/deps/libvlc_wrapper/libvlc-sdk/lib/msvc/libvlc.x64.lib'],
             }]
        ]
      }],
      ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
        'sources': [],
        'libraries': ['-lvlc']
      }]
    ]
  }]
}