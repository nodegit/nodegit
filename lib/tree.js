var git = require( '../' )
  , events = require( 'events' );

var _Tree = function( obj, tree ) {
  var self = {};
  if ( obj instanceof git.raw.Repo && tree instanceof git.raw.Tree ) {
    self.repo = obj;
    self.tree = tree;
  }
  else if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.tree = new git.raw.Tree( tree );
  }
  else if ( obj instanceof git.raw.Tree ) {
    self.tree = obj;
  }
  else {
    self.tree = new git.raw.Tree();
  }

  Object.defineProperty( self, 'length', {
    get: function() {
      return self.tree.entryCount();
    },
    enumerable: true
  });

  self.walk = function( repo ) {
    var entry
      , i
      , len = self.length
      , repo = repo || self.repo
      , event = new events.EventEmitter();

    function next(i) {
      var dir;
      var tree;

      entry = git.entry( repo );

      self.tree.entryByIndex(entry.entry, i, function() {
        if(entry.isFile()) {
          event.emit( 'entry', i, entry );
        }
        else {
          dir = entry.name;
          tree = entry.tree();

          !tree.error && tree.walk( repo ).on( 'entry', function( i, entry ) {
            entry.dir += dir + '/';
            event.emit( 'entry', i, entry );
          });
        }

        if( i<len-1 ) {
          next(i=i+1);
        }
        else {
          event.emit( 'end' );
        }
      });
    }

    next(0);

    return event;
  };

  self.entry = function( name, callback ) {
    if( !callback ) { return; }

    var entry = git.entry( self.repo );

    var path = name.split( '/' );
    if( path.length === 1 ) {
      self.tree.entryByName( entry.entry, path[0], function( valid ) {
        callback( valid ? entry : undefined );
      });
    }
    else {
      function recurse( tree ) {
        var name = path.shift();
        var tree = tree || self.tree;

        tree.entryByName( entry.entry, name, function( valid ) {
          if( !path.length ) {
            callback( valid ? entry : undefined );
          }
          else {
            if (valid) {
              recurse( entry.tree().tree );
            }
            else {
              callback(undefined);
            }
          }
        });
      }

      recurse();
    }
  };

  return self;
};

exports.tree = _Tree;
