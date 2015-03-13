---
layout: full
menu_item: guides
title: NW.js
description: How to install NodeGit with NW.js
---

[Return to install guides](../)

* * *

Install for nwjs
----------------

For an application that is built using [NW.js](https://github.com/nwjs/nw.js) you'll need to have the `engines['nw.js']` set to the version of NW.js that you are targeting in your root `package.json` file.

For example if you have an NS.js app that's targeting version 0.12.0 your package.json file would look something like

```json
{
  "name": "nw-app",
  "displayName": "My NW.js app",
  "version": "0.0.1",
  "description": "",
  "engines": {
    "nw.js": "0.12.0"
  }
}
```

And when doing an `npm install` inside of your NW.js app's root folder it will compile NodeGit targeting that version of NW.js during the install.

*NOTE: there are no pre-built binaries for NW.js, you must rebuild NodeGit. Visit our [building guides](../from-source) for help*
