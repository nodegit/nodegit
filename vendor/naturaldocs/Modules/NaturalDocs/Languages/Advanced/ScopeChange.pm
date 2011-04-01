###############################################################################
#
#   Class: NaturalDocs::Languages::Advanced::ScopeChange
#
###############################################################################
#
#   A class used to store a scope change.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Advanced::ScopeChange;

#
#   Constants: Implementation
#
#   The object is implemented as a blessed arrayref.  The constants below are used as indexes.
#
#   SCOPE - The new scope <SymbolString>.
#   LINE_NUMBER - The line number of the change.
#
use NaturalDocs::DefineMembers 'SCOPE', 'LINE_NUMBER';
# Dependency: New() depends on the order of these constants as well as that there is no inherited members.


#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       scope - The <SymbolString> the scope was changed to.
#       lineNumber - What line it occurred on.
#
sub New #(scope, lineNumber)
    {
    # Dependency: This depends on the order of the parameters matching the constants, and that there are no inherited
    # members.
    my $self = shift;

    my $object = [ @_ ];
    bless $object, $self;

    return $object;
    };


# Function: Scope
# Returns the <SymbolString> the scope was changed to.
sub Scope
    {  return $_[0]->[SCOPE];  };

# Function: SetScope
# Replaces the <SymbolString> the scope was changed to.
sub SetScope #(scope)
    {  $_[0]->[SCOPE] = $_[1];  };

# Function: LineNumber
# Returns the line number of the change.
sub LineNumber
    {  return $_[0]->[LINE_NUMBER];  };


1;
