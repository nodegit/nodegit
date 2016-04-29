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
        "src/lock_master.cc",
        "src/nodegit.cc",
        "src/init_ssh2.cc",
        "src/promise_completion.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",
        "src/functions/sleep_for_ms.cc",
        "src/convenient_patch.cc",
        "src/convenient_hunk.cc",
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
        "vendor/openssl/openssl/include",
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
            "cflags": [
              "/EHsc"
            ],
            "defines": [
              "_HAS_EXCEPTIONS=1"
            ]
          }
        ], [
          "OS=='linux' and '<!(echo \"$CXX\")'=='clang++'", {
            "cflags": [
              "-Wno-c++11-extensions"
            ]
          }
        ], [
          "OS=='linux' and '<!(echo \"$CXX\")'!='clang++'", {
            "cflags": [
              "-std=c++0x"
            ]
          }
        ]
      ]
    }
  ]
}
