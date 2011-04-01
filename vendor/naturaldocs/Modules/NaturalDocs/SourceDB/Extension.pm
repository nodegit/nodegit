###############################################################################
#
#   Package: NaturalDocs::SourceDB::Extension
#
###############################################################################
#
#   A base package for all <SourceDB> extensions.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::SourceDB::Extension;


###############################################################################
# Group: Interface Functions
# These functions must be overridden by the derived class.


#
#   Function: Register
#
#   Override this function to register the package with <NaturalDocs::SourceDB->RegisterExtension()>.
#
sub Register
    {
    die "Called SourceDB::Extension->Register().  This function should be overridden by every extension.";
    };


#
#   Function: Load
#
#   Called by <NaturalDocs::SourceDB->Load()> to load the extension's data.  Returns whether it was successful.
#
#   *This function might not be called.*  If there's a situation that would cause all the source files to be reparsed anyway,
#   <NaturalDocs::SourceDB> may skip calling Load() for the remaining extensions.  You should *not* depend on this function
#   for any critical initialization that needs to happen every time regardless.
#
sub Load # => bool
    {
    return 1;
    };


#
#   Function: Save
#
#   Called by <NaturalDocs::SourceDB->Save()> to save the extension's data.
#
sub Save
    {
    };


#
#   Function: OnDeletedDefinition
#
#   Called for each definition deleted by <NaturalDocs::SourceDB>.  This is called *after* the definition has been deleted from
#   the database, so don't expect to be able to read it.
#
sub OnDeletedDefinition #(string itemString, FileName file, bool wasLastDefinition)
    {
    };


#
#   Function: OnChangedDefinition
#
#   Called for each definition changed by <NaturalDocs::SourceDB>.  This is called *after* the definition has been changed, so
#   don't expect to be able to read the original value.
#
sub OnChangedDefinition #(string itemString, FileName file)
    {
    };


1;
