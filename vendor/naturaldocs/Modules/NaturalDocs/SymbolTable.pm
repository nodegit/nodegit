###############################################################################
#
#   Package: NaturalDocs::SymbolTable
#
###############################################################################
#
#   A package that handles all the gory details of managing symbols.  It handles where they are defined, which files
#   reference them, if any are undefined or duplicated, and loading and saving them to a file.
#
#   Usage and Dependencies:
#
#       - At any time, <RebuildAllIndexes()> can be called.
#
#       - <NaturalDocs::Settings>, <NaturalDocs::Languages>, and <NaturalDocs::Project> must be initialized before use.
#
#       - <Load()> must be called to initialize the package.  At this point, the <Information Functions> will return the symbol
#         table as of the last time Natural Docs was run.
#
#       - Note that <Load()> and <Save()> only manage <REFERENCE_TEXT> references.  All other reference types must be
#         managed by their respective classes.  They should be readded after <Load()> to recreate the state of the last time
#         Natural Docs was run.
#
#       - <Purge()> must be called, and then <NaturalDocs::Parser->ParseForInformation()> on all files that have changed so it
#         can fully resolve the symbol table via the <Modification Functions>.  Afterwards <PurgeResolvingInfo()> can be called
#         to reclaim some memory, and the symbol table will reflect the current state of the code.
#
#       - <Save()> must be called to commit any changes to the symbol table back to disk.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use NaturalDocs::SymbolTable::Symbol;
use NaturalDocs::SymbolTable::SymbolDefinition;
use NaturalDocs::SymbolTable::Reference;
use NaturalDocs::SymbolTable::File;
use NaturalDocs::SymbolTable::ReferenceTarget;
use NaturalDocs::SymbolTable::IndexElement;

use strict;
use integer;

package NaturalDocs::SymbolTable;



###############################################################################
# Group: Variables

#
#   handle: SYMBOLTABLE_FILEHANDLE
#
#   The file handle used with <SymbolTable.nd>.
#

#
#   hash: symbols
#
#   A hash of all <SymbolStrings>.  The keys are the <SymbolStrings> and the values are <NaturalDocs::SymbolTable::Symbol>
#   objects.
#
#   Prior to <PurgeResolvingInfo()>, both defined symbols and symbols that are merely potential interpretations of references
#   will be here.  Afterwards, only defined symbols will be here.
#
my %symbols;

#
#   hash: references
#
#   A hash of all references in the project.  The keys are <ReferenceStrings> and the values are
#   <NaturalDocs::SymbolTable::Reference> objects.
#
#   Prior to <PurgeResolvingInfo()>, all possible interpretations will be stored for each reference.  Afterwards, only the current
#   interpretation will be.
#
my %references;

#
#   hash: files
#
#   A hash of all the files that define symbols and references in the project.  The keys are the <FileNames>, and the values are
#   <NaturalDocs::SymbolTable::File> objects.
#
#   After <PurgeResolvingInfo()>, this hash will be empty.
#
my %files;

#
#   object: watchedFile
#
#   A <NaturalDocs::SymbolTable::File> object of the file being watched for changes.  This is compared to the version in <files>
#   to see if anything was changed since the last parse.
#
my $watchedFile;

#
#   string: watchedFileName
#
#   The <FileName> of the watched file, if any.  If there is no watched file, this will be undef.
#
my $watchedFileName;

#
#   hash: watchedFileSymbolDefinitions
#
#   A hashref of the symbol definition information for all the <SymbolStrings> in the watched file.  The keys are the symbol strings,
#   and the values are <NaturalDocs::SymbolTable::SymbolDefinition> objects.
#
my %watchedFileSymbolDefinitions;


#
#   hash: indexes
#
#   A hash of generated symbol indexes.  The keys are <TopicTypes> and the values are sorted arrayrefs of
#   <NaturalDocs::SymbolTable::IndexElements>, or undef if its empty.
#
my %indexes;


#
#   hash: indexChanges
#
#   A hash of all the indexes that have changed.  The keys are the <TopicTypes> and the entries are undef if they have not
#   changed, or 1 if they have.  The key will not exist if the <TopicType> has not been checked.
#
my %indexChanges;


#
#   hash: indexSectionsWithContent
#
#   A hash of which sections in an index have content.  The keys are the <TopicTypes> of each index, and the values are
#   arrayrefs of bools where the first represents symbols, the second numbers, and the rest A-Z.  If there is no information
#   available for an index, it's entry will not exist here.
#
my %indexSectionsWithContent;


#
#   bool: rebuildIndexes
#
#   Whether all indexes should be rebuilt regardless of whether they have been changed.
#
my $rebuildIndexes;



###############################################################################
# Group: Files


#
#   File: SymbolTable.nd
#
#   The storage file for the symbol table.
#
#   Format:
#
#       > [BINARY_FORMAT]
#       > [VersionInt: app version]
#
#       The file starts with the standard <BINARY_FORMAT> <VersionInt> header.
#
#       The first stage of the file is for symbol definitions, analogous to <symbols>.
#
#       > [SymbolString: symbol or undef to end] ...
#       >
#       > [UInt16: number of definitions]
#       >
#       >    [AString16: global definition file] [AString16: TopicType]
#       >       [AString16: prototype] [AString16: summary]
#       >
#       >    [AString16: definition file] ...
#       >
#       >    ...
#
#       These blocks continue until the <SymbolString> is undef.  Only defined symbols will be included in this file, so
#       number of definitions will never be zero.  The first one is always the global definition.  If a symbol does not have a
#       prototype or summary, the UInt16 length of the string will be zero.
#
#       The second stage is for references, which is analogous to <references>.  Only <REFERENCE_TEXT> references are
#       stored in this file, and their <Resolving Flags> are implied so they aren't stored either.
#
#       > [ReferenceString (no type, resolving flags): reference or undef to end]
#       >
#       > [UInt8: number of definition files]
#       >    [AString16: definition file] [AString16: definition file] ...
#
#       These blocks continue until the <ReferenceString> is undef.  Since there can be multiple using <SymbolStrings>, those
#       continue until the number of identifiers is zero.  Note that all interpretations are rebuilt rather than stored.
#
#   See Also:
#
#       <File Format Conventions>
#
#   Revisions:
#
#       1.3:
#
#           - Symbol <TopicTypes> were changed from UInt8s to AString16s, now that <TopicTypes> are strings instead of
#             integer constants.
#
#       1.22:
#
#           - File format was completely rebuilt to accommodate the new symbol format and to be in binary.  To see the plain text
#             format prior to 1.22, check out 1.21's version of this file from CVS.  It is too big a change to note here.
#


#
#   File: IndexInfo.nd
#
#   The storage file for information about the indexes.
#
#   Format:
#
#       > [Standard Header]
#
#       The standard binary file header.
#
#       > [AString16: index topic name]
#       > [uint8: symbols have content (0 or 1)]
#       > [uint8: numbers have content (0 or 1)]
#       > [uint8: A has content] [uint8: B has content] ...
#       > ...
#
#       Every index that has information about it is stored with the topic type name first, then 28 uint8s that say whether that
#       part of the index has content or not.  The first is for symbols, the second is for numbers, and the rest are for A-Z.  If an
#       index's state is unknown, it won't appear in this file.
#
#   Revisions:
#
#       1.4:
#
#           - The file is introduced.
#



###############################################################################
# Group: File Functions


