{
  "targets": [
    {
      "target_name": "nodegit",

      "sources": [
        "src/base.cc",
        "src/blob.cc",
        "src/commit.cc",
        "src/oid.cc",
        "src/reference.cc",
        "src/object.cc",
        "src/repo.cc",
        "src/index.cc",
        "src/index_entry.cc",
        "src/index_time.cc",
        "src/tag.cc",
        "src/revwalk.cc",
        "src/signature.cc",
        "src/time.cc",
        "src/tree.cc",
        "src/tree_builder.cc",
        "src/tree_entry.cc",
        "src/diff_find_options.cc",
        "src/diff_options.cc",
        "src/diff_list.cc",
        "src/patch.cc",
        "src/delta.cc",
        "src/diff_file.cc",
        "src/diff_range.cc",
        "src/threads.cc",
        "src/wrapper.cc",
        "src/refdb.cc",
        "src/odb_object.cc",
        "src/odb.cc",
        "src/submodule.cc",
        "src/remote.cc",
        "src/clone_options.cc",
        "src/functions/copy.cc",
      ],

      "include_dirs": [
        "vendor/libv8-convert",
        "vendor/libgit2/include",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags": [
        "-Wall"
      ],

      "ldflags": [
        "-Wl,-rpath,\$$ORIGIN/../../vendor/libgit2/build"
      ],

      "conditions": [
        ['OS=="win"', {
          "link_settings": {
            "libraries": [
              "-lgit2.lib",
            ]
          },
          "configurations": {
            "Release": {
              "msvs_settings": {
                "VCCLCompilerTool": {
                  "RuntimeLibrary": 0,
                  "Optimization": 3,
                  "FavorSizeOrSpeed": 1,
                  "InlineFunctionExpansion": 2,
                  "WholeProgramOptimization": "true",
                  "OmitFramePointers": "true",
                  "EnableFunctionLevelLinking": "true",
                  "EnableIntrinsicFunctions": "true",
                  "RuntimeTypeInfo": "false",
                  "ExceptionHandling": "0",
                  "GenerateDebugInformation": "true",
                  "AdditionalOptions": [
                    "/MP /EHsc"
                  ]
                },
                "VCLibrarianTool": {
                  "AdditionalOptions": [
                    "/LTCG"
                  ]
                },
                "VCLinkerTool": {
                  "LinkTimeCodeGeneration": 1,
                  "OptimizeReferences": 2,
                  "EnableCOMDATFolding": 2,
                  "LinkIncremental": 1,
                  "AdditionalLibraryDirectories": [
                    "../vendor/libgit2/build/debug"
                  ]
                }
              }
            }
          }
        }, { # Not Windows.
          "libraries": [
           "-L<!(pwd)/vendor/libgit2/build",
            "-lgit2"
          ]
        }],

        ['OS=="mac"', {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          }
        }]
      ]
    }
  ]
}
