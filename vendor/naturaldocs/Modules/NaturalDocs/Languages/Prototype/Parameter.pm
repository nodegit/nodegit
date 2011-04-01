###############################################################################
#
#   Class: NaturalDocs::Languages::Prototype::Parameter
#
###############################################################################
#
#   A data class for storing parsed prototype parameters.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Prototype::Parameter;

use NaturalDocs::DefineMembers 'TYPE', 'Type()', 'SetType()',
                                                 'TYPE_PREFIX', 'TypePrefix()', 'SetTypePrefix()',
                                                 'NAME', 'Name()', 'SetName()',
                                                 'NAME_PREFIX', 'NamePrefix()', 'SetNamePrefix()',
                                                 'DEFAULT_VALUE', 'DefaultValue()', 'SetDefaultValue()',
                                                 'DEFAULT_VALUE_PREFIX', 'DefaultValuePrefix()', 'SetDefaultValuePrefix()';
# Dependency: New() depends on the order of these constants and that they don't inherit from another class.


#
#   Constants: Members
#
#   The object is implemented as a blessed arrayref, with the following constants as its indexes.
#
#       TYPE - The parameter type, if any.
#       TYPE_PREFIX - The parameter type prefix which should be aligned separately, if any.
#       NAME - The parameter name.
#       NAME_PREFIX - The parameter name prefix which should be aligned separately, if any.
#       DEFAULT_VALUE - The default value expression, if any.
#       DEFAULT_VALUE_PREFIX - The default value prefix which should be aligned separately, if any.
#

#
#   Function: New
#
#   Creates and returns a new prototype object.
#
#   Parameters:
#
#       type - The parameter type, if any.
#       typePrefix - The parameter type prefix which should be aligned separately, if any.
#       name - The parameter name.
#       namePrefix - The parameter name prefix which should be aligned separately, if any.
#       defaultValue - The default value expression, if any.
#       defaultValuePrefix - The default value prefix which should be aligned separately, if any.
#
sub New #(type, typePrefix, name, namePrefix, defaultValue, defaultValuePrefix)
    {
    my ($package, @params) = @_;

    # Dependency: This depends on the order of the parameters being the same as the order of the constants, and that the
    # constants don't inherit from another class.

    my $object = [ @params ];
    bless $object, $package;

    return $object;
    };


#
#   Functions: Members
#
#   Type - The parameter type, if any.
#   SetType - Replaces the parameter type.
#   TypePrefix - The parameter type prefix, which should be aligned separately, if any.
#   SetTypePrefix - Replaces the parameter type prefix.
#   Name - The parameter name.
#   SetName - Replaces the parameter name.
#   NamePrefix - The parameter name prefix, which should be aligned separately, if any.
#   SetNamePrefix - Replaces the parameter name prefix.
#   DefaultValue - The default value expression, if any.
#   SetDefaultValue - Replaces the default value expression.
#   DefaultValuePrefix - The default value prefix, which should be aligned separately, if any.
#   SetDefaultValuePrefix - Replaces the default value prefix.
#


1;
