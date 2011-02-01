var path = require('path')

var Repo = require('libgit2').Repo

var repo_path = path.join(__dirname, '.git')
var repo = new Repo(repo_path)
repo.refs('HEAD', function(head) {
  repo.walk(head, function(commit) {
    console.log('commit ' + commit.id)
    console.log('Author: ' + commit.author.name + ' <' + commit.author.email + '>')
    var d = new Date(commit.author.time * 1000)
    console.log('Date:   ' + d)
    console.log()
    console.log('    '+ commit.message_short)
    console.log()
  })
});
