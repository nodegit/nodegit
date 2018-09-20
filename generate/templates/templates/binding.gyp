{
  "targets": [
    {
      "target_name": "nodegit",

      "dependencies": [
        "vendor/libgit2.gyp:libgit2"
      ],

      "variables": {
        "coverage%": 0
      },
      "sources": [
        "src/async_baton.cc",
        "src/lock_master.cc",
        "src/nodegit.cc",
        "src/init_ssh2.cc",
        "src/promise_completion.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",
        "src/functions/free.cc",
        "src/convenient_patch.cc",
        "src/convenient_hunk.cc",
        "src/filter_registry.cc",
        "src/git_buf_converter.cc",
        "src/str_array_converter.cc",
        "src/thread_pool.cc",
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
            "conditions": [
              ["node_root_dir.split('/')[-1].startswith('iojs')", {
                "include_dirs": [
                  "/usr/local/opt/openssl@1.1/include"
                ],
                "libraries": [
                  "/usr/local/opt/openssl@1.1/lib/libcrypto.a",
                  "/usr/local/opt/openssl@1.1/lib/libssl.a"
                ]
              }]
            ],
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "MACOSX_DEPLOYMENT_TARGET": "10.7",

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
              ["node_root_dir.split('\\\\')[-1].startswith('iojs')", {
                "conditions": [
                  ["target_arch == 'x64'", {
                    "include_dirs": ["vendor/win/openssl/include64"],
                    "libraries": [
                      "<(module_root_dir)/vendor/win/openssl/lib64/libcryptoMT.lib",
                      "<(module_root_dir)/vendor/win/openssl/lib64/libsslMT.lib",
                      "winhttp.lib",
                      "crypt32.lib",
                      "rpcrt4.lib"
                    ]
                  }, {
                    "include_dirs": ["vendor/win/openssl/include"],
                    "libraries": [
                      "<(module_root_dir)/vendor/win/openssl/lib/libcryptoMT.lib",
                      "<(module_root_dir)/vendor/win/openssl/lib/libsslMT.lib",
                      "winhttp.lib",
                      "crypt32.lib",
                      "rpcrt4.lib"
                    ]
                  }]
                ]
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
            }
          }
        ],
        [
          "OS=='linux' or OS=='mac'", {
            "libraries": [
              "<!(curl-config --libs)"
            ]
          }
        ],
        [
          "OS=='linux' or OS.endswith('bsd')", {
            "libraries": [
              "-lcrypto",
              "-lssl"
            ],
            "cflags": [
              "-std=c++11"
            ]
          }
        ]
      ]
    }
  ]
}
