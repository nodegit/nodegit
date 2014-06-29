{
  "targets": [
    {
      "target_name": "nodegit",

      "dependencies": [
        "<(module_root_dir)/vendor/libgit2.gyp:libgit2"
      ],

      "sources": [
        # Maintained.
        "src/nodegit.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",

        # Generated files.
        "src/blob.cc",
        "src/clone.cc",
        "src/commit.cc",
        "src/config.cc",
        #"src/diff.cc",
        "src/index.cc",
        "src/object.cc",
        "src/odb.cc",
        "src/oid.cc",
        "src/refdb.cc",
        "src/refs.cc",
        "src/repository.cc",
        "src/revwalk.cc",
        "src/signature.cc",
        "src/threads.cc",
        "src/time.cc",
        "src/trace.cc",
        "src/tree.cc",
        "src/treebuilder.cc",
        "src/tree_entry.cc",
      ],

      "include_dirs": [
        "vendor/libv8-convert",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags": [
        "-Wall",
      ],

      "conditions": [
        [
          "OS=='mac'", {
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",

              "WARNING_CFLAGS": [
                "-Wno-unused-variable",
              ],
            }
          }
        ]
      ]
    },
  ]
}
