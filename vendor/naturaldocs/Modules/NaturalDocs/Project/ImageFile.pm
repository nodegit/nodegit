###############################################################################
#
#   Class: NaturalDocs::Project::ImageFile
#
###############################################################################
#
#   A simple information class about project image files.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Project::ImageFile;



###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are used as indexes.
#
#       LAST_MODIFIED - The integer timestamp of when the file was last modified.
#       STATUS - <FileStatus> since the last build.
#       REFERENCE_COUNT - The number of references to the image from the source files.
#       WAS_USED - Whether the image was used the last time Natural Docs was run.
#       WIDTH - The image width.  Undef if can't be determined, -1 if haven't attempted to determine yet.
#       HEIGHT - The image height.  Undef if can't be determined, -1 if haven't attempted to determine yet.
#

use NaturalDocs::DefineMembers 'LAST_MODIFIED', 'LastModified()', 'SetLastModified()',
                                                 'STATUS', 'Status()', 'SetStatus()',
                                                 'REFERENCE_COUNT', 'ReferenceCount()',
                                                 'WAS_USED', 'WasUsed()', 'SetWasUsed()',
                                                 'WIDTH', 'Width()',
                                                 'HEIGHT', 'Height()';


#
#   Topic: WasUsed versus References
#
#   <WasUsed()> is a simple true/false that notes whether this image file was used the last time Natural Docs was run.
#   <ReferenceCount()> is a counter for the number of times it's used *this* run.  As such, it starts at zero regardless of whether
#   <WasUsed()> is set or not.
#


###############################################################################
# Group: Functions

#
#   Function: New
#
#   Creates and returns a new file object.
#
#   Parameters:
#
#       lastModified - The image file's last modification timestamp
#       status - The <FileStatus>.
#       wasUsed - Whether this image file was used the *last* time Natural Docs was run.
#
sub New #(timestamp lastModified, FileStatus status, bool wasUsed)
    {
    my ($package, $lastModified, $status, $width, $height, $wasUsed) = @_;

    my $object = [ ];
    $object->[LAST_MODIFIED] = $lastModified;
    $object->[STATUS] = $status;
    $object->[REFERENCE_COUNT] = 0;
    $object->[WAS_USED] = $wasUsed;
    $object->[WIDTH] = -1;
    $object->[HEIGHT] = -1;

    bless $object, $package;

    return $object;
    };


#
#   Functions: Member Functions
#
#   LastModified - Returns the integer timestamp of when the file was last modified.
#   SetLastModified - Sets the file's last modification timestamp.
#   Status - Returns the <FileStatus> since the last build.
#   SetStatus - Sets the <FileStatus> since the last build.
#

#
#   Function: ReferenceCount
#   Returns the current number of references to this image file during *this* Natural Docs execution.
#

#
#   Function: AddReference
#   Increases the number of references to this image file by one.  Returns the new reference count.
#
sub AddReference
    {
    my $self = shift;

    $self->[REFERENCE_COUNT]++;
    return $self->[REFERENCE_COUNT];
    };

#
#   Function: DeleteReference
#   Decreases the number of references to this image file by one.  Returns the new reference count.
#
sub DeleteReference
    {
    my $self = shift;
    $self->[REFERENCE_COUNT]--;

    if ($self->[REFERENCE_COUNT] < 0)
        {  die "Deleted more references to an image file than existed.";  };

    return $self->[REFERENCE_COUNT];
    };


#
#   Functions: Member Functions
#
#   WasUsed - Returns whether this image file was used during the *last* Natural Docs execution.
#   SetWasUsed - Sets whether this image file was used during the *last* Natural Docs execution.
#   Width - Returns the width in pixels, undef if it can't be determined, and -1 if determination hasn't been attempted yet.
#   Height - Returns the width in pixels, undef if it can't be determined, and -1 if determination hasn't been attempted yet.
#


#
#   Function: SetDimensions
#   Sets the width and height of the image.  Set to undef if they can't be determined.
#
sub SetDimensions #(int width, int height)
    {
    my ($self, $width, $height) = @_;

    # If either are undef, both should be undef.  This will also convert zeroes to undef.
    if (!$width || !$height)
        {
        $self->[WIDTH] = undef;
        $self->[HEIGHT] = undef;
        }
    else
        {
        $self->[WIDTH] = $width;
        $self->[HEIGHT] = $height;
        };
    };


1;
