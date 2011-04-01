###############################################################################
#
#   Class: NaturalDocs::Settings::BuildTarget
#
###############################################################################
#
#   A class that stores information about a build target.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Settings::BuildTarget;

use NaturalDocs::DefineMembers 'BUILDER', 'Builder()', 'SetBuilder()',
                                                 'DIRECTORY', 'Directory()', 'SetDirectory()';


#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref with the members below.
#
#       BUILDER      - The <NaturalDocs::Builder::Base>-derived object for the target's output format.
#       DIRECTORY - The output directory of the target.
#

#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       builder - The <NaturalDocs::Builder::Base>-derived object for the target's output format.
#       directory - The directory to place the output files in.
#
sub New #(builder, directory)
    {
    my ($package, $builder, $directory) = @_;

    my $object = [ ];
    bless $object, $package;

    $object->SetBuilder($builder);
    $object->SetDirectory($directory);

    return $object;
    };


#
#   Functions: Member Functions
#
#   Builder - Returns the <NaturalDocs::Builder::Base>-derived object for the target's output format.
#   SetBuilder - Replaces the <NaturalDocs::Builder::Base>-derived object for the target's output format.
#   Directory - Returns the directory for the target's output files.
#   SetDirectory - Replaces the directory for the target's output files.
#


1;
