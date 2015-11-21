#!/usr/bin/env bash


# delete "nodegit.github.com" folder if it exists
rm -rf "nodegit.github.com" || exit 0;

# clone "nodegit.github.com" repository
git clone "https://github.com/nodegit/nodegit.github.com.git"

# change into "nodegit.github.com" folder
cd "nodegit.github.com"

# install node dependencies
npm install

# link "nodegit" folder
ln -s ../.. generate/nodegit

# generate new docs
node generate

# configure git user information
git config user.name "Travis CI"
git config user.email "noreply@travis-ci.org"

# commit changes
git add .
git commit -m "Deploy to GitHub Pages

see https://github.com/nodegit/nodegit.github.com/commit/${TRAVIS_COMMIT}"
git tag "${TRAVIS_COMMIT}"

# push to the "nodegit.github.com" repository
git push --quiet "https://${GH_TOKEN}@github.com/nodegit/nodegit.github.com.git" master "${TRAVIS_COMMIT}" > /dev/null 2>&1
