---
layout: full
menu_item: guides
title: NW.js
description: How to install NodeGit with NW.js
---

[Return to all guides](../../)

* * *

Install for nwjs
----------------

Please see the official nw.js docs [here](http://docs.nwjs.io/en/latest/For%20Users/Advanced/Use%20Native%20Node%20Modules/#node-pre-gyp)

For a slightly simpler version of the third method, use an `.npmrc` file. For example if you have an NW.js app that's targeting version 0.13.0, your `.npmrc` file would look something like:
```
runtime = node-webkit
target = 0.13.0
target_arch = x64
disturl = http://node-webkit.s3.amazonaws.com
```

*NOTE: NW.js support is not thoroughly tested. Additionally, there are no pre-built binaries for NW.js, you must compile NodeGit. Visit our [building guides](../from-source) for help*
