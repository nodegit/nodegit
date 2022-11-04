const targetSpecified = process.argv[2] !== 'none';

let isNode18OrElectron20AndUp = false;
if (targetSpecified) {
  // Assume electron if target is specified.
  // If building node 18 / 19 via target, will need to specify C++ standard manually
  const majorVersion = process.argv[2].split('.')[0];
  isNode18OrElectron20AndUp = majorVersion >= 20;
} else {
  // Node 18 === 108
  isNode18OrElectron20AndUp = Number.parseInt(process.versions.modules) >= 108;
}

const defaultCxxStandard = isNode18OrElectron20AndUp
  ? '17'
  : '14';

process.stdout.write(defaultCxxStandard);
