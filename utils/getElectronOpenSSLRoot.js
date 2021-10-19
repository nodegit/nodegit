const path = require("path");

if (process.argv.length < 3) {
  process.exit(1);
}

const [, , moduleRootDir] = process.argv;

const openSSLRoot = process.env.npm_config_openssl_dir || path.join(moduleRootDir, 'vendor', 'openssl');
process.stdout.write(openSSLRoot);
