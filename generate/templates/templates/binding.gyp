{
  "variables": {
    "is_electron%": "<!(node ./utils/isBuildingForElectron.js <(node_root_dir))",
    "is_IBMi%": "<!(node -p \"os.platform() == 'aix' && os.type() == 'OS400' ? 1 : 0\")",
    "electron_openssl_root%": "<!(node ./utils/getElectronOpenSSLRoot.js <(module_root_dir))",
    "electron_openssl_static%": "<!(node -p \"process.platform !== 'linux' || process.env.NODEGIT_OPENSSL_STATIC_LINK === '1' ? 1 : 0\")",
    "macOS_deployment_target": "10.11"
  },

  "targets": [
    {
      "target_name": "acquireOpenSSL",
        "type": "none",
        "conditions": [
        ["<(is_electron) == 1 and <!(node -p \"process.env.npm_config_openssl_dir ? 0 : 1\")", {
          "actions": [{
            "action_name": "acquire",
            "action": ["node", "utils/acquireOpenSSL.js", "<(macOS_deployment_target)"],
            "inputs": [""],
            "outputs": ["vendor/openssl"],
            "message": "Acquiring OpenSSL binaries and headers"
          }]
        }]
      ]
    },
    {
      "target_name": "configureLibssh2",
      "type": "none",
      "actions": [{
        "action_name": "configure",
        "action": ["node", "utils/configureLibssh2.js"],
        "inputs": [""],
        "outputs": [""]
      }],
      "hard_dependencies": [
        "acquireOpenSSL"
      ]
    },
    {
      "target_name": "nodegit",

      "hard_dependencies": [
        "configureLibssh2"
      ],

      "dependencies": [
        "vendor/libgit2.gyp:libgit2"
      ],

      "variables": {
        "coverage%": 0
      },
      "sources": [
        "src/async_baton.cc",
        "src/async_worker.cc",
        "src/lock_master.cc",
        "src/reference_counter.cc",
        "src/thread_pool.cc",
        "src/nodegit.cc",
        "src/init_ssh2.cc",
        "src/promise_completion.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",
        "src/functions/free.cc",
        "src/cleanup_handle.cc",
        "src/convenient_patch.cc",
        "src/convenient_hunk.cc",
        "src/filter_registry.cc",
        "src/git_buf_converter.cc",
        "src/str_array_converter.cc",
        "src/context.cc",
        "src/v8_helpers.cc",
        "src/tracker_wrap.cc",
        {% each %}
          {% if type != "enum" %}
            "src/{{ name }}.cc",
          {% endif %}
        {% endeach %}
      ],

      "include_dirs": [
        "vendor/libv8-convert",
        "vendor/libssh2/include",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags": [
        "-Wall"
      ],

      "conditions": [
        [
          "coverage==1", {
            "cflags": [
              "-ftest-coverage",
              "-fprofile-arcs"
            ],
            "link_settings": {
              "libraries": [
                "-lgcov"
              ]
            },
          }
        ],
        [
          "OS=='mac'", {
            "libraries": [
              "-liconv",
            ],
            "conditions": [
              ["<(is_electron) == 1", {
                "include_dirs": [
                  "<(electron_openssl_root)/include"
                ],
                "libraries": [
                  "<(electron_openssl_root)/lib/libssl.a",
                  "<(electron_openssl_root)/lib/libcrypto.a"
                ]
              }]
            ],
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "MACOSX_DEPLOYMENT_TARGET": "<(macOS_deployment_target)",
              'CLANG_CXX_LIBRARY': 'libc++',
              'CLANG_CXX_LANGUAGE_STANDARD':'c++14',

              "WARNING_CFLAGS": [
                "-Wno-unused-variable",
                "-Wint-conversions",
                "-Wmissing-field-initializers",
                "-Wno-c++11-extensions"
              ]
            }
          }
        ],
        [
          "OS=='win'", {
            "conditions": [
              ["<(is_electron) == 1", {
                "include_dirs": ["<(electron_openssl_root)/include"],
                "libraries": [
                  "<(electron_openssl_root)/lib/libcrypto.lib",
                  "<(electron_openssl_root)/lib/libssl.lib"
                ],
                "copies": [{
                  "destination": "<(module_root_dir)/build/Release/",
                  "files": ["<(electron_openssl_root)/bin/libcrypto-1_1-x64.dll", "<(electron_openssl_root)/bin/libssl-1_1-x64.dll"]
                }]
              }]
            ],
            "defines": [
              "_HAS_EXCEPTIONS=1"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": [
                  "/EHsc"
                ]
              },
              "VCLinkerTool": {
                "AdditionalOptions": [
                  "/FORCE:MULTIPLE"
                ]
              }
            },
            "libraries": [
              "winhttp.lib",
              "crypt32.lib",
              "rpcrt4.lib"
            ]
          }
        ],
        ["OS=='mac' or OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "libraries": [
            "<!(krb5-config gssapi --libs)"
          ]
        }],
        ["OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "cflags": [
            "-std=c++14"
          ],
          "conditions": [
            ["<(is_electron) == 1 and <(electron_openssl_static) == 1", {
              "include_dirs": [
                "<(electron_openssl_root)/include"
              ],
              "libraries": [
                # this order is significant on centos7 apparently...
                "<(electron_openssl_root)/lib/libssl.a",
                "<(electron_openssl_root)/lib/libcrypto.a"
              ]
            }],
            ["<(is_electron) == 1 and <(electron_openssl_static) != 1", {
              "libraries": [
                "-lcrypto",
                "-lssl"
              ]
            }]
          ],
        }],
        [
          "<(is_IBMi) == 1", {
            "include_dirs": [
              "/QOpenSys/pkgs/include"
            ],
            "libraries": [
              "-L/QOpenSys/pkgs/lib"
            ]
          }
        ]
      ]
    }
  ]
}
