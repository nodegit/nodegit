###############################################################################
#
#   Class: NaturalDocs::SymbolTable::IndexElement
#
###############################################################################
#
#   A class representing part of an indexed symbol.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use Tie::RefHash;

use strict;
use integer;


package NaturalDocs::SymbolTable::IndexElement;


#
#   Topic: How IndexElements Work
#
#   This is a little tricky, so make sure you understand this.  Indexes are sorted by symbol, then packages, then file.  If there is only
#   one package for a symbol, or one file definition for a package/symbol, they are added inline to the entry.  However, if there are
#   multiple packages or files, the function for it returns an arrayref of IndexElements instead.  Which members are defined and
#   undefined should follow common sense.  For example, if a symbol is defined in multiple packages, the symbol's IndexElement
#   will not define <File()>, <Type()>, or <Prototype()>; those will be defined in child elements.  Similarly, the child elements will
#   not define <Symbol()> since it's redundant.
#
#   Diagrams may be clearer.  If a member isn't listed for an element, it isn't defined.
#
#   A symbol that only has one package and file:
#   > [Element]
#   > - Symbol
#   > - Package
#   > - File
#   > - Type
#   > - Prototype
#   > - Summary
#
#   A symbol that is defined by multiple packages, each with only one file:
#   > [Element]
#   > - Symbol
#   > - Package
#   >     [Element]
#   >     - Package
#   >     - File
#   >     - Type
#   >     - Prototype
#   >     - Summary
#   >     [Element]
#   >     - ...
#
#   A symbol that is defined by one package, but has multiple files
#   > [Element]
#   > - Symbol
#   > - Package
#   > - File
#   >    [Element]
#   >    - File
#   >    - Type
#   >    - Protype
#   >    - Summary
#   >    [Element]
#   >    - ...
#
#   A symbol that is defined by multiple packages which have multiple files:
#   > [Element]
#   > - Symbol
#   > - Package
#   >    [Element]
#   >    - Package
#   >    - File
#   >      [Element]
#   >      - File
#   >      - Type
#   >      - Prototype
#   >      - Summary
#   >      [Element]
#   >      - ...
#   >    [Element]
#   >    - ...
#
#   Why is it done this way?:
#
#   Because it makes it easier to generate nice indexes since all the splitting and combining is done for you.  If a symbol
#   has only one package, you just want to link to it, you don't want to break out a subindex for just one package.  However, if
#   it has multiple package, you do want the subindex and to link to each one individually.  Use <HasMultiplePackages()> and
#   <HasMultipleFiles()> to determine whether you need to add a subindex for it.
#
#
#   Combining Properties:
#
#   All IndexElements also have combining properties set.
#
#   CombinedType - The general <TopicType> of the entry.  Conflicts combine into <TOPIC_GENERAL>.
#   PackageSeparator - The package separator symbol of the entry.  Conflicts combine into a dot.
#
#   So if an IndexElement only has one definition, <CombinedType()> is the same as the <TopicType> and <PackageSeparator()>
#   is that of the definition's language.  If other definitions are added and they have the same properties, the combined properties
#   will remain the same.  However, if they're different, they switch values as noted above.
#
#
#   Sortable Symbol:
#
#   <SortableSymbol()> is a pseudo-combining property.  There were a few options for dealing with multiple languages defining
#   the same symbol but stripping different prefixes off it, but ultimately I decided to go with whatever the language does that
#   has the most definitions.  There's not likely to be many conflicts here in the real world; probably the only thing would be
#   defining it in a text file and forgetting to specify the prefixes to strip there too.  So this works.
#
#   Ties are broken pretty much randomly, except that text files always lose if its one of the options.
#
#   It's a pseudo-combining property because it's done after the IndexElements are all filled in and only stored in the top-level
#   ones.
#


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The class is implemented as a blessed arrayref.  The following constants are its members.
#
#   SYMBOL - The <SymbolString> without the package portion.
#   PACKAGE - The package <SymbolString>.  Will be a package <SymbolString>, undef for global, or an arrayref of
#                    <NaturalDocs::SymbolTable::IndexElement> objects if multiple packages define the symbol.
#   FILE - The <FileName> the package/symbol is defined in.  Will be the file name or an arrayref of
#            <NaturalDocs::SymbolTable::IndexElements> if multiple files define the package/symbol.
#   TYPE - The package/symbol/file <TopicType>.
#   PROTOTYPE - The package/symbol/file prototype, or undef if not applicable.
#   SUMMARY - The package/symbol/file summary, or undef if not applicable.
#   COMBINED_TYPE - The combined <TopicType> of the element.
#   PACKAGE_SEPARATOR - The combined package separator symbol of the element.
#   SORTABLE_SYMBOL - The sortable symbol as a text string.
#   IGNORED_PREFIX - The part of the symbol that was stripped off to make the sortable symbol.
#
use NaturalDocs::DefineMembers 'SYMBOL', 'Symbol()',
                                                 'PACKAGE', 'Package()',
                                                 'FILE', 'File()',
                                                 'TYPE', 'Type()',
                                                 'PROTOTYPE', 'Prototype()',
                                                 'SUMMARY', 'Summary()',
                                                 'COMBINED_TYPE', 'CombinedType()',
                                                 'PACKAGE_SEPARATOR', 'PackageSeparator()',
                                                 'SORTABLE_SYMBOL', 'SortableSymbol()',
                                                 'IGNORED_PREFIX', 'IgnoredPrefix()';
