###############################################################################
#
#   Package: NaturalDocs::SourceDB::ItemDefinition
#
###############################################################################
#
#   A base class for all item definitions for extensions that track more than existence.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::SourceDB::ItemDefinition;


#
#   Function: Compare
#
#   Returns whether the definitions are equal.  This version returns true by default, you must override it in your subclasses
#   to make the results relevant.  This is important for <NaturalDocs::SourceDB->AnalyzeTrackedFileChanges()>.
#
#   This will only be called between objects of the same <ExtensionID>.  If you use multiple derived classes for the same
#   <ExtensionID>, you will have to take that into account yourself.
#
#   Parameters:
#
#       other - Another <NaturalDocs::SourceDB::ItemDefinition>-derived object to compare this one to.  It will always be from
#                  the same <ExtensionID>.
#
#   Returns:
#
#       Whether they are equal.
#
sub Compare #(other)
    {
    return 1;
    };


1;
