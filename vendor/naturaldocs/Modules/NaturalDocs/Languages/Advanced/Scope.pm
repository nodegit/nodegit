###############################################################################
#
#   Class: NaturalDocs::Languages::Advanced::Scope
#
###############################################################################
#
#   A class used to store a scope level.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Advanced::Scope;

#
#   Constants: Implementation
#
#   The object is implemented as a blessed arrayref.  The constants below are used as indexes.
#
#   CLOSING_SYMBOL - The closing symbol character of the scope.
#   PACKAGE - The package <SymbolString> of the scope.
#   USING - An arrayref of <SymbolStrings> for using statements, or undef if none.
#
use NaturalDocs::DefineMembers 'CLOSING_SYMBOL', 'PACKAGE', 'USING';
# Dependency: New() depends on the order of these constants as well as that there is no inherited members.


#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       closingSymbol - The closing symbol character of the scope.
#       package - The package <SymbolString> of the scope.
#       using - An arrayref of using <SymbolStrings>, or undef if none.  The contents of the array will be duplicated.
#
#       If package is set to undef, it is assumed that it inherits the value of the previous scope on the stack.
#
sub New #(closingSymbol, package, using)
    {
    # Dependency: This depends on the order of the parameters matching the constants, and that there are no inherited
    # members.
    my $package = shift;

    my $object = [ @_ ];
    bless $object, $package;

    if (defined $object->[USING])
        {  $object->[USING] = [ @{$object->[USING]} ];  };

    return $object;
    };


# Function: ClosingSymbol
# Returns the closing symbol character of the scope.
sub ClosingSymbol
    {  return $_[0]->[CLOSING_SYMBOL];  };

# Function: Package
# Returns the package <SymbolString> of the scope, or undef if none.
sub Package
    {  return $_[0]->[PACKAGE];  };

# Function: SetPackage
# Sets the package <SymbolString> of the scope.
sub SetPackage #(package)
    {  $_[0]->[PACKAGE] = $_[1];  };

# Function: Using
# Returns an arrayref of <SymbolStrings> for using statements, or undef if none
sub Using
    {  return $_[0]->[USING];  };

# Function: AddUsing
# Adds a <SymbolString> to the <Using()> array.
sub AddUsing #(using)
    {
    my ($self, $using) = @_;

    if (!defined $self->[USING])
        {  $self->[USING] = [ ];  };

    push @{$self->[USING]}, $using;
    };



1;
