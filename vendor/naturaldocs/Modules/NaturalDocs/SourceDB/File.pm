###############################################################################
#
#   Package: NaturalDocs::SourceDB::File
#
###############################################################################
#
#   A class used to index items by file.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::SourceDB::File;

use NaturalDocs::DefineMembers 'ITEMS';


#
#   Variables: Members
#
#   These constants serve as indexes into the object array.
#
#   ITEMS - An arrayref where an <ExtensionID> is the index and the members are existence hashrefs of the item strigs defined
#               in this file.  The arrayref will always exist, but the hashrefs may be undef.
#


#
#   Function: New
#
#   Returns a new object.
#
sub New
    {
    my $package = shift;

    my $object = [ ];
    $object->[ITEMS] = [ ];

    bless $object, $package;
    return $object;
    };


#
#   Function: AddItem
#
#   Adds an item to this file.  Returns whether this added a new item.
#
sub AddItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (!defined $self->[ITEMS]->[$extension])
        {
        $self->[ITEMS]->[$extension] = { $itemString => 1 };
        return 1;
        }
    elsif (!exists $self->[ITEMS]->[$extension]->{$itemString})
        {
        $self->[ITEMS]->[$extension]->{$itemString} = 1;
        return 1;
        }
    else
        {
        return 0;
        };
    };


#
#   Function: HasItem
#
#   Returns whether the item exists in this file.
#
sub HasItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $self->[ITEMS]->[$extension])
        {  return exists $self->[ITEMS]->[$extension]->{$itemString};  }
    else
        {  return 0;  };
    };


#
#   Function: DeleteItem
#
#   Deletes the passed item.  Returns whether it existed.
#
sub DeleteItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (!defined $self->[ITEMS]->[$extension])
        {  return 0;  }
    elsif (exists $self->[ITEMS]->[$extension]->{$itemString})
        {
        delete $self->[ITEMS]->[$extension]->{$itemString};
        return 1;
        }
    else
        {  return 0;  };
    };


#
#   Function: ListItems
#
#   Returns an array of all the item strings defined for a particular extension, or an empty list if none.
#
sub ListItems #(ExtensionID extension) => string array
    {
    my ($self, $extension) = @_;

    if (defined $self->[ITEMS]->[$extension])
        {  return keys %{$self->[ITEMS]->[$extension]};  }
    else
        {  return ( );  };
    };


1;
