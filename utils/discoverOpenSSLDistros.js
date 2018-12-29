const cheerio = require("cheerio");
const fse = require("fs-extra");
const path = require("path");
const R = require("ramda");
const request = require("request-promise-native");

const windowsCommonConditions = [
  R.test(/^\s*os=Windows$/gm),
  R.test(/^\s*shared=False$/gm)
];

const macCommonConditions = [
  R.test(/^\s*arch=x86_64$/gm),
  R.test(/^\s*os=Macos$/gm),
  R.test(/^\s*compiler=apple-clang$/gm),
  R.test(/^\s*shared=False$/gm)
];

const debugPairs = R.toPairs({
  "win32-vs12-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  "win32-vs14-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  "win32-vs15-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  "win64-vs12-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  "win64-vs14-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  "win64-vs15-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),
  
  "macOS-clang-9-static-debug": R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.version=9.0$/gm)
  ]),
  "macOS-clang-8.1-static-debug": R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.version=8\.1$/gm)
  ])
});

const releasePairs = R.toPairs({
  "win32-vs12-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  "win32-vs14-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  "win32-vs15-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  "win64-vs12-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  "win64-vs14-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  "win64-vs15-static-release": R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  "macOS-clang-9-static-release": R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.version=9.0$/gm)
  ]),
  "macOS-clang-8.1-static-release": R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.version=8\.1$/gm)
  ])
});

const distributionPairs = [...debugPairs, ...releasePairs];

const getDistributionConfigURLFromHash = itemHash =>
  `https://dl.bintray.com/conan-community/conan/conan/OpenSSL/1.1.0i/stable/package/${itemHash}/conaninfo.txt`;

const getDistributionDownloadURLFromHash = itemHash =>
  `https://dl.bintray.com/conan-community/conan/conan/OpenSSL/1.1.0i/stable/package/${itemHash}/conan_package.tgz`;

const getDistributionsRootURL = () =>
  "https://dl.bintray.com/conan-community/conan/conan/OpenSSL/1.1.0i/stable/package/";

const detectDistributionPairFromConfig = (itemHash, body) => R.pipe(
  R.find(([_, predicate]) => predicate(body)),
  (distributionPair) => distributionPair
    ? [distributionPair[0], getDistributionDownloadURLFromHash(itemHash)]
    : undefined
)(distributionPairs);

const getDistributionConfig = (itemHash) =>
  request.get(getDistributionConfigURLFromHash(itemHash))
    .then((body) => detectDistributionPairFromConfig(itemHash, body));

const discoverDistributions = (treeHtml) => {
  const releaseHashes = [];

  const $ = cheerio.load(treeHtml);
  $("a").each((_, link) => {
    const linkText = link.children[0].data;
    if (!linkText) {
      return;
    }
    // Trim off the trailing '/'
    const releaseHash = linkText.substring(0, linkText.length - 1);
    releaseHashes.push(releaseHash);
  });

  return Promise.all(
    R.map(releaseHash => getDistributionConfig(releaseHash), releaseHashes)
  );
}

const writeFile = (distributions) =>
  fse.ensureDir(path.dirname(outputPath))
    .then(fse.writeFile(outputPath, JSON.stringify(distributions, null, 2)));

const outputPath = path.resolve(__dirname, "..", "vendor", "static_config", "openssl_distributions.json");
request(getDistributionsRootURL())
  .then(discoverDistributions)
  .then(R.filter(R.identity))
  .then(R.sortBy(R.prop(0)))
  .then(R.fromPairs)
  .then(writeFile);
