# Change Log

## <a name="v0-23-0" href="#v0-23-0">v0.23.0</a> [(2018-10-22)](https://github.com/nodegit/nodegit/releases/tag/v0.23.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.22.2...v0.23.0)

#### Summary of changes
- Added Node 10 support.
- We no longer compile OpenSSL. Instead, we prefer the OpenSSL shipped with Node. In electron builds, we prefer the system openssl on linux, and we get prebuilt static libs from Conan.
- Cleaned up some compiler warnings
- Our http_parser dependency is now a submodule.
- Updated some dependencies in npm to get rid of security vulnerabilities and warnings.
- Exposed transfer progress callback for pushing.
- Libssh2 is now preconfigured for both Mac OS and Windows

#### Merged PRs into NodeGit
- [warn about node v10 incompatibility in readme #1534](https://github.com/nodegit/nodegit/pull/1534)
- [Define error codes to fix compiler warnings about unused variables #1486](https://github.com/nodegit/nodegit/pull/1486)
- [Include http_parser via submodule #1551](https://github.com/nodegit/nodegit/pull/1551)
- [Update dependencies to get rid of security vulnerabilities #1547](https://github.com/nodegit/nodegit/pull/1547)
- [add github issue template #1548](https://github.com/nodegit/nodegit/pull/1548)
- [Enable git_push_transfer_progress - Help wanted #1500](https://github.com/nodegit/nodegit/pull/1500)
- [Fixed createBranch API params #1552](https://github.com/nodegit/nodegit/pull/1552)
- [Use curl-config to find curl #1555](https://github.com/nodegit/nodegit/pull/1555)
- [Update readme #1554](https://github.com/nodegit/nodegit/pull/1554)
- [Node 10 support #1545](https://github.com/nodegit/nodegit/pull/1545)
- [Update dependencies #1519](https://github.com/nodegit/nodegit/pull/1519)
- [Run submodule updates in sequence rather than parallel #1558](https://github.com/nodegit/nodegit/pull/1558)
- [Fix Electron builds on win32 #1560](https://github.com/nodegit/nodegit/pull/1560)
- [Use static `libssh2_config.h` on MacOS builds #1569](https://github.com/nodegit/nodegit/pull/1569)


## <a name="v0-23-0-alpha.2" href="#v0-23-0-alpha.2">v0.23.0-alpha.2</a> [(2018-10-19)](https://github.com/nodegit/nodegit/releases/tag/v0.23.0-alpha.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.23.0-alpha.1...v0.23.0-alpha.2)

#### Summary of changes
On Mac OS Mojave, libssh2 configure failed to configure, because libssh2 always tries to configure against system libraries. The configure process uses links to determine that a OpenSSL is present. Since OSX and Windows no longer link against the system lib, we've prebuilt the configure steps for both environments. This should be stable when building NodeGit on Mojave.

#### Merged PRs into NodeGit
- [Use static `libssh2_config.h` on MacOS builds #1569](https://github.com/nodegit/nodegit/pull/1569)


## <a name="v0-23-0-alpha.1" href="#v0-23-0-alpha.1">v0.23.0-alpha.1</a> [(2018-10-01)](https://github.com/nodegit/nodegit/releases/tag/v0.23.0-alpha.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.22.2...v0.23.0-alpha.1)

#### Summary of changes
- Added Node 10 support.
- We no longer compile OpenSSL. Instead, we prefer the OpenSSL shipped with Node. In electron builds, we prefer the system openssl on linux, and we get prebuilt static libs from Conan.
- Cleaned up some compiler warnings
- Our http_parser dependency is now a submodule.
- Updated some dependencies in npm to get rid of security vulnerabilities and warnings.
- Exposed transfer progress callback for pushing.

#### Merged PRs into NodeGit
- [warn about node v10 incompatibility in readme #1534](https://github.com/nodegit/nodegit/pull/1534)
- [Define error codes to fix compiler warnings about unused variables #1486](https://github.com/nodegit/nodegit/pull/1486)
- [Include http_parser via submodule #1551](https://github.com/nodegit/nodegit/pull/1551)
- [Update dependencies to get rid of security vulnerabilities #1547](https://github.com/nodegit/nodegit/pull/1547)
- [add github issue template #1548](https://github.com/nodegit/nodegit/pull/1548)
- [Enable git_push_transfer_progress - Help wanted #1500](https://github.com/nodegit/nodegit/pull/1500)
- [Fixed createBranch API params #1552](https://github.com/nodegit/nodegit/pull/1552)
- [Use curl-config to find curl #1555](https://github.com/nodegit/nodegit/pull/1555)
- [Update readme #1554](https://github.com/nodegit/nodegit/pull/1554)
- [Node 10 support #1545](https://github.com/nodegit/nodegit/pull/1545)
- [Update dependencies #1519](https://github.com/nodegit/nodegit/pull/1519)
- [Run submodule updates in sequence rather than parallel #1558](https://github.com/nodegit/nodegit/pull/1558)
- [Fix Electron builds on win32 #1560](https://github.com/nodegit/nodegit/pull/1560)


## <a name="v0-22-2" href="#v0-22-2">v0.22.2</a> [(2018-07-10)](https://github.com/nodegit/nodegit/releases/tag/v0.22.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.22.1...v0.22.2)

#### Summary of changes
- Bumped libgit2 to 0.27.3 to incorporate security patch. Details can be found here https://github.com/libgit2/libgit2/releases/tag/v0.27.3
- Also includes changes made in 0.27.2. Details can be found here https://github.com/libgit2/libgit2/releases/tag/v0.27.2
- Ensure optional params are respected as optional in `NodeGit.Merge.trees`

#### Merged PRs into NodeGit
- [Fix argument handling in Git.Merge.trees #1507](https://github.com/nodegit/nodegit/pull/1507)

#### Included non-merged libgit2 PRs:
 - [Parallelize checkout_create_the_new for ntfs perf gains #4205](https://github.com/libgit2/libgit2/pull/4205)
 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)

#### Included merged libgit2 PRs:
- [Bugfix release v0.27.2 #4632](https://github.com/libgit2/libgit2/pull/4632)
- [Release v0.27.3 #4717](https://github.com/libgit2/libgit2/pull/4717)


## <a name="v0-22-1" href="#v0-22-1">v0.22.1</a> [(2018-04-09)](https://github.com/nodegit/nodegit/releases/tag/v0.22.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.22.0...v0.22.1)

#### Summary of changes
Bumped libgit2 to 0.27.1 to address security flaw with submodule name validation (CVE-2018-11235, reported by Etienne Stalmans).
Details can be found here https://github.com/libgit2/libgit2/releases/tag/v0.27.1


## <a name="v0-22-0" href="#v0-22-0">v0.22.0</a> [(2018-04-09)](https://github.com/nodegit/nodegit/releases/tag/v0.22.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.21.2...v0.22.0)

#### Summary of changes
- Expose [Tag.listMatch](https://libgit2.github.com/libgit2/#v0.26.3/group/tag/git_tag_list_match)
- Expose [Repo.prototype.createCommitBuffer](https://libgit2.github.com/libgit2/#v0.26.3/group/commit/git_commit_create_buffer)
- Bump Libgt2 to 0.27.0. For more information about what was in this release. [Check upstream](https://github.com/libgit2/libgit2/releases/tag/v0.27.0).
- Errors are now properly thrown from
  - `Attr.prototype.get`
  - `Blob.createFrombuffer`
  - `Blob.createFromworkdir`
  - `Reference.list`
  - `Remote.prototype.getFetchRefspecs`
  - `Remote.prototype.getPushRefspecs`
  - `Status.file`
- WorkTree is now exposed
  - Static methods
    - `add`
    - `list`
    - `lookup`
    - `openFromRepository`
  - Instance methods
    - `isLocked`
    - `isPrunable`
    - `lock`
    - `prune`
    - `unlock`
    - `validate`
- **BREAKING** Functions that are now async
  - `Attr.prototype.getMany`
  - `Tag.prototype.target`
  - `Treebuilder.prototype.Write`
- **BREAKING** Diffs generated from `Diff.treeToIndex` and `Diff.treeToWorkdirWithIndex` conditionally support `Diff.OPTION.IGNORE_CASE`. Only on repositories where the index is case insensitive will the flag be set on the output diff. The flag itself is ignored when passed to `DiffOptions`.

#### Merged PRs into NodeGit
- [Add JavaScript Tag.listMatch function for git_tag_list_match #1470](https://github.com/nodegit/nodegit/pull/1470)
- [Expose error code in Status.file #1468](https://github.com/nodegit/nodegit/pull/1468)
- [Travis documentation deploy fixes #1466](https://github.com/nodegit/nodegit/pull/1466)
- [Bump to libgit2 v0.27.0 #1477](https://github.com/nodegit/nodegit/pull/1477)
- [Add repo.createCommitBuffer #1481](https://github.com/nodegit/nodegit/pull/1481)

#### Included non-merged libgit2 PRs:
 - [Parallelize checkout_create_the_new for ntfs perf gains #4205](https://github.com/libgit2/libgit2/pull/4205)
 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)


## <a name="v0-21-2" href="#v0-21-2">v0.21.2</a> [(2018-03-19)](https://github.com/nodegit/nodegit/releases/tag/v0.21.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.21.1...v0.21.2)

#### Summary of changes
- Drop support for node 7
- Fixed an issue where fast-forward rebase would throw an error unexepctedly.
- Expose git_commit_extract_signature
- TreeEntry.prototype.path() should always return posix style separators
- Expose git_config_open_ondisk
- Expose git_config_find_global
- Expose git_config_find_system
- Expose git_config_find_xdg
- Expose git_config_get_path

#### Merged PRs into NodeGit
 - [Fix typo in CHANGELOG.md #1455](https://github.com/nodegit/nodegit/pull/1455)
 - [Add two tests for Tag #1452](https://github.com/nodegit/nodegit/pull/1452)
 - [Handle fast-forward merges properly in repository.performRebase #1457](https://github.com/nodegit/nodegit/pull/1457)
 - [Enable git_commit_extract_signature #1458](https://github.com/nodegit/nodegit/pull/1458)
 - [removes 'only' from test #1460](https://github.com/nodegit/nodegit/pull/1460)
 - [Deploy documentation and make extended tests visible #1447](https://github.com/nodegit/nodegit/pull/1447)
 - [resolves #1433 TreeEntry#path should return posix path #1434](https://github.com/nodegit/nodegit/pull/1434)
 - [Exposed git_config_open_ondisk #1459](https://github.com/nodegit/nodegit/pull/1459)
 - [Expose git_config functions #1463](https://github.com/nodegit/nodegit/pull/1463)


## <a name="v0-21-1" href="#v0-21-1">v0.21.1</a> [(2018-03-05)](https://github.com/nodegit/nodegit/releases/tag/v0.21.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.21.0...v0.21.1)

#### Summary of changes
- Fixed an issue where having html escape characters in the path portion of an ssh url always fails when performing remote actions.

#### Included non-merged libgit2 PRs:
- [Unescape special characters in SSH repo paths #4557](https://github.com/libgit2/libgit2/pull/4557)

## <a name="v0-21-0" href="#v0-21-0">v0.21.0</a> [(2018-02-26)](https://github.com/nodegit/nodegit/releases/tag/v0.21.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.20.3...v0.21.0)

#### Summary of changes to NodeGit:
 - [Generate default options for Revert.revert to prevent an exception #1404](https://github.com/nodegit/nodegit/pull/1404)
 - [fix issue with re-require()'ing native modules #1354](https://github.com/nodegit/nodegit/pull/1354)
 - [Expose JavaScript function name in a rejected promise #1393](https://github.com/nodegit/nodegit/pull/1393)
 - [Fix compiling with clang #1344](https://github.com/nodegit/nodegit/pull/1344)
 - [Fix typos #1415](https://github.com/nodegit/nodegit/pull/1415)
 - [build: add NetBSD support to opensslconf.h #1374](https://github.com/nodegit/nodegit/pull/1374)
 - [Fix example by forcing backslashes with path.posix #1346](https://github.com/nodegit/nodegit/pull/1346)
 - [Expose the Node.js GC when running `mocha `scripts #1258](https://github.com/nodegit/nodegit/pull/1258)
 - [Exposed filter_list; Fixed discard lines to use filters #1414](https://github.com/nodegit/nodegit/pull/1414)
 - [Add tests for AnnotatedCommit #1325](https://github.com/nodegit/nodegit/pull/1325)
 - [Adding docs for NodeGit #1350](https://github.com/nodegit/nodegit/pull/1350)
 - [Fix details-for-tree-entry.js to access fields instead of calling a function #1326](https://github.com/nodegit/nodegit/pull/1326)
 - [Add JavaScript version of `git_tag_peel()` #1269](https://github.com/nodegit/nodegit/pull/1269)
 - [Fixed documentation error #1417](https://github.com/nodegit/nodegit/pull/1417)
 - [Annotate Blame.file as async #1421](https://github.com/nodegit/nodegit/pull/1421)
 - [Add two missing tests to Commit #1422](https://github.com/nodegit/nodegit/pull/1422)
 - [CI: Update Node.js versions + some deps #1444](https://github.com/nodegit/nodegit/pull/1444)
 - [Expose fetch_options.custom_headers #1430](https://github.com/nodegit/nodegit/pull/1430)

## <a name="v0-20-3" href="#v0-20-3">v0.20.3</a> [(2017-10-18)](https://github.com/nodegit/nodegit/releases/tag/v0.20.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.20.2...v0.20.3)

#### Summary of changes outside of libgit2 bump:
 - [Bumped libgit to libgit-next head #1387](https://github.com/nodegit/nodegit/pull/1387)
 - [Added a better normalizeOptions pattern for the submodule update routine #1388](https://github.com/nodegit/nodegit/pull/1388)
  - [Updated input docs to allow for git_push_update_reference_cb to generate #1386](https://github.com/nodegit/nodegit/pull/1386)
  - [Updated descriptors for git_index_find and git_index_find_prefix #1391](https://github.com/nodegit/nodegit/pull/1391)

#### Included merged libgit2 PRs:
 - [Fix template dir empty string #4273](https://github.com/libgit2/libgit2/pull/4273)
 - [travis: upgrade container to Ubuntu 14.04 #4211](https://github.com/libgit2/libgit2/pull/4211)
 - [merge: perform exact rename detection in linear time #4202](https://github.com/libgit2/libgit2/pull/4202)
 - [merge: fix potential free of uninitialized memory #4277](https://github.com/libgit2/libgit2/pull/4277)
 - [cmake: Permit disabling external http-parser #4278](https://github.com/libgit2/libgit2/pull/4278)
 - [Out of tree builds #4235](https://github.com/libgit2/libgit2/pull/4235)
 - [Convert port with htons() in p_getaddrinfo() #4280](https://github.com/libgit2/libgit2/pull/4280)
 - [tests: config: fix missing declaration causing error #4291](https://github.com/libgit2/libgit2/pull/4291)
 - [git_reset_*: pass parameters as const pointers #4287](https://github.com/libgit2/libgit2/pull/4287)
 - [signature: don't leave a dangling pointer to the strings on parse failure #4306](https://github.com/libgit2/libgit2/pull/4306)
 - [Patch ID calculation #4272](https://github.com/libgit2/libgit2/pull/4272)
 - [Configuration file fixes with includes #4250](https://github.com/libgit2/libgit2/pull/4250)
 - [win32: provide fast-path for retrying filesystem operations #4311](https://github.com/libgit2/libgit2/pull/4311)
 - [Build with patched libcurl #4317](https://github.com/libgit2/libgit2/pull/4317)
 - [tsort: remove idempotent conditional assignment #4314](https://github.com/libgit2/libgit2/pull/4314)
 - [tests: rebase::submodule: verify initialization method calls #4320](https://github.com/libgit2/libgit2/pull/4320)
 - [Remove unused 'sys/remote.h' header #4323](https://github.com/libgit2/libgit2/pull/4323)
 - [patch_generate: represent buffers as void pointers #4304](https://github.com/libgit2/libgit2/pull/4304)
 - [sha1_position: convert do-while to while #4326](https://github.com/libgit2/libgit2/pull/4326)
 - [sha1_lookup: drop sha1_entry_pos function #4327](https://github.com/libgit2/libgit2/pull/4327)
 - [oid: use memcmp in git_oid__hashcmp #4328](https://github.com/libgit2/libgit2/pull/4328)
 - [Docs: Fix inline comments for git_diff_hunk #4330](https://github.com/libgit2/libgit2/pull/4330)
 - [Split up CMakeLists.txt build instructions #4282](https://github.com/libgit2/libgit2/pull/4282)
 - [-Werror builds for Travis #4279](https://github.com/libgit2/libgit2/pull/4279)
 - [Submodules with bare repo #4305](https://github.com/libgit2/libgit2/pull/4305)
 - [Fix negative ignore rules with patterns #4296](https://github.com/libgit2/libgit2/pull/4296)
 - [README: Mention Guile-Git bindings. #4342](https://github.com/libgit2/libgit2/pull/4342)
 - [features.h: allow building without CMake-generated feature header #4346](https://github.com/libgit2/libgit2/pull/4346)
 - [Clear the remote_ref_name buffer in git_push_update_tips() #4344](https://github.com/libgit2/libgit2/pull/4344)
 - [Fix AppVeyor build failures due to CRTDBG linking issue #4347](https://github.com/libgit2/libgit2/pull/4347)
 - [diff: cleanup hash ctx in `git_diff_patchid` #4348](https://github.com/libgit2/libgit2/pull/4348)
 - [Reproducible builds #4334](https://github.com/libgit2/libgit2/pull/4334)
 - [Static linking for bundled deps #4339](https://github.com/libgit2/libgit2/pull/4339)
 - [Use SOCK_CLOEXEC when creating sockets #4364](https://github.com/libgit2/libgit2/pull/4364)
 - [Document that a commit is not a descendant of itself #4362](https://github.com/libgit2/libgit2/pull/4362)
 - [refs: do not use peeled OID if peeling to a tag #4367](https://github.com/libgit2/libgit2/pull/4367)
 - [remote: add typedef to normalize push_update_reference callback #4363](https://github.com/libgit2/libgit2/pull/4363)
 - [travis: add custom apt sources #4321](https://github.com/libgit2/libgit2/pull/4321)
 - [Fix Issue #4047 Check return codes and free objects #4370](https://github.com/libgit2/libgit2/pull/4370)
 - [Plug some leaks in curl's proxy handling #4359](https://github.com/libgit2/libgit2/pull/4359)
 - [Checkout typechange-only deltas #4369](https://github.com/libgit2/libgit2/pull/4369)
 - [tests: checkout::tree: verify status entrycount changes on chmod #4371](https://github.com/libgit2/libgit2/pull/4371)
 - [transports: smart: fix memory leak when skipping symbolic refs #4368](https://github.com/libgit2/libgit2/pull/4368)
 - [cmake: fix linking in Xcode with object libraries only #4372](https://github.com/libgit2/libgit2/pull/4372)
 - [cmake: use static dependencies when building static libgit2 #4356](https://github.com/libgit2/libgit2/pull/4356)

#### Included non-merged libgit2 PRs:
 - [Parallelize checkout_create_the_new for ntfs perf gains #4205](https://github.com/libgit2/libgit2/pull/4205)
 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)



## <a name="v0-20-2" href="#v0-20-2">v0.20.2</a> [(2017-09-13)](https://github.com/nodegit/nodegit/releases/tag/v0.20.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.20.0...v0.20.2)

#### Summary of changes outside of libgit2 bump:
 - [Fixed an issue where large buffer allocations in filters (> 256MB) causes a segfault #1368](https://github.com/nodegit/nodegit/pull/1368)
 - [Fix git_tree_entry double free #1332](https://github.com/nodegit/nodegit/pull/1332)

## <a name="v0-20-0" href="#v0-20-0">v0.20.0</a> [(2017-08-16)](https://github.com/nodegit/nodegit/releases/tag/v0.20.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.19.0...v0.20.0)

#### Summary of changes outside of libgit2 bump:
 - [BREAKING: Added blob methods and converted to async #1327](https://github.com/nodegit/nodegit/pull/1327)
 - [BREAKING: Convert sync methods to async methods #1348](https://github.com/nodegit/nodegit/pull/1348)
 - [Exposed libgit2 git_branch_remote_name method #1340](https://github.com/nodegit/nodegit/pull/1340)
 - [Adding git_filter support in nodegit #1331](https://github.com/nodegit/nodegit/pull/1331)
 - [Add CI build config for node stable version #1337](https://github.com/nodegit/nodegit/pull/1337)
 - [removed node v4 tests #1330](https://github.com/nodegit/nodegit/pull/1330)

#### Included merged libgit2 PRs:
 - [Include fixups #4288](https://github.com/libgit2/libgit2/pull/4288)
 - [Docs: Fix inline comments for git_diff_hunk #4330](https://github.com/libgit2/libgit2/pull/4330)
 - [oid: use memcmp in git_oid__hashcmp #4328](https://github.com/libgit2/libgit2/pull/4328)
 - [sha1_lookup: drop sha1_entry_pos function #4327](https://github.com/libgit2/libgit2/pull/4327)
 - [sha1_position: convert do-while to while #4326](https://github.com/libgit2/libgit2/pull/4326)
 - [patch_generate: represent buffers as void pointers #4304](https://github.com/libgit2/libgit2/pull/4304)
 - [Remove unused 'sys/remote.h' header #4323](https://github.com/libgit2/libgit2/pull/4323)
 - [tests: rebase::submodule: verify initialization method calls #4320](https://github.com/libgit2/libgit2/pull/4320)
 - [tests: rewrite rebase-submodule .gitmodule file #4275](https://github.com/libgit2/libgit2/pull/4275)
 - [tsort: remove idempotent conditional assignment #4314](https://github.com/libgit2/libgit2/pull/4314)
 - [Build with patched libcurl #4317](https://github.com/libgit2/libgit2/pull/4317)
 - [win32: provide fast-path for retrying filesystem operations #4311](https://github.com/libgit2/libgit2/pull/4311)
 - [Configuration file fixes with includes #4250](https://github.com/libgit2/libgit2/pull/4250)
 - [Patch ID calculation #4272](https://github.com/libgit2/libgit2/pull/4272)
 - [signature: don't leave a dangling pointer to the strings on parse failure #4306](https://github.com/libgit2/libgit2/pull/4306)
 - [git_reset_*: pass parameters as const pointers #4287](https://github.com/libgit2/libgit2/pull/4287)
 - [tests: config: fix missing declaration causing error #4291](https://github.com/libgit2/libgit2/pull/4291)
 - [Convert port with htons() in p_getaddrinfo() #4280](https://github.com/libgit2/libgit2/pull/4280)
 - [Out of tree builds #4235](https://github.com/libgit2/libgit2/pull/4235)
 - [cmake: Permit disabling external http-parser #4278](https://github.com/libgit2/libgit2/pull/4278)
 - [merge: fix potential free of uninitialized memory #4277](https://github.com/libgit2/libgit2/pull/4277)
 - [merge: perform exact rename detection in linear time #4202](https://github.com/libgit2/libgit2/pull/4202)
 - [travis: upgrade container to Ubuntu 14.04 #4211](https://github.com/libgit2/libgit2/pull/4211)
 - [Fix template dir empty string #4273](https://github.com/libgit2/libgit2/pull/4273)
 - [adding GIT_FILTER_VERSION to GIT_FILTER_INIT as part of convention #4267](https://github.com/libgit2/libgit2/pull/4267)
 - [travis: replace use of deprecated homebrew/dupes tap #4268](https://github.com/libgit2/libgit2/pull/4268)
 - [Test improvements #4269](https://github.com/libgit2/libgit2/pull/4269)
 - [Read prefix tests #4265](https://github.com/libgit2/libgit2/pull/4265)
 - [Allow creation of a configuration object in an in-memory repository #4263](https://github.com/libgit2/libgit2/pull/4263)
 - [travis: install openssl explicitly #4266](https://github.com/libgit2/libgit2/pull/4266)
 - [smart_protocol: fix parsing of server ACK responses #4261](https://github.com/libgit2/libgit2/pull/4261)
 - [odb_read_prefix: reset error in backends loop #4264](https://github.com/libgit2/libgit2/pull/4264)
 - [Update version number to 0.26 #4262](https://github.com/libgit2/libgit2/pull/4262)
 - [CHANGELOG: add various changes introduced since v0.25 #4254](https://github.com/libgit2/libgit2/pull/4254)
 - [Ensure packfiles with different contents have different names #4088](https://github.com/libgit2/libgit2/pull/4088)
 - [Update to forced checkout and untracked files #4260](https://github.com/libgit2/libgit2/pull/4260)
 - [settings: rename `GIT_OPT_ENABLE_SYNCHRONOUS_OBJECT_CREATION` #4259](https://github.com/libgit2/libgit2/pull/4259)
 - [Buffer growing cleanups #4255](https://github.com/libgit2/libgit2/pull/4255)
 - [Coverity fixes #4253](https://github.com/libgit2/libgit2/pull/4253)
 - [SHA1DC update #4258](https://github.com/libgit2/libgit2/pull/4258)
 - [Fix path computations for compressed index entries #4236](https://github.com/libgit2/libgit2/pull/4236)
 - [(Temporarily) disable UNC tests #4256](https://github.com/libgit2/libgit2/pull/4256)
 - [fix build with libressl #4251](https://github.com/libgit2/libgit2/pull/4251)
 - [Fix issue with directory glob ignore in subdirectories #4239](https://github.com/libgit2/libgit2/pull/4239)
 - [Submodule working directory #4243](https://github.com/libgit2/libgit2/pull/4243)
 - [Introduce home directory expansion function for config files, attribute files #4179](https://github.com/libgit2/libgit2/pull/4179)
 - [Fix proxy auto detect not utilizing callbacks #4097](https://github.com/libgit2/libgit2/pull/4097)
 - [git_repository_set_head: use tag name in reflog #4174](https://github.com/libgit2/libgit2/pull/4174)
 - [revparse: support open-ended ranges #4231](https://github.com/libgit2/libgit2/pull/4231)
 - [Fix GCC warnings #4240](https://github.com/libgit2/libgit2/pull/4240)
 - [Update README: VS -> VSTS #4238](https://github.com/libgit2/libgit2/pull/4238)
 - [tests: repo: fix repo discovery tests on overlayfs #4232](https://github.com/libgit2/libgit2/pull/4232)
 - [libssh2 shutdown #4229](https://github.com/libgit2/libgit2/pull/4229)
 - [WIP: squash some memleaks #4226](https://github.com/libgit2/libgit2/pull/4226)
 - [Verify object hashes #4197](https://github.com/libgit2/libgit2/pull/4197)
 - [transport: provide a getter for the proxy options #4206](https://github.com/libgit2/libgit2/pull/4206)
 - [Debian HTTPS feature test failure #4216](https://github.com/libgit2/libgit2/pull/4216)
 - [Do not free config when creating remote #4224](https://github.com/libgit2/libgit2/pull/4224)
 - [socket_stream: continue to next addrinfo on socket creation failure #4219](https://github.com/libgit2/libgit2/pull/4219)
 - [Honor read-only flag when writing to config backends #4217](https://github.com/libgit2/libgit2/pull/4217)
 - [diff_parse: free object instead of its pointer #4215](https://github.com/libgit2/libgit2/pull/4215)

#### Included non-merged libgit2 PRs:

 - [Parallelize checkout_create_the_new for ntfs perf gains #4205](https://github.com/libgit2/libgit2/pull/4205)
 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)


## <a name="v0-19-0" href="#v0-19-0">v0.19.0</a> [(2017-04-20)](https://github.com/nodegit/nodegit/releases/tag/v0.19.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.18.0...v0.19.0)

#### Summary of changes outside of libgit2 bump:
 - You can now convert tree entries in blobs [PR #1272](https://github.com/nodegit/nodegit/pull/1272)

#### Included merged libgit2 PRs:

 - [fileops: fix leaking fd in `mmap_ro_file` #4201](https://github.com/libgit2/libgit2/pull/4201)
 - [README: document our relation to changes in upstream #4198](https://github.com/libgit2/libgit2/pull/4198)
 - [filter: only close filter if it's been initialized correctly #4196](https://github.com/libgit2/libgit2/pull/4196)
 - [Fix building against OpenSSL v1.1 #4195](https://github.com/libgit2/libgit2/pull/4195)
 - [pkgconfig: fix handling of prefixes containing whitespaces #4193](https://github.com/libgit2/libgit2/pull/4193)
 - [Refactor some of the win32 POSIX emulation #4192](https://github.com/libgit2/libgit2/pull/4192)
 - [Correct non-existent file references in `odb.h` #4188](https://github.com/libgit2/libgit2/pull/4188)
 - [Coverity #4183](https://github.com/libgit2/libgit2/pull/4183)
 - [git_treebuilder_write_with_buffer refactorings #4182](https://github.com/libgit2/libgit2/pull/4182)
 - [Fix memory leaks #4180](https://github.com/libgit2/libgit2/pull/4180)
 - [sha1dc: perf improvements from upstream #4178](https://github.com/libgit2/libgit2/pull/4178)
 - [inet_pton: don't assume addr families don't exist #4176](https://github.com/libgit2/libgit2/pull/4176)
 - [git_futils: don't O_EXCL and O_TRUNC #4175](https://github.com/libgit2/libgit2/pull/4175)
 - [Fix the documentation for git_cred_acquire_cb #4172](https://github.com/libgit2/libgit2/pull/4172)
 - [Fix typo in remote.h API #4171](https://github.com/libgit2/libgit2/pull/4171)
 - [Fix resolving absolute symlinks (on at least Win32) #4169](https://github.com/libgit2/libgit2/pull/4169)
 - [Coverity fixes #4167](https://github.com/libgit2/libgit2/pull/4167)
 - [Remove `map_free` macros #4166](https://github.com/libgit2/libgit2/pull/4166)
 - [Worktree fixes #4163](https://github.com/libgit2/libgit2/pull/4163)
 - [merge_driver: fix const-correctness for source getters #4162](https://github.com/libgit2/libgit2/pull/4162)
 - [submodule: catch when submodule is not staged on update #4161](https://github.com/libgit2/libgit2/pull/4161)
 - [Diff fixes #4160](https://github.com/libgit2/libgit2/pull/4160)
 - [Skip uninteresting commits in revwalk timesort iterator #4157](https://github.com/libgit2/libgit2/pull/4157)
 - [git_futils_readbuffer: don't compute sha-1 #4156](https://github.com/libgit2/libgit2/pull/4156)
 - [Support namespaced references again #4154](https://github.com/libgit2/libgit2/pull/4154)
 - [rebase: ignore untracked files in submodules #4151](https://github.com/libgit2/libgit2/pull/4151)
 - [git_commit_create: freshen tree objects in commit #4150](https://github.com/libgit2/libgit2/pull/4150)
 - [cmake: only enable supported compiler warning flags #4148](https://github.com/libgit2/libgit2/pull/4148)
 - [winhttp: disambiguate error messages when sending requests #4146](https://github.com/libgit2/libgit2/pull/4146)
 - [tests: refs::create: fix memory leak #4145](https://github.com/libgit2/libgit2/pull/4145)
 - [Fix: make reflog include \"(merge)\" for merge commits #4143](https://github.com/libgit2/libgit2/pull/4143)
 - [Fix minor typos in CONVENTIONS.md #4142](https://github.com/libgit2/libgit2/pull/4142)
 - [Fix inet_pton tests triggering an assert in Haiku #4141](https://github.com/libgit2/libgit2/pull/4141)
 - [README: Mention how to run tests #4139](https://github.com/libgit2/libgit2/pull/4139)
 - [tests: Add create__symbolic_with_arbitrary_content #4138](https://github.com/libgit2/libgit2/pull/4138)
 - [appveyor: don't rewrite the system mingw #4137](https://github.com/libgit2/libgit2/pull/4137)
 - [Introduce (optional) SHA1 collision attack detection #4136](https://github.com/libgit2/libgit2/pull/4136)
 - [Provide error on gcc < 4.1 when THREADSAFE #4135](https://github.com/libgit2/libgit2/pull/4135)
 - [fix regression from #4092 #4133](https://github.com/libgit2/libgit2/pull/4133)
 - [Attrcache cleanups #4131](https://github.com/libgit2/libgit2/pull/4131)
 - [Improve clar messages #4130](https://github.com/libgit2/libgit2/pull/4130)
 - [Minor comment fix #4127](https://github.com/libgit2/libgit2/pull/4127)
 - [refdb: catch additional per-worktree refs #4124](https://github.com/libgit2/libgit2/pull/4124)
 - [Signature cleanups #4122](https://github.com/libgit2/libgit2/pull/4122)
 - [Changes to provide option to turn off/on ofs_delta #4115](https://github.com/libgit2/libgit2/pull/4115)
 - [khash cleanups #4092](https://github.com/libgit2/libgit2/pull/4092)
 - [fsync all the things #4030](https://github.com/libgit2/libgit2/pull/4030)
 - [Worktree implementation #3436](https://github.com/libgit2/libgit2/pull/3436)

#### Included non-merged libgit2 PRs:

 - [Parallelize checkout_create_the_new for ntfs perf gains #4205](https://github.com/libgit2/libgit2/pull/4205)
 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)
 - [Fix proxy auto detect not utilizing callbacks #4097](https://github.com/libgit2/libgit2/pull/4097)

## <a name="v0-18-0" href="#v0-18-0">v0.18.0</a> [(2017-02-28)](https://github.com/nodegit/nodegit/releases/tag/v0.18.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.17.0...v0.18.0)

### API Changes

 - All callbacks that go to libgit2 now have an optional `waitForResult` flag that can be `true`/`false`. Defaults to false. When true it will not stop libgit2 from continuing on before the JS code is fully executed and resolved (in cases of a Promise). This is useful for progress callbacks (like fetching) where the bubbling up of the progress to JS doesn't really need the C/C++ code to wait for the JS code to fully handle the event before continuing. This can have serious performance implications for many callbacks that can be fired quite frequently.
 - `given_opts` in `Revert.revert` are now optional
 - `checkout_opts` in `Reset.fromAnnotated` and `Reset.reset` are now optional
 - `Reset.fromAnnotated` is now async
 - `message` on `Stash.save` is now optional
 - Added `processMergeMessageCallback` on `Repository#mergeBranches` to allow for custom merge messages
 - Add `beforeFinishFn` to `Repository#rebaseBranches` and `Repository#continueRebase`. This is called before the invocation of `finish()`. If the callback returns a promise, `finish()` will be called when the promise resolves. The `beforeFinishFn` will be called with an object that has on it:
  - `ontoName` The name of the branch that we rebased onto
  - `ontoSha` The sha that we rebased onto
  - `originalHeadName` The name of the branch that we rebased
  - `originalHeadSha` The sha of the branch that was rebased
  - `rewitten` which is an array of sha pairs that contain which contain what the commit sha was before the rebase and what the commit sha is after the rebase

### Summary of Changes from bumping libgit2 to 43275f5

[PR #1123](https://github.com/nodegit/nodegit/pull/1223) bumped libgit2 which brought in many changes and bug fixes.

#### Included merged libgit2 PRs:

 - [Use a shared buffer in calls of git_treebuilder_write to avoid heap contention #3892](https://github.com/libgit2/libgit2/pull/3892)
 - [WinHTTP: set proper error messages when SSL fails #4050](https://github.com/libgit2/libgit2/pull/4050)
 - [Clang analyzer run #4051](https://github.com/libgit2/libgit2/pull/4051)
 - [Extend packfile in increments of page_size. #4053](https://github.com/libgit2/libgit2/pull/4053)
 - [Fix general example memory leaks #4078](https://github.com/libgit2/libgit2/pull/4078)
 - [WIP: some coverity & compiler warning fixes #4086](https://github.com/libgit2/libgit2/pull/4086)
 - [Fix a few recent warnings #4087](https://github.com/libgit2/libgit2/pull/4087)
 - [Fix uninitialized variable warning #4095](https://github.com/libgit2/libgit2/pull/4095)
 - [Update docs for git_oid_fromstrn and p #4096](https://github.com/libgit2/libgit2/pull/4096)
 - [Fix digest credentials for proxy in windows #4104](https://github.com/libgit2/libgit2/pull/4104)
 - [Vector reverse overflow #4105](https://github.com/libgit2/libgit2/pull/4105)
 - [Flag given_opts in git_revert as optional #4108](https://github.com/libgit2/libgit2/pull/4108)
 - [Flag checkout_opts in git_reset as optional #4109](https://github.com/libgit2/libgit2/pull/4109)
 - [dirname with DOS prefixes #4111](https://github.com/libgit2/libgit2/pull/4111)
 - [Add support for lowercase proxy environment variables #4112](https://github.com/libgit2/libgit2/pull/4112)
 - [Flag options in git_stash_apply and git_stash_pop as being optional #4117](https://github.com/libgit2/libgit2/pull/4117)
 - [rename detection: don't try to detect submodule renames #4119](https://github.com/libgit2/libgit2/pull/4119)
 - [tests: fix permissions on testrepo.git index file #4121](https://github.com/libgit2/libgit2/pull/4121)

#### Included non-merged libgit2 PRs:

 - [negotiate always fails via libcurl #4126](https://github.com/libgit2/libgit2/pull/4126)
 - [Fix proxy auto detect not utilizing callbacks #4097](https://github.com/libgit2/libgit2/pull/4097)

### Summary of Changes to NodeGit outside of libgit2 bump

 - Don't overwrite C++ files for things that haven't changed [PR #1091](https://github.com/nodegit/nodegit/pull/1091)
 - Add the option to "fire and forget" callbacks so libgit2 doesn't wait for JS to finish before proceeding [PR #1208](https://github.com/nodegit/nodegit/pull/1208)
 - Send back the error code from libgit2 when a call fails [PR #1209](https://github.com/nodegit/nodegit/pull/1209)
 - Initialize pointers to null [PR #1210](https://github.com/nodegit/nodegit/pull/1210)
 - Replace Gitter with Slack [PR #1212](https://github.com/nodegit/nodegit/pull/1212)
 - Make `given_opts` in `Revert.revert` optional [PR #1213](https://github.com/nodegit/nodegit/pull/1213)
 - Make `Reset.fromAnnotated` async and `checkout_opts` optional [PR #1214](https://github.com/nodegit/nodegit/pull/1214)
 - Make `message` on `Stash.save` optional [PR #1215](https://github.com/nodegit/nodegit/pull/1215)
 - Add `Remote.ls` to NodeGit [PR #1218](https://github.com/nodegit/nodegit/pull/1218)
 - Add `processMergeMessageCallback` to `Repository#mergeBranches` to allow for custom merge messages [PR #1219](https://github.com/nodegit/nodegit/pull/1219)
 - Bump libgit2 to 43275f5 [PR #1223](https://github.com/nodegit/nodegit/pull/1223) from srajko/bump-libgit
 - Provide rebase details on finish [PR #1224](https://github.com/nodegit/nodegit/pull/1224)
 - Use wider int to calculate throttle window [PR #1232](https://github.com/nodegit/nodegit/pull/1232)
 - Update comment to reflect the correct path for generated code output [PR #1236](https://github.com/nodegit/nodegit/pull/1236)
 - Remove nwjs example from the docs [PR #1238](https://github.com/nodegit/nodegit/pull/1238)
 - Remove `sudo` requirement from linux 32-bit builds [PR #1241](https://github.com/nodegit/nodegit/pull/1241)

## <a name="v0-17-0" href="#v0-17-0">v0.17.0</a> [(2017-01-06)](https://github.com/nodegit/nodegit/releases/tag/v0.17.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.16.0...v0.17.0)

### Targeted Platform Changes

In this release we had added support for Node v7 and latest Electron. We have removed support for Node v0.12 and v5.

We are also deprecating nw.js support since it is currently broken, no one in the current team uses it and we would not be able to currently support nw.js in an effective manner with a good user experience.

### Now building against shared libcurl lib

For proxy support we now use libcurl for HTTP/HTTPS transport which should have no noticeable change in NodeGit remote operations but if changes are noticed this is worth mentioning as a potential source.

### Memory leak fixes and stability increases

Many PR's were made to fix memory leaks as they were found so memory usage should go down in this version for long running scripts. Additionally, when performing operations with callbacks (transfer progress, credentials, etc...) there was a small chance for a segfault when NodeGit would schedule the callback to go into JavaScript. This is now fixed.

### Fixes to the build

Many users, especially on windows, were experiencing errors during the build. The build still isn't perfect but a lot of the bugs were fixed.

### Bump libgit2 to 0bf0526

The majority of changes to NodeGit v17 were in libgit2. The API breaking changes that are known are:

- `RevWalk` is returning different results [libgit2 PR #3921](https://github.com/libgit2/libgit2/pull/3921)
- Changes in error messages returned [libgit2 PR #4049](https://github.com/libgit2/libgit2/pull/4049)

Summary of changes that were brought in:

https://github.com/nodegit/nodegit/pull/1187#issuecomment-277760323

### Changes to NodeGit outside of libgit2 bump

- Define GIT_SSH_MEMORY_CREDENTIALS for libgit2 [PR #949](https://github.com/nodegit/nodegit/pull/949)
- Add "Path" to ssh variable names in tests for clarity [PR #1135](https://github.com/nodegit/nodegit/pull/1135)
- Fix revwalk memory leaks [PR #1137](https://github.com/nodegit/nodegit/pull/1137)
- Fix tree entry leaks [PR #1138](https://github.com/nodegit/nodegit/pull/1138)
- Fix typo in postinstall script [PR #1141](https://github.com/nodegit/nodegit/pull/1141)
- Fix windows exception handling in build [PR #1143](https://github.com/nodegit/nodegit/pull/1143)
- Fix CI failures on node 0.12 [PR #1144](https://github.com/nodegit/nodegit/pull/1144)
- Fix postinstall script crash when install location has spaces in its path [PR #1148](https://github.com/nodegit/nodegit/pull/1148)
- Update 0.13 changelog [PR #1151](https://github.com/nodegit/nodegit/pull/1151)
- Minor documentation fix in Checkout.index [PR #1164](https://github.com/nodegit/nodegit/pull/1164)
- FreeBSD also uses struct timespec st_mtim [PR #1165](https://github.com/nodegit/nodegit/pull/1165)
- README.md needs to show where to get Slack invitation [PR #1170](https://github.com/nodegit/nodegit/pull/1170)
- Add @async tag to `Tree#getEntry` [PR #1178](https://github.com/nodegit/nodegit/pull/1178)
- Fix incorrect anchor link in TESTING.md [PR #1179](https://github.com/nodegit/nodegit/pull/1179)
- Added tests for Tag [PR #1180](https://github.com/nodegit/nodegit/pull/1180)
- Added tests for Branch [PR #1181](https://github.com/nodegit/nodegit/pull/1181)
- Escape the spaces in dir for shell command [PR #1186](https://github.com/nodegit/nodegit/pull/1186)
- Bump libgit to 0bf0526 [PR #1187](https://github.com/nodegit/nodegit/pull/1187)
- Checkout's tree* functions do not support Oid as a parameter [PR #1190](https://github.com/nodegit/nodegit/pull/1190)
- Build against shared library for libcurl [PR #1195](https://github.com/nodegit/nodegit/pull/1195)
- Move libuv calls to correct thread [PR #1197](https://github.com/nodegit/nodegit/pull/1197)
- Update `Repository#createBranch` docs [PR #1198](https://github.com/nodegit/nodegit/pull/1198)
- Remove Node v0.12 and v5 [PR #1199](https://github.com/nodegit/nodegit/pull/1199)
- Specify acceptable types for `lookup` and `dwim` in Reference [PR #1203](https://github.com/nodegit/nodegit/pull/1203)
- Build for Node 7.4.0 [PR #1204](https://github.com/nodegit/nodegit/pull/1204)
- Write the index to a repository directly in merge-cleanly.js examples [PR #1205](https://github.com/nodegit/nodegit/pull/1205)

## <a name="v0-16-0" href="#v0-16-0">v0.16.0</a> [(2016-09-15)](https://github.com/nodegit/nodegit/releases/tag/v0.16.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.15.1...v0.16.0)

- Bump libssh2 to 1.7.0 [PR #1071](https://github.com/nodegit/nodegit/pull1071)
- Js cleanup [PR #1074](https://github.com/nodegit/nodegit/pull1074)
- Expose merge options and diff flags [PR #1076](https://github.com/nodegit/nodegit/pull1076)
- Fix Tree#entryByName function and add test [PR #1079](https://github.com/nodegit/nodegit/pull1079)
- Add isSubmodule() method [PR #1080](https://github.com/nodegit/nodegit/pull1080)
- Fix two typos in the documentation for Commit#parents [PR #1081](https://github.com/nodegit/nodegit/pull1081)
- Memory management: duplicate time [PR #1090](https://github.com/nodegit/nodegit/pull1090)
- Preempt nan to fix deprecated calls to Get/SetHiddenValue [PR #1106](https://github.com/nodegit/nodegit/pull1106)
- Try re-enabling node >6.2 [PR #1107](https://github.com/nodegit/nodegit/pull1107)
- Bump openssl to 1.0.2h (same as node 6.3.1) [PR #1108](https://github.com/nodegit/nodegit/pull1108)
- Don't run postbuild when we detect electron install [PR #1111](https://github.com/nodegit/nodegit/pull1111)
- Added instructions for CircleCI users [PR #1113](https://github.com/nodegit/nodegit/pull1113)
- Fix up electron and nw.js docs [PR #1114](https://github.com/nodegit/nodegit/pull1114)
- Patch libssh2 to work with vs2015 [PR #1125](https://github.com/nodegit/nodegit/pull1125)
- Fix CI [PR #1126](https://github.com/nodegit/nodegit/pull1126)


## <a name="v0-15-1" href="#v0-15-1">v0.15.1</a> [(2016-06-20)](https://github.com/nodegit/nodegit/releases/tag/v0.15.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.15.0...v0.15.1)

- Fix postinstall breaking build if it fails.

## <a name="v0-14-1" href="#v0-14-1">v0.14.1</a> [(2016-06-20)](https://github.com/nodegit/nodegit/releases/tag/v0.14.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.14.0...v0.14.1)

- Fix postinstall breaking build if it fails.

## <a name="v0-15-0" href="#v0-15-0">v0.15.0</a> [(2016-06-20)](https://github.com/nodegit/nodegit/releases/tag/v0.15.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.14.0...v0.15.0)

- Update to libgit2 @ 37dba1a [PR #1041](https://github.com/nodegit/nodegit/pull/1041)

This updates NodeGit to use the latest `HEAD` version of libgit2. The plan for staying on the official tagged releases of libgit2 is that they will get a maintenance branch and not-breaking API fixes will be backported to them. The first branch of this sort is `maint/0.14`. Going forward new releases of NodeGit will follow closely to the `master` branch of libgit2.

Summary of changes that were brought in:

https://github.com/libgit2/libgit2/commit/37dba1a739b5ee6c45dc9f3c0bd1f7f7a18f13f7
-------

### Changes or improvements

* `NodeGit.FetchOptions`, and `NodeGit.PushOptions` now have a `proxyOpts` field that accepts a `NodeGit.ProxyOptions` object that allows NodeGit to use a proxy for all remote communication

* `NodeGit.MergeOptions` has a `defaultDriver` field that lets the caller change the driver used to when both sides of a merge have changed

### API additions

* `Commit.createWithSignature` allows the caller to create a signed commit. There are no tests for this currently so it's labelled experimental.

* `Blob`, `Commit`, `Tag`, and `Tree` all have a new prototype `dup` method on them to make a low-level copy of the libgit2 object if needed.

* `Odb#expandIds` is exposed which takes in a list of short ids and expands them in-place to the full id of the object in the database

## <a name="v0-14-0" href="#v0-14-0">v0.14.0</a> [(2016-06-20)](https://github.com/nodegit/nodegit/releases/tag/v0.14.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.13.2...v0.14.0)

- Improve lifecycle scripts and install process [PR #1055](https://github.com/nodegit/nodegit/pull/1055)
- Fix example code [PR #1058](https://github.com/nodegit/nodegit/pull/1058)

## <a name="v0-13-2" href="#v0-13-2">v0.13.2</a> [(2016-06-09)](https://github.com/nodegit/nodegit/releases/tag/v0.13.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.13.1...v0.13.2)

- Stop `RevWalk#walk` from swallowing errors in the callback [PR #1047](https://github.com/nodegit/nodegit/pull/1047)
- Stop swallowing errors in the install script [PR #1048](https://github.com/nodegit/nodegit/pull/1048)
- Fix initializing submodules when installing from npm [PR #1050](https://github.com/nodegit/nodegit/pull/1050)

## <a name="v0-13-1" href="#v0-13-1">v0.13.1</a> [(2016-06-03)](https://github.com/nodegit/nodegit/releases/tag/v0.13.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.13.0...v0.13.1)

## Added

- `Repository#discardLines` is now a thing [PR #1021](https://github.com/nodegit/nodegit/pull/1021)

## Modified

- Async methods now use a custom threadpool to prevent thread-locking the event loop [PR #1019](https://github.com/nodegit/nodegit/pull/1019)

## Bug fixes

- Fix building NodeGit from NPM [PR #1026](https://github.com/nodegit/nodegit/pull/1026)
- Plug a memory leak in `RevWalk.fastWalk` [PR #1030](https://github.com/nodegit/nodegit/pull/1030)
- Plug a memory leak with `Oid` [PR #1033](https://github.com/nodegit/nodegit/pull/1033)
- Fixed some underlying libgit2 objects getting freed incorrectly [PR #1036](https://github.com/nodegit/nodegit/pull/1036)

## <a name="v0-13-0" href="#v0-13-0">v0.13.0</a> [(2016-05-04)](https://github.com/nodegit/nodegit/releases/tag/v0.13.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.12.2...v0.13.0)

## Summary

This is a big update! Lots of work was done to bring NodeGit up to the latest stable libgit2 version (v0.24.1), to use babel in the library, to make it more stable, remove memory leaks, squash bugs and in general just improve the library for all. Make sure to see all of the API changes below (there are a lot).

## Node support

We have added Node 6 as a supported platform! Going forward we aim to have 1:1 support for versions of Node that are either current or LTS. That means that v0.12 will not be supported soon so if you're on that please upgrade to at least Node v4. Also Node v5 will *NOT* be LTS so when Node stops supporting that in the coming months we will as well. You can read more about the current Node upgrade plan [here](https://nodejs.org/en/blog/release/v6.0.0/).

## API Changes
-------

### Modified

- `Index#add`, `Index#addByPath`, `Index#clear`, `Index#conflictAdd`, `Index#conflictCleanup`, `Index#conflictGet`, `Index#conflictRemove`, `Index.open`, `Index#read`, `Index#readTree`, `Index#remove`, `Index#removeByPath`, `Index#removeDirectory`, `Index#read`, `Index#write`, `Index#writeTree`, and `Index#writeTreeTo` are all now asynchronous functions [PR #971](https://github.com/nodegit/nodegit/pull/971)
- Made `ancestoryEntry`, `outEntry` and `theirEntry` optional parameters on `Index#conflictAdd` [PR #997](https://github.com/nodegit/nodegit/pull/997)
- `Repository#refreshIndex` will return an Index object back that has the latest data loaded off of disk [PR #986](https://github.com/nodegit/nodegit/pull/986)
- `Commit.create` is now asynchronous [PR #1022](https://github.com/nodegit/nodegit/pull/1022)
- `Remote.create` is now asynchronous [PR #990](https://github.com/nodegit/nodegit/pull/990)

### Added

- `Diff#merge` will combine a diff into itself [PR #1000](https://github.com/nodegit/nodegit/pull/1000)
- `ReflogEntry#committer`, `ReflogEntry#idNew`, `ReflogEntry#idOld`, and `ReflogEntry#message` have been added
[PR #1013](https://github.com/nodegit/nodegit/pull/1013)

### Removed

- `Repository#openIndex` [PR #989](https://github.com/nodegit/nodegit/pull/989), use `Repository#index` or `Repository#refreshIndex` instead
- `Reflog#entryCommitter`, `Reflog#entryIdNew`, `Reflog#entryIdOld`, and `Reflog#entryMessage` have been moved to be under `ReflogEntry`
[PR #1013](https://github.com/nodegit/nodegit/pull/1013)

### Bug fixes

- `Branch.name` works now [PR #991](https://github.com/nodegit/nodegit/pull/991)
- Fixed a crash with callbacks from libgit2 [PR #944](https://github.com/nodegit/nodegit/pull/944)
- Fixed a crash in `Tree#entryByName` [PR #998](https://github.com/nodegit/nodegit/pull/998)
- More memory leaks have been plugged [PR #1005](https://github.com/nodegit/nodegit/pull/1005), [PR #1006](https://github.com/nodegit/nodegit/pull/1006), [PR #1014](https://github.com/nodegit/nodegit/pull/1014), and [PR #1015](https://github.com/nodegit/nodegit/pull/1015)
- `Commit#getDiffWithOptions` now actually passes the options correctly [PR #1008](https://github.com/nodegit/nodegit/pull/1008)

## Upgraded to libgit2 v0.24.1 [PR #1010](https://github.com/nodegit/nodegit/pull/1010)
-------

### Changes or improvements

- Custom merge drivers can now be registered, which allows callers to
  configure callbacks to honor `merge=driver` configuration in
  `.gitattributes`.

- Custom filters can now be registered with wildcard attributes, for
  example `filter=*`.  Consumers should examine the attributes parameter
  of the `check` function for details.

- Symlinks are now followed when locking a file, which can be
  necessary when multiple worktrees share a base repository.

- You can now set your own user-agent to be sent for HTTP requests by
  using the `Libgit2.OPT.SET_USER_AGENT` with `Libgit2.opts()`.

- You can set custom HTTP header fields to be sent along with requests
  by passing them in the fetch and push options.

- Tree objects are now assumed to be sorted. If a tree is not
  correctly formed, it will give bad results. This is the git approach
  and cuts a significant amount of time when reading the trees.

- Filter registration is now protected against concurrent
  registration.

- Filenames which are not valid on Windows in an index no longer cause
  to fail to parse it on that OS.

- Rebases can now be performed purely in-memory, without touching the
  repository's workdir.

- When adding objects to the index, or when creating new tree or commit
  objects, the inputs are validated to ensure that the dependent objects
  exist and are of the correct type.  This object validation can be
  disabled with the `Libgit2.OPT.ENABLE_STRICT_OBJECT_CREATION` option.

- The WinHTTP transport's handling of bad credentials now behaves like
  the others, asking for credentials again.

### API additions

- `Blob.createFromStream()` and
  `Blob.createFromStreamCommit` allow you to create a blob by
  writing into a stream. Useful when you do not know the final size or
  want to copy the contents from another stream.

- `Config#lock` has been added, which allow for
  transactional/atomic complex updates to the configuration, removing
  the opportunity for concurrent operations and not committing any
  changes until the unlock.

- `DiffOptions` added a new callback `progress_cb` to report on the
  progress of the diff as files are being compared. The documentation of
  the existing callback `notify_cb` was updated to reflect that it only
  gets called when new deltas are added to the diff.

- `FetchOptions` and `PushOptions` have gained a `customHeaders`
  field to set the extra HTTP header fields to send.

- `Commit#headerField` allows you to look up a specific header
  field in a commit.

### Breaking API changes

- `MergeOptions` now provides a `defaultDriver` that can be used
  to provide the name of a merge driver to be used to handle files changed
  during a merge.

- The `Merge.TREE_FLAG` is now `Merge.FLAG`.  Subsequently,
  `treeFlags` field of the `MergeOptions` structure is now named `flags`.

- The `Merge.FILE_FLAGS` enum is now `Merge.FILE_FLAG` for
  consistency with other enum type names.

- `Cert` descendent types now have a proper `parent` member

- It is the responsibility of the refdb backend to decide what to do
  with the reflog on ref deletion. The file-based backend must delete
  it, a database-backed one may wish to archive it.

- `Index#add` and `Index#conflictAdd` will now use the case
  as provided by the caller on case insensitive systems.  Previous
  versions would keep the case as it existed in the index.  This does
  not affect the higher-level `Index#addByPath` or
  `Index#addFromBuffer` functions.

- The `Config.LEVEL` enum has gained a higher-priority value
  `PROGRAMDATA` which represent a rough Windows equivalent
  to the system level configuration.

- `RebaseOptions` now has a `mergeOptions` field.

- The index no longer performs locking itself. This is not something
  users of the library should have been relying on as it's not part of
  the concurrency guarantees.

- `Remote#connect()` now takes a `customHeaders` argument to set
  the extra HTTP header fields to send.

- `Tree.entryFilemode`, `Tree.entryFilemodeRaw`, `Tree.entryId`, `Tree.entryName`,
  `Tree.entryToObject`, and `Tree.entryType` have all been moved to the `TreeEntry` prototype.
  Additionally, the `TreeEntry` fields have been removed in lieu of the corresponding functions to return
  the data.

## <a name="v0-12-2" href="#v0-12-2">v0.12.2</a> [(2016-04-07)](https://github.com/nodegit/nodegit/releases/tag/v0.12.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.12.1...v0.12.2)

## Added

- We now provide 32-bit binaries for linux [PR #980](https://github.com/nodegit/nodegit/pull/980)

## Bug fixes

- Added memory clean up for references [PR #977](https://github.com/nodegit/nodegit/pull/977) and remotes [PR #981](https://github.com/nodegit/nodegit/pull/981)

## <a name="v0-12-1" href="#v0-12-1">v0.12.1</a> [(2016-03-30)](https://github.com/nodegit/nodegit/releases/tag/v0.12.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.12.0...v0.12.1)

## Bug fixes

- Fixed post install script dying on windows [PR #978](https://github.com/nodegit/nodegit/pull/978)

## <a name="v0-12-0" href="#v0-12-0">v0.12.0</a> [(2016-03-28)](https://github.com/nodegit/nodegit/releases/tag/v0.12.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.9...v0.12.0)

## API changes
- `Ignore`
  - Made `Ignore.pathIsIgnored` async [PR #970](https://github.com/nodegit/nodegit/pull/970)

## Bug fixes

- Added an error message when trying to install NodeGit without a required version of libstdc++ [PR #972](https://github.com/nodegit/nodegit/pull/972)
- Fix a crash when grabbing content out of a buffer that has unicode [PR #966](https://github.com/nodegit/nodegit/pull/966)
- Added some plumbing for better memory management [PR #958](https://github.com/nodegit/nodegit/pull/958)
- Fix `checkoutOptions` in `Stash#apply` [PR #956](https://github.com/nodegit/nodegit/pull/956)
- Fixed install when there is a space in the username on windows [PR #951](https://github.com/nodegit/nodegit/pull/951)
- Bump to nan@2.2.0 [PR #952](https://github.com/nodegit/nodegit/pull/952)

## <a name="v0-11-9" href="#v0-11-9">v0.11.9</a> [(2016-03-09)](https://github.com/nodegit/nodegit/releases/tag/v0.11.9)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.8...v0.11.9)

- Fixed crash when calculating diff via `ConvenientPatch` [PR #945](https://github.com/nodegit/nodegit/pull/945)

## <a name="v0-11-8" href="#v0-11-8">v0.11.8</a> [(2016-03-07)](https://github.com/nodegit/nodegit/releases/tag/v0.11.8)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.7...v0.11.8)

- Removed callback throttling due to segmentation faults. Will be implemented later. [PR #943](https://github.com/nodegit/nodegit/pull/943)

## <a name="v0-11-7" href="#v0-11-7">v0.11.7</a> [(2016-03-07)](https://github.com/nodegit/nodegit/releases/tag/v0.11.7)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.6...v0.11.7)

- Added `Repository#mergeheadForeach` [PR #937](https://github.com/nodegit/nodegit/pull/937)
- Improved speed of all callbacks dramatically [PR #932](https://github.com/nodegit/nodegit/pull/932)
- Fixed `Merge.merge` docs to show it takes in an `AnnotatedCommit` and not a `Commit` [PR #935](https://github.com/nodegit/nodegit/pull/935)
- Fixed unicode in `Diff.blobToBuffer` getting corrupted [PR #935](https://github.com/nodegit/nodegit/pull/935)
- Fixed fetching/pulling to bitbucket in versions > v5.6 of node [PR #942](https://github.com/nodegit/nodegit/pull/942)

## <a name="v0-11-6" href="#v0-11-6">v0.11.6</a> [(2016-03-01)](https://github.com/nodegit/nodegit/releases/tag/v0.11.6)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.5...v0.11.6)

- Added `Repository#checkoutRef` [PR #891](https://github.com/nodegit/nodegit/pull/891)
- `Repository#createCommitOnHead` no longer dies if the repo is empty [PR #927](https://github.com/nodegit/nodegit/pull/927)
- Fixed memory leak in `Patch#convenientFromDiff` [PR #930](https://github.com/nodegit/nodegit/pull/930)
- Generated files now have a header comment indicating that they are generated [PR #924](https://github.com/nodegit/nodegit/pull/924)
- Fixed http parsing errors in Node 5.6 [PR #931](https://github.com/nodegit/nodegit/pull/931)
- Fixed `Tree#walk` not returning the correct entries on `end` [PR #929](https://github.com/nodegit/nodegit/pull/929)

## <a name="v0-11-5" href="#v0-11-5">v0.11.5</a> [(2016-02-25)](https://github.com/nodegit/nodegit/releases/tag/v0.11.5)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.4...v0.11.5)

- Fixed crash when calculating a diff [PR #922](https://github.com/nodegit/nodegit/pull/922)
- Fixed an issue with return values getting randomly corrupted [PR #923](https://github.com/nodegit/nodegit/pull/923))

## <a name="v0-11-4" href="#v0-11-4">v0.11.4</a> [(2016-02-24)](https://github.com/nodegit/nodegit/releases/tag/v0.11.4)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.3...v0.11.4)

- Fixed callback out values in callbacks from C++. This affects any NodeGit call that is passed a callback as an option [PR #921](https://github.com/nodegit/nodegit/pull/921)
- Fixed an issue with building the debug version of NodeGit on windows [PR #918](https://github.com/nodegit/nodegit/pull/918)

## <a name="v0-11-3" href="#v0-11-3">v0.11.3</a> [(2016-02-22)](https://github.com/nodegit/nodegit/releases/tag/v0.11.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.2...v0.11.3)

- Fixed an issue where initializing NodeGit would sometimes seg fault. Also fixed an error when fetching concurrently [PR #912](https://github.com/nodegit/nodegit/pull/912)

## <a name="v0-11-2" href="#v0-11-2">v0.11.2</a> [(2016-02-18)](https://github.com/nodegit/nodegit/releases/tag/v0.11.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.1...v0.11.2)

- Fixed an issue where when staging lines if the index is locked NodeGit just nuked it [PR #906](https://github.com/nodegit/nodegit/pull/906)
- Fixed diff calculation when staging lines/hunks [PR #906](https://github.com/nodegit/nodegit/pull/906)
- Fixed seg-fault in linux that happens when getting the diff of very small files [PR #908](https://github.com/nodegit/nodegit/pull/908)
- Fixed `RevWalk#fastWalk` dying when an error happens in libgit2 [PR #909](https://github.com/nodegit/nodegit/pull/909)

## <a name="v0-11-1" href="#v0-11-1">v0.11.1</a> [(2016-02-09)](https://github.com/nodegit/nodegit/releases/tag/v0.11.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.11.0...v0.11.1)

- Numerous fixes and perf boosts to file history [PR #900](https://github.com/nodegit/nodegit/pull/900)[PR #896](https://github.com/nodegit/nodegit/pull/896)
- Several doc fixes [PR #899](https://github.com/nodegit/nodegit/pull/899)[PR #897](https://github.com/nodegit/nodegit/pull/897)

## <a name="v0-11-0" href="#v0-11-0">v0.11.0</a> [(2016-02-04)](https://github.com/nodegit/nodegit/releases/tag/v0.11.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.10.0...v0.11.0)

- Change `Revert.commit` and `Revert.revert` to by async. [PR #887](https://github.com/nodegit/nodegit/pull/887)
- Added `RevWalk#fileHistoryWalk` for a faster way to retrieve history for a specific file. [PR #889](https://github.com/nodegit/nodegit/pull/889)

## <a name="v0-10-0" href="#v0-10-0">v0.10.0</a> [(2016-02-01)](https://github.com/nodegit/nodegit/releases/tag/v0.10.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.9.0...v0.10.0)

- Clean mutexes are part of GC. No longer leaves processes running after the script ends [PR #880](https://github.com/nodegit/nodegit/pull/880)
- Increased the performance of `ConvenientPatch` by an order of magnitude [PR #883](https://github.com/nodegit/nodegit/pull/883)

# API changes
- `ConvenientPatch`
  - `ConvenientPatch` does not have a `patch` or a `delta` property associated with it, if you were using the `delta`, please just use prototype methods `oldFile`, `newFile`, and `Status`, which are stripped directly from the `delta`.
  - `ConvenientPatch#hunks` returns a promise with an array of `ConvenientHunks`.
- `ConvenientHunk`
  - `ConvenientHunk` does not have an exposed diffHunk associated with it, but does have the same members as diffHunk:
    - `size()` : number of lines in the hunk
    - `oldStart()` : old starting position
    - `oldLines()` : number of lines in old file
    - `newStart()` : new starting position
    - `newLines()` : number of lines in new file
    - `headerLen()` : length of header
    - `header()` : returns the header of the hunk
    - `lines()` : returns a promise containing `DiffLines`, not `ConvenientLines`.
- `DiffLine`
- `DiffLine` now contains the members `rawContent()` and `content()`.
  - `rawContent()` contains the unformatted content of the line. This is no longer a string from the line to the end of the file.
  - `content()` contains the utf8 formatted content of the line.

## <a name="v0-9-0" href="#v0-9-0">v0.9.0</a> [(2016-01-21)](https://github.com/nodegit/nodegit/releases/tag/v0.9.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.8.0...v0.9.0)

- Thread safe fix to stop crashing on releasing mutexes [PR #876](https://github.com/nodegit/nodegit/pull/876)
- `Submodule#setIgnore`, `Submodule#setUpdate`, and `Submodule#setUrl` are now all async. `Submodule#status` and `Submodule#location` are now available [PR #867](https://github.com/nodegit/nodegit/pull/867) and [PR #870](https://github.com/nodegit/nodegit/pull/870)
- `Remote#defaultBranch` is now available [PR #872](https://github.com/nodegit/nodegit/pull/872)
- `Repository#mergeBranches` now takes in a `MergeOptions` parameter [PR #873](https://github.com/nodegit/nodegit/pull/873)
- Remove a NodeGit specific hack to make `Index#addAll` faster since that is fixed in libgit2 [PR #875](https://github.com/nodegit/nodegit/pull/875))

## <a name="v0-8-0" href="#v0-8-0">v0.8.0</a> [(2016-01-15)](https://github.com/nodegit/nodegit/releases/tag/v0.8.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.7.0...0.8.0)

- Thread safe locking has been added and currently is defaulted to off. Use `NodeGit.enableThreadSafety()` to turn on
- NodeGit no longer requires a specific Promise object from the `nodegit-promise` library to be passed in. You can now use whatever you want!
- `Repository#stageFilemode` now can accept an array of strings for files to update
- `Submodule#addToIndex`, `Submodule#addFinalize`, `Submodule#init`, `Submodule#open`, `Submodule#sync`, and `Submodule#update` are now all async methodss

## <a name="v0-7-0" href="#v0-7-0">v0.7.0</a> [(2016-01-08)](https://github.com/nodegit/nodegit/releases/tag/v0.7.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.6.3...0.7.0)

- Bumped openssl to 1.0.2e to fix issues with prebuilts on linux platforms
- Fixed a bug with GIT_ITER_OVER breaking rebase and other iterative methods
- Make GraphDescendentOf asynchronous
- Fixed line length of utf8 stringss

## <a name="v0-6-3" href="#v0-6-3">v0.6.3</a> [(2015-12-16)](https://github.com/nodegit/nodegit/releases/tag/v0.6.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.6.2...0.6.3)

 - Fixed a bug where manually building for vanilla node would fail without explicitly
   setting the target

## <a name="v0-6-2" href="#v0-6-2">v0.6.2</a> [(2015-12-16)](https://github.com/nodegit/nodegit/releases/tag/v0.6.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.6.1...0.6.2)

 - Fixed a bug where manually building on windows would fail (if unable to download a prebuilt binary)

## <a name="v0-6-1" href="#v0-6-1">v0.6.1</a> [(2015-12-14)](https://github.com/nodegit/nodegit/releases/tag/v0.6.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.6.0...0.6.1)

 - Fixed Treebuilder.create to have an optional source
 - Added Repository.getSubmoduleNames
 - Added Submodule.Foreach

## <a name="v0-6-0" href="#v0-6-0">v0.6.0</a> [(2015-12-08)](https://github.com/nodegit/nodegit/releases/tag/v0.6.0)

 - Added file mode staging
 - Added a fast rev walk to do the rev walk in C++ and bubble the result up to JS
 - Updated to latest libgit2
 - Updated to latest openssl
 - Updated to latest nodegit-promise
 - Removed c++11 dependency
 - Fixed weirdness in lifecycle scripts
 - Added downloading prebuilt binaries for electron

## <a name="v0-4-1" href="#v0-4-1">v0.4.1</a> [(2015-06-02)](https://github.com/nodegit/nodegit/tree/0.4.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.4.0...0.4.1)

**Closed issues:**

- Error: Module did not self-register [\#593](https://github.com/nodegit/nodegit/issues/593)

- A guide on how to create a new branch, switch to it and delete it. [\#588](https://github.com/nodegit/nodegit/issues/588)

- A way to get "gone" branches [\#583](https://github.com/nodegit/nodegit/issues/583)

- Missing documentation pages for BranchIterator and NodeIterator [\#581](https://github.com/nodegit/nodegit/issues/581)

- ELIFECYCLE error on `npm rebuild` [\#578](https://github.com/nodegit/nodegit/issues/578)

- npm rebuild fails \(lifecycleScripts/clean should not delete lifecycleScripts!\) [\#576](https://github.com/nodegit/nodegit/issues/576)

- Unable to compile and install v0.4.0 on Windows [\#575](https://github.com/nodegit/nodegit/issues/575)

- Doesn't work with Electron [\#574](https://github.com/nodegit/nodegit/issues/574)

- Doesn't work with io.js 2.x [\#573](https://github.com/nodegit/nodegit/issues/573)

- Getting an exception during a fetchAll in defaultSignature in repository.js [\#572](https://github.com/nodegit/nodegit/issues/572)

- tree\_entry path function not working when calling getEntry with a path including subdir [\#570](https://github.com/nodegit/nodegit/issues/570)

- Build is broken on windows [\#565](https://github.com/nodegit/nodegit/issues/565)

- Cloning git sub modules using "nodegit" npm module [\#560](https://github.com/nodegit/nodegit/issues/560)

- How to get remote latest commit? [\#559](https://github.com/nodegit/nodegit/issues/559)

- npm install fails for nw.js [\#558](https://github.com/nodegit/nodegit/issues/558)

- nodegit and nw.js [\#557](https://github.com/nodegit/nodegit/issues/557)

**Merged pull requests:**

- Cherrypick tests [\#595](https://github.com/nodegit/nodegit/pull/595) ([jdgarcia](https://github.com/jdgarcia))

- Fix for issue \#591.  TreeEntry.path\(\) throws when TreeEntry came from Tree.entries\(\) [\#592](https://github.com/nodegit/nodegit/pull/592) ([tomruggs](https://github.com/tomruggs))

- Standard merge [\#589](https://github.com/nodegit/nodegit/pull/589) ([jdgarcia](https://github.com/jdgarcia))

- Add `git\_index\_conflict\_get` and test [\#586](https://github.com/nodegit/nodegit/pull/586) ([johnhaley81](https://github.com/johnhaley81))

- Bump nan [\#584](https://github.com/nodegit/nodegit/pull/584) ([johnhaley81](https://github.com/johnhaley81))

- Fix CI false positives [\#582](https://github.com/nodegit/nodegit/pull/582) ([johnhaley81](https://github.com/johnhaley81))

- Added NodeGit.Checkout.index [\#579](https://github.com/nodegit/nodegit/pull/579) ([jdgarcia](https://github.com/jdgarcia))

- Check for existence to avoid throwing an error when there is no default signature [\#577](https://github.com/nodegit/nodegit/pull/577) ([tomruggs](https://github.com/tomruggs))

- Fix path function in tree\_entry \(fix for issue \#570\) [\#571](https://github.com/nodegit/nodegit/pull/571) ([jfremy](https://github.com/jfremy))

- Fix Rebase issues [\#568](https://github.com/nodegit/nodegit/pull/568) ([jdgarcia](https://github.com/jdgarcia))

- Fix build issues with 0.4.0 [\#566](https://github.com/nodegit/nodegit/pull/566) ([maxkorp](https://github.com/maxkorp))

- stop cleaning on post-install [\#562](https://github.com/nodegit/nodegit/pull/562) ([maxkorp](https://github.com/maxkorp))

## <a name="v0-4-0" href="#v0-4-0">v0.4.0</a> [(2015-05-07)](https://github.com/nodegit/nodegit/tree/v0.4.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.3.3...v0.4.0)

**Closed issues:**

- Error installing nodegit as dependency of an atom-shell app [\#556](https://github.com/nodegit/nodegit/issues/556)

- New version of nan is breaking compile [\#554](https://github.com/nodegit/nodegit/issues/554)

- Install error from openssl [\#551](https://github.com/nodegit/nodegit/issues/551)

- How to get Tag instance by tag\_name? [\#543](https://github.com/nodegit/nodegit/issues/543)

- ELIFECYCLE Error on install [\#540](https://github.com/nodegit/nodegit/issues/540)

- Remote.delete returns -3 [\#539](https://github.com/nodegit/nodegit/issues/539)

- Repository.init should accept boolean value for is\_bare [\#538](https://github.com/nodegit/nodegit/issues/538)

- getStatus hangs [\#537](https://github.com/nodegit/nodegit/issues/537)

- Unable to compile or install with npm install nodegit [\#536](https://github.com/nodegit/nodegit/issues/536)

- `options` not reusable, nodegit destroys it [\#533](https://github.com/nodegit/nodegit/issues/533)

- 'Error: 'directory' exists and is not an empty directory' \(but it doesn't exist\) [\#530](https://github.com/nodegit/nodegit/issues/530)

- hey !:-\) problem with Branch.iteratorNew  \(support\) [\#528](https://github.com/nodegit/nodegit/issues/528)

- hey !:-\) problem with Branch.iteratorNew [\#527](https://github.com/nodegit/nodegit/issues/527)

- hey !:-\) problem with Branch.iteratorNew [\#526](https://github.com/nodegit/nodegit/issues/526)

- hey !:-\) problem with Branch.iteratorNew [\#525](https://github.com/nodegit/nodegit/issues/525)

- Error: Reference 'refs/remotes/user/foo/HEAD' not found [\#523](https://github.com/nodegit/nodegit/issues/523)

- Path issues windows [\#522](https://github.com/nodegit/nodegit/issues/522)

- Issues on scientific linux 6.6 [\#521](https://github.com/nodegit/nodegit/issues/521)

- It's looking for node-typ under `/Users/johnh/.node-gyp` [\#518](https://github.com/nodegit/nodegit/issues/518)

- Not working with iojs [\#516](https://github.com/nodegit/nodegit/issues/516)

- Cred.sshKeyNew not working: Too many redirects or authentication replays [\#511](https://github.com/nodegit/nodegit/issues/511)

- Open a Repo from a subfolder [\#509](https://github.com/nodegit/nodegit/issues/509)

- Create git-like CLI [\#508](https://github.com/nodegit/nodegit/issues/508)

- Cannot create an instance of Packbuilder [\#507](https://github.com/nodegit/nodegit/issues/507)

- Cannot find module '../build/Debug/nodegit' [\#506](https://github.com/nodegit/nodegit/issues/506)

- Bug with oid implicit cast inside C++ [\#501](https://github.com/nodegit/nodegit/issues/501)

- Failed to `require` on Ubuntu 12.04LTS [\#493](https://github.com/nodegit/nodegit/issues/493)

- Enable `git\_config`  [\#449](https://github.com/nodegit/nodegit/issues/449)

- Pull example doesn't fully update the index [\#389](https://github.com/nodegit/nodegit/issues/389)

**Merged pull requests:**

- There is an incompatibility with NaN 1.8.x, keeping 1.7.x for now. [\#552](https://github.com/nodegit/nodegit/pull/552) ([wiggzz](https://github.com/wiggzz))

- A wrapper for git\_diff\_blob\_to\_buffer [\#550](https://github.com/nodegit/nodegit/pull/550) ([bleathem](https://github.com/bleathem))

- Update to 0.4.0 [\#548](https://github.com/nodegit/nodegit/pull/548) ([tbranyen](https://github.com/tbranyen))

- Removed the superflous "line" argument [\#547](https://github.com/nodegit/nodegit/pull/547) ([bleathem](https://github.com/bleathem))

- This fixes polling sync promises in callbacks. [\#546](https://github.com/nodegit/nodegit/pull/546) ([johnhaley81](https://github.com/johnhaley81))

- Add get/set config string methods and tests [\#545](https://github.com/nodegit/nodegit/pull/545) ([johnhaley81](https://github.com/johnhaley81))

- Make `Remote.delete` async and return error messages correctly [\#544](https://github.com/nodegit/nodegit/pull/544) ([johnhaley81](https://github.com/johnhaley81))

- Bump "nodegit-promise" version [\#542](https://github.com/nodegit/nodegit/pull/542) ([johnhaley81](https://github.com/johnhaley81))

- Introduced a new ConvenientLine class to wrap the lines returned from ConvenientHunk. [\#541](https://github.com/nodegit/nodegit/pull/541) ([bleathem](https://github.com/bleathem))

- Fix some things missed by the generating scripts [\#535](https://github.com/nodegit/nodegit/pull/535) ([johnhaley81](https://github.com/johnhaley81))

- Attempt remove the delete keyword [\#534](https://github.com/nodegit/nodegit/pull/534) ([tbranyen](https://github.com/tbranyen))

- Fix freeing a `GitOid` that was passed as a string [\#531](https://github.com/nodegit/nodegit/pull/531) ([johnhaley81](https://github.com/johnhaley81))

- fix typo: "byes" [\#529](https://github.com/nodegit/nodegit/pull/529) ([rutsky](https://github.com/rutsky))

- Add convenience methods to status file [\#524](https://github.com/nodegit/nodegit/pull/524) ([maxkorp](https://github.com/maxkorp))

- Lots of complaints of missing build/Debug/nodegit [\#520](https://github.com/nodegit/nodegit/pull/520) ([tbranyen](https://github.com/tbranyen))

- Add `Graph.aheadBehind` and tests [\#517](https://github.com/nodegit/nodegit/pull/517) ([johnhaley81](https://github.com/johnhaley81))

- Update to use libgit2 v0.22.2 [\#515](https://github.com/nodegit/nodegit/pull/515) ([johnhaley81](https://github.com/johnhaley81))

- Add `Repository.prototype.fetchheadForeach` and tests [\#514](https://github.com/nodegit/nodegit/pull/514) ([johnhaley81](https://github.com/johnhaley81))

- Converted create methods to be synchronous [\#513](https://github.com/nodegit/nodegit/pull/513) ([tbranyen](https://github.com/tbranyen))

- Fix atom-shell build on windows [\#512](https://github.com/nodegit/nodegit/pull/512) ([johnhaley81](https://github.com/johnhaley81))

- Update Checkout and Merge [\#505](https://github.com/nodegit/nodegit/pull/505) ([orderedlist](https://github.com/orderedlist))

- Add note tests [\#504](https://github.com/nodegit/nodegit/pull/504) ([tbranyen](https://github.com/tbranyen))

- Revert "Guide navigation is currently confusing" [\#503](https://github.com/nodegit/nodegit/pull/503) ([thgaskell](https://github.com/thgaskell))

- Improve coverage [\#502](https://github.com/nodegit/nodegit/pull/502) ([tbranyen](https://github.com/tbranyen))

- Adds in CPP code coverage and joined JS [\#499](https://github.com/nodegit/nodegit/pull/499) ([tbranyen](https://github.com/tbranyen))

- Add twitter username to README.md [\#498](https://github.com/nodegit/nodegit/pull/498) ([johnhaley81](https://github.com/johnhaley81))

- Fix symbolic reference handling in getReferences [\#496](https://github.com/nodegit/nodegit/pull/496) ([billt2006](https://github.com/billt2006))

- Enable `git\_stash\_foreach` [\#495](https://github.com/nodegit/nodegit/pull/495) ([johnhaley81](https://github.com/johnhaley81))

- Guide navigation is currently confusing [\#494](https://github.com/nodegit/nodegit/pull/494) ([tbranyen](https://github.com/tbranyen))

- Fix gitter badge for npm [\#492](https://github.com/nodegit/nodegit/pull/492) ([billt2006](https://github.com/billt2006))

- Add automatically generated change log file. [\#465](https://github.com/nodegit/nodegit/pull/465) ([skywinder](https://github.com/skywinder))

## <a name="v0-3-3" href="#v0-3-3">v0.3.3</a> [(2015-03-16)](https://github.com/nodegit/nodegit/tree/v0.3.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.3.2...v0.3.3)

**Merged pull requests:**

- Download all dev dependencies before build [\#491](https://github.com/nodegit/nodegit/pull/491) ([johnhaley81](https://github.com/johnhaley81))

## <a name="v0-3-2" href="#v0-3-2">v0.3.2</a> [(2015-03-16)](https://github.com/nodegit/nodegit/tree/v0.3.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.3.1...v0.3.2)

**Closed issues:**

- Amazon S3 CDN link is broken [\#482](https://github.com/nodegit/nodegit/issues/482)

**Merged pull requests:**

- Confirm builder exists before building [\#490](https://github.com/nodegit/nodegit/pull/490) ([johnhaley81](https://github.com/johnhaley81))

## <a name="v0-3-1" href="#v0-3-1">v0.3.1</a> [(2015-03-14)](https://github.com/nodegit/nodegit/tree/v0.3.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.3.0...v0.3.1)

**Merged pull requests:**

- Revert node-pre-gyp to install not build [\#486](https://github.com/nodegit/nodegit/pull/486) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-3-0" href="#v0-3-0">v0.3.0</a> [(2015-03-13)](https://github.com/nodegit/nodegit/tree/v0.3.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.7...v0.3.0)

**Closed issues:**

- Push [\#463](https://github.com/nodegit/nodegit/issues/463)

- Suppress astyle errors [\#459](https://github.com/nodegit/nodegit/issues/459)

- io.js support [\#447](https://github.com/nodegit/nodegit/issues/447)

- Meteor: icon fonts not working \(Resource interpreted as Font but transferred with MIME type text/html\) [\#443](https://github.com/nodegit/nodegit/issues/443)

- AnnotatedCommit.x listing as Annotated.commitX [\#437](https://github.com/nodegit/nodegit/issues/437)

- fetchAll\(\) fails unless a default signature is available [\#431](https://github.com/nodegit/nodegit/issues/431)

- Question: Is there a certificateCheck option available for pushing to a remote repository? [\#420](https://github.com/nodegit/nodegit/issues/420)

- Repository.open returns empty object [\#412](https://github.com/nodegit/nodegit/issues/412)

- Missing documentation for Tree.walk\(\) [\#411](https://github.com/nodegit/nodegit/issues/411)

- comparing from 0.1.4 to 0.2.0 [\#410](https://github.com/nodegit/nodegit/issues/410)

- Potential example issue in add-and-commit.js L45-48 [\#409](https://github.com/nodegit/nodegit/issues/409)

- failed to install on ubuntu 14.04 [\#408](https://github.com/nodegit/nodegit/issues/408)

- Return promises instead of nesting them [\#407](https://github.com/nodegit/nodegit/issues/407)

- segfault when cloning from private BitBucket repo [\#406](https://github.com/nodegit/nodegit/issues/406)

- Subtrees + custom error handling [\#400](https://github.com/nodegit/nodegit/issues/400)

- How to use nodegit in atom shell ..... [\#393](https://github.com/nodegit/nodegit/issues/393)

- Cannot create a new branch [\#391](https://github.com/nodegit/nodegit/issues/391)

- Remove fixappveyor from clone tests [\#385](https://github.com/nodegit/nodegit/issues/385)

- Commit isn't working [\#381](https://github.com/nodegit/nodegit/issues/381)

- Rename combyne folder to templates [\#378](https://github.com/nodegit/nodegit/issues/378)

- Cloning SSH repos seem to fail  [\#372](https://github.com/nodegit/nodegit/issues/372)

- Commit.getDiff is backwards? [\#368](https://github.com/nodegit/nodegit/issues/368)

- List all files in repo \(git ls-tree\) [\#365](https://github.com/nodegit/nodegit/issues/365)

- Checking out a branch [\#361](https://github.com/nodegit/nodegit/issues/361)

- nodegit no longer builds in nwjs [\#360](https://github.com/nodegit/nodegit/issues/360)

- Module install/build fails on Heroku  [\#332](https://github.com/nodegit/nodegit/issues/332)

- 2 Step Authentication [\#323](https://github.com/nodegit/nodegit/issues/323)

**Merged pull requests:**

- Rename `Annotated` to `AnnotatedCommit` [\#485](https://github.com/nodegit/nodegit/pull/485) ([johnhaley81](https://github.com/johnhaley81))

- Bump version to 0.3.0 [\#484](https://github.com/nodegit/nodegit/pull/484) ([johnhaley81](https://github.com/johnhaley81))

- Remove unneeded connect call from push example [\#483](https://github.com/nodegit/nodegit/pull/483) ([johnhaley81](https://github.com/johnhaley81))

- Update push example [\#481](https://github.com/nodegit/nodegit/pull/481) ([billt2006](https://github.com/billt2006))

- Fix trailing space in atom-shell windows install [\#480](https://github.com/nodegit/nodegit/pull/480) ([johnhaley81](https://github.com/johnhaley81))

- Fix atom-shell install on windows [\#479](https://github.com/nodegit/nodegit/pull/479) ([johnhaley81](https://github.com/johnhaley81))

- Updated API documentation link to work with NPM's markdown renderer.  [\#477](https://github.com/nodegit/nodegit/pull/477) ([hughfdjackson](https://github.com/hughfdjackson))

- Add option to `fetch` to prune the remote afterwards [\#476](https://github.com/nodegit/nodegit/pull/476) ([johnhaley81](https://github.com/johnhaley81))

- Make index.addAll use status to increase performance [\#475](https://github.com/nodegit/nodegit/pull/475) ([maxkorp](https://github.com/maxkorp))

- Add defaults to `Remote.prototype.push` [\#474](https://github.com/nodegit/nodegit/pull/474) ([johnhaley81](https://github.com/johnhaley81))

- Fix `createCommitOnHead` [\#473](https://github.com/nodegit/nodegit/pull/473) ([johnhaley81](https://github.com/johnhaley81))

- Move guides around to remove subindexes [\#472](https://github.com/nodegit/nodegit/pull/472) ([orderedlist](https://github.com/orderedlist))

- Put `Remote.Push` on the remote prototype [\#470](https://github.com/nodegit/nodegit/pull/470) ([johnhaley81](https://github.com/johnhaley81))

- Change Repository.prototype.setHead to be asynchronous [\#469](https://github.com/nodegit/nodegit/pull/469) ([jrbalsano](https://github.com/jrbalsano))

- Test in Node 0.12 and io.js [\#468](https://github.com/nodegit/nodegit/pull/468) ([tbranyen](https://github.com/tbranyen))

- Add checkoutBranch convenience method [\#466](https://github.com/nodegit/nodegit/pull/466) ([johnhaley81](https://github.com/johnhaley81))

- Don't assign enums to \_\_proto\_\_ [\#464](https://github.com/nodegit/nodegit/pull/464) ([orderedlist](https://github.com/orderedlist))

- Fix push example [\#462](https://github.com/nodegit/nodegit/pull/462) ([johnhaley81](https://github.com/johnhaley81))

- Adds support for strarray in structs [\#461](https://github.com/nodegit/nodegit/pull/461) ([orderedlist](https://github.com/orderedlist))

- supress astyle warnings [\#460](https://github.com/nodegit/nodegit/pull/460) ([maxkorp](https://github.com/maxkorp))

- Template proto functions [\#458](https://github.com/nodegit/nodegit/pull/458) ([maxkorp](https://github.com/maxkorp))

- Remote push [\#457](https://github.com/nodegit/nodegit/pull/457) ([mattyclarkson](https://github.com/mattyclarkson))

- Include missing lib files in nodegit.js template [\#455](https://github.com/nodegit/nodegit/pull/455) ([orderedlist](https://github.com/orderedlist))

- StrArray memory fix [\#454](https://github.com/nodegit/nodegit/pull/454) ([mattyclarkson](https://github.com/mattyclarkson))

- Better cloning with NodeGit [\#453](https://github.com/nodegit/nodegit/pull/453) ([tbranyen](https://github.com/tbranyen))

- Add Diff.prototype.findSimilar [\#452](https://github.com/nodegit/nodegit/pull/452) ([orderedlist](https://github.com/orderedlist))

- Str array converter fix [\#451](https://github.com/nodegit/nodegit/pull/451) ([mattyclarkson](https://github.com/mattyclarkson))

- Default signature always returns valid signature [\#450](https://github.com/nodegit/nodegit/pull/450) ([johnhaley81](https://github.com/johnhaley81))

- Status.byIndex and StatusEntry [\#448](https://github.com/nodegit/nodegit/pull/448) ([orderedlist](https://github.com/orderedlist))

- Upgrade to nan 1.7.0 [\#446](https://github.com/nodegit/nodegit/pull/446) ([orderedlist](https://github.com/orderedlist))

- Added in an HTTP url for test [\#445](https://github.com/nodegit/nodegit/pull/445) ([tbranyen](https://github.com/tbranyen))

- Add examples [\#442](https://github.com/nodegit/nodegit/pull/442) ([johnhaley81](https://github.com/johnhaley81))

- hide callback payloads from javascript [\#441](https://github.com/nodegit/nodegit/pull/441) ([maxkorp](https://github.com/maxkorp))

- Fix transfer callback stats [\#440](https://github.com/nodegit/nodegit/pull/440) ([johnhaley81](https://github.com/johnhaley81))

- Automatically free repositories post clone [\#434](https://github.com/nodegit/nodegit/pull/434) ([tbranyen](https://github.com/tbranyen))

- Skip transfer progress test until it's fixed [\#433](https://github.com/nodegit/nodegit/pull/433) ([johnhaley81](https://github.com/johnhaley81))

- Change environment to default for upgraded service [\#428](https://github.com/nodegit/nodegit/pull/428) ([maxkorp](https://github.com/maxkorp))

- Make the `git\_remote\_push` function async [\#427](https://github.com/nodegit/nodegit/pull/427) ([johnhaley81](https://github.com/johnhaley81))

- Attempt to fix Windows file locking bug [\#425](https://github.com/nodegit/nodegit/pull/425) ([tbranyen](https://github.com/tbranyen))

- Fix seg faults [\#424](https://github.com/nodegit/nodegit/pull/424) ([johnhaley81](https://github.com/johnhaley81))

- Clean up the persisting of props [\#423](https://github.com/nodegit/nodegit/pull/423) ([johnhaley81](https://github.com/johnhaley81))

- Fix indexEntry construction and blobFromBuffer [\#422](https://github.com/nodegit/nodegit/pull/422) ([orderedlist](https://github.com/orderedlist))

- Allow for saving of props to an object [\#421](https://github.com/nodegit/nodegit/pull/421) ([johnhaley81](https://github.com/johnhaley81))

- Fixes segfault issue recorded in \#406 [\#419](https://github.com/nodegit/nodegit/pull/419) ([tbranyen](https://github.com/tbranyen))

- Update jsdoc and ignore some methods [\#418](https://github.com/nodegit/nodegit/pull/418) ([orderedlist](https://github.com/orderedlist))

- Converting Examples to Guides [\#417](https://github.com/nodegit/nodegit/pull/417) ([tbranyen](https://github.com/tbranyen))

- Fix callbacks with just return value and single payload [\#416](https://github.com/nodegit/nodegit/pull/416) ([johnhaley81](https://github.com/johnhaley81))

- Add `git\_reset` and `git\_reset\_default` [\#415](https://github.com/nodegit/nodegit/pull/415) ([johnhaley81](https://github.com/johnhaley81))

- Enable `git\_index\_remove\_all` and `git\_index\_update\_all` [\#414](https://github.com/nodegit/nodegit/pull/414) ([johnhaley81](https://github.com/johnhaley81))

- Added code for `git\_strarray` and enabled `git\_index\_add\_all` [\#413](https://github.com/nodegit/nodegit/pull/413) ([johnhaley81](https://github.com/johnhaley81))

- Tree Entry getBlob\(\) should also support the callback pattern. [\#405](https://github.com/nodegit/nodegit/pull/405) ([jeffwilcox](https://github.com/jeffwilcox))

- Adds in git\_checkout\_tree [\#402](https://github.com/nodegit/nodegit/pull/402) ([tbranyen](https://github.com/tbranyen))

- Made changes to the README [\#399](https://github.com/nodegit/nodegit/pull/399) ([tbranyen](https://github.com/tbranyen))

- Expose gc so all tests run in CI [\#398](https://github.com/nodegit/nodegit/pull/398) ([tbranyen](https://github.com/tbranyen))

- One more dependency update [\#397](https://github.com/nodegit/nodegit/pull/397) ([maxkorp](https://github.com/maxkorp))

- Update moar dependencies. [\#396](https://github.com/nodegit/nodegit/pull/396) ([maxkorp](https://github.com/maxkorp))

- Updated most dependencies to latest versions [\#394](https://github.com/nodegit/nodegit/pull/394) ([johnhaley81](https://github.com/johnhaley81))

- Index diffing [\#392](https://github.com/nodegit/nodegit/pull/392) ([orderedlist](https://github.com/orderedlist))

- Update to libgit2 v0.22.1 [\#390](https://github.com/nodegit/nodegit/pull/390) ([johnhaley81](https://github.com/johnhaley81))

- Fix test issues [\#388](https://github.com/nodegit/nodegit/pull/388) ([maxkorp](https://github.com/maxkorp))

- Fix building when a space is in the path [\#387](https://github.com/nodegit/nodegit/pull/387) ([billt2006](https://github.com/billt2006))

- General maintenance [\#386](https://github.com/nodegit/nodegit/pull/386) ([maxkorp](https://github.com/maxkorp))

- Add 2 convenience methods to revwalk [\#384](https://github.com/nodegit/nodegit/pull/384) ([maxkorp](https://github.com/maxkorp))

- Make all cred generators sync. [\#377](https://github.com/nodegit/nodegit/pull/377) ([maxkorp](https://github.com/maxkorp))

- Status and StatusList [\#374](https://github.com/nodegit/nodegit/pull/374) ([orderedlist](https://github.com/orderedlist))

- Fix the package scripts [\#373](https://github.com/nodegit/nodegit/pull/373) ([maxkorp](https://github.com/maxkorp))

- Removes Node 0.11 testing completely [\#371](https://github.com/nodegit/nodegit/pull/371) ([tbranyen](https://github.com/tbranyen))

- Allow null trees on Diff.treeToTree [\#370](https://github.com/nodegit/nodegit/pull/370) ([orderedlist](https://github.com/orderedlist))

- Atom shell support [\#369](https://github.com/nodegit/nodegit/pull/369) ([maxkorp](https://github.com/maxkorp))

- `Checkout.head` initializes options if none are passed [\#367](https://github.com/nodegit/nodegit/pull/367) ([johnhaley81](https://github.com/johnhaley81))

- INCLUDE\_UNTRACKED option not working for diffs [\#366](https://github.com/nodegit/nodegit/pull/366) ([kmctown](https://github.com/kmctown))

- Updated fs-extra to 0.15.0 [\#363](https://github.com/nodegit/nodegit/pull/363) ([johnhaley81](https://github.com/johnhaley81))

- Make remote\#download async [\#326](https://github.com/nodegit/nodegit/pull/326) ([tbranyen](https://github.com/tbranyen))

- Enable transfer progress [\#325](https://github.com/nodegit/nodegit/pull/325) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-2-7" href="#v0-2-7">v0.2.7</a> [(2015-01-21)](https://github.com/nodegit/nodegit/tree/v0.2.7)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.6...v0.2.7)

## <a name="v0-2-6" href="#v0-2-6">v0.2.6</a> [(2015-01-20)](https://github.com/nodegit/nodegit/tree/v0.2.6)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.5...v0.2.6)

**Merged pull requests:**

- \[WIP\] Added in some diff functions from libgit2 [\#348](https://github.com/nodegit/nodegit/pull/348) ([johnhaley81](https://github.com/johnhaley81))

## <a name="v0-2-5" href="#v0-2-5">v0.2.5</a> [(2015-01-20)](https://github.com/nodegit/nodegit/tree/v0.2.5)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.4...v0.2.5)

**Closed issues:**

- Lookup a non existent commit crashes the process. [\#353](https://github.com/nodegit/nodegit/issues/353)

- Why node-git uses 90% rotated hexagon? [\#344](https://github.com/nodegit/nodegit/issues/344)

- Needed pull example or help with code [\#341](https://github.com/nodegit/nodegit/issues/341)

- Can't require nodegit without building it explicitly [\#340](https://github.com/nodegit/nodegit/issues/340)

- Tracking down bugs [\#331](https://github.com/nodegit/nodegit/issues/331)

- Document possible values of CloneOptions [\#330](https://github.com/nodegit/nodegit/issues/330)

- Require generating error [\#329](https://github.com/nodegit/nodegit/issues/329)

- Failed getting Banner [\#328](https://github.com/nodegit/nodegit/issues/328)

- Documentation broken [\#327](https://github.com/nodegit/nodegit/issues/327)

- Fetch doesn't seem to work with https urls. [\#322](https://github.com/nodegit/nodegit/issues/322)

**Merged pull requests:**

- Refactor installation and publication [\#359](https://github.com/nodegit/nodegit/pull/359) ([maxkorp](https://github.com/maxkorp))

- Lint examples [\#358](https://github.com/nodegit/nodegit/pull/358) ([maxkorp](https://github.com/maxkorp))

- Commit.getParents working with merge commits [\#357](https://github.com/nodegit/nodegit/pull/357) ([bjornarg](https://github.com/bjornarg))

- Fixed a typo in the debug build instruction. [\#356](https://github.com/nodegit/nodegit/pull/356) ([mcollina](https://github.com/mcollina))

- \[WIP\] Attempt at fixing appveyor [\#352](https://github.com/nodegit/nodegit/pull/352) ([johnhaley81](https://github.com/johnhaley81))

- Updated to nan 1.5.0 and fixed build errors [\#351](https://github.com/nodegit/nodegit/pull/351) ([johnhaley81](https://github.com/johnhaley81))

- Added debug build instructions. [\#349](https://github.com/nodegit/nodegit/pull/349) ([mcollina](https://github.com/mcollina))

- Added checkout head method and tests [\#347](https://github.com/nodegit/nodegit/pull/347) ([johnhaley81](https://github.com/johnhaley81))

- bump devDependencies [\#346](https://github.com/nodegit/nodegit/pull/346) ([PeterDaveHello](https://github.com/PeterDaveHello))

- Update dependency node-pre-gyp to ~0.6 [\#345](https://github.com/nodegit/nodegit/pull/345) ([PeterDaveHello](https://github.com/PeterDaveHello))

- Update dependency fs-extra to ~0.14.0 [\#343](https://github.com/nodegit/nodegit/pull/343) ([PeterDaveHello](https://github.com/PeterDaveHello))

- Add Dependency badge in readme [\#342](https://github.com/nodegit/nodegit/pull/342) ([PeterDaveHello](https://github.com/PeterDaveHello))

- Fixed promise chain on install [\#339](https://github.com/nodegit/nodegit/pull/339) ([johnhaley81](https://github.com/johnhaley81))

- Do not double free during callbacks. [\#338](https://github.com/nodegit/nodegit/pull/338) ([mcollina](https://github.com/mcollina))

- Use svg instead of png to get better image quality [\#337](https://github.com/nodegit/nodegit/pull/337) ([PeterDaveHello](https://github.com/PeterDaveHello))

- Update to libgit 0.21.4 [\#336](https://github.com/nodegit/nodegit/pull/336) ([johnhaley81](https://github.com/johnhaley81))

- Fix issue 333 [\#334](https://github.com/nodegit/nodegit/pull/334) ([johnhaley81](https://github.com/johnhaley81))

- Update appveyor.yml to remove project id [\#324](https://github.com/nodegit/nodegit/pull/324) ([vladikoff](https://github.com/vladikoff))

- moving some deps to devdeps [\#320](https://github.com/nodegit/nodegit/pull/320) ([maxkorp](https://github.com/maxkorp))

## <a name="v0-2-4" href="#v0-2-4">v0.2.4</a> [(2014-12-05)](https://github.com/nodegit/nodegit/tree/v0.2.4)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.3...v0.2.4)

**Closed issues:**

- Fetch does not really fetch [\#314](https://github.com/nodegit/nodegit/issues/314)

- Generate Missing Tests - Unable [\#313](https://github.com/nodegit/nodegit/issues/313)

- Unable to get reference for HEAD [\#311](https://github.com/nodegit/nodegit/issues/311)

- nodegit.Signature.now broken [\#306](https://github.com/nodegit/nodegit/issues/306)

- current branch [\#305](https://github.com/nodegit/nodegit/issues/305)

- Module fails to load [\#299](https://github.com/nodegit/nodegit/issues/299)

- How to list all tags? [\#298](https://github.com/nodegit/nodegit/issues/298)

- Building for ARM [\#292](https://github.com/nodegit/nodegit/issues/292)

- Next release checklist [\#256](https://github.com/nodegit/nodegit/issues/256)

**Merged pull requests:**

- Fixed fetch to be async and use callbacks [\#319](https://github.com/nodegit/nodegit/pull/319) ([johnhaley81](https://github.com/johnhaley81))

- Make contributing.md generic and add testing.md [\#318](https://github.com/nodegit/nodegit/pull/318) ([maxkorp](https://github.com/maxkorp))

- Fix repo init ext [\#317](https://github.com/nodegit/nodegit/pull/317) ([maxkorp](https://github.com/maxkorp))

- Fix 313 generate scripts [\#315](https://github.com/nodegit/nodegit/pull/315) ([xinUmbralis](https://github.com/xinUmbralis))

- Fix \#311 [\#312](https://github.com/nodegit/nodegit/pull/312) ([johnhaley81](https://github.com/johnhaley81))

- Fix publishing [\#310](https://github.com/nodegit/nodegit/pull/310) ([maxkorp](https://github.com/maxkorp))

- detect node-webkit and build with nw-gyp [\#309](https://github.com/nodegit/nodegit/pull/309) ([maxkorp](https://github.com/maxkorp))

- fix signature.now and add signature tests [\#308](https://github.com/nodegit/nodegit/pull/308) ([maxkorp](https://github.com/maxkorp))

- move nodegit.js to a template to remove idefs dependency [\#303](https://github.com/nodegit/nodegit/pull/303) ([maxkorp](https://github.com/maxkorp))

- Fixed tag list and added a test for it [\#300](https://github.com/nodegit/nodegit/pull/300) ([johnhaley81](https://github.com/johnhaley81))

- Convenience methods [\#297](https://github.com/nodegit/nodegit/pull/297) ([johnhaley81](https://github.com/johnhaley81))

- Clean up the contents of the generate folder [\#296](https://github.com/nodegit/nodegit/pull/296) ([maxkorp](https://github.com/maxkorp))

- Styling  [\#295](https://github.com/nodegit/nodegit/pull/295) ([maxkorp](https://github.com/maxkorp))

## <a name="v0-2-3" href="#v0-2-3">v0.2.3</a> [(2014-11-25)](https://github.com/nodegit/nodegit/tree/v0.2.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.2...v0.2.3)

## <a name="v0-2-2" href="#v0-2-2">v0.2.2</a> [(2014-11-25)](https://github.com/nodegit/nodegit/tree/v0.2.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.1...v0.2.2)

**Merged pull requests:**

- Moved some dependencies around to help the generate not fail [\#294](https://github.com/nodegit/nodegit/pull/294) ([johnhaley81](https://github.com/johnhaley81))

## <a name="v0-2-1" href="#v0-2-1">v0.2.1</a> [(2014-11-25)](https://github.com/nodegit/nodegit/tree/v0.2.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.2.0...v0.2.1)

**Merged pull requests:**

- Rewrite installer [\#293](https://github.com/nodegit/nodegit/pull/293) ([johnhaley81](https://github.com/johnhaley81))

## <a name="v0-2-0" href="#v0-2-0">v0.2.0</a> [(2014-11-25)](https://github.com/nodegit/nodegit/tree/v0.2.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.1.4...v0.2.0)

**Closed issues:**

- Find some way to automatically generate a list of missing tests. [\#272](https://github.com/nodegit/nodegit/issues/272)

- libgit2 creation methods have name collisions with internal V8 functions [\#271](https://github.com/nodegit/nodegit/issues/271)

- Enums are still being manually assigned in javascript [\#268](https://github.com/nodegit/nodegit/issues/268)

- We're using too many promise libraries [\#264](https://github.com/nodegit/nodegit/issues/264)

- unable to resolve symbolic references [\#262](https://github.com/nodegit/nodegit/issues/262)

- nodegit installation falls back when Python install dir contains spaces [\#261](https://github.com/nodegit/nodegit/issues/261)

- Probe features [\#245](https://github.com/nodegit/nodegit/issues/245)

- require\('path'\).Repo.open\(...\) returns {} [\#241](https://github.com/nodegit/nodegit/issues/241)

- RevWalk malloc error [\#239](https://github.com/nodegit/nodegit/issues/239)

- OS X tests in Travis-CI [\#237](https://github.com/nodegit/nodegit/issues/237)

- Fix RevWalk tests [\#236](https://github.com/nodegit/nodegit/issues/236)

- Simple clone fails. [\#231](https://github.com/nodegit/nodegit/issues/231)

- Create templates for remaining src and include files [\#230](https://github.com/nodegit/nodegit/issues/230)

- Error: SSL is not supported by this copy of libgit2. [\#228](https://github.com/nodegit/nodegit/issues/228)

- error while install nodegit latest version 0.1.4 [\#225](https://github.com/nodegit/nodegit/issues/225)

- error while install nodegit latest version 0.1.4 [\#224](https://github.com/nodegit/nodegit/issues/224)

- Did getReferences dissapear? [\#223](https://github.com/nodegit/nodegit/issues/223)

- Again for \#147 [\#218](https://github.com/nodegit/nodegit/issues/218)

- Update documentation on nodegit.org [\#217](https://github.com/nodegit/nodegit/issues/217)

- Stable = bump to 1.0 [\#215](https://github.com/nodegit/nodegit/issues/215)

- Example on nodegit.com homepage is invalid [\#211](https://github.com/nodegit/nodegit/issues/211)

- tree.diffWorkDir deprecated? [\#209](https://github.com/nodegit/nodegit/issues/209)

- Abort on getRemotes [\#201](https://github.com/nodegit/nodegit/issues/201)

- Generic Logging/Tracing mechanism [\#199](https://github.com/nodegit/nodegit/issues/199)

- Repo\#openIndex missing [\#197](https://github.com/nodegit/nodegit/issues/197)

- Documentation on http://www.nodegit.org/ out of date [\#196](https://github.com/nodegit/nodegit/issues/196)

- Remove extern "C" with 0.21 bump [\#193](https://github.com/nodegit/nodegit/issues/193)

- CloneOptions documentation lacking [\#192](https://github.com/nodegit/nodegit/issues/192)

- Webpage examples are not up to date [\#190](https://github.com/nodegit/nodegit/issues/190)

- Automatically generate structs from types array [\#187](https://github.com/nodegit/nodegit/issues/187)

- Error: connect ETIMEDOUT during install [\#179](https://github.com/nodegit/nodegit/issues/179)

- TODO [\#177](https://github.com/nodegit/nodegit/issues/177)

- Notes [\#176](https://github.com/nodegit/nodegit/issues/176)

- Integration improvements. [\#171](https://github.com/nodegit/nodegit/issues/171)

**Merged pull requests:**

- add history.md and update readme [\#291](https://github.com/nodegit/nodegit/pull/291) ([maxkorp](https://github.com/maxkorp))

- Added tests for commit [\#290](https://github.com/nodegit/nodegit/pull/290) ([nkt](https://github.com/nkt))

- Added editor config [\#289](https://github.com/nodegit/nodegit/pull/289) ([nkt](https://github.com/nkt))

- \[WIP\] Push example [\#288](https://github.com/nodegit/nodegit/pull/288) ([johnhaley81](https://github.com/johnhaley81))

- \[WIP\] Better installation flow [\#287](https://github.com/nodegit/nodegit/pull/287) ([tbranyen](https://github.com/tbranyen))

- Merge methods and tests and examples [\#286](https://github.com/nodegit/nodegit/pull/286) ([maxkorp](https://github.com/maxkorp))

- Add details-for-tree-entry [\#285](https://github.com/nodegit/nodegit/pull/285) ([johnhaley81](https://github.com/johnhaley81))

- Add repo init example [\#284](https://github.com/nodegit/nodegit/pull/284) ([maxkorp](https://github.com/maxkorp))

- update add-and-commit example to include new paths [\#283](https://github.com/nodegit/nodegit/pull/283) ([maxkorp](https://github.com/maxkorp))

- General cleanup [\#282](https://github.com/nodegit/nodegit/pull/282) ([maxkorp](https://github.com/maxkorp))

- Added osx for testing on Travis [\#281](https://github.com/nodegit/nodegit/pull/281) ([johnhaley81](https://github.com/johnhaley81))

- Added " around python path to help fix issues with spaces in path [\#280](https://github.com/nodegit/nodegit/pull/280) ([johnhaley81](https://github.com/johnhaley81))

- Tests for branch class [\#279](https://github.com/nodegit/nodegit/pull/279) ([johnhaley81](https://github.com/johnhaley81))

- Exposes the NodeGit Promise implementation [\#278](https://github.com/nodegit/nodegit/pull/278) ([tbranyen](https://github.com/tbranyen))

- \[WIP\] Update examples [\#276](https://github.com/nodegit/nodegit/pull/276) ([johnhaley81](https://github.com/johnhaley81))

- Added script to generate missing tests [\#274](https://github.com/nodegit/nodegit/pull/274) ([johnhaley81](https://github.com/johnhaley81))

- Rename new [\#273](https://github.com/nodegit/nodegit/pull/273) ([maxkorp](https://github.com/maxkorp))

- MSBUILD doesn't allow an array of size 0 [\#270](https://github.com/nodegit/nodegit/pull/270) ([johnhaley81](https://github.com/johnhaley81))

- \[WIP\] generate enum definitions [\#269](https://github.com/nodegit/nodegit/pull/269) ([maxkorp](https://github.com/maxkorp))

- add Refs.nameToId and test [\#267](https://github.com/nodegit/nodegit/pull/267) ([maxkorp](https://github.com/maxkorp))

- voidcheck string pointers and reenable attr test [\#266](https://github.com/nodegit/nodegit/pull/266) ([maxkorp](https://github.com/maxkorp))

- require --documentation flag to include text in idefs [\#265](https://github.com/nodegit/nodegit/pull/265) ([maxkorp](https://github.com/maxkorp))

- Added ability for callbacks to poll promises for fulfillment value [\#260](https://github.com/nodegit/nodegit/pull/260) ([johnhaley81](https://github.com/johnhaley81))

- Generate nodegit from libgit2 docs and refactor descriptor [\#259](https://github.com/nodegit/nodegit/pull/259) ([johnhaley81](https://github.com/johnhaley81))

- Fix revwalk tests [\#258](https://github.com/nodegit/nodegit/pull/258) ([maxkorp](https://github.com/maxkorp))

- Bump to latest libgit2 [\#257](https://github.com/nodegit/nodegit/pull/257) ([tbranyen](https://github.com/tbranyen))

- Use Start-Process to start pageant.exe [\#254](https://github.com/nodegit/nodegit/pull/254) ([FeodorFitsner](https://github.com/FeodorFitsner))

- Adds in a broken unit test for \#109 [\#252](https://github.com/nodegit/nodegit/pull/252) ([tbranyen](https://github.com/tbranyen))

- Added more git\_cred methods [\#251](https://github.com/nodegit/nodegit/pull/251) ([johnhaley81](https://github.com/johnhaley81))

- Refactor classes [\#250](https://github.com/nodegit/nodegit/pull/250) ([maxkorp](https://github.com/maxkorp))

- Update Readme, to improve example code [\#248](https://github.com/nodegit/nodegit/pull/248) ([nmn](https://github.com/nmn))

- \[TEST\] Appveyor agent [\#247](https://github.com/nodegit/nodegit/pull/247) ([tbranyen](https://github.com/tbranyen))

- Refactor classes [\#246](https://github.com/nodegit/nodegit/pull/246) ([maxkorp](https://github.com/maxkorp))

- Buf methods [\#244](https://github.com/nodegit/nodegit/pull/244) ([tbranyen](https://github.com/tbranyen))

- Branch methods [\#243](https://github.com/nodegit/nodegit/pull/243) ([tbranyen](https://github.com/tbranyen))

- Blame methods [\#242](https://github.com/nodegit/nodegit/pull/242) ([tbranyen](https://github.com/tbranyen))

- Add revwalk.hide and revwalk.simplifyFirstParent [\#235](https://github.com/nodegit/nodegit/pull/235) ([tbranyen](https://github.com/tbranyen))

- Add revwalk.hide and revwalk.simplifyFirstParent [\#234](https://github.com/nodegit/nodegit/pull/234) ([orderedlist](https://github.com/orderedlist))

- Moved wrapper/copy out of include/src [\#233](https://github.com/nodegit/nodegit/pull/233) ([johnhaley81](https://github.com/johnhaley81))

- Removed ejs dependency [\#232](https://github.com/nodegit/nodegit/pull/232) ([johnhaley81](https://github.com/johnhaley81))

- Bump to latest libgit2 [\#229](https://github.com/nodegit/nodegit/pull/229) ([tbranyen](https://github.com/tbranyen))

- WIP: Refactor source generation templates from EJS to Combyne [\#227](https://github.com/nodegit/nodegit/pull/227) ([tbranyen](https://github.com/tbranyen))

- Test fixes [\#226](https://github.com/nodegit/nodegit/pull/226) ([johnhaley81](https://github.com/johnhaley81))

- Added new methods in checkout and repository [\#207](https://github.com/nodegit/nodegit/pull/207) ([tbranyen](https://github.com/tbranyen))

- Added additional remote methods [\#206](https://github.com/nodegit/nodegit/pull/206) ([tbranyen](https://github.com/tbranyen))

- Added git\_remote\_url and git\_remote\_load [\#205](https://github.com/nodegit/nodegit/pull/205) ([tbranyen](https://github.com/tbranyen))

- Add in remote listing support and test [\#204](https://github.com/nodegit/nodegit/pull/204) ([tbranyen](https://github.com/tbranyen))

- Attr methods [\#203](https://github.com/nodegit/nodegit/pull/203) ([tbranyen](https://github.com/tbranyen))

- Support latest libgit2 v0.21.0 [\#200](https://github.com/nodegit/nodegit/pull/200) ([tbranyen](https://github.com/tbranyen))

- Add Repo.openIndex [\#198](https://github.com/nodegit/nodegit/pull/198) ([tbranyen](https://github.com/tbranyen))

- Clone methods [\#195](https://github.com/nodegit/nodegit/pull/195) ([tbranyen](https://github.com/tbranyen))

- Remove all unused vendor directories [\#194](https://github.com/nodegit/nodegit/pull/194) ([tbranyen](https://github.com/tbranyen))

- \[WIP\] Mocha integration [\#189](https://github.com/nodegit/nodegit/pull/189) ([tbranyen](https://github.com/tbranyen))

- Auto gen structs [\#188](https://github.com/nodegit/nodegit/pull/188) ([tbranyen](https://github.com/tbranyen))

- Add in support for repository init ext [\#186](https://github.com/nodegit/nodegit/pull/186) ([tbranyen](https://github.com/tbranyen))

- moved libgit2 gyp to separate dir [\#184](https://github.com/nodegit/nodegit/pull/184) ([deepak1556](https://github.com/deepak1556))

- Remove all generated source code. [\#181](https://github.com/nodegit/nodegit/pull/181) ([tbranyen](https://github.com/tbranyen))

- Better installation flow for developing. [\#180](https://github.com/nodegit/nodegit/pull/180) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-1-4" href="#v0-1-4">v0.1.4</a> [(2014-06-13)](https://github.com/nodegit/nodegit/tree/v0.1.4)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.1.3...v0.1.4)

**Closed issues:**

- Redis Backend [\#173](https://github.com/nodegit/nodegit/issues/173)

- using "Branch" object results in "undefined" error =\> branch.cc missing from binding.gyp? [\#166](https://github.com/nodegit/nodegit/issues/166)

- Windows: Failure on install [\#158](https://github.com/nodegit/nodegit/issues/158)

- Can't install v0.1.2 under OSX [\#155](https://github.com/nodegit/nodegit/issues/155)

**Merged pull requests:**

- \[WIP\] Prebuilt binaries. [\#178](https://github.com/nodegit/nodegit/pull/178) ([tbranyen](https://github.com/tbranyen))

- NodeJS v0.11.13 compatibility [\#175](https://github.com/nodegit/nodegit/pull/175) ([3y3](https://github.com/3y3))

- Fixed: "ReferenceError: error is not defined" [\#169](https://github.com/nodegit/nodegit/pull/169) ([danyshaanan](https://github.com/danyshaanan))

## <a name="v0-1-3" href="#v0-1-3">v0.1.3</a> [(2014-05-02)](https://github.com/nodegit/nodegit/tree/v0.1.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.1.2...v0.1.3)

**Merged pull requests:**

- Fix erroneous OS detection for installation in OS X. [\#156](https://github.com/nodegit/nodegit/pull/156) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-1-2" href="#v0-1-2">v0.1.2</a> [(2014-05-02)](https://github.com/nodegit/nodegit/tree/v0.1.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.1.1...v0.1.2)

**Closed issues:**

- gyp ERR cannot find -lgit2 [\#150](https://github.com/nodegit/nodegit/issues/150)

- Read file from git server [\#145](https://github.com/nodegit/nodegit/issues/145)

- "emulate git log" example error [\#144](https://github.com/nodegit/nodegit/issues/144)

- repo.workdir\(\) crashes \(SIGSEGV\) on a bare repo [\#128](https://github.com/nodegit/nodegit/issues/128)

- How to create Branch using the API? [\#124](https://github.com/nodegit/nodegit/issues/124)

- 'npm run-script gen && npm install' on Ubuntu 13.04 [\#122](https://github.com/nodegit/nodegit/issues/122)

- Error while installing Nodegit 0.1.0 [\#120](https://github.com/nodegit/nodegit/issues/120)

- Question: How would I implement the equivalent of `git status`? [\#117](https://github.com/nodegit/nodegit/issues/117)

- Sync versions of all the methods [\#115](https://github.com/nodegit/nodegit/issues/115)

- Tick version \# [\#107](https://github.com/nodegit/nodegit/issues/107)

- Windows support [\#71](https://github.com/nodegit/nodegit/issues/71)

- Create test for history with merge commits [\#64](https://github.com/nodegit/nodegit/issues/64)

**Merged pull requests:**

- Fixed OSX Directions [\#143](https://github.com/nodegit/nodegit/pull/143) ([nickpoorman](https://github.com/nickpoorman))

- Add ubuntu lib dependencies to the readme [\#141](https://github.com/nodegit/nodegit/pull/141) ([bigthyme](https://github.com/bigthyme))

- WIP New installer. [\#140](https://github.com/nodegit/nodegit/pull/140) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-1-1" href="#v0-1-1">v0.1.1</a> [(2014-03-23)](https://github.com/nodegit/nodegit/tree/v0.1.1)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.1.0...v0.1.1)

**Closed issues:**

- Misleading Readme [\#138](https://github.com/nodegit/nodegit/issues/138)

-  Cannot find module './build/Debug/nodegit' [\#137](https://github.com/nodegit/nodegit/issues/137)

- Support for Node 0.11+ [\#134](https://github.com/nodegit/nodegit/issues/134)

- installer cant seem to find python [\#126](https://github.com/nodegit/nodegit/issues/126)

- Cannot build when parent directory contains space\(s\)  [\#123](https://github.com/nodegit/nodegit/issues/123)

- question: how cvv8 is used? [\#118](https://github.com/nodegit/nodegit/issues/118)

- question: gen.js does not generate wrapper.h and wrapper.cc [\#116](https://github.com/nodegit/nodegit/issues/116)

- tree.diffIndex: pointer being freed was not allocated [\#112](https://github.com/nodegit/nodegit/issues/112)

- Use as a dependency of another node project? [\#110](https://github.com/nodegit/nodegit/issues/110)

- Segmentation faults with concurrent access? [\#104](https://github.com/nodegit/nodegit/issues/104)

- tree.diffWorkDir [\#101](https://github.com/nodegit/nodegit/issues/101)

- getReference passes unexpected object into callback [\#98](https://github.com/nodegit/nodegit/issues/98)

- index.removeByPath stops execution [\#97](https://github.com/nodegit/nodegit/issues/97)

- Missing example: commit to a local repo \(i.e. git add, git commit\) [\#96](https://github.com/nodegit/nodegit/issues/96)

- Get contents of index entry? [\#94](https://github.com/nodegit/nodegit/issues/94)

- Failure to Build nodegit  at Commit 0aa9a3c120 on OS X 10.6.8 [\#92](https://github.com/nodegit/nodegit/issues/92)

- TypeError: Cannot call method 'clone' of undefined [\#91](https://github.com/nodegit/nodegit/issues/91)

- missing cstring [\#88](https://github.com/nodegit/nodegit/issues/88)

- Installing fails - can't find vendor/libgit2/build [\#80](https://github.com/nodegit/nodegit/issues/80)

- Improving JavaScript API [\#73](https://github.com/nodegit/nodegit/issues/73)

- Using code-generation to generate  [\#70](https://github.com/nodegit/nodegit/issues/70)

**Merged pull requests:**

- Fix and improve testing. [\#139](https://github.com/nodegit/nodegit/pull/139) ([tbranyen](https://github.com/tbranyen))

- Support for Node 0.11+ [\#135](https://github.com/nodegit/nodegit/pull/135) ([pierreinglebert](https://github.com/pierreinglebert))

- Added git\_diff\_delta\_dup to git\_diff\_get\_patch to fix a memory issue whe... [\#113](https://github.com/nodegit/nodegit/pull/113) ([kmctown](https://github.com/kmctown))

- Try requiring build/Debug/nodegit if build/Release/nodegit wasn't found. [\#108](https://github.com/nodegit/nodegit/pull/108) ([papandreou](https://github.com/papandreou))

- Updated v0.18.0.json to make the index and DiffOptions arguments in Inde... [\#106](https://github.com/nodegit/nodegit/pull/106) ([kmctown](https://github.com/kmctown))

- Duplicate git\_error struct before passing it on [\#105](https://github.com/nodegit/nodegit/pull/105) ([papandreou](https://github.com/papandreou))

- Changed v0.18.0.json so diffWorkDir DiffOptions argument is optional. Ad... [\#103](https://github.com/nodegit/nodegit/pull/103) ([kmctown](https://github.com/kmctown))

- Reviewd and fixed examples [\#102](https://github.com/nodegit/nodegit/pull/102) ([micha149](https://github.com/micha149))

- cmake 2.8 is required to build nodegit [\#100](https://github.com/nodegit/nodegit/pull/100) ([dcolens](https://github.com/dcolens))

- new add-and-commit.js and remove-and-commit.js examples [\#99](https://github.com/nodegit/nodegit/pull/99) ([dcolens](https://github.com/dcolens))

- Add missing fields to index entry [\#95](https://github.com/nodegit/nodegit/pull/95) ([papandreou](https://github.com/papandreou))

- Made the tests pass and making each test self-contained [\#90](https://github.com/nodegit/nodegit/pull/90) ([FrozenCow](https://github.com/FrozenCow))

- Fixed compile error: memcpy not defined [\#89](https://github.com/nodegit/nodegit/pull/89) ([FrozenCow](https://github.com/FrozenCow))

- Add system dependencies for OSX install [\#82](https://github.com/nodegit/nodegit/pull/82) ([philschatz](https://github.com/philschatz))

## <a name="v0-1-0" href="#v0-1-0">v0.1.0</a> [(2013-09-07)](https://github.com/nodegit/nodegit/tree/v0.1.0)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.79...v0.1.0)

**Closed issues:**

- The api in README is incorrect [\#87](https://github.com/nodegit/nodegit/issues/87)

- message\_encoding in documentation for Repo.createCommit [\#86](https://github.com/nodegit/nodegit/issues/86)

- How to retrieve blob with binary content? [\#83](https://github.com/nodegit/nodegit/issues/83)

- Incorrect commit oid's when aggregated from commit.history\(\) [\#81](https://github.com/nodegit/nodegit/issues/81)

- How do you list branches in repo? [\#76](https://github.com/nodegit/nodegit/issues/76)

- License? [\#74](https://github.com/nodegit/nodegit/issues/74)

- Nested walks scatter memory and cause SEGFAULTS [\#72](https://github.com/nodegit/nodegit/issues/72)

- feature request: Provide fileMode / getType method on tree entries [\#67](https://github.com/nodegit/nodegit/issues/67)

- Document DiffList [\#66](https://github.com/nodegit/nodegit/issues/66)

- Procedure for moving development to nodegit/nodegit [\#55](https://github.com/nodegit/nodegit/issues/55)

- Cannot install on OSX [\#49](https://github.com/nodegit/nodegit/issues/49)

**Merged pull requests:**

- Codegen [\#79](https://github.com/nodegit/nodegit/pull/79) ([nkallen](https://github.com/nkallen))

- Updated LICENSE to MIT [\#75](https://github.com/nodegit/nodegit/pull/75) ([tbranyen](https://github.com/tbranyen))

## <a name="v0-0-79" href="#v0-0-79">v0.0.79</a> [(2013-04-05)](https://github.com/nodegit/nodegit/tree/v0.0.79)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.778...v0.0.79)

**Closed issues:**

- Clarify commit.history documentation [\#63](https://github.com/nodegit/nodegit/issues/63)

- Python error on installing nodegit 0.0.77 [\#59](https://github.com/nodegit/nodegit/issues/59)

## <a name="v0-0-778" href="#v0-0-778">v0.0.778</a> [(2013-03-26)](https://github.com/nodegit/nodegit/tree/v0.0.778)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.77...v0.0.778)

**Merged pull requests:**

- See issue \#59 [\#60](https://github.com/nodegit/nodegit/pull/60) ([dctr](https://github.com/dctr))

## <a name="v0-0-77" href="#v0-0-77">v0.0.77</a> [(2013-03-24)](https://github.com/nodegit/nodegit/tree/v0.0.77)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.76...v0.0.77)

## <a name="v0-0-76" href="#v0-0-76">v0.0.76</a> [(2013-03-24)](https://github.com/nodegit/nodegit/tree/v0.0.76)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.75...v0.0.76)

## <a name="v0-0-75" href="#v0-0-75">v0.0.75</a> [(2013-03-24)](https://github.com/nodegit/nodegit/tree/v0.0.75)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.74...v0.0.75)

**Closed issues:**

- incomplete error reporting [\#57](https://github.com/nodegit/nodegit/issues/57)

- Segmentation Fault in raw-commit.js [\#56](https://github.com/nodegit/nodegit/issues/56)

- Another Mac OSX install fail [\#53](https://github.com/nodegit/nodegit/issues/53)

- unit tests broken in travis-ci [\#52](https://github.com/nodegit/nodegit/issues/52)

- "Image not found" with require\("nodegit"\) on Mac OS X [\#51](https://github.com/nodegit/nodegit/issues/51)

- Cannot Compile on 0.8.\* [\#47](https://github.com/nodegit/nodegit/issues/47)

- No suitable image found. [\#46](https://github.com/nodegit/nodegit/issues/46)

- Fails to require module on latest node version [\#43](https://github.com/nodegit/nodegit/issues/43)

- Compilation error node 0.6.1 [\#32](https://github.com/nodegit/nodegit/issues/32)

- commit.history work like slice [\#17](https://github.com/nodegit/nodegit/issues/17)

- Sync and Async methods [\#16](https://github.com/nodegit/nodegit/issues/16)

- Comment all code methods [\#1](https://github.com/nodegit/nodegit/issues/1)

## <a name="v0-0-74" href="#v0-0-74">v0.0.74</a> [(2013-03-21)](https://github.com/nodegit/nodegit/tree/v0.0.74)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.73...v0.0.74)

## <a name="v0-0-73" href="#v0-0-73">v0.0.73</a> [(2013-03-21)](https://github.com/nodegit/nodegit/tree/v0.0.73)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.72...v0.0.73)

**Closed issues:**

- pass through python flag to node-gyp [\#54](https://github.com/nodegit/nodegit/issues/54)

- update package.json [\#28](https://github.com/nodegit/nodegit/issues/28)

- Rewrite Notes [\#27](https://github.com/nodegit/nodegit/issues/27)

- Tree each method is synchronous [\#15](https://github.com/nodegit/nodegit/issues/15)

## <a name="v0-0-72" href="#v0-0-72">v0.0.72</a> [(2013-03-06)](https://github.com/nodegit/nodegit/tree/v0.0.72)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.71...v0.0.72)

## <a name="v0-0-71" href="#v0-0-71">v0.0.71</a> [(2013-03-06)](https://github.com/nodegit/nodegit/tree/v0.0.71)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.6...v0.0.71)

**Closed issues:**

- Unable to load shared library [\#39](https://github.com/nodegit/nodegit/issues/39)

- Expand Convenience Unit Tests [\#38](https://github.com/nodegit/nodegit/issues/38)

- repo has no method 'branch' [\#35](https://github.com/nodegit/nodegit/issues/35)

- update for node 0.5.9 [\#29](https://github.com/nodegit/nodegit/issues/29)

**Merged pull requests:**

- Converted from eio\_custom to uv\_queue\_work [\#48](https://github.com/nodegit/nodegit/pull/48) ([faceleg](https://github.com/faceleg))

- Fix Load-Order Bug [\#44](https://github.com/nodegit/nodegit/pull/44) ([fatlotus](https://github.com/fatlotus))

- Update documented commands needed to run tests [\#42](https://github.com/nodegit/nodegit/pull/42) ([shama](https://github.com/shama))

- Fix typo in README.md [\#41](https://github.com/nodegit/nodegit/pull/41) ([Skomski](https://github.com/Skomski))

- Issue 35: repo has no method 'branch' [\#40](https://github.com/nodegit/nodegit/pull/40) ([cholin](https://github.com/cholin))

- Refactor [\#37](https://github.com/nodegit/nodegit/pull/37) ([mmalecki](https://github.com/mmalecki))

## <a name="v0-0-6" href="#v0-0-6">v0.0.6</a> [(2011-12-19)](https://github.com/nodegit/nodegit/tree/v0.0.6)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.4...v0.0.6)

**Closed issues:**

- commit event with undefined commit [\#26](https://github.com/nodegit/nodegit/issues/26)

- Convenience methods are not convenience!  [\#24](https://github.com/nodegit/nodegit/issues/24)

**Merged pull requests:**

- Node 0.6x fixes [\#34](https://github.com/nodegit/nodegit/pull/34) ([moneal](https://github.com/moneal))

## <a name="v0-0-4" href="#v0-0-4">v0.0.4</a> [(2011-05-14)](https://github.com/nodegit/nodegit/tree/v0.0.4)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.3...v0.0.4)

**Closed issues:**

- repo.branch fails on empty repo [\#22](https://github.com/nodegit/nodegit/issues/22)

- example/convenience-repo.js errors [\#21](https://github.com/nodegit/nodegit/issues/21)

- Branch history each method is asynchronous [\#11](https://github.com/nodegit/nodegit/issues/11)

## <a name="v0-0-3" href="#v0-0-3">v0.0.3</a> [(2011-04-13)](https://github.com/nodegit/nodegit/tree/v0.0.3)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.2...v0.0.3)

**Closed issues:**

- error handling [\#18](https://github.com/nodegit/nodegit/issues/18)

- Windows link issue [\#12](https://github.com/nodegit/nodegit/issues/12)

## <a name="v0-0-2" href="#v0-0-2">v0.0.2</a> [(2011-03-14)](https://github.com/nodegit/nodegit/tree/v0.0.2)

[Full Changelog](https://github.com/nodegit/nodegit/compare/v0.0.1...v0.0.2)

## <a name="v0-0-1" href="#v0-0-1">v0.0.1</a> [(2011-03-10)](https://github.com/nodegit/nodegit/tree/v0.0.1)



\* *This Change Log was automatically generated by [github_changelog_generator](https://github.com/skywinder/Github-Changelog-Generator)*
