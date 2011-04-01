###############################################################################
#
#   Package: NaturalDocs::Version
#
###############################################################################
#
#   A package for handling version information.  What?  That's right.  Although it should be easy and obvious, version numbers
#   need to be dealt with in a variety of formats, plus there's compatibility with older releases which handled it differently.  I
#   wanted to centralize the code after it started getting complicated.  So there ya go.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Version;


###############################################################################
# Group: Functions


#
#   Function: ToString
#
#   Converts a <VersionInt> to a string.
#
sub ToString #(VersionInt version) => string
    {
    my ($self, $version) = @_;

    my ($major, $minor, $month, $day, $year) = $self->ToValues($version);

    if ($minor % 10 == 0)
        {  $minor /= 10;  };

    if ($day)
        {  return sprintf('Development Release %02d-%02d-%d (%d.%d base)', $month, $day, $year, $major, $minor);  }
    else
        {  return $major . '.' . $minor;  };
    };


#
#   Function: FromString
#
#   Converts a version string to a <VersionInt>.
#
sub FromString #(string string) => VersionInt
    {
    my ($self, $string) = @_;

    if ($string eq '1')
        {
        return $self->FromValues(0, 91, 0, 0, 0);  # 0.91
        }
    else
        {
        my ($major, $minor, $month, $day, $year);

        if ($string =~ /^(\d{1,2})\.(\d{1,2})$/)
            {
            ($major, $minor) = ($1, $2);
            ($month, $day, $year) = (0, 0, 0);
            }
        elsif ($string =~ /^Development Release (\d{1,2})-(\d{1,2})-(\d\d\d\d) \((\d{1,2})\.(\d{1,2}) base\)$/)
            {
            ($month, $day, $year, $major, $minor) = ($1, $2, $3, $4, $5);

            # We have to do sanity checking because these can come from user-editable text files.  The version numbers should
            # already be constrained simply by being forced to have only two digits.

            if ($month > 12 || $month < 1 || $day > 31 || $day < 1 || $year > 2255 || $year < 2000)
                {  die 'The version string ' . $string . " doesn't have a valid date.\n";  };
            }
        else
            {
            die 'The version string ' . $string . " isn't in a recognized format.\n";
            };

        if (length $minor == 1)
            {  $minor *= 10;  };

        return $self->FromValues($major, $minor, $month, $day, $year);
        };
    };


#
#   Function: ToTextFile
#
#   Writes a <VersionInt> to a text file.
#
#   Parameters:
#
#       fileHandle - The handle of the file to write it to.  It should be at the correct location.
#       version - The <VersionInt> to write.
#
sub ToTextFile #(handle fileHandle, VersionInt version)
    {
    my ($self, $fileHandle, $version) = @_;

    print $fileHandle $self->ToString($version) . "\n";
    };


#
#   Function: ToBinaryFile
#
#   Writes a <VersionInt> to a binary file.
#
#   Parameters:
#
#       fileHandle - The handle of the file to write it to.  It should be at the correct location.
#       version - The <VersionInt> to write.
#
sub ToBinaryFile #(handle fileHandle, VersionInt version)
    {
    my ($self, $fileHandle, $version) = @_;

    my ($major, $minor, $month, $day, $year) = $self->ToValues($version);

    # 1.35 development releases are encoded as 1.36.  Everything else is literal.
    if ($day && $major == 1 && $minor == 35)
        {  $minor = 36;  };

    print $fileHandle pack('CC', $major, $minor);

    # Date fields didn't exist with 1.35 stable and earlier.  1.35 development releases are encoded as 1.36, so this works.
    if ($major > 1 || ($major == 1 && $minor > 35))
        {
        if ($day)
            {  $year -= 2000;  };

        print $fileHandle pack('CCC', $month, $day, $year);
        };
    };


#
#   Function: FromBinaryFile
#
#   Retrieves a <VersionInt> from a binary file.
#
#   Parameters:
#
#       fileHandle - The handle of the file to read it from.  It should be at the correct location.
#
#   Returns:
#
#       The <VersionInt>.
#
sub FromBinaryFile #(handle fileHandle) => VersionInt
    {
    my ($self, $fileHandle) = @_;

    my ($major, $minor, $month, $day, $year);

    my $raw;
    read($fileHandle, $raw, 2);

    ($major, $minor) = unpack('CC', $raw);

    # 1.35 stable is the last release without the date fields.  1.35 development releases are encoded as 1.36, so this works.
    if ($major > 1 || ($major == 1 && $minor > 35))
        {
        read($fileHandle, $raw, 3);
        ($month, $day, $year) = unpack('CCC', $raw);

        if ($day)
            {  $year += 2000;  };
        }
    else
        {  ($month, $day, $year) = (0, 0, 0);  };

    # Fix the 1.35 development release special encoding.
    if ($major == 1 && $minor == 36)
        {  $minor = 35;  };


    return $self->FromValues($major, $minor, $month, $day, $year);
    };


#
#   Function: ToValues
#
#   Converts a <VersionInt> to the array ( major, minor, month, day, year ).  The minor version will be in two digit form, so x.2
#   will return 20.  The date fields will be zero for stable releases.
#
sub ToValues #(VersionInt version) => ( int, int, int, int, int )
    {
    my ($self, $version) = @_;

    my $major = ($version & 0x00003F80) >> 7;
    my $minor = ($version & 0x0000007F);
    my $month = ($version & 0x00780000) >> 19;
    my $day = ($version & 0x0007C000) >> 14;
    my $year = ($version & 0x7F800000) >> 23;

    if ($year)
        {  $year += 2000;  };

    return ( $major, $minor, $month, $day, $year );
    };


