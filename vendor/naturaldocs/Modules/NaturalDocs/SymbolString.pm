###############################################################################
#
#   Package: NaturalDocs::SymbolString
#
###############################################################################
#
#   A package to manage <SymbolString> handling throughout the program.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::SymbolString;


#
#   Function: FromText
#
#   Extracts and returns a <SymbolString> from plain text.
#
#   This should be the only way to get a <SymbolString> from plain text, as the splitting and normalization must be consistent
#   throughout the application.
#
sub FromText #(string textSymbol)
    {
    my ($self, $textSymbol) = @_;

    # The internal format of a symbol is all the normalized identifiers separated by 0x1F characters.

    # Convert whitespace and reserved characters to spaces, and condense multiple consecutive ones.
    $textSymbol =~ tr/ \t\r\n\x1C\x1D\x1E\x1F/ /s;

    # DEPENDENCY: ReferenceString->MakeFrom() assumes all 0x1E characters were removed.
    # DEPENDENCY: ReferenceString->MakeFrom() assumes this encoding doesn't use 0x1E characters.

    # Remove spaces unless they're separating two alphanumeric/underscore characters.
    $textSymbol =~ s/^ //;
    $textSymbol =~ s/ $//;
    $textSymbol =~ s/(\W) /$1/g;
    $textSymbol =~ s/ (\W)/$1/g;

    # Remove trailing empty parenthesis, so Function and Function() are equivalent.
    $textSymbol =~ s/\(\)$//;

    # Split the string into pieces.
    my @pieces = split(/(\.|::|->)/, $textSymbol);
    my $symbolString;

    my $lastWasSeparator = 1;

    foreach my $piece (@pieces)
        {
        if ($piece =~ /^(?:\.|::|->)$/)
            {
            if (!$lastWasSeparator)
                {
                $symbolString .= "\x1F";
                $lastWasSeparator = 1;
                };
            }
        elsif (length $piece)
            {
            $symbolString .= $piece;
            $lastWasSeparator = 0;
            };
        # Ignore empty pieces
        };

    $symbolString =~ s/\x1F$//;

    return $symbolString;
    };


#
#   Function: ToText
#
#   Converts a <SymbolString> to text, using the passed separator.
#
sub ToText #(SymbolString symbolString, string separator)
    {
    my ($self, $symbolString, $separator) = @_;

    my @identifiers = $self->IdentifiersOf($symbolString);
    return join($separator, @identifiers);
    };


#
#   Function: ToBinaryFile
#
#   Writes a <SymbolString> to the passed filehandle.  Can also encode an undef.
#
#   Parameters:
#
#       fileHandle - The filehandle to write to.
#       symbol - The <SymbolString> to write, or undef.
#
#   Format:
#
#       > [UInt8: number of identifiers]
#       >    [AString16: identifier] [AString16: identifier] ...
#
#       Undef is represented by a zero for the number of identifiers.
#
sub ToBinaryFile #(FileHandle fileHandle, SymbolString symbol)
    {
    my ($self, $fileHandle, $symbol) = @_;

    my @identifiers;
    if (defined $symbol)
        {  @identifiers = $self->IdentifiersOf($symbol);  };

    print $fileHandle pack('C', scalar @identifiers);

    foreach my $identifier (@identifiers)
        {
        print $fileHandle pack('nA*', length($identifier), $identifier);
        };
    };


#
#   Function: FromBinaryFile
#
#   Loads a <SymbolString> or undef from the filehandle and returns it.
#
#   Parameters:
#
#       fileHandle - The filehandle to read from.
#
#   Returns:
#
#       The <SymbolString> or undef.
#
#   See also:
#
#       See <ToBinaryFile()> for format and dependencies.
#
sub FromBinaryFile #(FileHandle fileHandle)
    {
    my ($self, $fileHandle) = @_;

    my $raw;

    # [UInt8: number of identifiers or 0 if none]

    read($fileHandle, $raw, 1);
    my $identifierCount = unpack('C', $raw);

    my @identifiers;

    while ($identifierCount)
        {
        # [AString16: identifier] [AString16: identifier] ...

        read($fileHandle, $raw, 2);
        my $identifierLength = unpack('n', $raw);

        my $identifier;
        read($fileHandle, $identifier, $identifierLength);

        push @identifiers, $identifier;

        $identifierCount--;
        };

    if (scalar @identifiers)
        {  return $self->Join(@identifiers);  }
    else
        {  return undef;  };
    };


#
#   Function: IdentifiersOf
#
#   Returns the <SymbolString> as an array of identifiers.
#
sub IdentifiersOf #(SymbolString symbol)
    {
    my ($self, $symbol) = @_;
    return split(/\x1F/, $symbol);
    };


#
#   Function: Join
#
#   Takes a list of identifiers and/or <SymbolStrings> and returns it as a new <SymbolString>.
#
sub Join #(string/SymbolString identifier/symbol, string/SymolString identifier/symbol, ...)
    {
    my ($self, @pieces) = @_;

    # Can't have undefs screwing everything up.
    while (scalar @pieces && !defined $pieces[0])
        {  shift @pieces;  };

    # We need to test @pieces first because joining on an empty array returns an empty string rather than undef.
    if (scalar @pieces)
       {  return join("\x1F", @pieces);  }
    else
        {  return undef;  };
    };


1;
