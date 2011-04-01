###############################################################################
#
#   Class: NaturalDocs::ClassHierarchy::Class
#
###############################################################################
#
#   An object that stores information about a class in the hierarchy.  It does not store its <SymbolString>; it assumes that it will
#   be stored in a hashref where the key is the <SymbolString>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::ClassHierarchy::Class;


#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The keys are the constants below.
#
#   DEFINITIONS - An existence hashref of all the <FileNames> which define this class.  Undef if none.
#   PARENTS - An existence hashref of the <SymbolStrings> of all the parents this class has.
#   CHILDREN - An existence hashref of the <SymbolStrings> of all the children this class has.
#   PARENT_REFERENCES - A hashref of the parent <ReferenceStrings> this class has.  The keys are the <ReferenceStrings>,
#                                      and the values are existence hashrefs of all the <FileNames> that define them.  Undef if none.
#
use NaturalDocs::DefineMembers 'DEFINITIONS', 'PARENTS', 'CHILDREN', 'PARENT_REFERENCES';
# Dependency: New() depends on the order of these constants, as well as the class not being derived from any other.


###############################################################################
# Group: Modification Functions


#
#   Function: New
#
#   Creates and returns a new class.
#
sub New
    {
    # Dependency: This function depends on the order of the constants, as well as the class not being derived from any other.
    my ($package, $definitionFile) = @_;

    my $object = [ undef, undef, undef, undef ];
    bless $object, $package;

    return $object;
    };


#
#   Function: AddDefinition
#
#   Adds a rew definition of this class and returns if that was the first definition.
#
#   Parameters:
#
#       file - The <FileName> the definition appears in.
#
#   Returns:
#
#       Whether this was the first definition of this class.
#
sub AddDefinition #(file)
    {
    my ($self, $file) = @_;

    my $wasFirst;

    if (!defined $self->[DEFINITIONS])
        {
        $self->[DEFINITIONS] = { };
        $wasFirst = 1;
        };

    $self->[DEFINITIONS]->{$file} = 1;

    return $wasFirst;
    };


#
#   Function: DeleteDefinition
#
#   Removes the definition of this class and returns if there are no more definitions.  Note that if there are no more
#   definitions, you may still want to keep the object around if <HasChildren()> returns true.
#
#   Parameters:
#
#       file - The <FileName> the definition appears in.
#
#   Returns:
#
#       Whether this deleted the last definition of this class.
#
sub DeleteDefinition #(file)
    {
    my ($self, $file) = @_;

    if (defined $self->[DEFINITIONS])
        {
        delete $self->[DEFINITIONS]->{$file};

        if (!scalar keys %{$self->[DEFINITIONS]})
            {
            $self->[DEFINITIONS] = undef;
            return 1;
            };
        };

    return undef;
    };


#
#   Function: AddParentReference
#
#   Adds a parent reference to the class and return whether it resulted in a new parent class.
#
#   Parameters:
#
#       reference - The <ReferenceString> used to determine the parent.
#       file - The <FileName> the parent reference is in.
#       referenceTranslations - A hashref of what each reference currently resolves to.  The keys are the
#                                         <ReferenceStrings> and the values are class <SymbolStrings>.  It should include an entry for
#                                         the reference parameter above.
#
#   Returns:
#
#       If the reference adds a new parent, it will return that parent's <SymbolString>.  Otherwise it will return undef.
#
sub AddParentReference #(reference, file, referenceTranslations)
    {
    my ($self, $reference, $file, $referenceTranslations) = @_;

    if (!defined $self->[PARENT_REFERENCES])
        {  $self->[PARENT_REFERENCES] = { };  };
    if (!defined $self->[PARENTS])
        {  $self->[PARENTS] = { };  };


    if (!exists $self->[PARENT_REFERENCES]->{$reference})
        {
        $self->[PARENT_REFERENCES]->{$reference} = { $file => 1 };

        my $symbol = $referenceTranslations->{$reference};

        if (!exists $self->[PARENTS]->{$symbol})
            {
            $self->[PARENTS]->{$symbol} = 1;
            return $symbol;
            }
        else
            {  return undef;  };
        }
    else
        {
        $self->[PARENT_REFERENCES]->{$reference}->{$file} = 1;
        return undef;
        };
    };

#
#   Function: DeleteParentReference
#
#   Deletes a parent reference from the class and return whether it resulted in a loss of a parent class.
#
#   Parameters:
#
#       reference - The <ReferenceString> used to determine the parent.
#       file - The <FileName> the parent declaration is in.
#       referenceTranslations - A hashref of what each reference currently resolves to.  The keys are the
#                                         <ReferenceStrings> and the values are class <SymbolStrings>.  It should include an entry for
#                                         the reference parameter above.
#
#   Returns:
#
#       If this causes a parent class to be lost, it will return that parent's <SymbolString>.  Otherwise it will return undef.
#
sub DeleteParentReference #(reference, file, referenceTranslations)
    {
    my ($self, $reference, $file, $referenceTranslations) = @_;

    if (defined $self->[PARENT_REFERENCES] && exists $self->[PARENT_REFERENCES]->{$reference} &&
        exists $self->[PARENT_REFERENCES]->{$reference}->{$file})
        {
        delete $self->[PARENT_REFERENCES]->{$reference}->{$file};

        # Quit if there are other definitions of this reference.
        if (scalar keys %{$self->[PARENT_REFERENCES]->{$reference}})
            {  return undef;  };

        delete $self->[PARENT_REFERENCES]->{$reference};

        if (!scalar keys %{$self->[PARENT_REFERENCES]})
            {  $self->[PARENT_REFERENCES] = undef;  };

        my $parent = $referenceTranslations->{$reference};

        # Check if any other references resolve to the same parent.
        if (defined $self->[PARENT_REFERENCES])
            {
            foreach my $parentReference (keys %{$self->[PARENT_REFERENCES]})
                {
                if ($referenceTranslations->{$parentReference} eq $parent)
                    {  return undef;  };
                };
            };

        # If we got this far, no other parent references resolve to this symbol.

        delete $self->[PARENTS]->{$parent};

        if (!scalar keys %{$self->[PARENTS]})
            {  $self->[PARENTS] = undef;  };

        return $parent;
        }
    else
        {  return undef;  };
    };


