---
layout: full
menu_item: guides
title: Electron
description: How to install NodeGit with Electron
---

[Return to all guides](../../)

* * *

Install for Electron
----------------------

Please see the official electron docs [here](https://github.com/electron/electron/blob/master/docs/tutorial/using-native-node-modules.md)

For a slightly simpler version of the first method, use an `.npmrc` file. For example if you have an Electron app that's targeting version 1.2.8 your .npmrc file would look something like:
```
runtime = electron
target = 1.2.8
target_arch = x64
disturl = https://atom.io/download/atom-shell
```

*NOTE: We try to provide prebuilt binaries for electron, but we don't always have them available for every version. If prebuilts are not available and you have trouble with local compilation, visit our [building guides](../from-source) for help*