#
#   Function: FromValues
#
#   Returns a <VersionInt> created from the passed values.
#
#   Parameters:
#
#       major - The major version number.  For development releases, it should be the stable version it's based off of.
#       minor - The minor version number.  It should always be two digits, so x.2 should pass 20.  For development
#                  releases, it should be the stable version it's based off of.
#       month - The numeric month of the development release.  For stable releases it should be zero.
#       day - The day of the development release.  For stable releases it should be zero.
#       year - The year of the development release.  For stable releases it should be zero.
#
#   Returns:
#
#       The <VersionInt>.
#
sub FromValues #(int major, int minor, int month, int day, int year) => VersionInt
    {
    my ($self, $major, $minor, $month, $day, $year) = @_;

    if ($day)
        {  $year -= 2000;  };

    return ($major << 7) + ($minor) + ($month << 19) + ($day << 14) + ($year << 23);
    };


#
#   Function: CheckFileFormat
#
#   Checks if a file's format is compatible with the current release.
#
#   - If the application is a development release or the file is from one, this only returns true if they are from the exact same
#     development release.
#   - If neither of them are development releases, this only returns true if the file is from a release between the minimum specified
#     and the current version.  If there's no minimum it just checks that it's below the current version.
#
#   Parameters:
#
#       fileVersion - The <VersionInt> of the file format.
#       minimumVersion - The minimum <VersionInt> required of the file format.  May be undef.
#
#   Returns:
#
#       Whether the file's format is compatible per the above rules.
#
sub CheckFileFormat #(VersionInt fileVersion, optional VersionInt minimumVersion) => bool
    {
    my ($self, $fileVersion, $minimumVersion) = @_;

    my $appVersion = NaturalDocs::Settings->AppVersion();

    if ($self->IsDevelopmentRelease($appVersion) || $self->IsDevelopmentRelease($fileVersion))
        {  return ($appVersion == $fileVersion);  }
    elsif ($minimumVersion && $fileVersion < $minimumVersion)
        {  return 0;  }
    else
        {  return ($fileVersion <= $appVersion);  };
    };


#
#   Function: IsDevelopmentRelease
#
#   Returns whether the passed <VersionInt> is for a development release.
#
sub IsDevelopmentRelease #(VersionInt version) => bool
    {
    my ($self, $version) = @_;

    # Return if any of the date fields are set.
    return ($version & 0x7FFFC000);
    };



###############################################################################
# Group: Implementation

#
#   About: String Format
#
#   Full Releases:
#
#       Full releases are in the common major.minor format.  Either part can be up to two digits.  The minor version is interpreted
#       as decimal places, so 1.3 > 1.22.  There are no leading or trailing zeroes.
#
#   Development Releases:
#
#       Development releases are in the format "Development Release mm-dd-yyyy (vv.vv base)" where vv.vv is the version
#       number of the full release it's based off of.  The month and day will have leading zeroes where applicable.  Example:
#       "Development Release 07-09-2006 (1.35 base)".
#
#   0.91 and Earlier:
#
#       Text files from releases prior to 0.95 had a separate file format version number that was used instead of the application
#       version.  These were never changed between 0.85 and 0.91, so they are simply "1".  Text version numbers that are "1"
#       instead of "1.0" will be interpreted as 0.91.
#

#
#   About: Integer Format
#
#   <VersionInts> are 32-bit values with the bit distribution below.
#
#   > s yyyyyyyy mmmm ddddd vvvvvvv xxxxxxx
#   > [syyy|yyyy] [ymmm|mddd] [ddvv|vvvv] [vxxx|xxxx]
#
#   s - The sign bit.  Always zero, so it's always interpreted as positive.
#   y - The year bits if it's a development release, zero otherwise.  2000 is added to the value, so the range is from 2000 to 2255.
#   m - The month bits if it's a development release, zero otherwise.
#   d - The day bits if it's a development release, zero otherwise.
#   v - The major version bits.  For development releases, it's the last stable version it was based off of.
#   x - The minor version bits.  It's always stored as two decimals, so x.2 would store 20 here.  For development releases, it's the
#        last stable version it was based off of.
#
#   It's stored with the development release date at a higher significance than the version because we want a stable release to
#   always treat a development release as higher than itself, and thus not attempt to read any of the data files.  I'm not tracking
#   data file formats at the development release level.
#

#
#   About: Binary File Format
#
#   Current:
#
#       Five 8-bit unsigned values, appearing major, minor, month, day, year.  Minor is always stored with two digits, so x.2 would
#       store 20.  Year is stored minus 2000, so 2006 is stored 6.  Stable releases store zero for all the date fields.
#
#   1.35 Development Releases:
#
#       1.35-based development releases are stored the same as current releases, but with 1.36 as the version number.  This is
#       done so previous versions of Natural Docs that didn't include the date fields would still know it's a higher version.  There is
#       no actual 1.36 release.
#
#   1.35 and Earlier:
#
#       Two 8-bit unsigned values, appearing major then minor.  Minor is always stored with two digits, so x.2 would store 20.
#

#
#   About: Text File Format
#
#   In text files, versions are the <String Format> followed by a native line break.
#


1;
