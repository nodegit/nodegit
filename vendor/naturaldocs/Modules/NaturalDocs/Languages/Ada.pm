###############################################################################
#
#   Class: NaturalDocs::Languages::Ada
#
###############################################################################
#
#   A subclass to handle the language variations of Ada
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Ada;

use base 'NaturalDocs::Languages::Simple';


#
#   Function: ParseParameterLine
#   Overridden because Ada uses Pascal-style parameters
#
sub ParseParameterLine #(...)
    {
    my ($self, @params) = @_;
    return $self->SUPER::ParsePascalParameterLine(@params);
    };

sub TypeBeforeParameter
    {
    return 0;
    };


1;
