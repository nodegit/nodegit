###############################################################################
#
#   Package: NaturalDocs::ReferenceString
#
###############################################################################
#
#   A package to manage <ReferenceString> handling throughout the program.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::ReferenceString;

use vars '@ISA', '@EXPORT';
@ISA = 'Exporter';
@EXPORT = ( 'BINARYREF_NOTYPE', 'BINARYREF_NORESOLVINGFLAGS',

                     'REFERENCE_TEXT', 'REFERENCE_CH_CLASS', 'REFERENCE_CH_PARENT',

                     'RESOLVE_RELATIVE', 'RESOLVE_ABSOLUTE', 'RESOLVE_NOPLURAL', 'RESOLVE_NOUSING' );


#
#   Constants: Binary Format Flags
#
#   These flags can be combined to specify the format when using <ToBinaryFile()> and <FromBinaryFile()>.  All are exported
#   by default.
#
#   BINARYREF_NOTYPE - Do not include the <ReferenceType>.
#   BINARYREF_NORESOLVEFLAGS - Do not include the <Resolving Flags>.
#
use constant BINARYREF_NOTYPE => 0x01;
use constant BINARYREF_NORESOLVINGFLAGS => 0x02;


#
#   Constants: ReferenceType
#
#   The type of a reference.
#
#       REFERENCE_TEXT - The reference appears in the text of the documentation.
#       REFERENCE_CH_CLASS - A class reference handled by <NaturalDocs::ClassHierarchy>.
#       REFERENCE_CH_PARENT - A parent class reference handled by <NaturalDocs::ClassHierarchy>.
#
#   Dependencies:
#
#       - <ToBinaryFile()> and <FromBinaryFile()> require that these values fit into a UInt8, i.e. are <= 255.
#
use constant REFERENCE_TEXT => 1;
use constant REFERENCE_CH_CLASS => 2;
use constant REFERENCE_CH_PARENT => 3;


#
#   Constants: Resolving Flags
#
#   Used to influence the method of resolving references in <NaturalDocs::SymbolTable>.
#
#       RESOLVE_RELATIVE - The reference text is truly relative, rather than Natural Docs' semi-relative.
#       RESOLVE_ABSOLUTE - The reference text is always absolute.  No local or relative references.
#       RESOLVE_NOPLURAL - The reference text may not be interpreted as a plural, and thus match singular forms as well.
#       RESOLVE_NOUSING - The reference text may not include "using" statements when being resolved.
#
#       If neither <RESOLVE_RELATIVE> or <RESOLVE_ABSOLUTE> is specified, Natural Docs' semi-relative kicks in instead,
#       which is where links are interpreted as local, then global, then relative.  <RESOLVE_RELATIVE> states that links are
#       local, then relative, then global.
#
#   Dependencies:
#
#       - <ToBinaryFile()> and <FromBinaryFile()> require that these values fit into a UInt8, i.e. are <= 255.
#
use constant RESOLVE_RELATIVE => 0x01;
use constant RESOLVE_ABSOLUTE => 0x02;
use constant RESOLVE_NOPLURAL => 0x04;
use constant RESOLVE_NOUSING => 0x08;


#
#
#   Function: MakeFrom
#
#   Encodes the passed information as a <ReferenceString>.  The format of the string should be treated as opaque.  However, the
#   characteristic you can rely on is that the same string will always be made from the same parameters, and thus it's suitable
#   for comparison and use as hash keys.
#
#   Parameters:
#
#       type - The <ReferenceType>.
#       symbol - The <SymbolString> of the reference.
#       language - The name of the language that defines the file this reference appears in.
#       scope - The scope <SymbolString> the reference appears in, or undef if none.
#       using - An arrayref of scope <SymbolStrings> that are also available for checking due to the equivalent a "using" statement,
#                  or undef if none.
#       resolvingFlags - The <Resolving Flags> to use with this reference.  They are ignored if the type is <REFERENCE_TEXT>.
#
#   Returns:
#
#       The encoded <ReferenceString>.
#
sub MakeFrom #(ReferenceType type, SymbolString symbol, string language, SymbolString scope, SymbolString[]* using, flags resolvingFlags)
    {
    my ($self, $type, $symbol, $language, $scope, $using, $resolvingFlags) = @_;

    if ($type == ::REFERENCE_TEXT() || $resolvingFlags == 0)
       {  $resolvingFlags = undef;  };

    # The format is [type] 0x1E [resolving flags] 0x1E [symbol] 0x1E [scope] ( 0x1E [using] )*
    # If there is no scope and/or using, the separator characters still remain.

    # DEPENDENCY: SymbolString->FromText() removed all 0x1E characters.
    # DEPENDENCY: SymbolString->FromText() doesn't use 0x1E characters in its encoding.

    my $string = $type . "\x1E" . $symbol . "\x1E" . $language . "\x1E" . $resolvingFlags . "\x1E";

    if (defined $scope)
        {
        $string .= $scope;
        };

    $string .= "\x1E";

    if (defined $using)
        {
        $string .= join("\x1E", @$using);
        };

    return $string;
    };