#
#   Function: AddChild
#   Adds a child <SymbolString> to the class.  Unlike <AddParentReference()>, this does not keep track of anything other than
#   whether it has it or not.
#
#   Parameters:
#
#       child - The <SymbolString> to add.
#
sub AddChild #(child)
    {
    my ($self, $child) = @_;

    if (!defined $self->[CHILDREN])
        {  $self->[CHILDREN] = { };  };

    $self->[CHILDREN]->{$child} = 1;
    };

#
#   Function: DeleteChild
#   Deletes a child <SymbolString> from the class.  Unlike <DeleteParentReference()>, this does not keep track of anything other
#   than whether it has it or not.
#
#   Parameters:
#
#       child - The <SymbolString> to delete.
#
sub DeleteChild #(child)
    {
    my ($self, $child) = @_;

    if (defined $self->[CHILDREN])
        {
        delete $self->[CHILDREN]->{$child};

        if (!scalar keys %{$self->[CHILDREN]})
            {  $self->[CHILDREN] = undef;  };
        };
    };



###############################################################################
# Group: Information Functions

#
#   Function: Definitions
#   Returns an array of the <FileNames> that define this class, or an empty array if none.
#
sub Definitions
    {
    my ($self) = @_;

    if (defined $self->[DEFINITIONS])
        {  return keys %{$self->[DEFINITIONS]};  }
    else
        {  return ( );  };
    };

#
#   Function: IsDefinedIn
#   Returns whether the class is defined in the passed <FileName>.
#
sub IsDefinedIn #(file)
    {
    my ($self, $file) = @_;

    if (defined $self->[DEFINITIONS])
        {  return exists $self->[DEFINITIONS]->{$file};  }
    else
        {  return 0;  };
    };

#
#   Function: IsDefined
#   Returns whether the class is defined in any files.
#
sub IsDefined
    {
    my ($self) = @_;
    return defined $self->[DEFINITIONS];
    };

#
#   Function: ParentReferences
#   Returns an array of the parent <ReferenceStrings>, or an empty array if none.
#
sub ParentReferences
    {
    my ($self) = @_;

    if (defined $self->[PARENT_REFERENCES])
        {  return keys %{$self->[PARENT_REFERENCES]};  }
    else
        {  return ( );  };
    };

#
#   Function: HasParentReference
#   Returns whether the class has the passed parent <ReferenceString>.
#
sub HasParentReference #(reference)
    {
    my ($self, $reference) = @_;
    return (defined $self->[PARENT_REFERENCES] && exists $self->[PARENT_REFERENCES]->{$reference});
    };

#
#   Function: HasParentReferences
#   Returns whether the class has any parent <ReferenceStrings>.
#
sub HasParentReferences
    {
    my ($self) = @_;
    return defined $self->[PARENT_REFERENCES];
    };

#
#   Function: Parents
#   Returns an array of the parent <SymbolStrings>, or an empty array if none.
#
sub Parents
    {
    my ($self) = @_;

    if (defined $self->[PARENTS])
        {  return keys %{$self->[PARENTS]};  }
    else
        {  return ( );  };
    };

#
#   Function: HasParents
#   Returns whether the class has any parent <SymbolStrings> defined.
#
sub HasParents
    {
    my ($self) = @_;
    return defined $self->[PARENTS];
    };

#
#   Function: Children
#   Returns an array of the child <SymbolStrings>, or an empty array if none.
#
sub Children
    {
    my ($self) = @_;

    if (defined $self->[CHILDREN])
        {  return keys %{$self->[CHILDREN]};  }
    else
        {  return ( );  };
    };

#
#   Function: HasChildren
#   Returns whether any child <SymbolStrings> are defined.
#
sub HasChildren
    {
    my ($self) = @_;
    return defined $self->[CHILDREN];
    };


#
#   Function: ParentReferenceDefinitions
#   Returns an array of the <FileNames> which define the passed parent <ReferenceString>, or an empty array if none.
#
sub ParentReferenceDefinitions #(reference)
    {
    my ($self, $reference) = @_;

    if (defined $self->[PARENT_REFERENCES] && exists $self->[PARENT_REFERENCES]->{$reference})
        {  return keys %{$self->[PARENT_REFERENCES]->{$reference}};  }
    else
        {  return ( );  };
    };


1;
