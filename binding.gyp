{
  "targets": [
    {
      "target_name": "nodegit",

      "dependencies": [
        "<(module_root_dir)/vendor/libgit2.gyp:libgit2"
      ],

      "sources": [
        # Maintained.
        "src/base.cc",
        "src/wrapper.cc",
        "src/functions/copy.cc",

        # Generated files.
        "src/base.cc",
        "src/blob.cc",
        #"src/commit.cc",
        "src/index.cc",
        #"src/indexer.cc",
        #"src/net.cc",
        "src/object.cc",
        "src/oid.cc",
        #"src/pack.cc",
        #"src/patch.cc",
        #"src/pathspec.cc",
        #"src/push.cc",
        #"src/refdb.cc",
        #"src/refs.cc",
        #"src/reflog.cc",
        #"src/refspec.cc",
        #"src/remote.cc",
        #"src/repository.cc",
        #"src/reset.cc",
        #"src/revparse.cc",
        #"src/revwalk.cc",
        #"src/signature.cc",
        #"src/stash.cc",
        #"src/status.cc",
        #"src/stdint.cc",
        #"src/submodule.cc",
        #"src/tag.cc",
        "src/threads.cc",
        #"src/time.cc",
        #"src/trace.cc",
        #"src/transport.cc",
        #"src/tree.cc",
        #"src/tree_entry.cc",
        #"src/types.cc",
        #"src/version.cc",
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
