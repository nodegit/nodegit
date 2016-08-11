var NodeGit = require("../");
var Status = NodeGit.Status;

var StatusFile = function(args) {
  var path = args.path;
  var status = args.status;
  var entry = args.entry;

  if (entry) {
    status = entry.status();
    if (entry.indexToWorkdir()) {
      path = entry.indexToWorkdir().newFile().path();
    } else {
      path = entry.headToIndex().newFile().path();
    }
  }

  var codes = Status.STATUS;

  var getStatus = function() {
    var fileStatuses = [];

    for(var key in Status.STATUS) {
      if (status & Status.STATUS[key]) {
        fileStatuses.push(key);
      }
    }

    return fileStatuses;
  };

  var data = {
    path: path,
    entry: entry,
    statusBit: status,
    statuses: getStatus()
  };

  return {
    headToIndex: function() {
      if (data.entry) {
        return entry.headToIndex();
      } else {
        return undefined;
      }
    },
    indexToWorkdir: function() {
      if (data.entry) {
        return entry.indexToWorkdir();
      } else {
        return undefined;
      }
    },
    inIndex: function() {
      return status & codes.INDEX_NEW ||
             status & codes.INDEX_MODIFIED ||
             status & codes.INDEX_DELETED ||
             status & codes.INDEX_TYPECHANGE ||
             status & codes.INDEX_RENAMED;
    },
    inWorkingTree: function() {
      return status & codes.WT_NEW ||
             status & codes.WT_MODIFIED ||
             status & codes.WT_DELETED ||
             status & codes.WT_TYPECHANGE ||
             status & codes.WT_RENAMED;
    },
    isConflicted: function() {
      return status & codes.CONFLICTED;
    },
    isDeleted: function() {
      return status & codes.WT_DELETED ||
             status & codes.INDEX_DELETED;
    },
    isIgnored: function() {
      return status & codes.IGNORED;
    },
    isModified: function() {
      return status & codes.WT_MODIFIED ||
             status & codes.INDEX_MODIFIED;
    },
    isNew: function() {
      return status & codes.WT_NEW ||
             status & codes.INDEX_NEW;
    },
    isRenamed: function() {
      return status & codes.WT_RENAMED ||
             status & codes.INDEX_RENAMED;
    },
    isTypechange: function() {
      return status & codes.WT_TYPECHANGE ||
             status & codes.INDEX_TYPECHANGE;
    },
    path: function() {
      return data.path;
    },
    status: function() {
      return data.statuses;
    },
    statusBit: function() {
      return data.statusBit;
    }
  };
};

NodeGit.StatusFile = StatusFile;
