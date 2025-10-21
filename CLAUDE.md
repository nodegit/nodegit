# NodeGit Development Guide

## Overview

This is [@readme/nodegit](https://www.npmjs.com/package/@readme/nodegit), a fork of the original NodeGit library that provides Node.js bindings to libgit2. This fork is maintained by Readme and includes compatibility updates for Node.js 20 and 22.

## Quick Start

### Prerequisites
- Node.js >= 20 (supports Node 20 and 22)
- Git installed on your system
- Build tools (automatically handled during install)

### Installation
```bash
npm install
```

The install process automatically:
1. Runs preinstall scripts
2. Builds native bindings using node-gyp
3. Runs postinstall scripts

## Running Tests

### Full Test Suite
```bash
npm test
```
This runs linting followed by the complete test suite.

### Tests Only (Skip Linting)
```bash
npm run mocha
```

### Debug Tests
```bash
npm run mochaDebug
```
Runs tests with inspector for debugging.

### Run Specific Tests
```bash
# Run tests matching a pattern
npm run mocha -- --grep "pattern"

# Example: Run only commit tests  
npm run mocha -- --grep "commit"

# Example: Run a specific test
npm run mocha -- --grep "can amend commit"
```

### Linting Only
```bash
npm run lint
```

## Development Commands

### Building
```bash
# Full rebuild (includes code generation)
npm run rebuild

# Debug build
npm run rebuildDebug

# Recompile only (skip code generation)
npm run recompile
```

### Code Generation
```bash
# Generate missing tests
npm run generateMissingTests

# Generate native code bindings
npm run generateNativeCode

# Generate JSON API definitions
npm run generateJson
```

## Test Structure

Tests are located in:
- `test/tests/` - Main test files
- `test/utils/` - Test utilities
- `test/repos/` - Test repositories

### Common Test Issues

**macOS Version Compatibility**: Tests may fail when upgrading macOS versions due to differences in Git behavior, file system precision, or system libraries. Hardcoded expected commit IDs are particularly sensitive to environment changes.

**Memory Management**: Tests use `--expose-gc` flag to test garbage collection behavior with native bindings.

**SSH Tests**: Some tests require SSH keys located in `test/id_rsa*` files.

## CI/CD

GitHub Actions workflows:
- **tests.yml**: Runs tests on Ubuntu 22.04 and macOS-26
- **publish.yml**: Handles package publishing

## Architecture

This library provides JavaScript bindings to the libgit2 C library:
- `src/` - C++ binding code
- `lib/` - Generated JavaScript APIs  
- `generate/` - Code generation scripts
- `include/` - C++ headers
- `vendor/` - Vendored dependencies (libgit2)

## Troubleshooting

### Build Issues
```bash
# Clean rebuild
rm -rf build node_modules
npm install
```

### Test Failures
- Check that Git is properly configured:
  ```bash
  git config --global user.name "Test User"
  git config --global user.email "test@example.com"
  ```
- Ensure SSH agent is running for SSH tests

### Platform-Specific Issues
- **Linux**: May require libssl-dev, libkrb5-dev, and pcre development packages
- **macOS**: Uses clang compiler, may require Xcode command line tools
- **Windows**: Requires Visual Studio build tools

## Contributing

1. Run tests locally: `npm test`
2. Ensure linting passes: `npm run lint`  
3. Test on target Node versions (20, 22)
4. Update tests if adding new functionality

For more detailed API documentation, visit [nodegit.org](http://www.nodegit.org/).