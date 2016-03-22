NodeGit FAQ
-----------

Feel free to add common problems with their solutions here, or just anything that wasn't clear at first.

#### Error: callback returned unsupported credentials type ####

As seen in nodegit/#959 -- some golang hackers have started to use the following stanza in .gitconfig to allow `go get` to work with private repos:
```
[url "git@github.com:"]
        insteadOf = https://github.com/
```
But if you do this, code can call `NodeGit.Clone.clone(url: 'https://foo')` and have the `authentication` callback be asked for **SSH** credentials instead of HTTPS ones, which might not be what your application expected.
