{
  "conditions": [
    ["(OS=='win' and node_root_dir.split('\\\\')[-1].startswith('iojs')) or (OS=='mac' and node_root_dir.split('/')[-1].startswith('iojs'))", {
      "conditions": [
        ["OS=='win'", {
          "variables": {
            "is_electron%": "1",
            "openssl_include_dir%": "<(module_root_dir)\\vendor\\openssl"
          }
        }, {
          "variables": {
            "is_electron%": "1",
            "openssl_include_dir%": "<(module_root_dir)/vendor/openssl"
          }
        }]
      ],
    }, {
      "conditions": [
        ["OS=='win'", {
          "variables": {
            "is_electron%": "0",
            "openssl_include_dir%": "<(node_root_dir)\\include\\node"
          }
        }, {
          "variables": {
            "is_electron%": "0",
            "openssl_include_dir%": "<(node_root_dir)/include/node"
          }
        }]
      ]
    }]
  ],

  "targets": [
    {
      "target_name": "acquireOpenSSL",
        "conditions": [
        ["<(is_electron) == 1", {
          "actions": [{
            "action_name": "acquire",
            "action": ["node", "utils/acquireOpenSSL.js"],
            "inputs": ["vendor/openssl_distributions.json"],
            "outputs": ["vendor/openssl"],
            "message": "Acquiring OpensSL binaries and headers"
          }]
        }]
      ]
    },
    {
      "target_name": "configureLibssh2",
      "actions": [{
        "action_name": "configure",
        "action": ["node", "utils/configureLibssh2.js", "<(openssl_include_dir)", "<(is_electron)"],
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
                  "vendor/openssl/include"
                ],
                "libraries": [
                  "<(module_root_dir)/vendor/openssl/lib/libcrypto.a",
                  "<(module_root_dir)/vendor/openssl/lib/libssl.a"
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
                "include_dirs": ["vendor/openssl/include"],
                "libraries": [
                  "<(module_root_dir)/vendor/openssl/lib/libcrypto.lib",
                  "<(module_root_dir)/vendor/openssl/lib/libssl.lib"
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
            },
            "libraries": [
              "winhttp.lib",
              "crypt32.lib",
              "rpcrt4.lib"
            ]
          }
        ],
        [
          "OS=='linux' or OS=='mac' or OS.endswith('bsd')", {
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
