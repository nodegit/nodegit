const targetSpecified = process.argv[2] !== 'none';

let cxxStandard = '14';

if (targetSpecified) {
  // Assume electron if target is specified.
  // If building node 18 / 19 via target, will need to specify C++ standard manually
  const majorVersion = process.argv[2].split('.')[0];
  if (Number.parseInt(majorVersion) >= 32) {
    cxxStandard = '20';
  } else if (Number.parseInt(majorVersion) >= 21) {
    cxxStandard = '17';
  }
} else {
  const abiVersion = Number.parseInt(process.versions.modules) ?? 0;
  // Node 18 === 108, Node 20 === 115
  if (abiVersion >= 131) {
    cxxStandard = '20';
  } else if (abiVersion >= 108) {
    cxxStandard = '17';
  }
}

process.stdout.write(cxxStandard);
