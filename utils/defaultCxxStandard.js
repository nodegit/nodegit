const fs = require("fs");
const path = require("path");

const args = process.argv.slice(2);
const target = args[0];
const targetSpecified = target !== "none";

const nodeRootDir = args.find(isNodeHeadersRoot) ?? null;

let cxxStandard = "14";

if (targetSpecified) {
  // Assume electron if target is specified.
  // If building node 18 / 19 via target, will need to specify C++ standard manually
  const majorVersion = target.split(".")[0];
  if (Number.parseInt(majorVersion) >= 32) {
    cxxStandard = "20";
  } else if (Number.parseInt(majorVersion) >= 21) {
    cxxStandard = "17";
  }
} else {
  const abiVersion = Number.parseInt(process.versions.modules) ?? 0;
  // Node 18 === 108
  if (abiVersion >= 131) {
    cxxStandard = "20";
  } else if (abiVersion >= 108) {
    cxxStandard = "17";
  }
}

if (cxxStandard !== "20" && nodeHeadersRequireCxx20(nodeRootDir)) {
  cxxStandard = "20";
}

process.stdout.write(cxxStandard);

function isNodeHeadersRoot(candidate) {
  return nodeVersionHeaderPath(candidate) !== null;
}

function nodeVersionHeaderPath(nodeRootDir) {
  if (!nodeRootDir) {
    return null;
  }

  const versionHeader = path.join(nodeRootDir, "include", "node", "node_version.h");
  return fs.existsSync(versionHeader) ? versionHeader : null;
}

function nodeHeadersRequireCxx20(nodeRootDir) {
  const versionHeader = nodeVersionHeaderPath(nodeRootDir);
  if (!versionHeader) {
    // Headers not available: keep the node-version based answer.
    return false;
  }

  try {
    const memorySpan = path.join(nodeRootDir, "include", "node", "v8-memory-span.h");
    return fs.readFileSync(memorySpan, "utf8").includes("std::ranges::");
  } catch (error) {
    // No v8-memory-span.h, or it predates the ranges specializations.
    return false;
  }
}