# DEPENDENCY: New() depends on the order of these constants and that there is no inheritance..


###############################################################################
# Group: Modification Functions

#
#   Function: New
#
#   Returns a new object.
#
#   This should only be used for creating an entirely new symbol.  You should *not* pass arrayrefs as package or file parameters
#   if you are calling this externally.  Use <Merge()> instead.
#
#   Parameters:
#
#       symbol  - The <SymbolString> without the package portion.
#       package - The package <SymbolString>, or undef for global.
#       file  - The symbol's definition file.
#       type  - The symbol's <TopicType>.
#       prototype  - The symbol's prototype, if applicable.
#       summary  - The symbol's summary, if applicable.
#
#   Optional Parameters:
#
#       These parameters don't need to be specified.  You should ignore them when calling this externally.
#
#       combinedType - The symbol's combined <TopicType>.
#       packageSeparator - The symbol's combined package separator symbol.
#
sub New #(symbol, package, file, type, prototype, summary, combinedType, packageSeparator)
    {
    # DEPENDENCY: This depends on the parameter list being in the same order as the constants.

    my $self = shift;

    my $object = [ @_ ];
    bless $object, $self;

    if (!defined $object->[COMBINED_TYPE])
        {  $object->[COMBINED_TYPE] = $object->[TYPE];  };

    if (!defined $object->[PACKAGE_SEPARATOR])
        {
        if ($object->[TYPE] eq ::TOPIC_FILE())
            {  $object->[PACKAGE_SEPARATOR] = '.';  }
        else
            {
            $object->[PACKAGE_SEPARATOR] = NaturalDocs::Languages->LanguageOf($object->[FILE])->PackageSeparator();
            };
        };

    return $object;
    };


#
#   Function: Merge
#
#   Adds another definition of the same symbol.  Perhaps it has a different package or defining file.
#
#   Parameters:
#
#       package - The package <SymbolString>, or undef for global.
#       file  - The symbol's definition file.
#       type  - The symbol's <TopicType>.
#       prototype  - The symbol's protoype if applicable.
#       summary  - The symbol's summary if applicable.
#
sub Merge #(package, file, type, prototype, summary)
    {
    my ($self, $package, $file, $type, $prototype, $summary) = @_;

    # If there's only one package...
    if (!$self->HasMultiplePackages())
        {
        # If there's one package and it's the same as the new one...
        if ($package eq $self->Package())
            {
            $self->MergeFile($file, $type, $prototype, $summary);
            }

        # If there's one package and the new one is different...
        else
            {
            my $selfDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, $self->Package(), $self->File(),
                                                                                                                 $self->Type(), $self->Prototype(),
                                                                                                                 $self->Summary(), $self->CombinedType(),
                                                                                                                 $self->PackageSeparator());
            my $newDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, $package, $file, $type, $prototype,
                                                                                                                  $summary);

            $self->[PACKAGE] = [ $selfDefinition, $newDefinition ];
            $self->[FILE] = undef;
            $self->[TYPE] = undef;
            $self->[PROTOTYPE] = undef;
            $self->[SUMMARY] = undef;

            if ($newDefinition->Type() ne $self->CombinedType())
                {  $self->[COMBINED_TYPE] = ::TOPIC_GENERAL();  };
            if ($newDefinition->PackageSeparator() ne $self->PackageSeparator())
                {  $self->[PACKAGE_SEPARATOR] = '.';  };
            };
        }

    # If there's more than one package...
    else
        {
        # See if the new package is one of them.
        my $selfPackages = $self->Package();
        my $matchingPackage;

        foreach my $testPackage (@$selfPackages)
            {
            if ($package eq $testPackage->Package())
                {
                $testPackage->MergeFile($file, $type, $prototype, $summary);;
                return;
                };
            };

        my $newDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, $package, $file, $type, $prototype,
                                                                                                              $summary);
        push @{$self->[PACKAGE]}, $newDefinition;

        if ($newDefinition->Type() ne $self->CombinedType())
            {  $self->[COMBINED_TYPE] = ::TOPIC_GENERAL();  };
        if ($newDefinition->PackageSeparator() ne $self->PackageSeparator())
            {  $self->[PACKAGE_SEPARATOR] = '.';  };
        };
    };


