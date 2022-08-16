{
  # Copyright (c) 2012 The Chromium Authors. All rights reserved.
  # Use of this source code is governed by a BSD-style license that can be
  # found in the LICENSE file.
  "variables": {
    "target_arch%": "x86",
    "library%": "static_library",
    "openssl_enable_asm%": 0, # only supported with the Visual Studio 2012 (VC11) toolchain.
    "gcc_version%": 0,
    "is_electron%": "<!(node ../utils/isBuildingForElectron.js <(node_root_dir))",
    "is_clang%": 0,
    "is_IBMi%": "<!(node -p \"os.platform() == 'aix' && os.type() == 'OS400' ? 1 : 0\")",
    "electron_openssl_root%": "<!(node ../utils/getElectronOpenSSLRoot.js <(module_root_dir))",
    "electron_openssl_static%": "<!(node -p \"process.platform !== 'linux' || process.env.NODEGIT_OPENSSL_STATIC_LINK === '1' ? 1 : 0\")",
  },
  "targets": [
    {
      "target_name": "libgit2",
      "type": "static_library",
      "defines": [
        "GIT_THREADS",
        "GIT_SSH",
        "GIT_SSH_MEMORY_CREDENTIALS",
        "LIBGIT2_NO_FEATURES_H",
        "GIT_SHA1_COLLISIONDETECT",
        "GIT_SHA256_OPENSSL",
        "GIT_USE_NSEC",
        "GIT_HTTPS",
        # Node's util.h may be accidentally included so use this to guard
        # against compilation error.
        "SRC_UTIL_H_"
      ],
      "dependencies": [
        "zlib",
        "http_parser/http_parser.gyp:http_parser",
        "libssh2"
      ],
      "sources": [
        "libgit2/src/libgit2/annotated_commit.c",
        "libgit2/src/libgit2/apply.c",
        "libgit2/src/libgit2/attr.c",
        "libgit2/src/libgit2/attr_file.c",
        "libgit2/src/libgit2/attrcache.c",
        "libgit2/src/libgit2/blame.c",
        "libgit2/src/libgit2/blame_git.c",
        "libgit2/src/libgit2/blob.c",
        "libgit2/src/libgit2/branch.c",
        "libgit2/src/libgit2/buf.c",
        "libgit2/src/libgit2/cache.c",
        "libgit2/src/libgit2/checkout.c",
        "libgit2/src/libgit2/cherrypick.c",
        "libgit2/src/libgit2/clone.c",
        "libgit2/src/libgit2/commit.c",
        "libgit2/src/libgit2/commit_graph.c",
        "libgit2/src/libgit2/commit_list.c",
        "libgit2/src/libgit2/config.c",
        "libgit2/src/libgit2/config_cache.c",
        "libgit2/src/libgit2/config_entries.c",
        "libgit2/src/libgit2/config_file.c",
        "libgit2/src/libgit2/config_mem.c",
        "libgit2/src/libgit2/config_parse.c",
        "libgit2/src/libgit2/config_snapshot.c",
        "libgit2/src/libgit2/crlf.c",
        "libgit2/src/libgit2/delta.c",
        "libgit2/src/libgit2/diff.c",
        "libgit2/src/libgit2/diff_driver.c",
        "libgit2/src/libgit2/diff_file.c",
        "libgit2/src/libgit2/diff_generate.c",
        "libgit2/src/libgit2/diff_parse.c",
        "libgit2/src/libgit2/diff_print.c",
        "libgit2/src/libgit2/diff_stats.c",
        "libgit2/src/libgit2/diff_tform.c",
        "libgit2/src/libgit2/diff_xdiff.c",
        "libgit2/src/libgit2/email.c",
        "libgit2/src/libgit2/errors.c",
        "libgit2/src/libgit2/fetch.c",
        "libgit2/src/libgit2/fetchhead.c",
        "libgit2/src/libgit2/filter.c",
        "libgit2/src/libgit2/graph.c",
        "libgit2/src/libgit2/grafts.c",
        "libgit2/src/libgit2/hashsig.c",
        "libgit2/src/libgit2/ident.c",
        "libgit2/src/libgit2/idxmap.c",
        "libgit2/src/libgit2/ignore.c",
        "libgit2/src/libgit2/index.c",
        "libgit2/src/libgit2/indexer.c",
        "libgit2/src/libgit2/iterator.c",
        "libgit2/src/libgit2/libgit2.c",
        "libgit2/src/libgit2/mailmap.c",
        "libgit2/src/libgit2/merge.c",
        "libgit2/src/libgit2/merge_driver.c",
        "libgit2/src/libgit2/merge_file.c",
        "libgit2/src/libgit2/message.c",
        "libgit2/src/libgit2/midx.c",
        "libgit2/src/libgit2/mwindow.c",
        "libgit2/src/libgit2/notes.c",
        "libgit2/src/libgit2/object.c",
        "libgit2/src/libgit2/object_api.c",
        "libgit2/src/libgit2/odb.c",
        "libgit2/src/libgit2/odb_loose.c",
        "libgit2/src/libgit2/odb_mempack.c",
        "libgit2/src/libgit2/odb_pack.c",
        "libgit2/src/libgit2/offmap.c",
        "libgit2/src/libgit2/oid.c",
        "libgit2/src/libgit2/oidarray.c",
        "libgit2/src/libgit2/oidmap.c",
        "libgit2/src/libgit2/pack-objects.c",
        "libgit2/src/libgit2/pack.c",
        "libgit2/src/libgit2/parse.c",
        "libgit2/src/libgit2/patch.c",
        "libgit2/src/libgit2/patch_generate.c",
        "libgit2/src/libgit2/patch_parse.c",
        "libgit2/src/libgit2/path.c",
        "libgit2/src/libgit2/pathspec.c",
        "libgit2/src/libgit2/proxy.c",
        "libgit2/src/libgit2/push.c",
        "libgit2/src/libgit2/reader.c",
        "libgit2/src/libgit2/rebase.c",
        "libgit2/src/libgit2/refdb.c",
        "libgit2/src/libgit2/refdb_fs.c",
        "libgit2/src/libgit2/reflog.c",
        "libgit2/src/libgit2/refs.c",
        "libgit2/src/libgit2/refspec.c",
        "libgit2/src/libgit2/remote.c",
        "libgit2/src/libgit2/repository.c",
        "libgit2/src/libgit2/reset.c",
        "libgit2/src/libgit2/revert.c",
        "libgit2/src/libgit2/revparse.c",
        "libgit2/src/libgit2/revwalk.c",
        "libgit2/src/libgit2/signature.c",
        "libgit2/src/libgit2/stash.c",
        "libgit2/src/libgit2/status.c",
        "libgit2/src/libgit2/strarray.c",
        "libgit2/src/libgit2/streams/mbedtls.c",
        "libgit2/src/libgit2/streams/openssl.c",
        "libgit2/src/libgit2/streams/openssl_legacy.c",
        "libgit2/src/libgit2/streams/registry.c",
        "libgit2/src/libgit2/streams/socket.c",
        "libgit2/src/libgit2/submodule.c",
        "libgit2/src/libgit2/sysdir.c",
        "libgit2/src/libgit2/tag.c",
        "libgit2/src/libgit2/threadstate.c",
        "libgit2/src/libgit2/trace.c",
        "libgit2/src/libgit2/trailer.c",
        "libgit2/src/libgit2/transaction.c",
        "libgit2/src/libgit2/transport.c",
        "libgit2/src/libgit2/transports/credential.c",
        "libgit2/src/libgit2/transports/credential_helpers.c",
        "libgit2/src/libgit2/transports/git.c",
        "libgit2/src/libgit2/transports/httpclient.c",
        "libgit2/src/libgit2/transports/local.c",
        "libgit2/src/libgit2/transports/smart.c",
        "libgit2/src/libgit2/transports/smart_pkt.c",
        "libgit2/src/libgit2/transports/smart_protocol.c",
        "libgit2/src/libgit2/transports/ssh.c",
        "libgit2/src/libgit2/tree-cache.c",
        "libgit2/src/libgit2/tree.c",
        "libgit2/src/libgit2/worktree.c",
        "libgit2/deps/xdiff/xdiffi.c",
        "libgit2/deps/xdiff/xemit.c",
        "libgit2/deps/xdiff/xhistogram.c",
        "libgit2/deps/xdiff/xmerge.c",
        "libgit2/deps/xdiff/xpatience.c",
        "libgit2/deps/xdiff/xprepare.c",
        "libgit2/deps/xdiff/xutils.c",
        "libgit2/src/util/alloc.c",
        "libgit2/src/util/allocators/failalloc.c",
        "libgit2/src/util/allocators/stdalloc.c",
        "libgit2/src/util/custom_tls.c",
        "libgit2/src/util/date.c",
        "libgit2/src/util/filebuf.c",
        "libgit2/src/util/fs_path.c",
        "libgit2/src/util/futils.c",
        "libgit2/src/util/hash.c",
        "libgit2/src/util/hash/collisiondetect.c",
        "libgit2/src/util/hash/openssl.c",
        "libgit2/src/util/hash/sha1dc/sha1.c",
        "libgit2/src/util/hash/sha1dc/ubc_check.c",
        "libgit2/src/util/net.c",
        "libgit2/src/util/pool.c",
        "libgit2/src/util/posix.c",
        "libgit2/src/util/pqueue.c",
        "libgit2/src/util/rand.c",
        "libgit2/src/util/regexp.c",
        "libgit2/src/util/runtime.c",
        "libgit2/src/util/sortedcache.c",
        "libgit2/src/util/str.c",
        "libgit2/src/util/strmap.c",
        "libgit2/src/util/thread.c",
        "libgit2/src/util/tsort.c",
        "libgit2/src/util/utf8.c",
        "libgit2/src/util/util.c",
        "libgit2/src/util/varint.c",
        "libgit2/src/util/vector.c",
        "libgit2/src/util/wildmatch.c",
        "libgit2/src/util/zstream.c",
      ],
      "conditions": [
        ["target_arch=='x64'", {
          "defines": [
            "GIT_ARCH_64"
          ]
        }, {
          "defines": [
            "GIT_ARCH_32"
          ]
        }],
        ["OS=='mac'", {
          "defines": [
              "GIT_SECURE_TRANSPORT",
              "GIT_USE_STAT_MTIMESPEC",
              "GIT_REGEX_REGCOMP_L",
              "GIT_USE_ICONV"
          ],
          "sources": [
              "libgit2/src/libgit2/streams/stransport.c",
          ],
          "libraries": [
            "-liconv",
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
        ["OS=='mac' or OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "conditions": [
            ["<(is_electron) == 1 and <(electron_openssl_static) == 1", {
              "include_dirs": [
                "<(electron_openssl_root)/include"
              ]
            }]
          ],
          "dependencies": [
            "ntlmclient"
          ],
          "include_dirs": [
            "libgit2/deps/ntlmclient"
          ],
          "defines": [
            "GIT_NTLM",
            "GIT_GSSAPI",
            "GIT_IO_POLL" # theres a chance this breaks bsd
          ],
          "sources": [
            "libgit2/src/libgit2/streams/tls.c",
            "libgit2/src/libgit2/transports/auth.c",
            "libgit2/src/libgit2/transports/auth_gssapi.c",
            "libgit2/src/libgit2/transports/auth_ntlmclient.c",
            "libgit2/src/libgit2/transports/http.c",
          ],
          "cflags": [
            "<!(krb5-config gssapi --cflags)"
          ],
          "libraries": [
            "<!(krb5-config gssapi --libs)"
          ]
        }],
        ["OS=='win' or OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "dependencies": [
            "pcre"
          ],
          "include_dirs": ["libgit2/deps/pcre"],
          "defines": [
            "GIT_REGEX_BUILTIN"
          ]
        }],
        ["OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "defines": [
            "GIT_OPENSSL",
            "GIT_USE_FUTIMENS",
            "GIT_USE_STAT_MTIM"
          ]
        }],
        ["<(is_IBMi) == 1", {
          "include_dirs": [
            "/QOpenSys/pkgs/include",
          ]
        }],
        ["OS=='win'", {
          "defines": [
            "GIT_WINHTTP",
            "GIT_IO_WSAPOLL"
          ],
          "conditions": [
            ["<(is_electron) == 1", {
              "include_dirs": [
                "<(electron_openssl_root)/include"
              ]
            }]
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
              }]
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
          "sources": [
            "libgit2/src/libgit2/transports/winhttp.c",
            "libgit2/src/util/allocators/win32_leakcheck.c",
            "libgit2/src/util/win32/dir.c",
            "libgit2/src/util/win32/error.c",
            "libgit2/src/util/win32/map.c",
            "libgit2/src/util/win32/path_w32.c",
            "libgit2/src/util/win32/posix_w32.c",
            "libgit2/src/util/win32/precompiled.c",
            "libgit2/src/util/win32/thread.c",
            "libgit2/src/util/win32/utf-conv.c",
            "libgit2/src/util/win32/w32_buffer.c",
            "libgit2/src/util/win32/w32_leakcheck.c",
            "libgit2/src/util/win32/w32_util.c",
          ],
        }, {
          "libraries": [
            "-lpthread",
          ],
          "sources": [
            "libgit2/src/util/unix/map.c",
            "libgit2/src/util/unix/pthread.c",
            "libgit2/src/util/unix/realpath.c",
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
        "libgit2/deps/xdiff",
        "libgit2/include",
        "libgit2/src/libgit2",
        "libgit2/src/util"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libgit2/include"
        ],
      },
    },
    {
      "target_name": "zlib",
      "type": "static_library",
      "sources": [
        "libgit2/deps/zlib/adler32.c",
        "libgit2/deps/zlib/crc32.c",
        "libgit2/deps/zlib/deflate.c",
        "libgit2/deps/zlib/inffast.c",
        "libgit2/deps/zlib/inflate.c",
        "libgit2/deps/zlib/inftrees.c",
        "libgit2/deps/zlib/trees.c",
        "libgit2/deps/zlib/zutil.c",
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
        "libssh2/src/bcrypt_pbkdf.c",
        "libssh2/src/blowfish.c",
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
        "libssh2/src/agent_win.c",
      ],
      "include_dirs": [
        ".",
        "libssh2/include",
      ],
      "hard_dependencies": [
        "../binding.gyp:configureLibssh2"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libssh2/include"
        ]
      },
      "conditions": [
        ["OS=='mac' or OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "conditions": [
            ["<(is_electron) == 1 and <(electron_openssl_static) == 1", {
              "include_dirs": [
                "<(electron_openssl_root)/include"
              ]
            }]
          ],
        }],
        ["OS=='win'", {
          "conditions": [
            ["<(is_electron) == 1", {
              "include_dirs": [
                "<(electron_openssl_root)/include"
              ],
              "libraries": [
                "<(electron_openssl_root)/lib/libssl.lib",
                "<(electron_openssl_root)/lib/libcrypto.lib"
              ]
            }, {
              "defines": [
                "OPENSSL_NO_RIPEMD",
                "OPENSSL_NO_CAST"
              ]
            }]
          ],
          "include_dirs": [
            "libssh2/src",
            "libssh2/include"
          ],
          "defines!": [
            "HAVE_POLL"
          ],
          "direct_dependent_settings": {
            "include_dirs": [
              "libssh2/src",
              "libssh2/include"
            ]
          }
        }],
        ["<(is_IBMi) == 1", {
          "include_dirs": [
            "/QOpenSys/pkgs/include"
          ]
        }],
      ]
    },
    {
      "target_name": "ntlmclient",
      "type": "static_library",
      "sources": [
        "libgit2/deps/ntlmclient/ntlm.c",
        "libgit2/deps/ntlmclient/unicode_builtin.c",
        "libgit2/deps/ntlmclient/util.c",
      ],
      "defines": [
        "UNICODE_BUILTIN"
      ],
      "conditions": [
        ["OS=='mac'", {
          "conditions": [
            ["<(is_electron) == 1", {
              "include_dirs": ["<(electron_openssl_root)/include"]
            }]
          ],
          "sources": [
            "libgit2/deps/ntlmclient/crypt_commoncrypto.c",
          ],
          "defines": [
            "CRYPT_COMMONCRYPTO"
          ]
        }],
        ["OS=='linux'", {
          "conditions": [
            ["<(is_electron) == 1 and <(electron_openssl_static) == 1", {
              "include_dirs": ["<(electron_openssl_root)/include"]
            }]
          ],
          "sources": [
            "libgit2/deps/ntlmclient/crypt_openssl.c",
          ],
          "defines": [
            "CRYPT_OPENSSL"
          ]
        }]
      ]
    },
    {
      "target_name": "pcre",
      "type": "static_library",
      "sources": [
        "libgit2/deps/pcre/pcre_byte_order.c",
        "libgit2/deps/pcre/pcre_chartables.c",
        "libgit2/deps/pcre/pcre_compile.c",
        "libgit2/deps/pcre/pcre_config.c",
        "libgit2/deps/pcre/pcre_dfa_exec.c",
        "libgit2/deps/pcre/pcre_exec.c",
        "libgit2/deps/pcre/pcre_fullinfo.c",
        "libgit2/deps/pcre/pcre_get.c",
        "libgit2/deps/pcre/pcre_globals.c",
        "libgit2/deps/pcre/pcre_jit_compile.c",
        "libgit2/deps/pcre/pcre_maketables.c",
        "libgit2/deps/pcre/pcre_newline.c",
        "libgit2/deps/pcre/pcre_ord2utf8.c",
        "libgit2/deps/pcre/pcreposix.c",
        "libgit2/deps/pcre/pcre_printint.c",
        "libgit2/deps/pcre/pcre_refcount.c",
        "libgit2/deps/pcre/pcre_string_utils.c",
        "libgit2/deps/pcre/pcre_study.c",
        "libgit2/deps/pcre/pcre_tables.c",
        "libgit2/deps/pcre/pcre_ucd.c",
        "libgit2/deps/pcre/pcre_valid_utf8.c",
        "libgit2/deps/pcre/pcre_version.c",
        "libgit2/deps/pcre/pcre_xclass.c",
      ],
      "conditions": [
        ["OS=='linux' or OS.endswith('bsd') or <(is_IBMi) == 1", {
          "defines": [
            "HAVE_DIRENT_H",
            "HAVE_SYS_STAT_H",
            "HAVE_SYS_TYPES_H",
            "HAVE_UNISTD_H",
            "HAVE_STDINT_H",
            "HAVE_INTTYPES_H",
            "HAVE_BCOPY",
            "HAVE_MEMMOVE",
            "HAVE_STRERROR",
            "HAVE_STRTOLL",
            "HAVE_STRTOQ",
            "SUPPORT_PCRE8",
            "NO_RECURSE",
            "HAVE_LONG_LONG",
            "HAVE_UNSIGNED_LONG_LONG",
            "NEWLINE=10",
            "POSIX_MALLOC_THRESHOLD=10",
            "LINK_SIZE=2",
            "PARENS_NEST_LIMIT=250",
            "MATCH_LIMIT=10000000",
            "MATCH_LIMIT_RECURSION=10000000",
            "PCREGREP_BUFSIZE",
            "MAX_NAME_SIZE=32",
            "MAX_NAME_COUNT=10000"
          ]
        }],
        ["OS=='win'", {
          "defines": [
            "HAVE_SYS_STAT_H",
            "HAVE_SYS_TYPES_H",
            "HAVE_WINDOWS_H",
            "HAVE_STDINT_H",
            "HAVE_INTTYPES_H",
            "HAVE_MEMMOVE",
            "HAVE_STRERROR",
            "HAVE_STRTOLL",
            "HAVE__STRTOI64",
            "SUPPORT_PCRE8",
            "NO_RECURSE",
            "HAVE_LONG_LONG",
            "HAVE_UNSIGNED_LONG_LONG",
            "NEWLINE=10",
            "POSIX_MALLOC_THRESHOLD=10",
            "LINK_SIZE=2",
            "PARENS_NEST_LIMIT=250",
            "MATCH_LIMIT=10000000",
            "MATCH_LIMIT_RECURSION=10000000",
            "PCREGREP_BUFSIZE",
            "MAX_NAME_SIZE=32",
            "MAX_NAME_COUNT=10000"
          ]
        }]
      ]
    }
  ]
}
