###############################################################################
#
#   Package: NaturalDocs::ImageReferenceTable::Reference
#
###############################################################################
#
#   A class for references being tracked in <NaturalDocs::SourceDB>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::ImageReferenceTable::Reference;

use base 'NaturalDocs::SourceDB::Item';


use NaturalDocs::DefineMembers 'TARGET', 'Target()', 'SetTarget()',
                                                 'NEEDS_REBUILD', 'NeedsRebuild()', 'SetNeedsRebuild()';


#
#   Variables: Members
#
#   The following constants are indexes into the object array.
#
#   TARGET - The image <FileName> this reference resolves to, or undef if none.
#


#
#   Functions: Member Functions
#
#   Target - Returns the image <FileName> this reference resolves to, or undef if none.
#   SetTarget - Replaces the image <FileName> this reference resolves to, or undef if none.
#


1;
