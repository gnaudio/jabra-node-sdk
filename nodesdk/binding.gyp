{
  "variables": {
    "conditions": [
      ["OS=='win' and target_arch=='ia32'", {
        "jabralibfolder": "libjabra/windows/x86",
        "jabralibfile": "libjabra.dll",
        "jabraliblibrary": "libjabra.lib"
      }],
      ["OS=='win' and target_arch=='x64'", {
        "jabralibfolder": "libjabra/windows/x64",
        "jabralibfile": "libjabra.dll",
        "jabraliblibrary": "libjabra.lib"
      }],
      ["OS=='mac'", {
        "jabralibfolder": "libjabra/mac",
        "jabralibfile": "libjabra.dylib"
      }],
      ["OS=='linux' and target_arch=='ia32'", {
        "jabralibfolder": "libjabra/ubuntu/x32",
        "jabralibfileglob": "libjabra.so.*",
        "jabralibfile": "<!(find '<(_jabralibfolder)' -type f -name '<(_jabralibfileglob)' -printf '%f')"
      }],
      ["OS=='linux' and target_arch=='x64'", {
        "jabralibfolder": "libjabra/ubuntu/x64",
        "jabralibfileglob": "libjabra.so.*",
        "jabralibfile": "<!(find '<(_jabralibfolder)' -type f -name '<(_jabralibfileglob)' -printf '%f')"
      }],
      ["OS=='linux' and target_arch=='arm'", {
        "jabralibfolder": "TODO",
        "jabralibfile": "TODO"
      }],
      ["OS=='linux' and target_arch=='arm64'", {
        "jabralibfolder": "TODO",
        "jabralibfile": "TODO"
      }],
    ],
  },
  "targets": [
    {
      "target_name": "sdkintegration",
      "cflags_cc": [
        "-std=c++14"
      ],
      "sources": [ "<!@(node -p \"require('fs').readdirSync('./src/main').filter(f => /\.cc$/.test(f)).map(f=>'src/main/'+f).join(' ')\")" ],
      "include_dirs": [
        "libjabra/headers",
        "includes",
        "<!@(node -p \"require('node-addon-api').include\")",
      ],
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'link_settings': {
         'library_dirs': [
            '.',
          ],
      },
      'conditions': [
        ['OS=="win"', {
          'conditions': [
            ['target_arch=="ia32"', {
              'libraries': [ '<(jabralibfolder)/<(jabraliblibrary)' ],
              "copies":
              [
                  {
                    'destination': '<(PRODUCT_DIR)',
                    'files': ['<(module_root_dir)/<(jabralibfolder)/<(jabralibfile)']
                  }
              ]
            }],
            ['target_arch=="x64"', {
              'libraries': [ '<(jabralibfolder)/<(jabraliblibrary)' ],
              "copies":
              [
                  {
                    'destination': '<(PRODUCT_DIR)',
                    'files': ['<(module_root_dir)/<(jabralibfolder)/<(jabralibfile)']
                  }
              ]
            }]
          ],
          'defines': [ '_HAS_EXCEPTIONS=1' ],
          'msvs_settings': {
            'VCCLCompilerTool': { 'ExceptionHandling': 1 },
          },
        }],
        ['OS=="linux"', {
          'libraries': [ "../<(jabralibfolder)/<(jabralibfile)" ],
          'ldflags': [
            "-Wl,-rpath,'$$ORIGIN'"
          ],
          'cflags_cc': [
            '-fexceptions',
            '-Wno-unused-variable'
          ],
          "copies":
          [
            {
              'destination': '<(PRODUCT_DIR)',
              'files': ['<(module_root_dir)/<(jabralibfolder)/<(jabralibfile)']
            }
          ],
        }],
        ['OS=="mac"', {
         'libraries': [ '../<(jabralibfolder)/<(jabralibfile)' ],
         'xcode_settings': {
           'ld_version_details': 'true',
           'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
           'CLANG_CXX_LIBRARY': 'libc++',
           'MACOSX_DEPLOYMENT_TARGET': '10.7',
           'WARNING_CFLAGS': [
                          '-Wall',
                          '-Wendif-labels',
                          '-W',
                          '-Wno-unused-variable',
                          '-Wno-unused-lambda-capture',
                          '-Wno-unused-parameter',
                          '-Wno-dangling-else'
            ],
         },
         "copies":
         [
            {
              'destination': '<(PRODUCT_DIR)',
              'files': ['<(module_root_dir)/<(jabralibfolder)/<(jabralibfile)']
            }
         ],
         'postbuilds': [
          {
            'postbuild_name': 'fix relative dylib lookup',
            'action': [
              'install_name_tool',
              '-change',
              '@rpath/libjabra.dylib',
              '@loader_path/libjabra.dylib',
              '${BUILT_PRODUCTS_DIR}/sdkintegration.node',
            ],
          },
         ]
        }],
      ]
    }
  ]
}
