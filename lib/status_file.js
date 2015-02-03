var NodeGit = require("../");

var Status = NodeGit.Status;

var StatusFile = function(path, status) {

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
    statusBit: status,
    statuses: getStatus()
  };

  return {
    status: function() {
      return data.statuses;
    },
    statusBit: function() {
      return data.statusBit;
    },
    path: function() {
      return data.path;
    },
    isNew: function() {
      return status & codes.WT_NEW ||
             status & codes.INDEX_NEW;
    },
    isModified: function() {
      return status & codes.WT_MODIFIED ||
             status & codes.INDEX_MODIFIED;
    },
    isDeleted: function() {
      return status & codes.WT_DELETED ||
             status & codes.INDEX_DELETED;
    },
    isTypechange: function() {
      return status & codes.WT_TYPECHANGE ||
             status & codes.INDEX_TYPECHANGE;
    },
    isRenamed: function() {
      return status & codes.WT_RENAMED ||
             status & codes.INDEX_RENAMED;
    },
    isIgnored: function() {
      return data.statusBit & codes.IGNORED;
    }
  };
};

module.exports = StatusFile;
