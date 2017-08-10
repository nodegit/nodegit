# Manual templates

  Manual templates override generated code from nodegit while generating source code. They really should be avoid untill absolutely necessary.

## Why?

If generated code does not accurately wrap the libgit2 calls, you might want to consider implementing manual templates in the following cases:

  #### 1. Performance
  > Everytime the library switches between the C land and the JS queue thread, there is a penalty in performance. If the generated code switches frequently, it might be better option to use manual templates.

  #### 2. Saftey
  > The generated code sometimes does not handle structures that are inter-dependant. Perfect example would be convenient_hunks. Hunks references a file pointer and diff lines that are dependant on it. If persisted, it would lock the file. If garbage collected, the diff lines would cause seg fault errors. Anytime a custom solution is required, that would be hard for generated code to implement, manual templates should be used.

  #### 3. Odd cases
  > If a new pattern exists in libgit that would be difficult to implement using generated code, manual templates can be used for one-off cases. Typically generated code takes care of most patterns seen in libgit, but if function signatures do not follow typical pattern, manual templates could be used. Example: git_filter.

<br />
-----
## Implementing manual templates

#### 1. Copy generated .cc and .h files to *generate/templates/manual/*
    *.cc files -> /generate/templates/manual/src/
    *.h files -> /generate/templates/manual/include/

#### 2. Remove all references from /generate configuration files
    
#### 3. Add references to binding.gyp template
    location: /generate/templates/templates/binding.gyp

#### 4. Add headers to nodegit.cc template
    location: /generate/templates/templates/nodegit.cc

#### 5. Add new wrapper to nodegit.js template
    use rawApi.ManualWrapper reference to add _ManualWrapper
    add any js wrapper (if any) via importExtension

