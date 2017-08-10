## /generate/input
  This folder contains the main config files to generate NodeGit

  #### Callbacks
  Add all meta data about the callbacks from libgit2 that need to be implemented in NodeGit

  #### Descriptor
  Customize the generated code using this configuration. Enter the function signature, arguments and their metadata and which functions can be skipped in this file. If you're using a manual template, remove all its references from this file.

  #### Libgit2-docs
  These are provided by the libgit2 team. Includes all metadata about the api provided by the libgit2 library.

  #### Libgit2-supplement
  Use this confiuration file to group and override parts of the generated code. NodeGit tries it's best to generate the right classes and structs, if that is not the case, use this config file to group/remove the functions.
  > If you're using manual templates, update the `cFile` reference to point to the manual template
