###############################################################################
#
#   Package: NaturalDocs::SymbolTable::File
#
###############################################################################
#
#   A class representing a file, keeping track of what symbols and references are defined in it.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::SymbolTable::File;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are its members.
#
#       SYMBOLS       - An existence hashref of the <SymbolStrings> it defines.
#       REFERENCES  - An existence hashref of the <ReferenceStrings> in the file.
#

# DEPENDENCY: New() depends on the order of these constants.  If they change, New() has to be updated.
use constant SYMBOLS => 0;
use constant REFERENCES => 1;


###############################################################################
# Group: Modification Functions


#
#   Function: New
#
#   Creates and returns a new object.
#
sub New
    {
    my $package = shift;

    # Let's make it safe, since normally you can pass values to New.  Having them just be ignored would be an obscure error.
    if (scalar @_)
        {  die "You can't pass values to NaturalDocs::SymbolTable::File->New()\n";  };

    # DEPENDENCY: This code depends on the order of the member constants.
    my $object = [ { }, { } ];
    bless $object, $package;

    return $object;
    };


#
#   Function: AddSymbol
#
#   Adds a <SymbolString> definition.
#
#   Parameters:
#
#       symbol - The <SymbolString> being added.
#
sub AddSymbol #(symbol)
    {
    my ($self, $symbol) = @_;
    $self->[SYMBOLS]{$symbol} = 1;
    };


#
#   Function: DeleteSymbol
#
#   Removes a <SymbolString> definition.
#
#   Parameters:
#
#       symbol - The <SymbolString> to delete.
#
sub DeleteSymbol #(symbol)
    {
    my ($self, $symbol) = @_;
    delete $self->[SYMBOLS]{$symbol};
    };


#
#   Function: AddReference
#
#   Adds a reference definition.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> being added.
#
sub AddReference #(referenceString)
    {
    my ($self, $referenceString) = @_;
    $self->[REFERENCES]{$referenceString} = 1;
    };


#
#   Function: DeleteReference
#
#   Removes a reference definition.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to delete.
#
sub DeleteReference #(referenceString)
    {
    my ($self, $referenceString) = @_;
    delete $self->[REFERENCES]{$referenceString};
    };



###############################################################################
# Group: Information Functions


#
#   Function: HasAnything
#
#   Returns whether the file has any symbol or reference definitions at all.
#
sub HasAnything
    {
    return (scalar keys %{$_[0]->[SYMBOLS]} || scalar keys %{$_[0]->[REFERENCES]});
    };

#
#   Function: Symbols
#
#   Returns an array of all the <SymbolStrings> defined in this file.  If none, returns an empty array.
#
sub Symbols
    {
    return keys %{$_[0]->[SYMBOLS]};
    };


#
#   Function: References
#
#   Returns an array of all the <ReferenceStrings> defined in this file.  If none, returns an empty array.
#
sub References
    {
    return keys %{$_[0]->[REFERENCES]};
    };


#
#   Function: DefinesSymbol
#
#   Returns whether the file defines the passed <SymbolString> or not.
#
sub DefinesSymbol #(symbol)
    {
    my ($self, $symbol) = @_;
    return exists $self->[SYMBOLS]{$symbol};
    };


#
#   Function: DefinesReference
#
#   Returns whether the file defines the passed <ReferenceString> or not.
#
sub DefinesReference #(referenceString)
    {
    my ($self, $referenceString) = @_;
    return exists $self->[REFERENCES]{$referenceString};
    };

1;
