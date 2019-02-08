const fs = require("fs")
const JSON5 = require("json5");
const path = require("path");

if (process.argv.length < 3) {
  process.exit(1);
}

const last = arr => arr[arr.length - 1];
const sep = process.platform === "win32" ? "\\\\" : "/";
const [, , nodeRootDir] = process.argv;

let isElectron = last(nodeRootDir.split(sep)).startsWith("iojs");

if (!isElectron) {
  try {
    // Not ideal, would love it if there were a full featured gyp package to do this operation instead.
    const { variables: { built_with_electron } } = JSON5.parse(
      fs.readFileSync(
        path.resolve(nodeRootDir, "include", "node", "config.gypi"),
        "utf8"
      )
    );

    if (built_with_electron) {
      isElectron = true;
    }
  } catch (e) {}
}

process.stdout.write(isElectron ? "1" : "0");