#
#   Function: Load
#
#   Loads all data files from disk.
#
sub Load
    {
    my ($self) = @_;

    $self->LoadSymbolTable();
    $self->LoadIndexInfo();
    };


#
#   Function: LoadSymbolTable
#
#   Loads <SymbolTable.nd> from disk.
#
sub LoadSymbolTable
    {
    my ($self) = @_;

    my $fileIsOkay;

    if (!NaturalDocs::Settings->RebuildData() &&
        open(SYMBOLTABLE_FILEHANDLE, '<' . NaturalDocs::Project->DataFile('SymbolTable.nd')) )
        {
        # See if it's binary.
        binmode(SYMBOLTABLE_FILEHANDLE);

        my $firstChar;
        read(SYMBOLTABLE_FILEHANDLE, $firstChar, 1);

        if ($firstChar == ::BINARY_FORMAT())
            {
            my $version = NaturalDocs::Version->FromBinaryFile(\*SYMBOLTABLE_FILEHANDLE);

            # 1.3 is incompatible with previous versions.

            if (NaturalDocs::Version->CheckFileFormat( $version, NaturalDocs::Version->FromString('1.3') ))
                {  $fileIsOkay = 1;  }
            else
                {  close(SYMBOLTABLE_FILEHANDLE);  };
            }

        else
            {  close(SYMBOLTABLE_FILEHANDLE);  };
        };


    if (!$fileIsOkay)
        {
        NaturalDocs::Project->ReparseEverything();
        return;
        }

    my $raw;


    # Symbols

    for (;;)
        {
        # [SymbolString: symbol or undef to end]

        my $symbol = NaturalDocs::SymbolString->FromBinaryFile(\*SYMBOLTABLE_FILEHANDLE);

        if (!defined $symbol)
            {  last;  };

        my $symbolObject = NaturalDocs::SymbolTable::Symbol->New();
        $symbols{$symbol} = $symbolObject;

        # [UInt16: number of definitions]

        read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
        my $definitionCount = unpack('n', $raw);

        do
            {
            # [AString16: (global?) definition file]

            read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
            my $fileLength = unpack('n', $raw);

            my $file;
            read(SYMBOLTABLE_FILEHANDLE, $file, $fileLength);

            # [AString16: TopicType]

            read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
            my $typeLength = unpack('n', $raw);

            my $type;
            read(SYMBOLTABLE_FILEHANDLE, $type, $typeLength);

            # [AString16: prototype]

            read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
            my $prototypeLength = unpack('n', $raw);

            my $prototype;
            if ($prototypeLength)
                {  read(SYMBOLTABLE_FILEHANDLE, $prototype, $prototypeLength);  };

            # [AString16: summary]

            read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
            my $summaryLength = unpack('n', $raw);

            my $summary;
            if ($summaryLength)
                {  read(SYMBOLTABLE_FILEHANDLE, $summary, $summaryLength);  };

            $symbolObject->AddDefinition($file, $type, $prototype, $summary);

            # Add it.

            if (!exists $files{$file})
                {  $files{$file} = NaturalDocs::SymbolTable::File->New();  };

            $files{$file}->AddSymbol($symbol);

            $definitionCount--;
            }
        while ($definitionCount);
        };


    # References

    for (;;)
        {
        # [ReferenceString (no type, resolving flags): reference or undef to end]

        my $referenceString = NaturalDocs::ReferenceString->FromBinaryFile(\*SYMBOLTABLE_FILEHANDLE,
                                                                                                              ::BINARYREF_NOTYPE() |
                                                                                                              ::BINARYREF_NORESOLVINGFLAGS(),
                                                                                                              ::REFERENCE_TEXT(), undef);

        if (!defined $referenceString)
            {  last;  };

        my $referenceObject = NaturalDocs::SymbolTable::Reference->New();
        $references{$referenceString} = $referenceObject;

        # [UInt8: number of definition files]

        read(SYMBOLTABLE_FILEHANDLE, $raw, 1);
        my $definitionCount = unpack('C', $raw);
        do
            {
            # [AString16: definition file] [AString16: definition file] ...

            read(SYMBOLTABLE_FILEHANDLE, $raw, 2);
            my $definitionLength = unpack('n', $raw);

            my $definition;
            read(SYMBOLTABLE_FILEHANDLE, $definition, $definitionLength);

            # Add it.

            $referenceObject->AddDefinition($definition);

            if (!exists $files{$definition})
                {  $files{$definition} = NaturalDocs::SymbolTable::File->New();  };

            $files{$definition}->AddReference($referenceString);

            $definitionCount--;
            }
        while ($definitionCount);

        $self->GenerateInterpretations($referenceString);
        $self->InterpretReference($referenceString);
        };

    close(SYMBOLTABLE_FILEHANDLE);
    };


#
#   Function: LoadIndexInfo
#
#   Loads <IndexInfo.nd> from disk.
#
sub LoadIndexInfo
    {
    my ($self) = @_;

    if (NaturalDocs::Settings->RebuildData())
        {  return;  };

    my $version = NaturalDocs::BinaryFile->OpenForReading( NaturalDocs::Project->DataFile('IndexInfo.nd') );

    if (!defined $version)
        {  return;  }

    # The file format hasn't changed since it was introduced.
    if (!NaturalDocs::Version->CheckFileFormat($version))
        {
        NaturalDocs::BinaryFile->Close();
        return;
        };

    my $topicTypeName;
    while ($topicTypeName = NaturalDocs::BinaryFile->GetAString16())
        {
        my $topicType = NaturalDocs::Topics->TypeFromName($topicTypeName);
        my $content = [ ];

        for (my $i = 0; $i < 28; $i++)
            {  push @$content, NaturalDocs::BinaryFile->GetUInt8();  };

        if (defined $topicType)  # The name in the file could be from a type that was deleted
            {  $indexSectionsWithContent{$topicType} = $content;  };
        };

    NaturalDocs::BinaryFile->Close();
    };


#
#   Function: Purge
#
#   Purges the symbol table of all symbols and references from files that no longer have Natural Docs content.
#
sub Purge
    {
    my ($self) = @_;

    my $filesToPurge = NaturalDocs::Project->FilesToPurge();

    # We do this in two stages.  First we delete all the references, and then we delete all the definitions.  This causes us to go
    # through the list twice, but it makes sure no purged files get added to the build list.  For example, if we deleted all of
    # Purge File A's references and definitions, and Purge File B had a reference to one of those symbols, Purge File B
    # would be added to the build list because one of its references changed.  By removing all the references in all the files
    # before removing the definitions, we avoid this.

    foreach my $file (keys %$filesToPurge)
        {
        if (exists $files{$file})
            {
            my @references = $files{$file}->References();
            foreach my $reference (@references)
                {  $self->DeleteReference($reference, $file);  };
            };
        };

    foreach my $file (keys %$filesToPurge)
        {
        if (exists $files{$file})
            {
            my @symbols = $files{$file}->Symbols();
            foreach my $symbol (@symbols)
                {  $self->DeleteSymbol($symbol, $file);  };

            delete $files{$file};
            };
        };
    };


#
#   Function: Save
#
#   Saves all data files to disk.
#
sub Save
    {
    my ($self) = @_;

    $self->SaveSymbolTable();
    $self->SaveIndexInfo();
    };


