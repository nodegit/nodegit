{
  # Copyright (c) 2012 The Chromium Authors. All rights reserved.
  # Use of this source code is governed by a BSD-style license that can be
  # found in the LICENSE file.
  "variables": {
    "target_arch%": "x86",
    "library%": "static_library",
    "openssl_enable_asm%": 0, # only supported with the Visual Studio 2012 (VC11) toolchain.
    "gcc_version%": 0,
    "is_clang%": 0
  },
  "targets": [
    {
      "target_name": "libgit2",
      "type": "static_library",
      "defines": [
        "GIT_THREADS",
        "GIT_SSH",
        "GIT_SSH_MEMORY_CREDENTIALS",
        # Node's util.h may be accidentally included so use this to guard
        # against compilation error.
        "SRC_UTIL_H_",
      ],
      "dependencies": [
        "zlib",
        "http_parser/http_parser.gyp:http_parser",
        "openssl/openssl.gyp:openssl",
        "libssh2"
      ],
      "sources": [
        "libgit2/include/git2/sys/hashsig.h",
        "libgit2/include/git2/sys/merge.h",
        "libgit2/include/git2/sys/time.h",
        "libgit2/src/annotated_commit.c",
        "libgit2/src/annotated_commit.h",
        "libgit2/src/array.h",
        "libgit2/src/apply.c",
        "libgit2/src/apply.h",
        "libgit2/src/attr_file.c",
        "libgit2/src/attr_file.h",
        "libgit2/src/attr.c",
        "libgit2/src/attr.h",
        "libgit2/src/attrcache.c",
        "libgit2/src/attrcache.h",
        "libgit2/src/bitvec.h",
        "libgit2/src/blame_git.c",
        "libgit2/src/blame_git.h",
        "libgit2/src/blame.c",
        "libgit2/src/blame.h",
        "libgit2/src/blob.c",
        "libgit2/src/blob.h",
        "libgit2/src/branch.c",
        "libgit2/src/branch.h",
        "libgit2/src/bswap.h",
        "libgit2/src/buf_text.c",
        "libgit2/src/buf_text.h",
        "libgit2/src/buffer.c",
        "libgit2/src/buffer.h",
        "libgit2/src/cache.c",
        "libgit2/src/cache.h",
        "libgit2/src/cc-compat.h",
        "libgit2/src/checkout.c",
        "libgit2/src/checkout.h",
        "libgit2/src/cherrypick.c",
        "libgit2/src/clone.c",
        "libgit2/src/clone.h",
        "libgit2/src/commit_list.c",
        "libgit2/src/commit_list.h",
        "libgit2/src/commit.c",
        "libgit2/src/commit.h",
        "libgit2/src/common.h",
        "libgit2/src/config_cache.c",
        "libgit2/src/config_file.c",
        "libgit2/src/config_file.h",
        "libgit2/src/config.c",
        "libgit2/src/config.h",
        "libgit2/src/curl_stream.c",
        "libgit2/src/curl_stream.h",
        "libgit2/src/crlf.c",
        "libgit2/src/date.c",
        "libgit2/src/delta.c",
        "libgit2/src/delta.h",
        "libgit2/src/diff_driver.c",
        "libgit2/src/diff_driver.h",
        "libgit2/src/diff_file.c",
        "libgit2/src/diff_file.h",
        "libgit2/src/diff_generate.c",
        "libgit2/src/diff_generate.h",
        "libgit2/src/diff_parse.c",
        "libgit2/src/diff_parse.h",
        "libgit2/src/diff_print.c",
        "libgit2/src/diff_stats.c",
        "libgit2/src/diff_tform.c",
        "libgit2/src/diff_tform.h",
        "libgit2/src/diff_xdiff.c",
        "libgit2/src/diff_xdiff.h",
        "libgit2/src/diff.c",
        "libgit2/src/diff.h",
        "libgit2/src/errors.c",
        "libgit2/src/fetch.c",
        "libgit2/src/fetch.h",
        "libgit2/src/fetchhead.c",
        "libgit2/src/fetchhead.h",
        "libgit2/src/filebuf.c",
        "libgit2/src/filebuf.h",
        "libgit2/src/fileops.c",
        "libgit2/src/fileops.h",
        "libgit2/src/filter.c",
        "libgit2/src/filter.h",
        "libgit2/src/fnmatch.c",
        "libgit2/src/fnmatch.h",
        "libgit2/src/global.c",
        "libgit2/src/global.h",
        "libgit2/src/graph.c",
        "libgit2/src/hash.c",
        "libgit2/src/hash.h",
        "libgit2/src/hash/hash_generic.c",
        "libgit2/src/hash/hash_generic.h",
        "libgit2/src/hash/hash_openssl.h",
        "libgit2/src/hashsig.c",
        "libgit2/src/ident.c",
        "libgit2/src/ignore.c",
        "libgit2/src/ignore.h",
        "libgit2/src/index.c",
        "libgit2/src/index.h",
        "libgit2/src/indexer.c",
        "libgit2/src/iterator.c",
        "libgit2/src/iterator.h",
        "libgit2/src/khash.h",
        "libgit2/src/map.h",
        "libgit2/src/merge_file.c",
        "libgit2/src/merge_file.h",
        "libgit2/src/merge.c",
        "libgit2/src/merge_driver.c",
        "libgit2/src/merge.h",
        "libgit2/src/message.c",
        "libgit2/src/message.h",
        "libgit2/src/mwindow.c",
        "libgit2/src/mwindow.h",
        "libgit2/src/netops.c",
        "libgit2/src/netops.h",
        "libgit2/src/notes.c",
        "libgit2/src/notes.h",
        "libgit2/src/object_api.c",
        "libgit2/src/object.c",
        "libgit2/src/object.h",
        "libgit2/src/odb_loose.c",
        "libgit2/src/odb_mempack.c",
        "libgit2/src/odb_pack.c",
        "libgit2/src/odb.c",
        "libgit2/src/odb.h",
        "libgit2/src/offmap.h",
        "libgit2/src/oid.c",
        "libgit2/src/oid.h",
        "libgit2/src/oidarray.c",
        "libgit2/src/oidarray.h",
        "libgit2/src/oidmap.h",
        "libgit2/src/openssl_stream.c",
        "libgit2/src/openssl_stream.h",
        "libgit2/src/pack-objects.c",
        "libgit2/src/pack-objects.h",
        "libgit2/src/pack.c",
        "libgit2/src/pack.h",
        "libgit2/src/path.c",
        "libgit2/src/path.h",
        "libgit2/src/patch.c",
        "libgit2/src/patch.h",
        "libgit2/src/patch_generate.c",
        "libgit2/src/patch_generate.h",
        "libgit2/src/patch_parse.c",
        "libgit2/src/patch_parse.h",
        "libgit2/src/pathspec.c",
        "libgit2/src/pathspec.h",
        "libgit2/src/pool.c",
        "libgit2/src/pool.h",
        "libgit2/src/posix.c",
        "libgit2/src/posix.h",
        "libgit2/src/pqueue.c",
        "libgit2/src/pqueue.h",
        "libgit2/src/proxy.c",
        "libgit2/src/push.c",
        "libgit2/src/push.h",
        "libgit2/src/rebase.c",
        "libgit2/src/refdb_fs.c",
        "libgit2/src/refdb_fs.h",
        "libgit2/src/refdb.c",
        "libgit2/src/refdb.h",
        "libgit2/src/reflog.c",
        "libgit2/src/reflog.h",
        "libgit2/src/refs.c",
        "libgit2/src/refs.h",
        "libgit2/src/refspec.c",
        "libgit2/src/refspec.h",
        "libgit2/src/remote.c",
        "libgit2/src/remote.h",
        "libgit2/src/repo_template.h",
        "libgit2/src/repository.c",
        "libgit2/src/repository.h",
        "libgit2/src/reset.c",
        "libgit2/src/revert.c",
        "libgit2/src/revparse.c",
        "libgit2/src/revwalk.c",
        "libgit2/src/revwalk.h",
        "libgit2/src/settings.c",
        "libgit2/src/sha1_lookup.c",
        "libgit2/src/sha1_lookup.h",
        "libgit2/src/signature.c",
        "libgit2/src/signature.h",
        "libgit2/src/socket_stream.c",
        "libgit2/src/socket_stream.h",
        "libgit2/src/sortedcache.c",
        "libgit2/src/sortedcache.h",
        "libgit2/src/stash.c",
        "libgit2/src/status.c",
        "libgit2/src/status.h",
        "libgit2/src/strmap.c",
        "libgit2/src/strmap.h",
        "libgit2/src/strnlen.h",
        "libgit2/src/submodule.c",
        "libgit2/src/submodule.h",
        "libgit2/src/sysdir.c",
        "libgit2/src/sysdir.h",
        "libgit2/src/tag.c",
        "libgit2/src/tag.h",
        "libgit2/src/thread-utils.c",
        "libgit2/src/thread-utils.h",
        "libgit2/src/trace.c",
        "libgit2/src/trace.h",
        "libgit2/src/transaction.c",
        "libgit2/src/transport.c",
        "libgit2/src/transports/auth.c",
        "libgit2/src/transports/auth.h",
        "libgit2/src/transports/cred_helpers.c",
        "libgit2/src/transports/cred.c",
        "libgit2/src/transports/git.c",
        "libgit2/src/transports/http.c",
        "libgit2/src/transports/local.c",
        "libgit2/src/transports/smart_pkt.c",
        "libgit2/src/transports/smart_protocol.c",
        "libgit2/src/transports/smart.c",
        "libgit2/src/transports/smart.h",
        "libgit2/src/transports/ssh.c",
        "libgit2/src/tree-cache.c",
        "libgit2/src/tree-cache.h",
        "libgit2/src/tree.c",
        "libgit2/src/tree.h",
        "libgit2/src/tsort.c",
        "libgit2/src/userdiff.h",
        "libgit2/src/util.c",
        "libgit2/src/util.h",
        "libgit2/src/varint.c",
        "libgit2/src/varint.h",
        "libgit2/src/vector.c",
        "libgit2/src/vector.h",
        "libgit2/src/xdiff/xdiff.h",
        "libgit2/src/xdiff/xdiffi.c",
        "libgit2/src/xdiff/xdiffi.h",
        "libgit2/src/xdiff/xemit.c",
        "libgit2/src/xdiff/xemit.h",
        "libgit2/src/xdiff/xhistogram.c",
        "libgit2/src/xdiff/xinclude.h",
        "libgit2/src/xdiff/xmacros.h",
        "libgit2/src/xdiff/xmerge.c",
        "libgit2/src/xdiff/xpatience.c",
        "libgit2/src/xdiff/xprepare.c",
        "libgit2/src/xdiff/xprepare.h",
        "libgit2/src/xdiff/xtypes.h",
        "libgit2/src/xdiff/xutils.c",
        "libgit2/src/xdiff/xutils.h",
        "libgit2/src/zstream.c",
        "libgit2/src/zstream.h"
      ],
      "conditions": [
        ["OS=='mac'", {
            "defines": [
                "GIT_SECURE_TRANSPORT",
                "GIT_USE_STAT_MTIMESPEC"
            ],
            "sources": [
                "libgit2/src/stransport_stream.c",
                "libgit2/src/stransport_stream.h",
                "libgit2/src/tls_stream.c",
                "libgit2/src/tls_stream.h"
            ],
            "link_settings": {
                "xcode_settings": {
                    "OTHER_LDFLAGS": [
                        "-framework Security",
                        "-framework CoreFoundation"
                    ],
                }
            }
        }],
        ["OS=='mac' or OS=='linux' or OS.endswith('bsd')", {
          "cflags": [
            "-DGIT_CURL"
          ],
          "defines": [
            "GIT_CURL"
          ]
        }],
        ["OS=='linux' or OS.endswith('bsd')" , {
          "cflags": [
            "-DGIT_SSH",
            "-DGIT_SSL",
            "-w",
          ],
          "defines": [
            "GIT_OPENSSL",
            "GIT_USE_STAT_MTIM"
          ],
          "sources": [
              "libgit2/src/tls_stream.c",
              "libgit2/src/tls_stream.h"
          ]
        }],
        ["OS=='win'", {
          "defines": [
            "GIT_WINHTTP",
          ],
          "msvs_settings": {
            "VCLinkerTool": {
              "AdditionalDependencies": [
                "ws2_32.lib"
              ],
            },
            # Workaround of a strange bug:
            # TargetMachine + static_library + x64 = nothing.
            "conditions": [
              ["target_arch=='x64'", {
                "VCLibrarianTool": {
                  "AdditionalOptions": [
                    "/MACHINE:X64",
                  ],
                },
              }, {
                "VCLibrarianTool": {
                  "AdditionalOptions": [
                    "/MACHINE:x86",
                  ],
                },
              }],
            ],
          },
          "msvs_disabled_warnings": [
            # Conversion from 'ssize_t' to 'int32_t', possible loss of data.
            4244,
            # Conversion from 'size_t' to 'int', possible loss of data.
            4267,
            # Different 'volatile' qualifiers.
            4090,
            # 'volatile void *' differs in levels of indirection from 'int'.
            4047,
            # 'InterlockedDecrement' undefined; assuming extern returning int.
            4013,
          ],
          "include_dirs": [
            "libgit2/deps/regex"
          ],
          "sources": [
            "libgit2/deps/regex/regex.c",
            "libgit2/src/transports/winhttp.c",
            "libgit2/src/win32/dir.c",
            "libgit2/src/win32/dir.h",
            "libgit2/src/win32/error.c",
            "libgit2/src/win32/error.h",
            "libgit2/src/win32/findfile.c",
            "libgit2/src/win32/findfile.h",
            "libgit2/src/win32/git2.rc",
            "libgit2/src/win32/map.c",
            "libgit2/src/win32/mingw-compat.h",
            "libgit2/src/win32/msvc-compat.h",
            "libgit2/src/win32/path_w32.c",
            "libgit2/src/win32/path_w32.h",
            "libgit2/src/win32/posix_w32.c",
            "libgit2/src/win32/posix.h",
            "libgit2/src/win32/precompiled.c",
            "libgit2/src/win32/precompiled.h",
            "libgit2/src/win32/thread.c",
            "libgit2/src/win32/thread.h",
            "libgit2/src/win32/reparse.h",
            "libgit2/src/win32/utf-conv.c",
            "libgit2/src/win32/utf-conv.h",
            "libgit2/src/win32/version.h",
            "libgit2/src/win32/w32_buffer.c",
            "libgit2/src/win32/w32_buffer.h",
            "libgit2/src/win32/w32_util.c",
            "libgit2/src/win32/w32_util.h",
          ],
        }, {
          "libraries": [
            "-lpthread",
          ],
          "sources": [
            "libgit2/src/unix/map.c",
            "libgit2/src/unix/posix.h",
            "libgit2/src/unix/pthread.h",
            "libgit2/src/unix/realpath.c",
          ],
          "cflags": [
            "-Wno-missing-field-initializers",
            "-Wno-unused-variable",
            "-Wno-deprecated-declarations",
          ],
          "xcode_settings": {
            "WARNING_CFLAGS": [
              "-Wno-missing-field-initializers",
              "-Wno-unused-variable",
              "-Wno-deprecated-declarations",
              "-Wno-uninitialized",
            ],
          },
        },
      ]
      ],
      "include_dirs": [
        "libgit2/include",
        "libgit2/src"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libgit2/include",
        ],
      },
    },
    {
      "target_name": "zlib",
      "type": "static_library",
      "sources": [
        "libgit2/deps/zlib/adler32.c",
        "libgit2/deps/zlib/crc32.c",
        "libgit2/deps/zlib/crc32.h",
        "libgit2/deps/zlib/deflate.c",
        "libgit2/deps/zlib/deflate.h",
        "libgit2/deps/zlib/inffast.c",
        "libgit2/deps/zlib/inffast.h",
        "libgit2/deps/zlib/inffixed.h",
        "libgit2/deps/zlib/inflate.c",
        "libgit2/deps/zlib/inflate.h",
        "libgit2/deps/zlib/inftrees.c",
        "libgit2/deps/zlib/inftrees.h",
        "libgit2/deps/zlib/trees.c",
        "libgit2/deps/zlib/trees.h",
        "libgit2/deps/zlib/zconf.h",
        "libgit2/deps/zlib/zlib.h",
        "libgit2/deps/zlib/zutil.c",
        "libgit2/deps/zlib/zutil.h",
      ],
      "defines": [
        "NO_VIZ",
        "STDC",
        "NO_GZIP",
      ],
      "conditions": [
        ["OS=='win'", {
          "include_dirs": [
            "libgit2/deps/regex"
          ]
        }]
      ],
      "include_dirs": [
        "libgit2/include"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libgit2/deps/zlib",
        ],
      },
    },
    {
      "target_name": "libssh2",
      "type": "static_library",
      "defines": [
        "NETSNMP_ENABLE_IPV6"
      ],
      "sources": [
        "libssh2/src/agent.c",
        "libssh2/src/crypt.c",
        "libssh2/src/keepalive.c",
        "libssh2/src/libgcrypt.c",
        "libssh2/src/openssl.c",
        "libssh2/src/publickey.c",
        "libssh2/src/sftp.c",
        "libssh2/src/version.c",
        "libssh2/src/channel.c",
        "libssh2/src/global.c",
        "libssh2/src/kex.c",
        "libssh2/src/mac.c",
        "libssh2/src/packet.c",
        "libssh2/src/scp.c",
        "libssh2/src/transport.c",
        "libssh2/src/comp.c",
        "libssh2/src/hostkey.c",
        "libssh2/src/knownhost.c",
        "libssh2/src/misc.c",
        "libssh2/src/pem.c",
        "libssh2/src/session.c",
        "libssh2/src/userauth.c",
      ],
      "include_dirs": [
        ".",
        "libssh2/include",
      ],
      "dependencies": [
        "openssl/openssl.gyp:openssl"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libssh2/include"
        ]
      },
      "conditions": [
        ["OS=='win'", {
          "include_dirs": [
            "libssh2/src",
            "libssh2/win32",
            "libssh2/include"
          ],
          "defines!": [
            "HAVE_POLL"
          ],
          "direct_dependent_settings": {
            "include_dirs": [
              "libssh2/src",
              "libssh2/win32",
              "libssh2/include"
            ]
          }
        }],
      ]
    }
  ]
}
