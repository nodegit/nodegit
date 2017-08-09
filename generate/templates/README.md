# Manual templates

  Manual templates override generated code from nodegit while generating source code.

## Why?

If generated code does not accurately wrap the libgit2 calls, you might want to consider implementing manual templates in the following cases:

  #### 1. Convert Sync functions to Async
  > typically this can be done by adding `"isAsync": true` in **descriptor.json**, but if you wish to not expose all parameters, customize/wrap new arguments, manual templates are the faster way to achieve it.

  #### 2. Segfaults non-deterministically
  > If that is the case, use the persistence pattern as seen in x, y wrappers. Garbage collector is probably eating up data that was not persisted after the function scope ends.

<br />
-----
## Implementing manual templates

#### 1. Copy generated .cc and .h files to *generate/templates/manual/*
    *.cc files -> generate/templates/manual/src/
    *.h files -> generate/templates/manual/include/

#### 2. Remove all references from json configuration files (descriptor, libgit2-supplement)
    
#### 3. Add references to binding.gyp template
    location: `generate/templates/templates/binding.gyp`

#### 4. Add headers to nodegit.cc template
    location: `generate/templates/templates/nodegit.cc`

#### 5. Add new wrapper to nodegit.js template
    use rawApi.ManualWrapper reference to add _ManualWrapper
    add any js wrapper (if any) via importExtension