#
#   Function: SaveSymbolTable
#
#   Saves <SymbolTable.nd> to disk.
#
sub SaveSymbolTable
    {
    my ($self) = @_;

    open (SYMBOLTABLE_FILEHANDLE, '>' . NaturalDocs::Project->DataFile('SymbolTable.nd'))
        or die "Couldn't save " . NaturalDocs::Project->DataFile('SymbolTable.nd') . ".\n";

    binmode(SYMBOLTABLE_FILEHANDLE);

    print SYMBOLTABLE_FILEHANDLE '' . ::BINARY_FORMAT();

    NaturalDocs::Version->ToBinaryFile(\*SYMBOLTABLE_FILEHANDLE, NaturalDocs::Settings->AppVersion());


    # Symbols

    while (my ($symbol, $symbolObject) = each %symbols)
        {
        # Only existing symbols.
        if ($symbolObject->IsDefined())
            {
            # [SymbolString: symbol or undef to end]

            NaturalDocs::SymbolString->ToBinaryFile(\*SYMBOLTABLE_FILEHANDLE, $symbol);

            # [UInt16: number of definitions]

            my @definitions = $symbolObject->Definitions();
            print SYMBOLTABLE_FILEHANDLE pack('n', scalar @definitions);

            # [AString16: global definition file] [AString16: TopicType]

            print SYMBOLTABLE_FILEHANDLE pack('nA*nA*', length $symbolObject->GlobalDefinition(),
                                                                                   $symbolObject->GlobalDefinition(),
                                                                                   length $symbolObject->GlobalType(),
                                                                                   $symbolObject->GlobalType());

            # [AString16: prototype]

            my $prototype = $symbolObject->GlobalPrototype();

            if (defined $prototype)
                {  print SYMBOLTABLE_FILEHANDLE pack('nA*', length($prototype), $prototype);  }
            else
                {  print SYMBOLTABLE_FILEHANDLE pack('n', 0);  };

            # [AString16: summary]

            my $summary = $symbolObject->GlobalSummary();

            if (defined $summary)
                {  print SYMBOLTABLE_FILEHANDLE pack('nA*', length($summary), $summary);  }
            else
                {  print SYMBOLTABLE_FILEHANDLE pack('n', 0);  };


            foreach my $definition (@definitions)
                {
                if ($definition ne $symbolObject->GlobalDefinition())
                    {
                    # [AString16: definition file] [AString16: TopicType]

                    print SYMBOLTABLE_FILEHANDLE pack('nA*nA*', length $definition, $definition,
                                                                                           length $symbolObject->TypeDefinedIn($definition),
                                                                                           $symbolObject->TypeDefinedIn($definition));

                    # [AString16: prototype]

                    my $prototype = $symbolObject->PrototypeDefinedIn($definition);

                    if (defined $prototype)
                        {  print SYMBOLTABLE_FILEHANDLE pack('nA*', length($prototype), $prototype);  }
                    else
                        {  print SYMBOLTABLE_FILEHANDLE pack('n', 0);  };

                    # [AString16: summary]

                    my $summary = $symbolObject->SummaryDefinedIn($definition);

                    if (defined $summary)
                        {  print SYMBOLTABLE_FILEHANDLE pack('nA*', length($summary), $summary);  }
                    else
                        {  print SYMBOLTABLE_FILEHANDLE pack('n', 0);  };
                    };
                };
            };
        };

     # [SymbolString: symbol or undef to end]

     NaturalDocs::SymbolString->ToBinaryFile(\*SYMBOLTABLE_FILEHANDLE, undef);


     # References

    while (my ($reference, $referenceObject) = each %references)
        {
        my $type = NaturalDocs::ReferenceString->TypeOf($reference);

        if ($type == ::REFERENCE_TEXT())
            {
            # [ReferenceString (no type, resolving flags): reference or undef to end]

            NaturalDocs::ReferenceString->ToBinaryFile(\*SYMBOLTABLE_FILEHANDLE, $reference,
                                                                             ::BINARYREF_NOTYPE() | ::BINARYREF_NORESOLVINGFLAGS());

            # [UInt8: number of definition files]

            my @definitions = $referenceObject->Definitions();
            print SYMBOLTABLE_FILEHANDLE pack('C', scalar @definitions);

            # [AString16: definition file] [AString16: definition file] ...

            foreach my $definition (@definitions)
                {
                print SYMBOLTABLE_FILEHANDLE pack('nA*', length($definition), $definition);
                };
            };
        };

    # [ReferenceString (no type, resolving flags): reference or undef to end]

    NaturalDocs::ReferenceString->ToBinaryFile(\*SYMBOLTABLE_FILEHANDLE, undef,
                                                                     ::BINARYREF_NOTYPE() | ::BINARYREF_NORESOLVINGFLAGS());

    close(SYMBOLTABLE_FILEHANDLE);
    };


#
#   Function: SaveIndexInfo
#
#   Saves <IndexInfo.nd> to disk.
#
sub SaveIndexInfo
    {
    my ($self) = @_;

    NaturalDocs::BinaryFile->OpenForWriting( NaturalDocs::Project->DataFile('IndexInfo.nd') );

    while (my ($topicType, $content) = each %indexSectionsWithContent)
        {
        NaturalDocs::BinaryFile->WriteAString16( NaturalDocs::Topics->NameOfType($topicType) );

        for (my $i = 0; $i < 28; $i++)
            {
            if ($content->[$i])
                {  NaturalDocs::BinaryFile->WriteUInt8(1);  }
            else
                {  NaturalDocs::BinaryFile->WriteUInt8(0);  };
            };
        };

    NaturalDocs::BinaryFile->Close();
    };



###############################################################################
# Group: Modification Functions
# These functions should not be called after <PurgeResolvingInfo()>.

#
#   Function: AddSymbol
#
#   Adds a symbol definition to the table, if it doesn't already exist.  If the definition changes or otherwise requires the files that
#   reference it to be updated, the function will call <NaturalDocs::Project->RebuildFile()> to make sure that they are.
#
#   Parameters:
#
#       symbol  - The <SymbolString>.
#       file        - The <FileName> where it's defined.
#       type      - The symbol's <TopicType>.
#       prototype - The symbol's prototype, if applicable.
#       summary - The symbol's summary, if applicable.
#
sub AddSymbol #(symbol, file, type, prototype, summary)
    {
    my ($self, $symbol, $file, $type, $prototype, $summary) = @_;


    # If the symbol doesn't exist...
    if (!exists $symbols{$symbol})
        {
        # Create the symbol.  There are no references that could be interpreted as this or else it would have existed already.

        my $newSymbol = NaturalDocs::SymbolTable::Symbol->New();
        $newSymbol->AddDefinition($file, $type, $prototype, $summary);

        $symbols{$symbol} = $newSymbol;

        $self->OnIndexChange($type);
        NaturalDocs::Project->RebuildFile($file);
        }


    # If the symbol already exists...
    else
        {
        my $symbolObject = $symbols{$symbol};

        # If the symbol isn't defined, i.e. it was a potential interpretation only...
        if (!$symbolObject->IsDefined())
            {
            $symbolObject->AddDefinition($file, $type, $prototype, $summary);

            # See if this symbol provides a better interpretation of any references.  We can assume this symbol has interpretations
            # because the object won't exist without either that or definitions.

            my %referencesAndScores = $symbolObject->ReferencesAndScores();

            while (my ($referenceString, $referenceScore) = each %referencesAndScores)
                {
                my $referenceObject = $references{$referenceString};

                if (!$referenceObject->HasCurrentInterpretation() ||
                    $referenceScore > $referenceObject->CurrentScore())
                    {
                    $referenceObject->SetCurrentInterpretation($symbol);
                    $self->OnInterpretationChange($referenceString);
                    };
                };

            $self->OnIndexChange($type);
            NaturalDocs::Project->RebuildFile($file);
            }

        # If the symbol is defined but not in this file...
        elsif (!$symbolObject->IsDefinedIn($file))
            {
            $symbolObject->AddDefinition($file, $type, $prototype, $summary);

            $self->OnIndexChange($type);
            NaturalDocs::Project->RebuildFile($file);

            # We don't have to check other files because if the symbol is defined it already has a global definiton,
            # and everything else is either using that or its own definition, and thus wouldn't be affected by this.
            };

        # If the symbol was already defined in this file, ignore it.

        };


    # Add it to the file index.

    if (!exists $files{$file})
        {  $files{$file} = NaturalDocs::SymbolTable::File->New();  };

    $files{$file}->AddSymbol($symbol);


    # Add it to the watched file, if necessary.

    if (defined $watchedFileName)
        {
        $watchedFile->AddSymbol($symbol);

        if (!exists $watchedFileSymbolDefinitions{$symbol})
            {
            $watchedFileSymbolDefinitions{$symbol} =
                 NaturalDocs::SymbolTable::SymbolDefinition->New($type, $prototype, $summary);
            };
        };
    };


