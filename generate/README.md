## /generate

The scripts and templates in this dir help generate the source code and tests for NodeGit. The major components of generate are:

  1. Input
  2. Scripts
  3. Templates

### Input

  All the **configuration** required for the parser to generate the source code and tests. For more details, check the Input directory.

### Scripts

  The scripts that generate the final configuration (*snapshot of the library*) `idefs.json`, `missing-tests.json`. These configurations are then used to generate `src` for the library.

### Templates

  All the Combyne templates are placed here. The filters, partials, templates all help NodeGit generate the source code.

  > For more information on Combyne: [tbranyen/combyne](https://github.com/tbranyen/combyne)
