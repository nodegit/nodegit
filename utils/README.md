## /utils

  Contains utilities for NodeGit

  #### buildFlags
  Determines how NodeGit should build. Use `BUILD_ONLY` environment variable to build from source.

  ## discoverOpenSSLDistros
  Crawls a series of static URLS on the [Conan package manager](https://conan.io/) for the [latest release of OpenSSL](https://bintray.com/conan-community/conan/OpenSSL%3Aconan#files/conan%2FOpenSSL%2F1.1.0i) (1.1.0i at the time of writing). It acquires URLS for releases of statically linked binaries and header files of OpenSSL for Mac and Windows. The provided binaries are compiled on:
  
  * Mac: clang-8.1 or clang-9.
  * Windows: vs12, vs14, vs15

  The discovered distributions are written into `vendor/static_config/openssl_distributions.json`. This script does not need to be run unless you are updating the version of OpenSSL to build against.

  ## acquireOpenSSL
  Download the OpenSSL binaries and headers applicable to the current OS for the latest compiler version (clang-9/vs14). Uses links from `vendor/static_config/openssl_distributions.json`.
  
  TODO:
  * Make the script pull the debug versions if node-gyp is building in debug mode
  * Make the script pull down a version of the binaries that matches the system compiler
