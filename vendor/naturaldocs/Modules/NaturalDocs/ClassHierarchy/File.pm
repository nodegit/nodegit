###############################################################################
#
#   Class: NaturalDocs::ClassHierarchy::File
#
###############################################################################
#
#   An object that stores information about what hierarchy information is present in a file.  It does not store its <FileName>; it
#   assumes that it will be stored in a hashref where the key is the <FileName>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::ClassHierarchy::File;


#
#   Topic: Implementation
#
#   Since there's only one member in the class, and it's a hashref, the class is simply the hashref itself blessed as a class.
#   The keys are the class <SymbolStrings> that are defined in the file, and the values are existence hashrefs of each class'
#   parent <ReferenceStrings>, or undef if none.
#


###############################################################################
# Group: Modification Functions


#
#   Function: New
#
#   Creates and returns a new class.
#
sub New
    {
    my ($package) = @_;

    my $object = { };
    bless $object, $package;

    return $object;
    };

#
#   Function: AddClass
#   Adds a rew class <SymbolString> to the file.
#
sub AddClass #(class)
    {
    my ($self, $class) = @_;

    if (!exists $self->{$class})
        {  $self->{$class} = undef;  };
    };

#
#   Function: DeleteClass
#   Deletes a class <SymbolString> from the file.
#
sub DeleteClass #(class)
    {
    my ($self, $class) = @_;
    delete $self->{$class};
    };

#
#   Function: AddParentReference
#   Adds a parent <ReferenceString> to a class <SymbolString>.
#
sub AddParentReference #(class, parentReference)
    {
    my ($self, $class, $parent) = @_;

    if (!exists $self->{$class} || !defined $self->{$class})
        {  $self->{$class} = { };  };

    $self->{$class}->{$parent} = 1;
    };

#
#   Function: DeleteParentReference
#   Deletes a parent <ReferenceString> from a class <SymbolString>.
#
sub DeleteParentReference #(class, parent)
    {
    my ($self, $class, $parent) = @_;

    if (exists $self->{$class})
        {
        delete $self->{$class}->{$parent};

        if (!scalar keys %{$self->{$class}})
            {  $self->{$class} = undef;  };
        };
    };



###############################################################################
# Group: Information Functions


#
#   Function: Classes
#   Returns an array of the class <SymbolStrings> that are defined by this file, or an empty array if none.
#
sub Classes
    {
    my ($self) = @_;
    return keys %{$self};
    };

#
#   Function: HasClass
#   Returns whether the file defines the passed class <SymbolString>.
#
sub HasClass #(class)
    {
    my ($self, $class) = @_;
    return exists $self->{$class};
    };

#
#   Function: ParentReferencesOf
#   Returns an array of the parent <ReferenceStrings> that are defined by the class, or an empty array if none.
#
sub ParentReferencesOf #(class)
    {
    my ($self, $class) = @_;

    if (!exists $self->{$class} || !defined $self->{$class})
        {  return ( );  }
    else
        {  return keys %{$self->{$class}};  };
    };

#
#   Function: HasParentReference
#   Returns whether the file defines the passed class <SymbolString> and parent <ReferenceString>.
#
sub HasParentReference #(class, parent)
    {
    my ($self, $class, $parent) = @_;

    if (!$self->HasClass($class))
        {  return undef;  };

    return exists $self->{$class}->{$parent};
    };


1;
