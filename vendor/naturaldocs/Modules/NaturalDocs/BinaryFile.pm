###############################################################################
#
#   Package: NaturalDocs::BinaryFile
#
###############################################################################
#
#   A package to manage Natural Docs' binary data files.
#
#   Usage:
#
#       - Only one data file can be managed with this package at a time.  You must close the file before opening another
#         one.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::BinaryFile;

use vars qw(@EXPORT @ISA);
require Exporter;
@ISA = qw(Exporter);

@EXPORT = ('BINARY_FORMAT');


###############################################################################
# Group: Format

#
#   Topic: Standard Header
#
#   > [UInt8: BINARY_FORMAT]
#   > [VersionInt: app version]
#
#   The first byte is <BINARY_FORMAT>, which distinguishes binary configuration files from text ones, since Natural Docs
#   used to use text data files with the same name.
#
#   The next section is the version of Natural Docs that wrote the file, as defined by <NaturalDocs::Settings->AppVersion>
#   and written by <NaturalDocs::Version->ToBinaryFile()>.
#

#
#   Topic: Data Types
#
#   All the integer data types are written most significant byte first, aka big endian.
#
#   An AString16 is a UInt16 followed by that many 8-bit ASCII characters.  It doesn't include a null character at the end.  Undef
#   strings are represented by a zero for the UInt16 and nothing following it.
#

#
#   Constant: BINARY_FORMAT
#
#   An 8-bit constant that's used as the first byte of binary data files.  This is used so that you can easily distinguish between
#   binary and old-style text data files.  It's not a character that would appear in plain text files.
#
use constant BINARY_FORMAT => pack('C', 0x06);
# Which is ACK or acknowledge in ASCII.  Is the cool spade character in DOS displays.


###############################################################################
# Group: Variables

#
#   handle: FH_BINARYDATAFILE
#
#   The file handle used for the data file.
#


#
#   string: currentFile
#
#   The <FileName> for the current configuration file being parsed.
#
my $currentFile;



###############################################################################
# Group: File Functions


#
#   Function: OpenForReading
#
#   Opens a binary file for reading.
#
#   Parameters:
#
#       minimumVersion - The minimum version of the file format that is acceptible.  May be undef.
#
#   Returns:
#
#       The format <VersionInt> or undef if it failed.  It could fail for any of the following reasons.
#
#       - The file doesn't exist.
#       - The file couldn't be opened.
#       - The file didn't have the proper header.
#       - Either the application or the file was from a development release, and they're not the exact same development release.
#       - The file's format was less than the minimum version, if one was defined.
#       - The file was from a later application version than the current.
#
sub OpenForReading #(FileName file, optional VersionInt minimumVersion) => VersionInt
    {
    my ($self, $file, $minimumVersion) = @_;

    if (defined $currentFile)
        {  die "Tried to open binary file " . $file . " for reading when " . $currentFile . " was already open.";  };

    $currentFile = $file;

    if (open(FH_BINARYDATAFILE, '<' . $currentFile))
        {
        # See if it's binary.
        binmode(FH_BINARYDATAFILE);

        my $firstChar;
        read(FH_BINARYDATAFILE, $firstChar, 1);

        if ($firstChar == ::BINARY_FORMAT())
            {
            my $version = NaturalDocs::Version->FromBinaryFile(\*FH_BINARYDATAFILE);

            if (NaturalDocs::Version->CheckFileFormat($version, $minimumVersion))
                {  return $version;  };
            };

        close(FH_BINARYDATAFILE);
        };

    $currentFile = undef;
    return undef;
    };


#
#   Function: OpenForWriting
#
#   Opens a binary file for writing and writes the standard header.  Dies if the file cannot be opened.
#
sub OpenForWriting #(FileName file)
    {
    my ($self, $file) = @_;

    if (defined $currentFile)
        {  die "Tried to open binary file " . $file . " for writing when " . $currentFile . " was already open.";  };

    $currentFile = $file;

    open (FH_BINARYDATAFILE, '>' . $currentFile)
        or die "Couldn't save " . $file . ".\n";

    binmode(FH_BINARYDATAFILE);

    print FH_BINARYDATAFILE '' . ::BINARY_FORMAT();
    NaturalDocs::Version->ToBinaryFile(\*FH_BINARYDATAFILE, NaturalDocs::Settings->AppVersion());
    };


#
#   Function: Close
#
#   Closes the current configuration file.
#
sub Close
    {
    my $self = shift;

    if (!$currentFile)
        {  die "Tried to close a binary file when one wasn't open.";  };

    close(FH_BINARYDATAFILE);
    $currentFile = undef;
    };



###############################################################################
# Group: Reading Functions


#
#   Function: GetUInt8
#   Reads and returns a UInt8 from the open file.
#
sub GetUInt8 # => UInt8
    {
    my $raw;
    read(FH_BINARYDATAFILE, $raw, 1);

    return unpack('C', $raw);
    };

#
#   Function: GetUInt16
#   Reads and returns a UInt16 from the open file.
#
sub GetUInt16 # => UInt16
    {
    my $raw;
    read(FH_BINARYDATAFILE, $raw, 2);

    return unpack('n', $raw);
    };

#
#   Function: GetUInt32
#   Reads and returns a UInt32 from the open file.
#
sub GetUInt32 # => UInt32
    {
    my $raw;
    read(FH_BINARYDATAFILE, $raw, 4);

    return unpack('N', $raw);
    };

#
#   Function: GetAString16
#   Reads and returns an AString16 from the open file.  Supports undef strings.
#
sub GetAString16 # => string
    {
    my $rawLength;
    read(FH_BINARYDATAFILE, $rawLength, 2);
    my $length = unpack('n', $rawLength);

    if (!$length)
        {  return undef;  };

    my $string;
    read(FH_BINARYDATAFILE, $string, $length);

    return $string;
    };



###############################################################################
# Group: Writing Functions


#
#   Function: WriteUInt8
#   Writes a UInt8 to the open file.
#
sub WriteUInt8 #(UInt8 value)
    {
    my ($self, $value) = @_;
    print FH_BINARYDATAFILE pack('C', $value);
    };

#
#   Function: WriteUInt16
#   Writes a UInt32 to the open file.
#
sub WriteUInt16 #(UInt16 value)
    {
    my ($self, $value) = @_;
    print FH_BINARYDATAFILE pack('n', $value);
    };

#
#   Function: WriteUInt32
#   Writes a UInt32 to the open file.
#
sub WriteUInt32 #(UInt32 value)
    {
    my ($self, $value) = @_;
    print FH_BINARYDATAFILE pack('N', $value);
    };

#
#   Function: WriteAString16
#   Writes an AString16 to the open file.  Supports undef strings.
#
sub WriteAString16 #(string value)
    {
    my ($self, $string) = @_;

    if (length($string))
        {  print FH_BINARYDATAFILE pack('nA*', length($string), $string);  }
    else
        {  print FH_BINARYDATAFILE pack('n', 0);  };
    };


1;
