const fse = require("fs-extra");
const path = require("path");
const R = require("ramda");
const got = require("got");

const windowsCommonConditions = [
  R.pathEq(['settings', 'os'], 'Windows'),
  R.pathEq(['options', 'shared'], 'False')
];

const macCommonConditions = [
  R.pathEq(['settings', 'arch'], 'x86_64'),
  R.pathEq(['settings', 'os'], 'Macos'),
  R.pathEq(['settings', 'compiler'], 'apple-clang'),
  R.pathEq(['options', 'shared'], 'False')
];

const debugPairs = R.toPairs({
  "win32-vs14-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86'),
    R.pathEq(['settings', 'build_type'], 'Debug'),
    R.pathEq(['settings', 'compiler.runtime'], 'MTd'),
    R.pathEq(['settings', 'compiler.version'], '14')
  ]),
  "win32-vs15-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86'),
    R.pathEq(['settings', 'build_type'], 'Debug'),
    R.pathEq(['settings', 'compiler.runtime'], 'MTd'),
    R.pathEq(['settings', 'compiler.version'], '15')
  ]),

  "win64-vs14-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86_64'),
    R.pathEq(['settings', 'build_type'], 'Debug'),
    R.pathEq(['settings', 'compiler.runtime'], 'MTd'),
    R.pathEq(['settings', 'compiler.version'], '14')
  ]),
  "win64-vs15-static-debug": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86_64'),
    R.pathEq(['settings', 'build_type'], 'Debug'),
    R.pathEq(['settings', 'compiler.runtime'], 'MTd'),
    R.pathEq(['settings', 'compiler.version'], '15')
  ]),

  "macOS-clang-9.1-static-debug": R.allPass([
    ...macCommonConditions,
    R.pathEq(['settings', 'build_type'], 'Debug'),
    R.pathEq(['settings', 'compiler.version'], '9.1')
  ])
});

const releasePairs = R.toPairs({
  "win32-vs14-static-release": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86'),
    R.pathEq(['settings', 'build_type'], 'Release'),
    R.pathEq(['settings', 'compiler.runtime'], 'MT'),
    R.pathEq(['settings', 'compiler.version'], '14')
  ]),
  "win32-vs15-static-release": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86'),
    R.pathEq(['settings', 'build_type'], 'Release'),
    R.pathEq(['settings', 'compiler.runtime'], 'MT'),
    R.pathEq(['settings', 'compiler.version'], '15')
  ]),

  "win64-vs14-static-release": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86_64'),
    R.pathEq(['settings', 'build_type'], 'Release'),
    R.pathEq(['settings', 'compiler.runtime'], 'MT'),
    R.pathEq(['settings', 'compiler.version'], '14')
  ]),
  "win64-vs15-static-release": R.allPass([
    ...windowsCommonConditions,
    R.pathEq(['settings', 'arch'], 'x86_64'),
    R.pathEq(['settings', 'build_type'], 'Release'),
    R.pathEq(['settings', 'compiler.runtime'], 'MT'),
    R.pathEq(['settings', 'compiler.version'], '15')
  ]),

  "macOS-clang-9.1-static-release": R.allPass([
    ...macCommonConditions,
    R.pathEq(['settings', 'build_type'], 'Release'),
    R.pathEq(['settings', 'compiler.version'], '9.1')
  ])
});

const distributionPairs = [...debugPairs, ...releasePairs];

const getDistributionsSearchURL = () =>
  "https://center.conan.io/v1/conans/openssl/1.1.1c/_/_/search";

const getDistributionDownloadURLMetadataFromHash = itemHash =>
  `https://center.conan.io/v1/conans/openssl/1.1.1c/_/_/packages/${itemHash}/download_urls`;

const detectDistributionPairFromConfig = (itemMetadata, body) => R.pipe(
  R.find(([_, predicate]) => predicate(body)),
  (distributionPair) => distributionPair
    ? [distributionPair[0], itemMetadata]
    : undefined
)(distributionPairs);

const getDistributionConfig = (itemHash, searchJson) => detectDistributionPairFromConfig(itemHash, searchJson[itemHash]);

const discoverDistributions = async (searchJson) => {
  const releaseHashes = Object.keys(searchJson);
  console.log(`Received info for ${releaseHashes.length} distributions`);

  return R.map(releaseHash => getDistributionConfig(releaseHash, searchJson), releaseHashes);
}

const outputPath = path.resolve(__dirname, "..", "vendor", "static_config", "openssl_distributions.json");
const writeFile = (distributions) =>
  fse.ensureDir(path.dirname(outputPath))
    .then(fse.writeFile(outputPath, JSON.stringify(distributions, null, 2)));

got(getDistributionsSearchURL())
  .then(({ body }) => discoverDistributions(JSON.parse(body)))
  .then(R.filter(R.identity))
  .then(R.sortBy(R.prop(0)))
  .then(async (pairs) => {
    console.log(`Fetching metadata for ${pairs.length} distributions`);
    const newPairs = [];
    for (let pair of pairs) {
      const itemHash = pair[1];
      const { body: itemMetadata } = await got(getDistributionDownloadURLMetadataFromHash(itemHash));
      console.log('Received metadata for ', itemHash);
      const url = JSON.parse(itemMetadata)['conan_package.tgz'];
      newPairs.push([pair[0], url]);
    }

    return newPairs;
  })
  .then(R.fromPairs)
  .then(writeFile)
  .then(() => console.log('Done.'));
