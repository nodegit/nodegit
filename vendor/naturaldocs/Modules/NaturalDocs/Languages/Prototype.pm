###############################################################################
#
#   Class: NaturalDocs::Languages::Prototype
#
###############################################################################
#
#   A data class for storing parsed prototypes.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

use NaturalDocs::Languages::Prototype::Parameter;


package NaturalDocs::Languages::Prototype;

use NaturalDocs::DefineMembers 'BEFORE_PARAMETERS', 'BeforeParameters()', 'SetBeforeParameters()',
                                                 'AFTER_PARAMETERS', 'AfterParameters()', 'SetAfterParameters()',
                                                 'PARAMETERS', 'Parameters()';
# Dependency: New(), constant order, no parents.


#
#   Function: New
#
#   Creates and returns a new prototype object.
#
#   Parameters:
#
#       beforeParameters - The part of the prototype before the parameter list.
#       afterParameters - The part of the prototype after the parameter list.
#
#   You cannot set the parameters from here.  Use <AddParameter()>.
#
sub New #(beforeParameters, afterParameters)
    {
    my ($package, @params) = @_;

    # Dependency: Constant order, no parents.

    my $object = [ @params ];
    bless $object, $package;

    return $object;
    };


#
#   Functions: Members
#
#   BeforeParameters - Returns the part of the prototype before the parameter list.  If there is no parameter list, this will be the
#                                only thing that returns content.
#   SetBeforeParameters - Replaces the part of the prototype before the parameter list.
#   AfterParameters - Returns the part of the prototype after the parameter list, if any.
#   SetAfterParameters - Replaces the part of the prototype after the parameter list.
#   Parameters - Returns the parameter list as an arrayref of <NaturalDocs::Languages::Prototype::Parameters>, or undef if none.
#

#
#   Function: AddParameter
#
#   Adds a <NaturalDocs::Languages::Prototype::Parameter> to the list.
#
sub AddParameter #(parameter)
    {
    my ($self, $parameter) = @_;

    if (!defined $self->[PARAMETERS])
        {  $self->[PARAMETERS] = [ ];  };

    push @{$self->[PARAMETERS]}, $parameter;
    };


#
#   Function: OnlyBeforeParameters
#
#   Returns whether <BeforeParameters()> is the only thing set.
#
sub OnlyBeforeParameters
    {
    my $self = shift;
    return (!defined $self->[PARAMETERS] && !defined $self->[AFTER_PARAMETERS]);
    };


1;