#
#   Function: Sort
#
#   Sorts the package and file lists of the symbol.
#
sub Sort
    {
    my $self = shift;

    if ($self->HasMultipleFiles())
        {
        @{$self->[FILE]} = sort { ::StringCompare($a->File(), $b->File()) } @{$self->File()};
        }

    elsif ($self->HasMultiplePackages())
        {
        @{$self->[PACKAGE]} = sort { ::StringCompare( $a->Package(), $b->Package()) } @{$self->[PACKAGE]};

        foreach my $packageElement ( @{$self->[PACKAGE]} )
            {
            if ($packageElement->HasMultipleFiles())
                {  $packageElement->Sort();  };
            };
        };
    };


#
#   Function: MakeSortableSymbol
#
#   Generates <SortableSymbol()> and <IgnoredPrefix()>.  Should only be called after everything is merged.
#
sub MakeSortableSymbol
    {
    my $self = shift;

    my $finalLanguage;

    if ($self->HasMultiplePackages() || $self->HasMultipleFiles())
        {
        # Collect all the files that define this symbol.

        my @files;

        if ($self->HasMultipleFiles())
            {
            my $fileElements = $self->File();

            foreach my $fileElement (@$fileElements)
                {  push @files, $fileElement->File();  };
            }
        else # HasMultiplePackages
            {
            my $packages = $self->Package();

            foreach my $package (@$packages)
                {
                if ($package->HasMultipleFiles())
                    {
                    my $fileElements = $package->File();

                    foreach my $fileElement (@$fileElements)
                        {  push @files, $fileElement->File();  };
                    }
                else
                    {  push @files, $package->File();  };
                };
            };


        # Determine which language defines it the most.

        # Keys are language objects, values are counts.
        my %languages;
        tie %languages, 'Tie::RefHash';

        foreach my $file (@files)
            {
            my $language = NaturalDocs::Languages->LanguageOf($file);

            if (exists $languages{$language})
                {  $languages{$language}++;  }
            else
                {  $languages{$language} = 1;  };
            };

        my $topCount = 0;
        my @topLanguages;

        while (my ($language, $count) = each %languages)
            {
            if ($count > $topCount)
                {
                $topCount = $count;
                @topLanguages = ( $language );
                }
            elsif ($count == $topCount)
                {
                push @topLanguages, $language;
                };
            };

        if (scalar @topLanguages == 1)
            {  $finalLanguage = $topLanguages[0];  }
        else
            {
            if ($topLanguages[0]->Name() ne 'Text File')
                {  $finalLanguage = $topLanguages[0];  }
            else
                {  $finalLanguage = $topLanguages[1];  };
            };
        }

    else # !hasMultiplePackages && !hasMultipleFiles
        {  $finalLanguage = NaturalDocs::Languages->LanguageOf($self->File());  };

    my $textSymbol = NaturalDocs::SymbolString->ToText($self->Symbol(), $self->PackageSeparator());
    my $ignoredPrefixLength = $finalLanguage->IgnoredPrefixLength($textSymbol, $self->CombinedType());

    if ($ignoredPrefixLength)
        {
        $self->[IGNORED_PREFIX] = substr($textSymbol, 0, $ignoredPrefixLength);
        $self->[SORTABLE_SYMBOL] = substr($textSymbol, $ignoredPrefixLength);
        }
    else
        {  $self->[SORTABLE_SYMBOL] = $textSymbol;  };
    };



