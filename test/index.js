#!/usr/bin/env node

require.paths.unshift('../vendor');

try {
    var reporter = require('../vendor/nodeunit').reporters.default;
}
catch(e) {
    var sys = require('sys');
    sys.puts("Cannot find nodeunit module.");
    sys.puts("You can download submodules for this project by doing:");
    sys.puts("");
    sys.puts("    git submodule init");
    sys.puts("    git submodule update");
    sys.puts("");
    process.exit();
}

process.chdir('./');
reporter.run(['test']);
