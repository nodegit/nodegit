## /generate/input
  This folder contains the main config files to generate NodeGit's JavaScript functions.

  #### callbacks.json
  Add all meta data about the callbacks from libgit2 that need to be implemented in NodeGit.

  #### descriptor.json
  Customize the generated code using this configuration file. Enter the function's signature, arguments and their metadata and which functions can be skipped in this file. If you are using a manual template, remove all of its references from this file.

  #### libgit2-docs.json
  These are provided by the libgit2 team. It includes all the metadata about the API provided by the libgit2 library. To grab the latest version of this file, download https://libgit2.org/libgit2/HEAD.json.

  #### libgit2-supplement.json
  Use this confiuration file to group and override parts of the generated code. NodeGit tries its best to generate the right classes and structs, if it is not quite right, then use this config file to group/remove the functions.
  > If you're using manual templates, update the `cFile` reference to point to the manual template.
