// Locates a directory containing the host's libcrypto/libssl.
//
// On macOS the addon is linked with `-undefined dynamic_lookup`, so libgit2's and
// libssh2's OpenSSL references (EVP_*, BN_*, EC_*, ...) are normally satisfied by
// the symbols the *host* process exports. Node links OpenSSL into its executable
// and re-exports them, but other N-API hosts do not, which makes `dlopen` fail:
//
//   symbol not found in flat namespace (_EVP_aes_128_cbc)   (Bun)
//
// Linking a real libcrypto/libssl binds those symbols to a concrete library so the
// addon loads in every N-API host (Node, Bun, Deno).
//
// Prints the library directory, or nothing when no OpenSSL installation is found.
// Set NODEGIT_OPENSSL_LIB_DIR to override the lookup.

const fs = require("fs");
const path = require("path");
const { execFileSync } = require("child_process");

function tryExec(command, args) {
  try {
    return execFileSync(command, args, {
      encoding: "utf8",
      stdio: ["ignore", "pipe", "ignore"],
    }).trim();
  } catch (error) {
    return "";
  }
}

function containsLibCrypto(libDir) {
  if (!libDir) {
    return false;
  }

  return [
    "libcrypto.dylib",
    "libcrypto.a",
    "libcrypto.so",
  ].some((name) => fs.existsSync(path.join(libDir, name)));
}

function candidateLibDirs() {
  const libDirs = [];

  if (process.env.NODEGIT_OPENSSL_LIB_DIR) {
    libDirs.push(process.env.NODEGIT_OPENSSL_LIB_DIR);
  }

  // Prefer Homebrew's stable `opt` path over `pkg-config`, which points at the
  // versioned Cellar directory and would be embedded as the dylib install name.
  const brewPrefix = tryExec("brew", ["--prefix", "openssl@3"]);
  if (brewPrefix) {
    libDirs.push(path.join(brewPrefix, "lib"));
  }

  for (const token of tryExec("pkg-config", ["--libs-only-L", "openssl"]).split(/\s+/)) {
    if (token.startsWith("-L")) {
      libDirs.push(token.slice(2));
    }
  }

  // Common Homebrew / MacPorts / system locations, covering both architectures.
  libDirs.push(
    "/usr/local/opt/openssl@3/lib",
    "/opt/homebrew/opt/openssl@3/lib",
    "/usr/local/opt/openssl/lib",
    "/opt/homebrew/opt/openssl/lib",
    "/opt/local/lib",
    "/usr/local/lib",
    "/opt/homebrew/lib"
  );

  return libDirs;
}

const libDir = candidateLibDirs().find(containsLibCrypto);

if (!libDir && process.platform === "darwin") {
  console.error(
    "[nodegit] No OpenSSL installation found. The addon will rely on the host " +
      "process exporting OpenSSL symbols, which works under Node but fails under " +
      "Bun and Deno. Install one (e.g. `brew install openssl@3`) or set " +
      "NODEGIT_OPENSSL_LIB_DIR."
  );
}

process.stdout.write(libDir || "");
