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

For an application that is built using [Electron](https://github.com/atom/Electron) you'll need to have the `engines.electron` set to the version of Electron that you are targeting in your root `package.json` file.

For example if you have an Electron app that's targeting version 0.20.7 your package.json file would look something like:

```json
{
  "name": "elec-app",
  "displayName": "My Electron app",
  "version": "0.0.1",
  "description": "",
  "main": "main.js",
  "author": "",
  "engines": {
    "electron": "0.20.7"
  }
}
```

And when doing an `npm install` or `apm install` inside of your Electron's root folder it will compile NodeGit targeting that version of Electron during the install.

*NOTE: there are no pre-built binaries for Electron, you must rebuild NodeGit. Visit our [building guides](../from-source) for help*
