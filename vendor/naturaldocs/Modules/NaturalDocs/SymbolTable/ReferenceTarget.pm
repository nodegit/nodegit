###############################################################################
#
#   Class: NaturalDocs::SymbolTable::ReferenceTarget
#
###############################################################################
#
#   A class for storing information about a reference target.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::SymbolTable::ReferenceTarget;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are its members.
#
#       SYMBOL  - The target <SymbolString>.
#       FILE        - The <FileName> the target is defined in.
#       TYPE       - The target <TopicType>.
#       PROTOTYPE - The target's prototype, or undef if none.
#       SUMMARY    - The target's summary, or undef if none.
#

# DEPENDENCY: New() depends on the order of these constants.  If they change, New() has to be updated.
use constant SYMBOL => 0;
use constant FILE => 1;
use constant TYPE => 2;
use constant PROTOTYPE => 3;
use constant SUMMARY => 4;

###############################################################################
# Group: Functions


#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       symbol - The target <SymbolString>.
#       file       - The <FileName> the target is defined in.
#       type     - The <TopicType> of the target symbol.
#       prototype - The target's prototype.  Set to undef if not defined or not applicable.
#       summary - The target's summary.  Set to undef if not defined or not applicable.
#
sub New #(symbol, file, type, prototype, summary)
    {
    # DEPENDENCY: This code depends on the order of the member constants.

    my $package = shift;

    my $object = [ @_ ];
    bless $object, $package;

    return $object;
    };


# Function: Symbol
# Returns the target's <SymbolString>.
sub Symbol
    {  return $_[0]->[SYMBOL];  };

# Function: File
# Returns the <FileName> the target is defined in.
sub File
    {  return $_[0]->[FILE];  };

# Function: Type
# Returns the target's <TopicType>.
sub Type
    {  return $_[0]->[TYPE];  };

# Function: Prototype
# Returns the target's prototype, or undef if not defined or not applicable.
sub Prototype
    {  return $_[0]->[PROTOTYPE];  };

# Function: Summary
# Returns the target's summary, or undef if not defined or not applicable.
sub Summary
    {  return $_[0]->[SUMMARY];  };

1;
