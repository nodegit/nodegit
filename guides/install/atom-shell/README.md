---
layout: full
menu_item: guides
title: Atom Shell
description: How to install NodeGit with Atom Shell
---

[Return to install guides](../)

* * *

Install for atom-shell
----------------------

For an application that is built using [atom-shell](https://github.com/atom/atom-shell) you'll need to have the `engines.atom-shell` set to the version of atom-shell that you are targeting in your root `package.json` file.

For example if you have an atom-shell app that's targeting version 0.20.7 your package.json file would look something like

```json
{
  "name": "as-app",
  "displayName": "My Atom Shell app",
  "version": "0.0.1",
  "description": "",
  "main": "main.js",
  "author": "",
  "engines": {
    "atom-shell": "0.20.7"
  }
}
```

And when doing an `npm install` or `apm install` inside of your atom-shell's root folder it will compile NodeGit targeting that version of atom-shell during the install.

*NOTE: there are no pre-built binaries for atom-shell, you must rebuild NodeGit. Visit our [building guides](../from-source) for help*
