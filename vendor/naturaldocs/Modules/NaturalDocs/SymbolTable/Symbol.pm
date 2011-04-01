###############################################################################
#
#   Package: NaturalDocs::SymbolTable::Symbol
#
###############################################################################
#
#   A class representing a symbol or a potential symbol.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::SymbolTable::Symbol;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are its members.
#
#       DEFINITIONS             - A hashref of all the files which define this symbol.  The keys are the <FileNames>, and the values are
#                                         <NaturalDocs::SymbolTable::SymbolDefinition> objects.  If no files define this symbol, this item will
#                                          be undef.
#       GLOBAL_DEFINITION  - The <FileName> which defines the global version of the symbol, which is what is used if
#                                          a file references the symbol but does not have its own definition.  If there are no definitions, this
#                                          item will be undef.
#       REFERENCES              - A hashref of the references that can be interpreted as this symbol.  This doesn't mean these
#                                          references necessarily are.  The keys are the reference strings, and the values are the scores of
#                                          the interpretations.  If no references can be interpreted as this symbol, this item will be undef.
#
use constant DEFINITIONS => 0;
use constant GLOBAL_DEFINITION => 1;
use constant REFERENCES => 2;


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
        {  die "You can't pass values to NaturalDocs::SymbolTable::Symbol->New()\n";  };

    my $object = [ undef, undef, undef ];
    bless $object, $package;

    return $object;
    };

#
#   Function: AddDefinition
#
#   Adds a symbol definition.  If this is the first definition for this symbol, it will become the global definition.  If the definition
#   already exists for the file, it will be ignored.
#
#   Parameters:
#
#       file   - The <FileName> that defines the symbol.
#       type - The <TopicType> of the definition.
#       prototype - The prototype of the definition, if applicable.  Undef otherwise.
#       summary - The summary for the definition, if applicable.  Undef otherwise.
#
#   Returns:
#
#       Whether this provided the first definition for this symbol.
#
sub AddDefinition #(file, type, prototype, summary)
    {
    my ($self, $file, $type, $prototype, $summary) = @_;

    my $isFirst;

    if (!defined $self->[DEFINITIONS])
        {
        $self->[DEFINITIONS] = { };
        $self->[GLOBAL_DEFINITION] = $file;
        $isFirst = 1;
        };

    if (!exists $self->[DEFINITIONS]{$file})
        {
        $self->[DEFINITIONS]{$file} = NaturalDocs::SymbolTable::SymbolDefinition->New($type, $prototype, $summary);
        };

    return $isFirst;
    };


#
#   Function: ChangeDefinition
#
#   Changes the information about an existing definition.
#
#   Parameters:
#
#       file   - The <FileName> that defines the symbol.  Must exist.
#       type - The new <TopicType> of the definition.
#       prototype - The new prototype of the definition, if applicable.  Undef otherwise.
#       summary - The new summary of the definition, if applicable.  Undef otherwise.
#
sub ChangeDefinition #(file, type, prototype, summary)
    {
    my ($self, $file, $type, $prototype, $summary) = @_;

    if (defined $self->[DEFINITIONS] &&
        exists $self->[DEFINITIONS]{$file})
        {
        $self->[DEFINITIONS]{$file}->SetType($type);
        $self->[DEFINITIONS]{$file}->SetPrototype($prototype);
        $self->[DEFINITIONS]{$file}->SetSummary($summary);
        };
    };


#
#   Function: DeleteDefinition
#
#   Removes a symbol definition.  If the definition served as the global definition, a new one will be selected.
#
#   Parameters:
#
#       file - The <FileName> which contains definition to delete.
#
#   Returns:
#
#       Whether that was the only definition, and the symbol is now undefined.
#
sub DeleteDefinition #(file)
    {
    my ($self, $file) = @_;

    # If there are no definitions...
    if (!defined $self->[DEFINITIONS])
        {  return undef;  };

    delete $self->[DEFINITIONS]{$file};

    # If there are no more definitions...
    if (!scalar keys %{$self->[DEFINITIONS]})
        {
        $self->[DEFINITIONS] = undef;

        # If definitions was previously defined, and now is empty, we can safely assume that the global definition was just deleted
        # without checking it against $file.

        $self->[GLOBAL_DEFINITION] = undef;

        return 1;
        }

    # If there are more definitions and the global one was just deleted...
    elsif ($self->[GLOBAL_DEFINITION] eq $file)
        {
        # Which one becomes global is pretty much random.
        $self->[GLOBAL_DEFINITION] = (keys %{$self->[DEFINITIONS]})[0];
        return undef;
        };
    };


#
#   Function: AddReference
#
#   Adds a reference that can be interpreted as this symbol.  It can be, but not necessarily is.
#
#   Parameters:
#
#       referenceString - The string of the reference.
#       score                - The score of this interpretation.
#
sub AddReference #(referenceString, score)
    {
    my ($self, $referenceString, $score) = @_;

    if (!defined $self->[REFERENCES])
        {  $self->[REFERENCES] = { };  };

    $self->[REFERENCES]{$referenceString} = $score;
    };


