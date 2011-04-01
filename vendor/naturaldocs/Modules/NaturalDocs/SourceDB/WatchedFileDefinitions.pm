###############################################################################
#
#   Package: NaturalDocs::SourceDB::WatchedFileDefinitions
#
###############################################################################
#
#   A class to track the definitions appearing in a watched file.  This is only used for extensions that track definition info with
#   <NaturalDocs::SourceDB::ItemDefinition>-derived objects.  Do not use it for extensions that only track existence.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::SourceDB::WatchedFileDefinitions;


#
#   Variables: Members
#
#   This object would only have one member, which is an array, so the object itself serves as that member.
#
#   <ExtensionIDs> are used as indexes into this object.  Each entry is a hashref that maps item strings to
#   <NaturalDocs::SourceDB::ItemDefinition>-derived objects.  This is only done for extensions that use those objects to track
#   definitions, it's not needed for extensions that only track existence.  If there are no definitions, the entry will be undef.
#


#
#   Function: New
#
#   Creates and returns a new object.
#
sub New
    {
    my $class = shift;

    my $object = [ ];
    bless $object, $class;

    return $object;
    };



###############################################################################
# Group: Definition Functions
#


#
#   Function: AddDefinition
#
#   Adds a definition for the passed item string.  If it's already defined, the new definition will be ignored.
#
#   Parameters:
#
#       extension - The <ExtensionID>.
#       itemString - The item string.
#       definition - The definition, which must be an object derived from <NaturalDocs::SourceDB::ItemDefinition>.
#
#   Returns:
#
#       Whether the definition was added, which is to say, whether this was the first definition for the passed <FileName>.
#
sub AddDefinition #(ExtensionID extension, string itemString, NaturalDocs::SourceDB::ItemDefinition definition) => bool
    {
    my ($self, $extension, $itemString, $definition) = @_;

    if (!defined $self->[$extension])
        {  $self->[$extension] = { };  };

    if (!exists $self->[$extension]->{$itemString})
        {
        $self->[$extension]->{$itemString} = $definition;
        return 1;
        }
    else
        {  return 0;  };
    };


#
#   Function: GetDefinition
#
#   Returns the <NaturalDocs::SourceDB::ItemDefinition>-derived object for the passed item string  or undef if there is none.
#
sub GetDefinition #(ExtensionID extension, string itemString) => NaturalDocs::SourceDB::ItemDefinition
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $self->[$extension])
        {  return $self->[$extension]->{$itemString};  }
    else
        {  return undef;  };
    };


#
#   Function: DeleteDefinition
#
#   Removes the definition for the passed item string.  Returns whether it was successful, meaning whether a definition existed
#   for that item.
#
sub DeleteDefinition #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $self->[$extension])
        {
        if (exists $self->[$extension]->{$itemString})
            {
            delete $self->[$extension]->{$itemString};

            if (!scalar keys %{$self->[$extension]})
                {  $self->[$extension] = undef;  };

            return 1;
            };
        };

    return 0;
    };


#
#   Function: HasDefinitions
#
#   Returns whether there are any definitions for this item.
#
sub HasDefinitions #(ExtensionID extension) => bool
    {
    my ($self, $extension) = @_;

    return (defined $self->[$extension]);
    };


#
#   Function: HasDefinition
#
#   Returns whether there is a definition for the passed item string.
#
sub HasDefinition #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $self->[$extension])
        {  return (exists $self->[$extension]->{$itemString});  }
    else
        {  return 0;  };
    };


1;