#
#   Function: AddReference
#
#   Adds a reference to the table, if it doesn't already exist.
#
#   Parameters:
#
#       type        - The <ReferenceType>.
#       symbol    - The reference <SymbolString>.
#       scope      - The scope <SymbolString> it appears in.
#       using       - An arrayref of scope <SymbolStrings> accessible to the reference via "using" statements, or undef if none.
#       file          - The <FileName> where the reference appears.  This is not required unless the type is <REFERENCE_TEXT>.
#       resolvingFlags - The <Resolving Flags> of the reference.  They will be ignored if the type is <REFERENCE_TEXT>.
#
#   Alternate Parameters:
#
#       referenceString - The <ReferenceString> to add.
#       file - The <FileName> where the reference appears.  This is not required unless the type is <REFERENCE_TEXT>.
#
sub AddReference #(type, symbol, scope, using, file, resolvingFlags) or (referenceString, file)
    {
    my ($self, $referenceString, $file);

    if (scalar @_ <= 3)
        {
        ($self, $referenceString, $file) = @_;
        }
    else
        {
        my ($type, $symbol, $scope, $using, $resolvingFlags);
        ($self, $type, $symbol, $scope, $using, $file, $resolvingFlags) = @_;

        $referenceString = NaturalDocs::ReferenceString->MakeFrom($type, $symbol,
                                                                                                   NaturalDocs::Languages->LanguageOf($file)->Name(),
                                                                                                   $scope, $using, $resolvingFlags);
        };


    # If the reference doesn't exist...
    if (!exists $references{$referenceString})
        {
        my $referenceObject = NaturalDocs::SymbolTable::Reference->New();

        $references{$referenceString} = $referenceObject;

        $self->GenerateInterpretations($referenceString);
        $self->InterpretReference($referenceString);
        }


    if (defined $file)
        {
        $references{$referenceString}->AddDefinition($file);


        # Add it to the file index.

        if (!exists $files{$file})
            {  $files{$file} = NaturalDocs::SymbolTable::File->New();  };

        $files{$file}->AddReference($referenceString);


        # Add it to the watched file, if necessary.

        if (defined $watchedFileName)
            {  $watchedFile->AddReference($referenceString);  };
        };
    };


#
#   Function: WatchFileForChanges
#
#   Tracks a file to see if any symbols or references were changed or deleted in ways that would require other files to be rebuilt.
#   Assumes that after this function call, the entire file will be parsed again, and thus every symbol and reference will go through
#   <AddSymbol()> and <AddReference()>.  Afterwards, call <AnalyzeChanges()> to handle any differences.
#
#   Parameters:
#
#       file - The <FileName> to watch.
#
sub WatchFileForChanges #(file)
    {
    my ($self, $file) = @_;

    $watchedFile = NaturalDocs::SymbolTable::File->New();
    $watchedFileName = $file;
    %watchedFileSymbolDefinitions = ( );
    };


#
#   Function: AnalyzeChanges
#
#   Handles any changes found when reparsing a file using <WatchFileForChanges()>.
#
sub AnalyzeChanges
    {
    my ($self) = @_;

    if (exists $files{$watchedFileName})
        {

        # Go through the references and remove any that were deleted.  Ones that were added will have already been added to
        # the table in AddReference().

        my @references = $files{$watchedFileName}->References();
        foreach my $reference (@references)
            {
            if (!$watchedFile->DefinesReference($reference))
                {  $self->DeleteReference($reference, $watchedFileName);  };
            };
        };

    # We have to check if the watched file exists again because DeleteReference() could have removed it.  I'm still not sure how a
    # file could have references without symbols, but apparently it's happened in the real world because it's crashed on people.
    if (exists $files{$watchedFileName})
        {
        # Go through the symbols.

        my $rebuildFile;

        my @symbols = $files{$watchedFileName}->Symbols();
        foreach my $symbol (@symbols)
            {
            # Delete symbols that don't exist.

            if (!$watchedFile->DefinesSymbol($symbol))
                {
                $self->DeleteSymbol($symbol, $watchedFileName);
                $rebuildFile = 1;
                }

            else
                {
                my $symbolObject = $symbols{$symbol};
                my $newSymbolDef = $watchedFileSymbolDefinitions{$symbol};

                # Update symbols that changed.

                if ( $symbolObject->TypeDefinedIn($watchedFileName) ne $newSymbolDef->Type() ||
                     $symbolObject->PrototypeDefinedIn($watchedFileName) ne $newSymbolDef->Prototype() ||
                     $symbolObject->SummaryDefinedIn($watchedFileName) ne $newSymbolDef->Summary() )
                    {
                    $self->OnIndexChange($symbolObject->TypeDefinedIn($watchedFileName));
                    $self->OnIndexChange($newSymbolDef->Type());
                    $rebuildFile = 1;

                    $symbolObject->ChangeDefinition($watchedFileName, $newSymbolDef->Type(), $newSymbolDef->Prototype(),
                                                                       $newSymbolDef->Summary());

                    # If the symbol definition was the global one, we need to update all files that reference it.  If it wasn't, the only file
                    # that could references it is itself, and the only way the symbol definition could change in the first place was if it was
                    # itself changed.
                    if ($symbolObject->GlobalDefinition() eq $watchedFileName)
                        {
                        # Rebuild the files that have references to this symbol
                        my @references = $symbolObject->References();
                        foreach my $reference (@references)
                            {
                            if ($references{$reference}->CurrentInterpretation() eq $symbol)
                                {  $self->OnTargetSymbolChange($reference);  };
                            }; # While references
                        }; # If global definition is watched file
                    }; # If the symbol definition changed
                }; # If the symbol still exists
            }; # foreach symbol in watched file

        if ($rebuildFile)
            {  NaturalDocs::Project->RebuildFile($watchedFileName);  };

        };


    $watchedFile = undef;
    $watchedFileName = undef;
    %watchedFileSymbolDefinitions = ( );
    };


