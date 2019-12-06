# This file is used with the GYP meta build system.
# http://code.google.com/p/gyp
# To build try this:
#   svn co http://gyp.googlecode.com/svn/trunk gyp
#   ./gyp/gyp -f make --depth=`pwd` libffi.gyp
#   make
#   ./out/Debug/test

{
  'variables': {
    'target_arch%': 'ia32', # built for a 32-bit CPU by default
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      # TODO: hoist these out and put them somewhere common, because
      #       RuntimeLibrary MUST MATCH across the entire project
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
      },
      'VCLibrarianTool': {
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
      },
    },
    'conditions': [
      ['OS == "win"', {
        'defines': [
          'WIN32'
        ],
      }]
    ],
  },

  # Compile .asm files on Windows
  'conditions': [
    ['OS=="win"', {
      'target_defaults': {
        'conditions': [
          ['target_arch=="ia32"', {
            'variables': { 'ml': ['ml', '/nologo', '/safeseh' ] }
          }, {
            'variables': { 'ml': ['ml64', '/nologo' ] }
          }]
        ],
        'rules': [
          {
            'rule_name': 'assembler',
            'msvs_cygwin_shell': 0,
            'extension': 'asm',
            'inputs': [
            ],
            'outputs': [
              '<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).obj',
            ],
            'action': [
              '<@(ml)', '/c', '/Fo<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).obj', '<(RULE_INPUT_PATH)'
            ],
            'message': 'Building assembly file <(RULE_INPUT_PATH)',
            'process_outputs_as_sources': 1,
          },
        ],
      },
    }],
  ],

  'targets': [
    {
      'target_name': 'player',
      'product_prefix': 'lib',
      'type': 'static_library',

      # for CentOS 5 support: https://github.com/rbranson/node-ffi/issues/110
      'standalone_static_library': 1,

      'sources': [
        'src/*.cpp'
      ],
      'defines': [
        'PIC',
        'PLAYER_BUILDING',
        'HAVE_CONFIG_H'
      ],
      'include_dirs': [
        'include',
        'libvlc-sdk/include'
        # platform and arch-specific headers
        # 'config/<(OS)/<(target_arch)'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include',
          # platform and arch-specific headers
          # 'config/<(OS)/<(target_arch)'
        ],
      },
      'conditions': [
        ['target_arch=="arm"', {
          'sources': [],
          'conditions': [
            ['OS=="linux"', {
              'sources': []
            }]
          ]
        }, { # ia32 or x64
          'sources': [
          ],
          'conditions': [
            ['OS=="mac"', {
              'sources': [
              ]
            }],
            ['OS=="win"', {
              # the libffi dlmalloc.c file has a bunch of implicit conversion
              # warnings, and the main ffi.c file contains one, so silence them
              'msvs_disabled_warnings': [ 4267 ],
              # the ffi64.c file is never compiled on Windows
              'sources!': [],
              'conditions': [
                ['target_arch=="ia32"', {
                  'sources': [  ],
                  'libraries': ['<(module_root_dir)/libvlc-sdk/lib/msvc/libvlc.lib']
                }, { # target_arch=="x64"
                  'sources': [ ],
                  'libraries': ['<(module_root_dir)/libvlc-sdk/lib/msvc/libvlc.x64.lib'],
                }]
              ]
            }],
            ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
              'sources': [
              ],
              'libraries': ['-lvlc']
            }]
          ]
        }],
      ]
    }
  ]
}
