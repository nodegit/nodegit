# This is a generated file, modify: generate/templates/binding.gyp.
{
  "targets": [
    {
      "target_name": "nodegit",

      "dependencies": [
        "<(module_root_dir)/vendor/libgit2.gyp:libgit2"
      ],

      "sources": [
        "src/nodegit.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",
        {%each%}
        "src/{{ name }}.cc",
        {%endeach%}
      ],

      "include_dirs": [
        "vendor/libv8-convert",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags": [
        "-Wall",
        "-std=c++11"
      ],

      "conditions": [
        [
          "OS=='mac'", {
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "MACOSX_DEPLOYMENT_TARGET": "10.7",

              "WARNING_CFLAGS": [
                "-Wno-unused-variable",
                "-Wint-conversions",
                "-Wmissing-field-initializers"
              ],
              "OTHER_CPLUSPLUSFLAGS" : [
                "-std=gnu++11",
                "-stdlib=libc++"
              ],
              "OTHER_LDFLAGS": [
                "-stdlib=libc++"
              ]
            }
          }
        ]
      ]
    },
  ]
}
