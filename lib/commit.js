var git = require( '../' )
  , events = require( 'events' );

var _Commit = function( obj ) {
  var self = { _cache: {} };

  //if( obj instanceof git.raw.Repo ) {
  //  self.commit = new git.raw.Commit( obj );
  //}
  //else if( obj instanceof git.raw.Commit ) {
  //  self.commit = obj;
  //}

  if( obj instanceof git.raw.Commit ) {
    self.commit = obj;
  }
  else {
    self.commit = new git.raw.Commit();
  }
 
  Object.defineProperty( self, 'id', {
    get: function() {
      var oid = new git.raw.Oid();
      self.commit.id( oid );

      return oid;
    },
    enumerable: true
  });

  Object.defineProperty( self, 'sha', {
    get: function() {
      var oid = new git.raw.Oid();
      self.commit.id( oid );

      return oid.toString( 40 );
    },
    enumerable: true
  });

  Object.defineProperty( self, 'msg', {
    get: function() {
      return self.commit.messageShort();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'message', {
    get: function() {
      return self.commit.message();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'time', {
    get: function() {
      return new Date( self.commit.time() * 1000 );
    },
    enumerable: true
  });

  Object.defineProperty( self, 'offset', {
    get: function() {
      return self.commit.timeOffset();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'author', {
    get: function() {
      var sig = new git.raw.Sig();
      
      self.commit.author( sig );

      return git.sig( sig );
    },
    enumerable: true
  });

  self.lookup = function( repo, oid, callback ) {
    self.repo = repo;
    self.commit.lookup( repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  self.tree = function() {
    var tree = new git.raw.Tree( self.repo );
    if( tree.error ) {
      return git.error( tree.error );
    }
    else {
      self.commit.tree( tree );
    }

    return git.tree( self.repo, tree );
  };

  self.file = function( path ) {
    return self.tree().entry( path );
  };

  self.history = function( start, end ) {
    var revwalk = git.revwalk( self.repo ),
        event = new events.EventEmitter(),
        commits = [];

    revwalk.walk( self.id, function( err, index, commit ) {
      if( err ) {
        event.emit( 'end', commits );
      }
      else {
        event.emit( 'commit', commit );
        commits.push( commit );
      }
    });

    return event;
  };


  return self;
};

exports.commit = _Commit;