###############################################################################
#
#   Functions: Information Functions
#
#   Symbol - Returns the <SymbolString> without the package portion.
#   Package - If <HasMultiplePackages()> is true, returns an arrayref of <NaturalDocs::SymbolTable::IndexElement> objects.
#                  Otherwise returns the package <SymbolString>, or undef if global.
#   File - If <HasMultipleFiles()> is true, returns an arrayref of <NaturalDocs::SymbolTable::IndexElement> objects.  Otherwise
#           returns the name of the definition file.
#   Type - Returns the <TopicType> of the package/symbol/file, if applicable.
#   Prototype - Returns the prototype of the package/symbol/file, if applicable.
#   Summary - Returns the summary of the package/symbol/file, if applicable.
#   CombinedType - Returns the combined <TopicType> of the element.
#   PackageSeparator - Returns the combined package separator symbol of the element.
#   SortableSymbol - Returns the sortable symbol as a text string.  Only available after calling <MakeSortableSymbol()>.
#   IgnoredPrefix - Returns the part of the symbol that was stripped off to make the <SortableSymbol()>, or undef if none.
#                          Only available after calling <MakeSortableSymbol()>.
#

#   Function: HasMultiplePackages
#   Returns whether <Packages()> is broken out into more elements.
sub HasMultiplePackages
    {  return ref($_[0]->[PACKAGE]);  };

#   Function: HasMultipleFiles
#   Returns whether <File()> is broken out into more elements.
sub HasMultipleFiles
    {  return ref($_[0]->[FILE]);  };






###############################################################################
# Group: Support Functions

#
#   Function: MergeFile
#
#   Adds another definition of the same package/symbol.  Perhaps the file is different.
#
#   Parameters:
#
#       file  - The package/symbol's definition file.
#       type  - The package/symbol's <TopicType>.
#       prototype  - The package/symbol's protoype if applicable.
#       summary  - The package/symbol's summary if applicable.
#
sub MergeFile #(file, type, prototype, summary)
    {
    my ($self, $file, $type, $prototype, $summary) = @_;

    # If there's only one file...
    if (!$self->HasMultipleFiles())
        {
        # If there's one file and it's the different from the new one...
        if ($file ne $self->File())
            {
            my $selfDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, undef, $self->File(), $self->Type(),
                                                                                                                 $self->Prototype(), $self->Summary(),
                                                                                                                 $self->CombinedType(),
                                                                                                                 $self->PackageSeparator());
            my $newDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, undef, $file, $type, $prototype,
                                                                                                                  $summary);

            $self->[FILE] = [ $selfDefinition, $newDefinition ];
            $self->[TYPE] = undef;
            $self->[PROTOTYPE] = undef;
            $self->[SUMMARY] = undef;

            if ($newDefinition->Type() ne $self->CombinedType())
                {  $self->[COMBINED_TYPE] = ::TOPIC_GENERAL();  };
            if ($newDefinition->PackageSeparator() ne $self->PackageSeparator())
                {  $self->[PACKAGE_SEPARATOR] = '.';  };
            }

        # If the file was the same, just ignore the duplicate in the index.
        }

    # If there's more than one file...
    else
        {
        # See if the new file is one of them.
        my $files = $self->File();

        foreach my $testElement (@$files)
            {
            if ($testElement->File() eq $file)
                {
                # If the new file's already in the index, ignore the duplicate.
                return;
                };
            };

        my $newDefinition = NaturalDocs::SymbolTable::IndexElement->New(undef, undef, $file, $type, $prototype,
                                                                                                              $summary);
        push @{$self->[FILE]}, $newDefinition;

        if ($newDefinition->Type() ne $self->CombinedType())
            {  $self->[COMBINED_TYPE] = ::TOPIC_GENERAL();  };
        if ($newDefinition->PackageSeparator() ne $self->PackageSeparator())
            {  $self->[PACKAGE_SEPARATOR] = '.';  };
        };
    };


1;