#
#   Function: ToBinaryFile
#
#   Writes a <ReferenceString> to the passed filehandle.  Can also encode an undef.
#
#   Parameters:
#
#       fileHandle - The filehandle to write to.
#       referenceString - The <ReferenceString> to write, or undef.
#       binaryFormatFlags - Any <Binary Format Flags> you want to use to influence encoding.
#
#   Format:
#
#       > [SymbolString: Symbol or undef for an undef reference]
#       > [AString16: language]
#       > [SymbolString: Scope or undef for none]
#       >
#       > [SymbolString: Using or undef for none]
#       > [SymbolString: Using or undef for no more]
#       > ...
#       >
#       > [UInt8: Type unless BINARYREF_NOTYPE is set]
#       > [UInt8: Resolving Flags unless BINARYREF_NORESOLVINGFLAGS is set]
#
#   Dependencies:
#
#       - <ReferenceTypes> must fit into a UInt8.  All values must be <= 255.
#       - All <Resolving Flags> must fit into a UInt8.  All values must be <= 255.
#
sub ToBinaryFile #(FileHandle fileHandle, ReferenceString referenceString, flags binaryFormatFlags)
    {
    my ($self, $fileHandle, $referenceString, $binaryFormatFlags) = @_;

    my ($type, $symbol, $language, $scope, $using, $resolvingFlags) = $self->InformationOf($referenceString);

    # [SymbolString: Symbol or undef for an undef reference]

    NaturalDocs::SymbolString->ToBinaryFile($fileHandle, $symbol);

    # [AString16: language]

    print $fileHandle pack('nA*', length $language, $language);

    # [SymbolString: scope or undef if none]

    NaturalDocs::SymbolString->ToBinaryFile($fileHandle, $scope);

    # [SymbolString: using or undef if none/no more] ...

    if (defined $using)
        {
        foreach my $usingScope (@$using)
            {  NaturalDocs::SymbolString->ToBinaryFile($fileHandle, $usingScope);  };
        };

    NaturalDocs::SymbolString->ToBinaryFile($fileHandle, undef);

    # [UInt8: Type unless BINARYREF_NOTYPE is set]

    if (!($binaryFormatFlags & BINARYREF_NOTYPE))
        {  print $fileHandle pack('C', $type);  };

    # [UInt8: Resolving Flags unless BINARYREF_NORESOLVINGFLAGS is set]

    if (!($binaryFormatFlags & BINARYREF_NORESOLVINGFLAGS))
        {  print $fileHandle pack('C', $type);  };
    };


#
#   Function: FromBinaryFile
#
#   Reads a <ReferenceString> or undef from the passed filehandle.
#
#   Parameters:
#
#       fileHandle - The filehandle to read from.
#       binaryFormatFlags - Any <Binary Format Flags> you want to use to influence decoding.
#       type - The <ReferenceType> to use if <BINARYREF_NOTYPE> is set.
#       resolvingFlags - The <Resolving Flags> to use if <BINARYREF_NORESOLVINGFLAGS> is set.
#
#   Returns:
#
#       The <ReferenceString> or undef.
#
#   See Also:
#
#       See <ToBinaryFile()> for format and dependencies.
#
sub FromBinaryFile #(FileHandle fileHandle, flags binaryFormatFlags, ReferenceType type, flags resolvingFlags)
    {
    my ($self, $fileHandle, $binaryFormatFlags, $type, $resolvingFlags) = @_;
    my $raw;

    # [SymbolString: Symbol or undef for an undef reference]

    my $symbol = NaturalDocs::SymbolString->FromBinaryFile($fileHandle);

    if (!defined $symbol)
        {  return undef;  };


    # [AString16: language]

    read($fileHandle, $raw, 2);
    my $languageLength = unpack('n', $raw);

    my $language;
    read($fileHandle, $language, $languageLength);


    # [SymbolString: scope or undef if none]

    my $scope = NaturalDocs::SymbolString->FromBinaryFile($fileHandle);

    # [SymbolString: using or undef if none/no more] ...

    my $usingSymbol;
    my @using;

    while ($usingSymbol = NaturalDocs::SymbolString->FromBinaryFile($fileHandle))
        {  push @using, $usingSymbol;  };

    if (scalar @using)
        {  $usingSymbol = \@using;  }
    else
        {  $usingSymbol = undef;  };

    # [UInt8: Type unless BINARYREF_NOTYPE is set]

    if (!($binaryFormatFlags & BINARYREF_NOTYPE))
        {
        my $raw;
        read($fileHandle, $raw, 1);
        $type = unpack('C', $raw);
        };

    # [UInt8: Resolving Flags unless BINARYREF_NORESOLVINGFLAGS is set]

    if (!($binaryFormatFlags & BINARYREF_NORESOLVINGFLAGS))
        {
        my $raw;
        read($fileHandle, $raw, 1);
        $resolvingFlags = unpack('C', $raw);
        };

    return $self->MakeFrom($type, $symbol, $language, $scope, $usingSymbol, $resolvingFlags);
    };


#
#   Function: InformationOf
#
#   Returns the information encoded in a <ReferenceString>.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to decode.
#
#   Returns:
#
#       The array ( type, symbol, language, scope, using, resolvingFlags ).
#
#       type - The <ReferenceType>.
#       symbol - The <SymbolString>.
#       language - The name of the language that defined the file the reference was defined in.
#       scope - The scope <SymbolString>, or undef if none.
#       using - An arrayref of scope <SymbolStrings> that the reference also has access to via "using" statements, or undef if none.
#       resolvingFlags - The <Resolving Flags> of the reference.
#
sub InformationOf #(ReferenceString referenceString)
    {
    my ($self, $referenceString) = @_;

    my ($type, $symbolString, $language, $resolvingFlags, $scopeString, @usingStrings) = split(/\x1E/, $referenceString);

    if (!length $resolvingFlags)
        {  $resolvingFlags = undef;  };

    return ( $type, $symbolString, $language, $scopeString, [ @usingStrings ], $resolvingFlags );
    };


#
#   Function: TypeOf
#
#   Returns the <ReferenceType> encoded in the reference string.  This is faster than <InformationOf()> if this is
#   the only information you need.
#
sub TypeOf #(ReferenceString referenceString)
    {
    my ($self, $referenceString) = @_;

    $referenceString =~ /^([^\x1E]+)/;
    return $1;
    };


1;
