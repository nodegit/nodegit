###############################################################################
#
#   Package: NaturalDocs::Constants
#
###############################################################################
#
#   Constants that are used throughout the script.  All are exported by default.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Constants;

use vars qw(@EXPORT @ISA);
require Exporter;
@ISA = qw(Exporter);

@EXPORT = ('MENU_TITLE', 'MENU_SUBTITLE', 'MENU_FILE', 'MENU_GROUP', 'MENU_TEXT', 'MENU_LINK', 'MENU_FOOTER',
                   'MENU_INDEX', 'MENU_FORMAT', 'MENU_ENDOFORIGINAL', 'MENU_DATA',

                   'MENU_FILE_NOAUTOTITLE', 'MENU_GROUP_UPDATETITLES', 'MENU_GROUP_UPDATESTRUCTURE',
                   'MENU_GROUP_UPDATEORDER', 'MENU_GROUP_HASENDOFORIGINAL',
                   'MENU_GROUP_UNSORTED', 'MENU_GROUP_FILESSORTED',
                   'MENU_GROUP_FILESANDGROUPSSORTED', 'MENU_GROUP_EVERYTHINGSORTED',
                   'MENU_GROUP_ISINDEXGROUP',

                   'FILE_NEW', 'FILE_CHANGED', 'FILE_SAME', 'FILE_DOESNTEXIST');

#
#   Topic: Assumptions
#
#   - No constant here will ever be zero.
#   - All constants are exported by default.
#


###############################################################################
# Group: Virtual Types
# These are only groups of constants, but should be treated like typedefs or enums.  Each one represents a distinct type and
# their values should only be one of their constants or undef.


#
#   Constants: MenuEntryType
#
#   The types of entries that can appear in the menu.
#
#       MENU_TITLE         - The title of the menu.
#       MENU_SUBTITLE   - The sub-title of the menu.
#       MENU_FILE           - A source file, relative to the source directory.
#       MENU_GROUP       - A group.
#       MENU_TEXT          - Arbitrary text.
#       MENU_LINK           - A web link.
#       MENU_FOOTER      - Footer text.
#       MENU_INDEX        - An index.
#       MENU_FORMAT     - The version of Natural Docs the menu file was generated with.
#       MENU_ENDOFORIGINAL - A dummy entry that marks where the original group content ends.  This is used when automatically
#                                           changing the groups so that the alphabetization or lack thereof can be detected without being
#                                           affected by new entries tacked on to the end.
#       MENU_DATA - Data not meant for user editing.
#
#   Dependency:
#
#       <PreviousMenuState.nd> depends on these values all being able to fit into a UInt8, i.e. <= 255.
#
use constant MENU_TITLE => 1;
use constant MENU_SUBTITLE => 2;
use constant MENU_FILE => 3;
use constant MENU_GROUP => 4;
use constant MENU_TEXT => 5;
use constant MENU_LINK => 6;
use constant MENU_FOOTER => 7;
use constant MENU_INDEX => 8;
use constant MENU_FORMAT => 9;
use constant MENU_ENDOFORIGINAL => 10;
use constant MENU_DATA => 11;


#
#   Constants: FileStatus
#
#   What happened to a file since Natural Docs' last execution.
#
#       FILE_NEW                - The file has been added since the last run.
#       FILE_CHANGED        - The file has been modified since the last run.
#       FILE_SAME               - The file hasn't been modified since the last run.
#       FILE_DOESNTEXIST  - The file doesn't exist, or was deleted.
#
use constant FILE_NEW => 1;
use constant FILE_CHANGED => 2;
use constant FILE_SAME => 3;
use constant FILE_DOESNTEXIST => 4;



###############################################################################
# Group: Flags
# These constants can be combined with each other.


#
#   Constants: Menu Entry Flags
#
#   The various flags that can apply to a menu entry.  You cannot mix flags of different types, since they may overlap.
#
#   File Flags:
#
#       MENU_FILE_NOAUTOTITLE - Whether the file is auto-titled or not.
#
#   Group Flags:
#
#       MENU_GROUP_UPDATETITLES - The group should have its auto-titles regenerated.
#       MENU_GROUP_UPDATESTRUCTURE - The group should be checked for structural changes, such as being removed or being
#                                                             split into subgroups.
#       MENU_GROUP_UPDATEORDER - The group should be resorted.
#
#       MENU_GROUP_HASENDOFORIGINAL - Whether the group contains a dummy <MENU_ENDOFORIGINAL> entry.
#       MENU_GROUP_ISINDEXGROUP - Whether the group is used primarily for <MENU_INDEX> entries.  <MENU_TEXT> entries
#                                                       are tolerated.
#
#       MENU_GROUP_UNSORTED - The group's contents are not sorted.
#       MENU_GROUP_FILESSORTED - The group's files are sorted alphabetically.
#       MENU_GROUP_FILESANDGROUPSSORTED - The group's files and sub-groups are sorted alphabetically.
#       MENU_GROUP_EVERYTHINGSORTED - All entries in the group are sorted alphabetically.
#
use constant MENU_FILE_NOAUTOTITLE => 0x0001;

use constant MENU_GROUP_UPDATETITLES => 0x0001;
use constant MENU_GROUP_UPDATESTRUCTURE => 0x0002;
use constant MENU_GROUP_UPDATEORDER => 0x0004;
use constant MENU_GROUP_HASENDOFORIGINAL => 0x0008;

# This could really be a two-bit field instead of four flags, but it's not worth the effort since it's only used internally.
use constant MENU_GROUP_UNSORTED => 0x0010;
use constant MENU_GROUP_FILESSORTED => 0x0020;
use constant MENU_GROUP_FILESANDGROUPSSORTED => 0x0040;
use constant MENU_GROUP_EVERYTHINGSORTED => 0x0080;

use constant MENU_GROUP_ISINDEXGROUP => 0x0100;




###############################################################################
# Group: Support Functions


#
#   Function: IsClassHierarchyReference
#   Returns whether the passed <ReferenceType> belongs to <NaturalDocs::ClassHierarchy>.
#
sub IsClassHierarchyReference #(reference)
    {
    my ($self, $reference) = @_;
    return ($reference == ::REFERENCE_CH_CLASS() || $reference == ::REFERENCE_CH_PARENT());
    };



1;