#
#   Function: DeleteReference
#
#   Deletes a reference from the table.
#
#   Be careful with this function, as deleting a reference means there are no more of them in the file at all.  The tables do not
#   keep track of how many times references appear in a file.  In these cases you should instead call <WatchFileForChanges()>,
#   reparse the file, thus readding all the references, and call <AnalyzeChanges()>.
#
#   <REFERENCE_TEXT> references should *always* be managed with <WatchFileForChanges()> and <AnalyzeChanges()>.
#   This function should only be used externally for other types of references.
#
#   Parameters:
#
#       referenceString - The <ReferenceString>.
#       file - The <FileName> where the reference is.  This is not required unless the type is <REFERENCE_TEXT>.
#
sub DeleteReference #(referenceString, file)
    {
    my ($self, $referenceString, $file) = @_;


    # If the reference exists...
    if (exists $references{$referenceString})
        {
        my $referenceObject = $references{$referenceString};

        if (defined $file)
            {  $referenceObject->DeleteDefinition($file);  };

        # If there are no other definitions, or it doesn't use file definitions to begin with...
        if (!$referenceObject->IsDefined())
            {
            my @interpretations = $referenceObject->Interpretations();
            foreach my $interpretation (@interpretations)
                {
                $symbols{$interpretation}->DeleteReference($referenceString);
                };

            delete $references{$referenceString};
            };


        if (defined $file)
            {
            # Remove it from the file index.

            $files{$file}->DeleteReference($referenceString);

            if (!$files{$file}->HasAnything())
                {  delete $files{$file};  };

            # We don't need to worry about the watched file, since this function will only be called by AnalyzeChanges() and
            # LoadAndPurge().
            };
        };
    };


#
#   Function: RebuildAllIndexes
#
#   When called, it makes sure all indexes are listed as changed by <IndexChanged()>, regardless of whether they actually did
#   or not.
#
#   This can be called at any time.
#
sub RebuildAllIndexes
    {
    my $self = shift;
    $rebuildIndexes = 1;
    };


#
#   Function: PurgeResolvingInfo
#
#   Purges unnecessary information from the symbol table after it is fully resolved.  This will reduce the memory footprint for the
#   build stage.  After calling this function, you can only call the <Information Functions> and <Save()>.
#
sub PurgeResolvingInfo
    {
    my ($self) = @_;

    # Go through the symbols.  We don't need to keep around potential symbols anymore, nor do we need what references can
    # be interpreted as the defined ones.

    while (my ($symbol, $symbolObject) = each %symbols)
        {
        if ($symbolObject->IsDefined())
            {  $symbolObject->DeleteAllReferences();  }
        else
            {  delete $symbols{$symbol};  };
        };


    # Go through the references.  We don't need any of the interpretations except for the current.

    foreach my $referenceObject (values %references)
        {  $referenceObject->DeleteAllInterpretationsButCurrent();  };


    # We don't need the information by file at all.

    %files = ( );
    };


#
#   Function: PurgeIndexes
#
#   Clears all generated indexes.
#
sub PurgeIndexes
    {
    my ($self) = @_;
    %indexes = ( );
    };


###############################################################################
# Group: Information Functions
# These functions should not be called until the symbol table is fully resolved.


#
#   Function: References
#
#   Returns what the passed reference information resolve to, if anything.  Note that this only works if the reference had
#   been previously added to the table via <AddReference()> with the exact same parameters.
#
#   Parameters:
#
#       type     - The <ReferenceType>.
#       symbol - The reference <SymbolString>.
#       scope   - The scope <SymbolString> the reference appears in, or undef if none.
#       using    - An arrayref of scope <SymbolStrings> available to the reference via using statements.
#       file       - The source <FileName> the reference appears in, or undef if none.
#       resolvingFlags - The <Resolving Flags> of the reference.  Ignored if the type is <REFERENCE_TEXT>.
#
#   Alternate Parameters:
#
#       referenceString - The <ReferenceString> to resolve.
#       file - The source <FileName> the reference appears in, or undef if none.
#
#   Returns:
#
#       A <NaturalDocs::SymbolTable::ReferenceTarget> object, or undef if the reference doesn't resolve to anything.
#
sub References #(type, symbol, scope, using, file, resolvingFlags) or (referenceString, file)
    {
    my ($self, $referenceString, $file);

    if (scalar @_ <= 3)
        {  ($self, $referenceString, $file) = @_;  }
    else
        {
        my ($type, $symbol, $scope, $using, $resolvingFlags);
        ($self, $type, $symbol, $scope, $using, $file, $resolvingFlags) = @_;

        $referenceString = NaturalDocs::ReferenceString->MakeFrom($type, $symbol,
                                                                                                  NaturalDocs::Languages->LanguageOf($file)->Name(),
                                                                                                  $scope, $using, $resolvingFlags);
        };

    if (exists $references{$referenceString} && $references{$referenceString}->HasCurrentInterpretation())
        {
        my $targetSymbol = $references{$referenceString}->CurrentInterpretation();
        my $targetObject = $symbols{$targetSymbol};

        my $targetFile;
        my $targetType;
        my $targetPrototype;
        my $targetSummary;

        if (defined $file && $targetObject->IsDefinedIn($file))
            {
            $targetFile = $file;
            $targetType = $targetObject->TypeDefinedIn($file);
            $targetPrototype = $targetObject->PrototypeDefinedIn($file);
            $targetSummary = $targetObject->SummaryDefinedIn($file);
            }
        else
            {
            $targetFile = $targetObject->GlobalDefinition();
            $targetType = $targetObject->GlobalType();
            $targetPrototype = $targetObject->GlobalPrototype();
            $targetSummary = $targetObject->GlobalSummary();
            };

        return NaturalDocs::SymbolTable::ReferenceTarget->New($targetSymbol, $targetFile, $targetType, $targetPrototype,
                                                                                             $targetSummary);
        }

    else
        {  return undef;  };
    };


#
#   Function: Lookup
#
#   Returns information on the passed <SymbolString>, if it exists.  Note that the symbol must be fully resolved.
#
#   Parameters:
#
#       symbol - The <SymbolString>.
#       file - The source <FileName> the reference appears in, or undef if none.
#
#   Returns:
#
#       A <NaturalDocs::SymbolTable::ReferenceTarget> object, or undef if the symbol isn't defined.
#
sub Lookup #(symbol, file)
    {
    my ($self, $symbol, $file) = @_;

    my $symbolObject = $symbols{$symbol};

    if (defined $symbolObject)
        {
        my $targetFile;
        my $targetType;
        my $targetPrototype;
        my $targetSummary;

        if (defined $file && $symbolObject->IsDefinedIn($file))
            {
            $targetFile = $file;
            $targetType = $symbolObject->TypeDefinedIn($file);
            $targetPrototype = $symbolObject->PrototypeDefinedIn($file);
            $targetSummary = $symbolObject->SummaryDefinedIn($file);
            }
        else
            {
            $targetFile = $symbolObject->GlobalDefinition();
            $targetType = $symbolObject->GlobalType();
            $targetPrototype = $symbolObject->GlobalPrototype();
            $targetSummary = $symbolObject->GlobalSummary();
            };

        return NaturalDocs::SymbolTable::ReferenceTarget->New($symbol, $targetFile, $targetType, $targetPrototype,
                                                                                             $targetSummary);
        }

    else
        {  return undef;  };
    };


