const cheerio = require('cheerio');
const fse = require('fs-extra');
const path = require('path');
const R = require('ramda');
const request = require('request-promise-native');

const windowsCommonConditions = [
  R.test(/^\s*os=Windows$/gm),
  R.test(/^\s*shared=False$/gm)
]

const macCommonConditions = [
  R.test(/^\s*arch=x86_64$/gm),
  R.test(/^\s*os=Macos$/gm),
  R.test(/^\s*compiler=apple-clang$/gm),
  R.test(/^\s*shared=False$/gm)
];

const debugPairs = R.toPairs({
  'win32-vs12-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  'win32-vs14-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  'win32-vs15-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  'win64-vs12-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  'win64-vs14-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  'win64-vs15-static-debug': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.runtime=MTd$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),
  
  'macOS-clang-9-static-debug': R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.version=9.0$/gm)
  ]),
  'macOS-clang-8.1-static-debug': R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Debug$/gm),
    R.test(/^\s*compiler\.version=8\.1$/gm)
  ])
})

const releasePairs = R.toPairs({
  'win32-vs12-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  'win32-vs14-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  'win32-vs15-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  'win64-vs12-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=12$/gm)
  ]),
  'win64-vs14-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=14$/gm)
  ]),
  'win64-vs15-static': R.allPass([
    ...windowsCommonConditions,
    R.test(/^\s*arch=x86_64$/gm),
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.runtime=MT$/gm),
    R.test(/^\s*compiler\.version=15$/gm)
  ]),

  'macOS-clang-9-static': R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.version=9.0$/gm)
  ]),
  'macOS-clang-8.1-static': R.allPass([
    ...macCommonConditions,
    R.test(/^\s*build_type=Release$/gm),
    R.test(/^\s*compiler\.version=8\.1$/gm)
  ])
});

const distributionPairs = [...debugPairs, ...releasePairs];

const getDistributionConfigURLFromHash = itemHash =>
  `https://bintray.com/conan-community/conan/download_file?file_path=conan%2FOpenSSL%2F1.1.0i%2Fstable%2Fpackage%2F${itemHash}%2Fconaninfo.txt`

const getDistributionDownloadURLFromHash = itemHash =>
  `https://dl.bintray.com/conan-community/conan/conan/OpenSSL/1.1.0i/stable/package/${itemHash}/conan_package.tgz`

const getDistributionsRootURL = () =>
  'https://bintray.com/package/files/conan-community/conan/OpenSSL%3Aconan?order=asc&sort=name&basePath=conan%2FOpenSSL%2F1.1.0i%2Fstable%2Fpackage&tab=files'

const detectDistributionPairFromConfig = (itemHash, body) => R.reduce(
  (acc, [releaseName, predicate]) => R.cond([
    [predicate, R.always([releaseName, getDistributionDownloadURLFromHash(itemHash)])],
    [R.T, R.always(acc)]
  ])(body),
  undefined,
  distributionPairs
);

const getDistributionConfig = (itemHash) =>
  request.get(getDistributionConfigURLFromHash(itemHash))
    .then((body) => detectDistributionPairFromConfig(itemHash, body));

const discoverDistributions = (treeHtml) => {
  const releaseHashes = [];

  const $ = cheerio.load(treeHtml);
  const links = $('#treeBrowseTable > tbody > tr > td.nameCol > a');
  links.each((_, link) => {
    const releaseHash = link.children[0].data;
    if (!releaseHash) {
      return;
    }
    releaseHashes.push(releaseHash);
  });

  return Promise.all(
    R.map(releaseHash => getDistributionConfig(releaseHash), releaseHashes)
  );
}

const outputPath = path.resolve(__dirname, '..', 'vendor', 'openssl_distributions.json');
request(getDistributionsRootURL())
  .then(discoverDistributions)
  .then(R.filter(R.identity))
  .then(R.sortBy(R.prop(0)))
  .then(R.fromPairs)
  .then(distributions => fse.writeFile(outputPath, JSON.stringify(distributions, null, 2)));
