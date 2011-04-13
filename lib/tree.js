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

  // Synchronous walk
  self.walk = function( callback ) {
    if( !callback ) { return; }

    var entry
      , i;

    for( i=0, len=self.length; i<len; i++ ) {
      entry = git.entry( self.repo );

      self.tree.entryByIndex( entry.entry, i );

      if( callback.apply( entry, [ i, entry ] ) === false ) {
        break;
      }
    }
  };

  //self.walk = function( callback ) {
  //  if( !callback ) { return; }

  //  var entry
  //    , i
  //    , entries
  //    , event = new events.EventEmitter();

  //  for( i=0, len=self.length; i<len; i++ ) {
  //    entry = git.entry();

  //    self.tree.entryByIndex( entry.entry, i );
  //    event.emit( 'entry', [ err, i, entry ] );

  //    entries.push( entry );
  //  }

  //  event.emit( 'end', entries );

  //  return event;
  //};

  self.entry = function( name ) {
    var entry = git.entry( self.repo );

    self.tree.entryByName( entry.entry, name );

    return entry;
  };

  return self;
};

exports.tree = _Tree;