#
#   Function: Index
#
#   Returns a symbol index.
#
#   Indexes are generated on demand, but they are stored so subsequent calls for the same index will be fast.  Call
#   <PurgeIndexes()> to clear the generated indexes.
#
#   Parameters:
#
#       type  - The <TopicType> of symbol to limit the index to, or undef for none.
#
#   Returns:
#
#       An arrayref of sections.  The first represents all the symbols, the second the numbers, and the rest A through Z.
#       Each section is a sorted arrayref of <NaturalDocs::SymbolTable::IndexElement> objects.  If a section has no content,
#       it will be undef.
#
sub Index #(type)
    {
    my ($self, $type) = @_;

    if (!exists $indexes{$type})
        {  $indexes{$type} = $self->MakeIndex($type);  };

    return $indexes{$type};
    };


#
#   Function: HasIndexes
#
#   Determines which indexes out of a list actually have content.
#
#   Parameters:
#
#       types  - An existence hashref of the <TopicTypes> to check for indexes.
#
#   Returns:
#
#       An existence hashref of all the specified indexes that have content.  Will return an empty hashref if none.
#
sub HasIndexes #(types)
    {
    my ($self, $types) = @_;

    # EliminationHash is a copy of all the types, and the types will be deleted as they are found.  This allows us to quit early if
    # we've found all the types because the hash will be empty.  We'll later return the original hash minus what was left over
    # in here, which are the ones that weren't found.
    my %eliminationHash = %$types;

    finddefs:
    foreach my $symbolObject (values %symbols)
        {
        foreach my $definition ($symbolObject->Definitions())
            {
            delete $eliminationHash{ $symbolObject->TypeDefinedIn($definition) };
            delete $eliminationHash{ ::TOPIC_GENERAL() };

            if (!scalar keys %eliminationHash)
                {  last finddefs;  };
            };
        };

    my $result = { %$types };

    foreach my $type (keys %eliminationHash)
        {  delete $result->{$type};  };

    return $result;
    };


#
#   Function: IndexChanged
#
#   Returns whether the specified index has changed.
#
#   Parameters:
#
#       type  - The <TopicType> to limit the index to.
#
sub IndexChanged #(TopicType type)
    {
    my ($self, $type) = @_;
    return ($rebuildIndexes || defined $indexChanges{$type});
    };


#
#   Function: IndexSectionsWithContent
#
#   Returns an arrayref of whether each section of the specified index has content.  The first entry will be for symbols, the second
#   for numbers, and the rest A-Z.  Do not change the arrayref.
#
sub IndexSectionsWithContent #(TopicType type)
    {
    my ($self, $type) = @_;

    if (!exists $indexSectionsWithContent{$type})
        {
        # This is okay because Index() stores generated indexes.  It's not an expensive operation unless the index was never asked
        # for before or it will never be asked for otherwise, and this shouldn't be the case.

        my $index = $self->Index($type);
        my $content = [ ];

        for (my $i = 0; $i < 28; $i++)
            {
            push @$content, (defined $index->[$i] ? 1 : 0);
            };

        $indexSectionsWithContent{$type} = $content;
        };

    return $indexSectionsWithContent{$type};
    };



###############################################################################
# Group: Event Handlers


#
#   Function: OnIndexChange
#
#   Called whenever a change happens to a symbol that would cause an index to be regenerated.
#
#   Parameters:
#
#       type - The <TopicType> of the symbol that caused the change.
#
sub OnIndexChange #(TopicType type)
    {
    my ($self, $type) = @_;

    $indexChanges{$type} = 1;
    $indexChanges{::TOPIC_GENERAL()} = 1;
    delete $indexSectionsWithContent{$type};
    };


#
#   Function: OnInterpretationChange
#
#   Called whenever the current interpretation of a reference changes, meaning it switched from one symbol to another.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> whose current interpretation changed.
#
sub OnInterpretationChange #(referenceString)
    {
    my ($self, $referenceString) = @_;
    my $referenceType = NaturalDocs::ReferenceString->TypeOf($referenceString);

    if ($referenceType == ::REFERENCE_TEXT())
        {
        my @referenceDefinitions = $references{$referenceString}->Definitions();

        foreach my $referenceDefinition (@referenceDefinitions)
            {
            NaturalDocs::Project->RebuildFile($referenceDefinition);
            };
        }

    elsif (NaturalDocs::Constants->IsClassHierarchyReference($referenceType))
        {
        NaturalDocs::ClassHierarchy->OnInterpretationChange($referenceString);
        };
    };


#
#   Function: OnTargetSymbolChange
#
#   Called whenever the symbol that serves as the interpretation of a reference changes, but the reference still resolves to
#   the same symbol.  This would happen if the type, prototype, summary, or which file serves as global definition of the symbol
#   changes.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> whose interpretation's symbol changed.
#
sub OnTargetSymbolChange #(referenceString)
    {
    my ($self, $referenceString) = @_;
    my $referenceType = NaturalDocs::ReferenceString->TypeOf($referenceString);

    if ($referenceType == ::REFERENCE_TEXT())
        {
        my @referenceDefinitions = $references{$referenceString}->Definitions();

        foreach my $referenceDefinition (@referenceDefinitions)
            {
            NaturalDocs::Project->RebuildFile($referenceDefinition);
            };
        }

    elsif (NaturalDocs::Constants->IsClassHierarchyReference($referenceType))
        {
        NaturalDocs::ClassHierarchy->OnTargetSymbolChange($referenceString);
        };
    };



###############################################################################
# Group: Support Functions


#
#   Function: DeleteSymbol
#
#   Removes a symbol definition from the table.  It will call <OnInterpretationChange()> for all references that have it as their
#   current interpretation.
#
#   External code should not attempt to delete symbols using this function.  Instead it should call <WatchFileFoChanges()>,
#   reparse the file, and call <AnalyzeChanges()>.
#
#   Parameters:
#
#       symbol - The <SymbolString>.
#       file       - The <FileName> where the definition is.
#
sub DeleteSymbol #(symbol, file)
    {
    my ($self, $symbol, $file) = @_;


    # If the symbol and definition exist...
    if (exists $symbols{$symbol} && $symbols{$symbol}->IsDefinedIn($file))
        {
        my $symbolObject = $symbols{$symbol};
        my $wasGlobal = ($symbolObject->GlobalDefinition() eq $file);

        $self->OnIndexChange($symbolObject->TypeDefinedIn($file));

        $symbolObject->DeleteDefinition($file);

        # If this was one definition of many...
        if ($symbolObject->IsDefined())
            {

            # If this was the global definition...
            if ($wasGlobal)
                {
                # Update every file that referenced the global symbol; i.e. every file that doesn't have its own definition.

                my @references = $symbolObject->References();

                foreach my $reference (@references)
                    {
                    if ($references{$reference}->CurrentInterpretation() eq $symbol)
                        {
                        $self->OnTargetSymbolChange($reference);
                        };
                    };
                }

            # If this wasn't the global definition...
            else
                {
                # It's a safe bet that we don't need to do anything here.  The only thing that we even need to look for here is if the
                # file referenced its own symbol and thus should be rebuilt.  However, if the file is having a symbol deleted, it either
                # changed or was itself deleted.  If it changed and still has other Natural Docs content, it should already be on the
                # rebuild list.  If it was deleted or no longer has Natural Docs content, we certainly don't want to add it to the rebuild
                # list.
                };
            }

        # If this is the only definition...
        else
            {
            # If this symbol is the interpretation of any references...
            if ($symbolObject->HasReferences())
                {
                # If this was the current interpretation of any references, reinterpret them and rebuild their files.

                my @references = $symbolObject->References();

                foreach my $reference (@references)
                    {
                    if ($references{$reference}->CurrentInterpretation() eq $symbol)
                        {
                        $self->InterpretReference($reference);
                        $self->OnInterpretationChange($reference);
                        };
                    };
                }

            # If there are no interpretations of the symbol...
            else
                {
                # Delete the symbol entirely.
                delete $symbols{$symbol};
                };
            };

        # Remove it from the file index.

        $files{$file}->DeleteSymbol($symbol);

        if (!$files{$file}->HasAnything())
            {  delete $files{$file};  };


        # We don't need to worry about the watched file, since this function will only be called by AnalyzeChanges() and
        # LoadAndPurge().
        };
    };


