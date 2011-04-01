###############################################################################
#
#   Package: NaturalDocs::NDMarkup
#
###############################################################################
#
#   A package of support functions for dealing with <NDMarkup>.
#
#   Usage and Dependencies:
#
#       The package doesn't depend on any Natural Docs packages and is ready to use right away.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use strict;
use integer;

package NaturalDocs::NDMarkup;

#
#   Function: ConvertAmpChars
#
#   Substitutes certain characters with their <NDMarkup> amp chars.
#
#   Parameters:
#
#       text - The block of text to convert.
#
#   Returns:
#
#       The converted text block.
#
sub ConvertAmpChars #(text)
    {
    my ($self, $text) = @_;

    $text =~ s/&/&amp;/g;
    $text =~ s/</&lt;/g;
    $text =~ s/>/&gt;/g;
    $text =~ s/\"/&quot;/g;

    return $text;
    };


#
#   Function: RestoreAmpChars
#
#   Replaces <NDMarkup> amp chars with their original symbols.
#
#   Parameters:
#
#       text - The text to restore.
#
#   Returns:
#
#       The restored text.
#
sub RestoreAmpChars #(text)
    {
    my ($self, $text) = @_;

    $text =~ s/&quot;/\"/g;
    $text =~ s/&gt;/>/g;
    $text =~ s/&lt;/</g;
    $text =~ s/&amp;/&/g;

    return $text;
    };


1;
