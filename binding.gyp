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
        "src/attr.cc",
        "src/blame.cc",
        "src/blob.cc",
        "src/branch.cc",
        "src/buffer.cc",
        "src/checkout.cc",
        "src/clone.cc",
        "src/commit.cc",
        "src/common.cc",
        "src/config.cc",
        "src/cred_helpers.cc",
        "src/diff.cc",
        "src/errors.cc",
        "src/filter.cc",
        "src/graph.cc",
        "src/ignore.cc",
        "src/index.cc",
        "src/indexer.cc",
        "src/inttypes.cc",
        "src/merge.cc",
        "src/message.cc",
        "src/net.cc",
        "src/notes.cc",
        "src/object.cc",
        "src/odb.cc",
        "src/odb_backend.cc",
        "src/oid.cc",
        "src/pack.cc",
        "src/patch.cc",
        "src/pathspec.cc",
        "src/push.cc",
        "src/refdb.cc",
        "src/refs.cc",
        "src/reflog.cc",
        "src/refspec.cc",
        "src/remote.cc",
        "src/repository.cc",
        "src/reset.cc",
        "src/revparse.cc",
        "src/revwalk.cc",
        "src/signature.cc",
        "src/stash.cc",
        "src/status.cc",
        "src/stdint.cc",
        "src/stdarray.cc",
        "src/submodule.cc",
        "src/tag.cc",
        "src/threads.cc",
        "src/time.cc",
        "src/trace.cc",
        "src/transport.cc",
        "src/tree.cc",
        #"src/tree_entry.cc",
        #"src/types.cc",
        "src/version.cc",
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