#
#   Function: GenerateInterpretations
#
#   Generates the list of interpretations for the passed reference.  Also creates potential symbols as necessary.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to generate the interpretations of.
#
sub GenerateInterpretations #(referenceString)
    {
    my ($self, $referenceString) = @_;

    my ($type, $symbol, $languageName, $scope, $using, $resolvingFlags) =
        NaturalDocs::ReferenceString->InformationOf($referenceString);

    # RESOLVE_NOPLURAL is handled by having @singulars be empty.
    my @singulars;
    if (!($resolvingFlags & ::RESOLVE_NOPLURAL()))
        {  @singulars = $self->SingularInterpretationsOf($symbol);  };

    # Since higher scores are better, we'll start at a high number and decrement.
    my $score = 50000;


    # If RESOLVE_RELATIVE is set, we do all the scope relatives before the global.
    if ($resolvingFlags & ::RESOLVE_RELATIVE())
        {
        $score = $self->GenerateRelativeInterpretations($referenceString, $symbol, \@singulars, $scope, $score);
        }

    # If neither RESOLVE_RELATIVE nor RESOLVE_ABSOLUTE is set, we only do the local before the global.
    elsif (!($resolvingFlags & ::RESOLVE_ABSOLUTE()))
        {
        $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join($scope, $symbol), $score);
        $score--;

        foreach my $singular (@singulars)
            {
            $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join($scope, $singular), $score);
            $score--;
            };
        };


    # Do the global.

    $self->AddInterpretation($referenceString, $symbol, $score);
    $score--;

    foreach my $singular (@singulars)
        {
        $self->AddInterpretation($referenceString, $singular, $score);
        $score--;
        };


    # If neither RESOLVE_RELATIVE nor RESOLVE_ABSOLUTE is set, we need to do the rest of the scope relatives after the global.
    if (!($resolvingFlags & ::RESOLVE_RELATIVE()) && !($resolvingFlags & ::RESOLVE_ABSOLUTE()))
        {
        $score = $self->GenerateRelativeInterpretations($referenceString, $symbol, \@singulars, $scope, $score, 1);
        };


    # Finally, if RESOLVE_NOUSING isn't set, go through the using scopes.
    if (!($resolvingFlags & ::RESOLVE_NOUSING()) && defined $using)
        {
        foreach my $usingScope (@$using)
            {
            if ($resolvingFlags & ::RESOLVE_ABSOLUTE())
                {
                $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join($usingScope, $symbol), $score);
                $score--;

                foreach my $singular (@singulars)
                    {
                    $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join($usingScope, $singular), $score);
                    $score--;
                    };
                }
            else
                {
                $score = $self->GenerateRelativeInterpretations($referenceString, $symbol, \@singulars, $usingScope, $score);
                };
            };
        };
    };


#
#   Function: GenerateRelativeInterpretations
#
#   Generates the list of relative interpretations for the passed reference and packages.  Also creates potential symbols as
#   necessary.
#
#   This function will _not_ create global interpretations.  It _will_ create a local interpretations (symbol + all packages) unless
#   you set dontUseFull.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to generate interpretations for.
#       symbol - The <SymbolString> to generate interpretations of.
#       singulars - A reference to an array of singular <SymbolStrings> to also generate interpretations of.  Set to an empty array
#                       if none.
#       package - The package <SymbolString> to use.  May be undef.
#       score - The starting score to apply.
#       dontUseFull - Whether to not generate an interpretation including the full package identifier.  If set, generated interpretations
#                           will start one level down.
#
#   Returns:
#
#       The next unused score.  This is basically the passed score minus the number of interpretations created.
#
sub GenerateRelativeInterpretations #(referenceString, symbol, singulars, package, score, dontUseFull)
    {
    my ($self, $referenceString, $symbol, $singulars, $package, $score, $dontUseFull) = @_;

    my @packages = NaturalDocs::SymbolString->IdentifiersOf($package);

    # The last package index to include.  This number is INCLUSIVE!
    my $packageLevel = scalar @packages - 1;

    if ($dontUseFull)
        {  $packageLevel--;  };

    while ($packageLevel >= 0)
        {
        $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join(@packages[0..$packageLevel], $symbol),
                                             $score);
        $score--;

        foreach my $singular (@$singulars)
            {
            $self->AddInterpretation($referenceString, NaturalDocs::SymbolString->Join(@packages[0..$packageLevel], $singular),
                                                 $score);
            $score--;
            };

        $packageLevel--;
        };

    return $score;
    };


