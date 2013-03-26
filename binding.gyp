{
    'targets': [
        {
            'target_name': 'nodegit',
            'sources': [
                'src/base.cc',
                'src/blob.cc',
                'src/commit.cc',
                'src/error.cc',
                'src/object.cc',
                'src/oid.cc',
                'src/reference.cc',
                'src/repo.cc',
                'src/revwalk.cc',
                'src/sig.cc',
                'src/tree.cc',
                'src/tree_entry.cc',
                'src/diff_list.cc',
                'src/threads.cc',
                'src/functions/commit.cc',
                'src/functions/string.cc'
            ],

            'include_dirs': [
                'vendor/libv8-convert',
                'vendor/libgit2/include'
            ],

            'libraries': [
                '-L<!(pwd)/vendor/libgit2/build',
                '-lgit2'
            ],

            'cflags': [
                '-Wall'
            ],

            'ldflags': [
                '-Wl,-rpath,\$$ORIGIN/../../vendor/libgit2/build'
            ],

            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    }
                }]
            ]
        }
    ]
}
