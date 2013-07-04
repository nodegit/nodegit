var fs = require('fs'),
    path = require('path');

var ds = new RegExp('\\*\\*');

var idefs = JSON.parse(fs.readFileSync('v0.18.0.json'));

var types2free = {};

var oidFns = "git_blob_id git_commit_id git_commit_tree_id git_commit_parent_id git_indexer_stream_hash git_note_oid git_object_id git_odb_object_id git_reflog_entry_id_old git_reflog_entry_id_new git_reference_target git_submodule_index_id git_submodule_head_id git_submodule_wd_id git_tag_id git_tag_target_id git_tree_entry_id git_tree_id".split(' ');

for (var i in idefs) {
  var idef = idefs[i];

  for (var f in idef.functions) {
    var fn = idef.functions[f];

    if (oidFns.indexOf(fn.cFunctionName) > -1) {
      fn.return.copy = "git_oid_dup";
    }
  }
}
console.log(JSON.stringify(idefs, null, 2));

return;

for (var i in idefs) {
  var idef = idefs[i];
  types2free[idef.cType] = idef.freeFunctionName;
}

for (var t in types2free) {
  var re = new RegExp(t);

  for (var i in idefs) {
    var idef = idefs[i];

    for (var f in idef.fields) {
      var fn = idef.functions[f];

      for (var a in fn.args) {
        var arg = fn.args[a];

        if (t == "git_oid" && arg.isReturn && re.test(arg.cType) && !ds.test(arg.cType)) {
          arg.shouldAlloc = true;
        }
      }
    }
  }
}
console.log(JSON.stringify(idefs, null, 2));