#
#   Function: DeleteReference
#
#   Deletes a reference that can be interpreted as this symbol.
#
#   Parameters:
#
#       referenceString - The string of the reference to delete.
#
sub DeleteReference #(referenceString)
    {
    my ($self, $referenceString) = @_;

    # If there are no definitions...
    if (!defined $self->[REFERENCES])
        {  return;  };

    delete $self->[REFERENCES]{$referenceString};

    # If there are no more definitions...
    if (!scalar keys %{$self->[REFERENCES]})
        {
        $self->[REFERENCES] = undef;
        };
    };


#
#   Function: DeleteAllReferences
#
#   Removes all references that can be interpreted as this symbol.
#
sub DeleteAllReferences
    {
    $_[0]->[REFERENCES] = undef;
    };


###############################################################################
# Group: Information Functions

#
#   Function: IsDefined
#
#   Returns whether the symbol is defined anywhere or not.  If it's not, that means it's just a potential interpretation of a
#   reference.
#
sub IsDefined
    {
    return defined $_[0]->[GLOBAL_DEFINITION];
    };

#
#   Function: IsDefinedIn
#
#   Returns whether the symbol is defined in the passed <FileName>.
#
sub IsDefinedIn #(file)
    {
    my ($self, $file) = @_;
    return ($self->IsDefined() && exists $self->[DEFINITIONS]{$file});
    };


#
#   Function: Definitions
#
#   Returns an array of all the <FileNames> that define this symbol.  If none do, will return an empty array.
#
sub Definitions
    {
    my $self = shift;

    if ($self->IsDefined())
        {  return keys %{$self->[DEFINITIONS]};  }
    else
        {  return ( );  };
    };


#
#   Function: GlobalDefinition
#
#   Returns the <FileName> that contains the global definition of this symbol, or undef if the symbol isn't defined.
#
sub GlobalDefinition
    {
    return $_[0]->[GLOBAL_DEFINITION];
    };


#
#   Function: TypeDefinedIn
#
#   Returns the <TopicType> of the symbol defined in the passed <FileName>, or undef if it's not defined in that file.
#
sub TypeDefinedIn #(file)
    {
    my ($self, $file) = @_;

    if ($self->IsDefined())
        {  return $self->[DEFINITIONS]{$file}->Type();  }
    else
        {  return undef;  };
    };


#
#   Function: GlobalType
#
#   Returns the <TopicType> of the global definition, or undef if the symbol isn't defined.
#
sub GlobalType
    {
    my $self = shift;

    my $globalDefinition = $self->GlobalDefinition();

    if (!defined $globalDefinition)
        {  return undef;  }
    else
        {  return $self->[DEFINITIONS]{$globalDefinition}->Type();  };
    };


#
#   Function: PrototypeDefinedIn
#
#   Returns the prototype of symbol defined in the passed <FileName>, or undef if it doesn't exist or is not defined in that file.
#
sub PrototypeDefinedIn #(file)
    {
    my ($self, $file) = @_;

    if ($self->IsDefined())
        {  return $self->[DEFINITIONS]{$file}->Prototype();  }
    else
        {  return undef;  };
    };


#
#   Function: GlobalPrototype
#
#   Returns the prototype of the global definition.  Will be undef if it doesn't exist or the symbol isn't defined.
#
sub GlobalPrototype
    {
    my $self = shift;

    my $globalDefinition = $self->GlobalDefinition();

    if (!defined $globalDefinition)
        {  return undef;  }
    else
        {  return $self->[DEFINITIONS]{$globalDefinition}->Prototype();  };
    };


#
#   Function: SummaryDefinedIn
#
#   Returns the summary of symbol defined in the passed <FileName>, or undef if it doesn't exist or is not defined in that file.
#
sub SummaryDefinedIn #(file)
    {
    my ($self, $file) = @_;

    if ($self->IsDefined())
        {  return $self->[DEFINITIONS]{$file}->Summary();  }
    else
        {  return undef;  };
    };


#
#   Function: GlobalSummary
#
#   Returns the summary of the global definition.  Will be undef if it doesn't exist or the symbol isn't defined.
#
sub GlobalSummary
    {
    my $self = shift;

    my $globalDefinition = $self->GlobalDefinition();

    if (!defined $globalDefinition)
        {  return undef;  }
    else
        {  return $self->[DEFINITIONS]{$globalDefinition}->Summary();  };
    };


#
#   Function: HasReferences
#
#   Returns whether the symbol can be interpreted as any references.
#
sub HasReferences
    {
    return defined $_[0]->[REFERENCES];
    };

#
#   Function: References
#
#   Returns an array of all the reference strings that can be interpreted as this symbol.  If none, will return an empty array.
#
sub References
    {
    if (defined $_[0]->[REFERENCES])
        {  return keys %{$_[0]->[REFERENCES]};  }
    else
        {  return ( );  };
    };


#
#   Function: ReferencesAndScores
#
#   Returns a hash of all the references that can be interpreted as this symbol and their scores.  The keys are the reference
#   strings, and the values are the scores.  If none, will return an empty hash.
#
sub ReferencesAndScores
    {
    if (defined $_[0]->[REFERENCES])
        {  return %{$_[0]->[REFERENCES]};  }
    else
        {  return ( );  };
    };

1;
