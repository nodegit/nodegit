###############################################################################
#
#   Package: NaturalDocs::SymbolTable::SymbolDefinition
#
###############################################################################
#
#   A class representing a symbol definition.  This does not store the definition symbol, class, or file.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::SymbolTable::SymbolDefinition;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are its members.
#
#       TYPE  - The symbol <TopicType>.
#       PROTOTYPE  - The symbol's prototype, if applicable.  Will be undef otherwise.
#       SUMMARY - The symbol's summary, if applicable.  Will be undef otherwise.
#
use constant TYPE => 0;
use constant PROTOTYPE => 1;
use constant SUMMARY => 2;
# New depends on the order of the constants.


###############################################################################
# Group: Functions

#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       type - The symbol <TopicType>.
#       prototype  - The symbol prototype, if applicable.  Undef otherwise.
#       summary - The symbol's summary, if applicable.  Undef otherwise.
#
sub New #(type, prototype, summary)
    {
    # This depends on the parameter list being the same as the constant order.

    my $package = shift;

    my $object = [ @_ ];
    bless $object, $package;

    return $object;
    };


#   Function: Type
#   Returns the definition's <TopicType>.
sub Type
    {  return $_[0]->[TYPE];  };

# Function: SetType
# Changes the <TopicType>.
sub SetType #(type)
    {  $_[0]->[TYPE] = $_[1];  };

#   Function: Prototype
#   Returns the definition's prototype, or undef if it doesn't have one.
sub Prototype
    {  return $_[0]->[PROTOTYPE];  };

# Function: SetPrototype
# Changes the prototype.
sub SetPrototype #(prototype)
    {  $_[0]->[PROTOTYPE] = $_[1];  };

#   Function: Summary
#   Returns the definition's summary, or undef if it doesn't have one.
sub Summary
    {  return $_[0]->[SUMMARY];  };

# Function: SetSummary
# Changes the summary.
sub SetSummary #(summary)
    {  $_[0]->[SUMMARY] = $_[1];  };


1;
