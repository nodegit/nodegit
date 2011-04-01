###############################################################################
#
#   Package: NaturalDocs::ImageReferenceTable::String
#
###############################################################################
#
#   A package for creating and managing <ImageReferenceStrings>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::ImageReferenceTable::String;


#
#   Type: ImageReferenceString
#
#   A string representing a unique image reference.  It's composed of the reference text and the directory of the source file.
#   The source file name itself isn't included because two files in the same directory with the same reference text will always go
#   to the same targets.
#


#
#   Function: Make
#
#   Converts a source <FileName> and the reference text to an <ImageReferenceString>.
#
sub Make #(FileName sourceFile, string text) => ImageReferenceString
    {
    my ($self, $sourceFile, $text) = @_;

    my $path = NaturalDocs::File->NoFileName($sourceFile);

    # Condense whitespace and remove any separator characters.
    $path =~ tr/ \t\r\n\x1C/ /s;
    $text =~ tr/ \t\r\n\x1C/ /s;

    return $path . "\x1C" . $text;
    };


#
#   Function: InformationOf
#
#   Returns the information contained in the <ImageReferenceString> as the array ( path, text ).
#
sub InformationOf #(ImageReferenceString referenceString)
    {
    my ($self, $referenceString) = @_;
    return split(/\x1C/, $referenceString);
    };


#
#   Function: ToBinaryFile
#
#   Writes an <ImageReferenceString> to <NaturalDocs::BinaryFile>.  Can also encode an undef.
#
#   Format:
#
#       > [AString16: path] [AString16: reference text] ...
#
#       Undef is represented by the first AString16 being undef.
#
sub ToBinaryFile #(ImageReferenceString referenceString)
    {
    my ($self, $referenceString) = @_;

    if (defined $referenceString)
        {
        my ($path, $text) = split(/\x1C/, $referenceString);

        NaturalDocs::BinaryFile->WriteAString16($path);
        NaturalDocs::BinaryFile->WriteAString16($text);
        }
    else
        {
        NaturalDocs::BinaryFile->WriteAString16(undef);
        };
    };


#
#   Function: FromBinaryFile
#
#   Loads an <ImageReferenceString> or undef from <NaturalDocs::BinaryFile> and returns it.
#
sub FromBinaryFile
    {
    my $self = shift;

    my $path = NaturalDocs::BinaryFile->GetAString16();

    if (!defined $path)
        {  return undef;  };

    my $text = NaturalDocs::BinaryFile->GetAString16();

    return $path . "\x1C" . $text;
    };


1;
