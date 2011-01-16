var path = require('path')

var Repo = require('libgit2').Repo

var repo_path = path.join(__dirname, '.git')
var repo = new Repo(repo_path)
repo.refs(function(ref, head) {
  repo.walk(head, function(commit) {
    console.log('commit ' + commit.id)
    console.log('Author: ' + commit.author.name + ' <' + commit.author.email + '>')
    //console.log('Date: ' + commit.author.time + ' ' + commit.author.offset)
    console.log()
    console.log('    '+ commit.message_short)
    console.log()
  })
});