#
#   Function: SingularInterpretationsOf
#
#   Generates singular interpretations of a <SymbolString> if it can be interpreted as a plural.  Not all of them will be valid singular
#   forms, but that doesn't matter since it's incredibly unlikely an invalid form would exist as a symbol.  What matters is that the
#   legimate singular is present on the list.
#
#   Parameters:
#
#       symbol - The <SymbolString>.
#
#   Returns:
#
#       An array of potential singular interpretations as <SymbolStrings>, in no particular order.  If the symbol can't be interpreted
#       as a plural, returns an empty array.
#
sub SingularInterpretationsOf #(symbol)
    {
    my ($self, $symbol) = @_;

    my @identifiers = NaturalDocs::SymbolString->IdentifiersOf($symbol);
    my $lastIdentifier = pop @identifiers;
    my $preIdentifiers = NaturalDocs::SymbolString->Join(@identifiers);

    my @results;

    # First cut off any 's or ' at the end, since they can appear after other plural forms.
    if ($lastIdentifier =~ s/\'s?$//i)
        {
        push @results, NaturalDocs::SymbolString->Join($preIdentifiers, $lastIdentifier);
        };

    # See http://www.gsu.edu/~wwwesl/egw/crump.htm for a good list of potential plural forms.  There are a couple more than
    # listed below, but they're fairly rare and this is already seriously over-engineered.  This is split by suffix length to make
    # comparisons more efficient.

    # The fact that this will generate some impossible combinations (leaves => leave, leav, leaf, leafe) doesn't matter.  It's very
    # unlikely that more than one will manage to match a defined symbol.  Even if they do (leave, leaf), it's incredibly unlikely
    # that someone has defined an impossible one (leav, leafe).  So it's not so important that we remove impossible combinations,
    # just that we include all the possible ones.

    my @suffixGroups = ( [ 's', undef,  # boys => boy
                                       'i', 'us',  # alumni => alumnus
                                       'a', 'um', # errata => erratum
                                       'a', 'on' ],  # phenomena => phenomenon

                                    [ 'es', undef,  # foxes => fox
                                      'ae', 'a' ],  # amoebae => amoeba

                                    [ 'ies', 'y',  # pennies => penny
                                      'ves', 'f',  # calves => calf
                                      'ves', 'fe',  # knives => knife
                                      'men', 'man',  # women => woman
                                      'ice', 'ouse',  # mice => mouse
                                      'oes', 'o',  # vetoes => veto
                                      'ces', 'x',  # matrices => matrix
                                      'xen', 'x' ],  # oxen => ox

                                    [ 'ices', 'ex',  # indices => index
                                      'feet', 'foot',  # feet => foot
                                      'eese', 'oose',  # geese => goose
                                      'eeth', 'ooth',  # teeth => tooth
                                      'dren', 'd' ] );  # children => child

    my $suffixLength = 1;

    foreach my $suffixGroup (@suffixGroups)
        {
        my $identifierSuffix = lc( substr($lastIdentifier, 0 - $suffixLength) );
        my $cutIdentifier = substr($lastIdentifier, 0, 0 - $suffixLength);

        for (my $i = 0; $i + 1 < scalar @$suffixGroup; $i += 2)
            {
            my $suffix = $suffixGroup->[$i];
            my $replacement = $suffixGroup->[$i + 1];

            if ($identifierSuffix eq $suffix)
                {
                if (defined $replacement)
                    {
                    push @results, NaturalDocs::SymbolString->Join($preIdentifiers, $cutIdentifier . $replacement);
                    push @results, NaturalDocs::SymbolString->Join($preIdentifiers, $cutIdentifier . uc($replacement));
                    }
                else
                    {
                    push @results, NaturalDocs::SymbolString->Join($preIdentifiers, $cutIdentifier);
                    };
                };
            };

        $suffixLength++;
        };

    return @results;
    };


#
#   Function: AddInterpretation
#
#   Adds an interpretation to an existing reference.  Creates potential symbols as necessary.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to add the interpretation to.
#       symbol - The <SymbolString> the reference can be interpreted as.
#       score - The score of the interpretation.
#
sub AddInterpretation #(referenceString, symbol, score)
    {
    my ($self, $referenceString, $symbol, $score) = @_;

    $references{$referenceString}->AddInterpretation($symbol, $score);

    # Create a potential symbol if it doesn't exist.

    if (!exists $symbols{$symbol})
        {  $symbols{$symbol} = NaturalDocs::SymbolTable::Symbol->New();  };

    $symbols{$symbol}->AddReference($referenceString, $score);
    };


#
#   Function: InterpretReference
#
#   Interprets the passed reference, matching it to the defined symbol with the highest score.  If the symbol is already
#   interpreted, it will reinterpret it.  If there are no matches, it will make it an undefined reference.
#
#   Parameters:
#
#       referenceString - The <ReferenceString> to interpret.
#
sub InterpretReference #(referenceString)
    {
    my ($self, $referenceString) = @_;

    my $interpretation;
    my $currentInterpretation;
    my $score;
    my $currentScore = -1;

    my $referenceObject = $references{$referenceString};

    my %interpretationsAndScores = $referenceObject->InterpretationsAndScores();
    while ( ($interpretation, $score) = each %interpretationsAndScores )
        {
        if ($score > $currentScore && $symbols{$interpretation}->IsDefined())
            {
            $currentScore = $score;
            $currentInterpretation = $interpretation;
            };
        };

    if ($currentScore > -1)
        {  $referenceObject->SetCurrentInterpretation($currentInterpretation);  }
    else
        {  $referenceObject->SetCurrentInterpretation(undef);  };
    };


#
#   Function: MakeIndex
#
#   Generates a symbol index.
#
#   Parameters:
#
#       type  - The <TopicType> to limit the index to.
#
#   Returns:
#
#       An arrayref of sections.  The first represents all the symbols, the second the numbers, and the rest A through Z.
#       Each section is a sorted arrayref of <NaturalDocs::SymbolTable::IndexElement> objects.  If a section has no content,
#       it will be undef.
#
sub MakeIndex #(type)
    {
    my ($self, $type) = @_;


    # Go through the symbols and generate IndexElements for any that belong in the index.

    # Keys are the symbol strings, values are IndexElements.
    my %indexSymbols;

    while (my ($symbolString, $object) = each %symbols)
        {
        my ($symbol, $package) = $self->SplitSymbolForIndex($symbolString, $object->GlobalType());
        my @definitions = $object->Definitions();

        foreach my $definition (@definitions)
            {
            my $definitionType = $object->TypeDefinedIn($definition);

            if ($type eq ::TOPIC_GENERAL() || $type eq $definitionType)
                {
                if (!exists $indexSymbols{$symbol})
                    {
                    $indexSymbols{$symbol} =
                        NaturalDocs::SymbolTable::IndexElement->New($symbol, $package, $definition, $definitionType,
                                                                                               $object->PrototypeDefinedIn($definition),
                                                                                               $object->SummaryDefinedIn($definition) );
                    }
                else
                    {
                    $indexSymbols{$symbol}->Merge($package, $definition, $definitionType,
                                                                       $object->PrototypeDefinedIn($definition),
                                                                       $object->SummaryDefinedIn($definition) );
                    };
                }; # If type matches
            }; # Each definition
        }; # Each symbol


    # Generate sortable symbols for each IndexElement, sort them internally, and divide them into sections.

    my $sections = [ ];

    foreach my $indexElement (values %indexSymbols)
        {
        $indexElement->Sort();
        $indexElement->MakeSortableSymbol();

        my $sectionNumber;

        if ($indexElement->SortableSymbol() =~ /^([a-z])/i)
            {  $sectionNumber = ord(lc($1)) - ord('a') + 2;  }
        elsif ($indexElement->SortableSymbol() =~ /^[0-9]/)
            {  $sectionNumber = 1;  }
        else
            {  $sectionNumber = 0;  };

        if (!defined $sections->[$sectionNumber])
            {  $sections->[$sectionNumber] = [ ];  };

        push @{$sections->[$sectionNumber]}, $indexElement;
        };


    # Sort each section.

    for (my $i = 0; $i < scalar @$sections; $i++)
        {
        if (defined $sections->[$i])
            {
            @{$sections->[$i]} = sort
                {
                my $result = ::StringCompare($a->SortableSymbol(), $b->SortableSymbol());

                if ($result == 0)
                    {  $result = ::StringCompare($a->IgnoredPrefix(), $b->IgnoredPrefix());  };

                return $result;
                }
            @{$sections->[$i]};
            };
        };

    return $sections;
    };


#
#   Function: SplitSymbolForIndex
#
#   Splits a <SymbolString> into its symbol and package portions for indexing.
#
#   Parameters:
#
#       symbol - The <SymbolString>.
#       type - Its <TopicType>.
#
#   Returns:
#
#       The array ( symbol, package ), which are both <SymbolStrings>.  If the symbol is global, package will be undef.
#
sub SplitSymbolForIndex #(symbol, type)
    {
    my ($self, $symbol, $type) = @_;

    my $scope = NaturalDocs::Topics->TypeInfo($type)->Scope();

    if ($scope == ::SCOPE_START() || $scope == ::SCOPE_ALWAYS_GLOBAL())
        {  return ( $symbol, undef );  }
    else
        {
        my @identifiers = NaturalDocs::SymbolString->IdentifiersOf($symbol);

        $symbol = pop @identifiers;
        my $package = NaturalDocs::SymbolString->Join(@identifiers);

        return ( $symbol, $package );
        };
    };


1;
