###############################################################################
#
#   Package: NaturalDocs::SourceDB::Item
#
###############################################################################
#
#   A base class for something being tracked in <NaturalDocs::SourceDB>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::SourceDB::Item;

use NaturalDocs::DefineMembers 'DEFINITIONS';


#
#   Variables: Members
#
#   The following constants are indexes into the object array.
#
#   DEFINITIONS - A hashref that maps <FileNames> to either <NaturalDocs::SourceDB::ItemDefinition>-derived objects or
#                         serves as an existence hashref depending on whether the extension only tracks existence.  Will be undef if
#                         there are none.
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
#
#   Group: Definition Functions
#
#   These functions should be called by <NaturalDocs::SourceDB>.  You should not be calling them directly.  Call functions
#   like <NaturalDocs::SourceDB->AddDefinition()> instead.
#


#
#   Function: AddDefinition
#
#   Adds a definition for the passed <FileName>.  If it's already defined, the new definition will be ignored.
#
#   Parameters:
#
#       file - The <FileName>.
#       definition - The definition, which must be an object derived from <NaturalDocs::SourceDB::ItemDefinition> or undef if
#                       the extension only tracks existence.
#
#   Returns:
#
#       Whether the definition was added, which is to say, whether this was the first definition for the passed <FileName>.
#
sub AddDefinition #(FileName file, optional NaturalDocs::SourceDB::ItemDefinition definition) => bool
    {
    my ($self, $file, $definition) = @_;

    if (!defined $self->[DEFINITIONS])
        {  $self->[DEFINITIONS] = { };  };

    if (!exists $self->[DEFINITIONS]->{$file})
        {
        if (!defined $definition)
            {  $definition = 1;  };

        $self->[DEFINITIONS]->{$file} = $definition;
        return 1;
        }
    else
        {  return 0;  };
    };


#
#   Function: ChangeDefinition
#
#   Changes the definition for the passed <FileName>.
#
#   Parameters:
#
#       file - The <FileName>.
#       definition - The definition, which must be an object derived from <NaturalDocs::SourceDB::ItemDefinition>.
#
sub ChangeDefinition #(FileName file, NaturalDocs::SourceDB::ItemDefinition definition)
    {
    my ($self, $file, $definition) = @_;

    if (!defined $self->[DEFINITIONS] || !exists $self->[DEFINITIONS]->{$file})
        {  die "Tried to change a non-existant definition in SourceD::Item.";  };

    $self->[DEFINITIONS]->{$file} = $definition;
    };


#
#   Function: GetDefinition
#
#   Returns the <NaturalDocs::SourceDB::ItemDefinition>-derived object for the passed <FileName>, non-zero if it only tracks
#   existence, or undef if there is no definition.
#
sub GetDefinition #(FileName file) => NaturalDocs::SourceDB::ItemDefinition or bool
    {
    my ($self, $file) = @_;

    if (defined $self->[DEFINITIONS])
        {  return $self->[DEFINITIONS]->{$file};  }
    else
        {  return undef;  };
    };


#
#   Function: DeleteDefinition
#
#   Removes the definition for the passed <FileName>.  Returns whether it was successful, meaning whether a definition existed
#   for that file.
#
sub DeleteDefinition #(FileName file) => bool
    {
    my ($self, $file) = @_;

    if (defined $self->[DEFINITIONS])
        {
        if (exists $self->[DEFINITIONS]->{$file})
            {
            delete $self->[DEFINITIONS]->{$file};

            if (!scalar keys %{$self->[DEFINITIONS]})
                {  $self->[DEFINITIONS] = undef;  };

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
sub HasDefinitions # => bool
    {
    my $self = shift;
    return (defined $self->[DEFINITIONS]);
    };


#
#   Function: HasDefinition
#
#   Returns whether there is a definition for the passed <FileName>.
#
sub HasDefinition #(FileName file) => bool
    {
    my ($self, $file) = @_;

    if (defined $self->[DEFINITIONS])
        {  return (exists $self->[DEFINITIONS]->{$file});  }
    else
        {  return 0;  };
    };


#
#   Function: GetAllDefinitionsHashRef
#
#   Returns a hashref of all the definitions of this item.  *Do not change.*  The keys are the <FileNames>, and the values are
#   either <NaturalDocs::SourceDB::ItemDefinition>-derived objects or it's just an existence hashref if those aren't used.
#
sub GetAllDefinitionsHashRef
    {
    my $self = shift;
    return $self->[DEFINITIONS];
    };


1;
