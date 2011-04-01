###############################################################################
#
#   Package: NaturalDocs::ClassHierarchy
#
###############################################################################
#
#   A package that handles all the gory details of managing the class hierarchy.  It handles the hierarchy itself, which files define
#   them, rebuilding the files that are affected by changes, and loading and saving them to a file.
#
#   Usage and Dependencies:
#
#       - <NaturalDocs::Settings> and <NaturalDocs::Project> must be initialized before use.
#
#       - <NaturalDocs::SymbolTable> must be initialized before <Load()> is called.  It must reflect the state as of the last time
#          Natural Docs was run.
#
#       - <Load()> must be called to initialize the package.  At this point, the <Information Functions> will return the state as
#         of the last time Natural Docs was run.  You are free to resolve <NaturalDocs::SymbolTable()> afterwards.
#
#       - <Purge()> must be called, and then <NaturalDocs::Parser->ParseForInformation()> must be called on all files that
#         have changed so it can fully resolve the hierarchy via the <Modification Functions()>.  Afterwards the
#         <Information Functions> will reflect the current state of the code.
#
#       - <Save()> must be called to commit any changes to the symbol table back to disk.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use strict;
use integer;

use NaturalDocs::ClassHierarchy::Class;
use NaturalDocs::ClassHierarchy::File;

package NaturalDocs::ClassHierarchy;


###############################################################################
# Group: Variables

#
#   handle: CLASS_HIERARCHY_FILEHANDLE
#   The file handle used with <ClassHierarchy.nd>.
#

#
#   hash: classes
#
#   A hash of all the classes.  The keys are the class <SymbolStrings> and the values are <NaturalDocs::ClassHierarchy::Classes>.
#
my %classes;

#
#   hash: files
#
#   A hash of the hierarchy information referenced by file.  The keys are the <FileNames>, and the values are
#   <NaturalDocs::ClassHierarchy::File>s.
#
my %files;

#
#   hash: parentReferences
#
#   A hash of all the parent reference strings and what they resolve to.  The keys are the <ReferenceStrings> and the values are
#   the class <SymbolStrings> that they resolve to.
#
my %parentReferences;

#
#   object: watchedFile
#
#   A <NaturalDocs::ClassHierarchy::File> object of the file being watched for changes.  This is compared to the version in <files>
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
#   bool: dontRebuildFiles
#
#   A bool to set if you don't want changes in the hierarchy to cause files to be rebuilt.
#
my $dontRebuildFiles;



###############################################################################
# Group: Files


#
#   File: ClassHierarchy.nd
#
#   Stores the class hierarchy on disk.
#
#   Format:
#
#       > [BINARY_FORMAT]
#       > [VersionInt: app version]
#
#       The standard <BINARY_FORMAT> and <VersionInt> header.
#
#       > [SymbolString: class or undef to end]
#
#       Next we begin a class segment with its <SymbolString>.  These continue until the end of the file.  Only defined classes are
#       included.
#
#       > [UInt32: number of files]
#       > [AString16: file] [AString16: file] ...
#
#       Next there is the number of files that define that class.  It's a UInt32, which seems like overkill, but I could imagine every
#       file in a huge C++ project being under the same namespace, and thus contributing its own definition.  It's theoretically
#       possible.
#
#       Following the number is that many file names.  You must remember the index of each file, as they will be important later.
#       Indexes start at one because zero has a special meaning.
#
#       > [UInt8: number of parents]
#       > ( [ReferenceString (no type): parent]
#       >   [UInt32: file index] [UInt32: file index] ... [UInt32: 0] ) ...
#
#       Next there is the number of parents defined for this class.  For each one, we define a parent segment, which consists of
#       its <ReferenceString>, and then a zero-terminated string of indexes of the files that define that parent as part of that class.
#       The indexes start at one, and are into the list of files we saw previously.
#
#       Note that we do store class segments for classes without parents, but not for undefined classes.
#
#       This concludes a class segment.  These segments continue until an undef <SymbolString>.
#
#   See Also:
#
#       <File Format Conventions>
#
#   Revisions:
#
#       1.22:
#
#           - Classes and parents switched from AString16s to <SymbolStrings> and <ReferenceStrings>.
#           - A ending undef <SymbolString> was added to the end.  Previously it stopped when the file ran out.
#
#       1.2:
#
#           - This file was introduced in 1.2.
#


###############################################################################
# Group: File Functions


#
#   Function: Load
#
#   Loads the class hierarchy from disk.
#
sub Load
    {
    my ($self) = @_;

    $dontRebuildFiles = 1;

    my $fileIsOkay;
    my $fileName = NaturalDocs::Project->DataFile('ClassHierarchy.nd');

    if (!NaturalDocs::Settings->RebuildData() && open(CLASS_HIERARCHY_FILEHANDLE, '<' . $fileName))
        {
        # See if it's binary.
        binmode(CLASS_HIERARCHY_FILEHANDLE);

        my $firstChar;
        read(CLASS_HIERARCHY_FILEHANDLE, $firstChar, 1);

        if ($firstChar != ::BINARY_FORMAT())
            {
            close(CLASS_HIERARCHY_FILEHANDLE);
            }
        else
            {
            my $version = NaturalDocs::Version->FromBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE);

            # Minor bugs were fixed in 1.33 that may affect the stored data.

            if (NaturalDocs::Version->CheckFileFormat( $version, NaturalDocs::Version->FromString('1.33') ))
                {  $fileIsOkay = 1;  }
            else
                {  close(CLASS_HIERARCHY_FILEHANDLE);  };
            };
        };


    if (!$fileIsOkay)
        {
        NaturalDocs::Project->ReparseEverything();
        }
    else
        {
        my $raw;

        for (;;)
            {
            # [SymbolString: class or undef to end]

            my $class = NaturalDocs::SymbolString->FromBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE);

            if (!defined $class)
                {  last;  };

            # [UInt32: number of files]

            read(CLASS_HIERARCHY_FILEHANDLE, $raw, 4);
            my $numberOfFiles = unpack('N', $raw);

            my @files;

            while ($numberOfFiles)
                {
                # [AString16: file]

                read(CLASS_HIERARCHY_FILEHANDLE, $raw, 2);
                my $fileLength = unpack('n', $raw);

                my $file;
                read(CLASS_HIERARCHY_FILEHANDLE, $file, $fileLength);

                push @files, $file;
                $self->AddClass($file, $class, NaturalDocs::Languages->LanguageOf($file)->Name());

                $numberOfFiles--;
                };

            # [UInt8: number of parents]

            read(CLASS_HIERARCHY_FILEHANDLE, $raw, 1);
            my $numberOfParents = unpack('C', $raw);

            while ($numberOfParents)
                {
                # [ReferenceString (no type): parent]

                my $parent = NaturalDocs::ReferenceString->FromBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE,
                                                                                                         ::BINARYREF_NOTYPE(),
                                                                                                         ::REFERENCE_CH_PARENT());

                for (;;)
                    {
                    # [UInt32: file index or 0]

                    read(CLASS_HIERARCHY_FILEHANDLE, $raw, 4);
                    my $fileIndex = unpack('N', $raw);

                    if ($fileIndex == 0)
                        {  last;  }

                    $self->AddParentReference( $files[$fileIndex - 1], $class, $parent );
                    };

                $numberOfParents--;
                };
            };

        close(CLASS_HIERARCHY_FILEHANDLE);
        };

    $dontRebuildFiles = undef;
    };


#
#   Function: Save
#
#   Saves the class hierarchy to disk.
#
sub Save
    {
    my ($self) = @_;

    open (CLASS_HIERARCHY_FILEHANDLE, '>' . NaturalDocs::Project->DataFile('ClassHierarchy.nd'))
        or die "Couldn't save " . NaturalDocs::Project->DataFile('ClassHierarchy.nd') . ".\n";

    binmode(CLASS_HIERARCHY_FILEHANDLE);

    print CLASS_HIERARCHY_FILEHANDLE '' . ::BINARY_FORMAT();
    NaturalDocs::Version->ToBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE, NaturalDocs::Settings->AppVersion());

    while (my ($class, $classObject) = each %classes)
        {
        if ($classObject->IsDefined())
            {
            # [SymbolString: class or undef to end]

            NaturalDocs::SymbolString->ToBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE, $class);

            # [UInt32: number of files]

            my @definitions = $classObject->Definitions();
            my %definitionIndexes;

            print CLASS_HIERARCHY_FILEHANDLE pack('N', scalar @definitions);

            for (my $i = 0; $i < scalar @definitions; $i++)
                {
                # [AString16: file]
                print CLASS_HIERARCHY_FILEHANDLE pack('nA*', length($definitions[$i]), $definitions[$i]);
                $definitionIndexes{$definitions[$i]} = $i + 1;
                };

            # [UInt8: number of parents]

            my @parents = $classObject->ParentReferences();
            print CLASS_HIERARCHY_FILEHANDLE pack('C', scalar @parents);

            foreach my $parent (@parents)
                {
                # [ReferenceString (no type): parent]

                NaturalDocs::ReferenceString->ToBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE, $parent, ::BINARYREF_NOTYPE());

                # [UInt32: file index]

                my @parentDefinitions = $classObject->ParentReferenceDefinitions($parent);

                foreach my $parentDefinition (@parentDefinitions)
                    {
                    print CLASS_HIERARCHY_FILEHANDLE pack('N', $definitionIndexes{$parentDefinition});
                    };

                # [UInt32: 0]
                print CLASS_HIERARCHY_FILEHANDLE pack('N', 0);
                };
            };
        };

    # [SymbolString: class or undef to end]

    NaturalDocs::SymbolString->ToBinaryFile(\*CLASS_HIERARCHY_FILEHANDLE, undef);

    close(CLASS_HIERARCHY_FILEHANDLE);
    };


#
#   Function: Purge
#
#   Purges the hierarchy of files that no longer have Natural Docs content.
#
sub Purge
    {
    my ($self) = @_;

    my $filesToPurge = NaturalDocs::Project->FilesToPurge();

    foreach my $file (keys %$filesToPurge)
        {
        $self->DeleteFile($file);
        };
    };



###############################################################################
# Group: Interface Functions


#
#   Function: OnInterpretationChange
#
#   Called by <NaturalDocs::SymbolTable> whenever a class hierarchy reference's intepretation changes, meaning it switched
#   from one symbol to another.
#
#       reference - The <ReferenceString> whose current interpretation changed.
#
sub OnInterpretationChange #(reference)
    {
    my ($self, $reference) = @_;

    if (NaturalDocs::ReferenceString->TypeOf($reference) == ::REFERENCE_CH_PARENT())
        {
        # The approach here is simply to completely delete the reference and readd it.  This is less than optimal efficiency, since it's
        # being removed and added from %files too, even though that isn't required.  However, the simpler code is worth it
        # considering this will only happen when a parent reference becomes defined or undefined, or on the rare languages (like C#)
        # that allow relative parent references.

        my $oldTargetSymbol = $parentReferences{$reference};
        my $oldTargetObject = $classes{$oldTargetSymbol};

        my @classesWithReferenceParent = $oldTargetObject->Children();

        # Each entry is an arrayref of file names.  Indexes are the same as classesWithReferenceParent's.
        my @filesDefiningReferenceParent;

        foreach my $classWithReferenceParent (@classesWithReferenceParent)
            {
            my $fileList = [ $classes{$classWithReferenceParent}->ParentReferenceDefinitions($reference) ];
            push @filesDefiningReferenceParent, $fileList;

            foreach my $fileDefiningReferenceParent (@$fileList)
                {
                $self->DeleteParentReference($fileDefiningReferenceParent, $classWithReferenceParent, $reference);
                };
            };


        # This will force the reference to be reinterpreted on the next add.

        delete $parentReferences{$reference};


        # Now we can just readd it.

        for (my $i = 0; $i < scalar @classesWithReferenceParent; $i++)
            {
            foreach my $file (@{$filesDefiningReferenceParent[$i]})
                {
                $self->AddParentReference($file, $classesWithReferenceParent[$i], $reference);
                };
            };
        };

    # The only way for a REFERENCE_CH_CLASS reference to change is if the symbol is deleted.  That will be handled by
    # <AnalyzeChanges()>, so we don't need to do anything here.
    };


#
#   Function: OnTargetSymbolChange
#
#   Called by <NaturalDocs::SymbolTable> whenever a class hierarchy reference's target symbol changes, but the reference
#   still resolves to the same symbol.
#
#   Parameters:
#
#       reference - The <ReferenceString> that was affected by the change.
#
sub OnTargetSymbolChange #(reference)
    {
    my ($self, $reference) = @_;

    my $type = NaturalDocs::ReferenceString->TypeOf($reference);
    my $class;

    if ($type == ::REFERENCE_CH_PARENT())
        {  $class = $parentReferences{$reference};  }
    else # ($type == ::REFERENCE_CH_CLASS())
        {
        # Class references are global absolute, so we can just yank the symbol.
        (undef, $class, undef, undef, undef, undef) = NaturalDocs::ReferenceString->InformationOf($reference);
        };

    $self->RebuildFilesFor($class, 1, 0, 1);
    };



###############################################################################
# Group: Modification Functions


#
#   Function: AddClass
#
#   Adds a class to the hierarchy.
#
#   Parameters:
#
#       file - The <FileName> the class was defined in.
#       class - The class <SymbolString>.
#       languageName - The name of the language this applies to.
#
#   Note:
#
#       The file parameter must be defined when using this function externally.  It may be undef for internal use only.
#
sub AddClass #(file, class, languageName)
    {
    my ($self, $file, $class, $languageName) = @_;

    if (!exists $classes{$class})
        {
        $classes{$class} = NaturalDocs::ClassHierarchy::Class->New();
        NaturalDocs::SymbolTable->AddReference($self->ClassReferenceOf($class, $languageName), $file)
        };

    if (defined $file)
        {
        # If this was the first definition for this class...
        if ($classes{$class}->AddDefinition($file))
            {  $self->RebuildFilesFor($class, 1, 1, 1);  };

        if (!exists $files{$file})
            {  $files{$file} = NaturalDocs::ClassHierarchy::File->New();  };

        $files{$file}->AddClass($class);

        if (defined $watchedFileName)
            {  $watchedFile->AddClass($class);  };
        };
    };


#
#   Function: AddParentReference
#
#   Adds a class-parent relationship to the hierarchy.  The classes will be created if they don't already exist.
#
#   Parameters:
#
#       file - The <FileName> the reference was defined in.
#       class - The class <SymbolString>.
#       symbol - The parent class <SymbolString>.
#       scope - The package <SymbolString> that the reference appeared in.
#       using - An arrayref of package <SymbolStrings> that the reference has access to via "using" statements.
#       resolvingFlags - Any <Resolving Flags> to be used when resolving the reference.
#
#   Alternate Parameters:
#
#       file - The <FileName> the reference was defined in.
#       class - The class <SymbolString>.
#       reference - The parent <ReferenceString>.
#
sub AddParentReference #(file, class, symbol, scope, using, resolvingFlags) or (file, class, reference)
    {
    my ($self, $file, $class, $symbol, $parentReference);

    if (scalar @_ == 7)
        {
        my ($scope, $using, $resolvingFlags);
        ($self, $file, $class, $symbol, $scope, $using, $resolvingFlags) = @_;

        $parentReference = NaturalDocs::ReferenceString->MakeFrom(::REFERENCE_CH_PARENT(), $symbol,
                                                                                                    NaturalDocs::Languages->LanguageOf($file)->Name(),
                                                                                                    $scope, $using, $resolvingFlags);
        }
    else
        {
        ($self, $file, $class, $parentReference) = @_;
        $symbol = (NaturalDocs::ReferenceString->InformationOf($parentReference))[1];
        };


    # In case it doesn't already exist.
    $self->AddClass($file, $class);

    my $parent;
    if (exists $parentReferences{$parentReference})
        {
        $parent = $parentReferences{$parentReference};
        }
    else
        {
        NaturalDocs::SymbolTable->AddReference($parentReference, $file);
        my $parentTarget = NaturalDocs::SymbolTable->References($parentReference);

        if (defined $parentTarget)
            {  $parent = $parentTarget->Symbol();  }
        else
            {  $parent = $symbol;  };

        # In case it doesn't already exist.
        $self->AddClass(undef, $parent);

        $parentReferences{$parentReference} = $parent;
        };


    # If this defined a new parent...
    if ($classes{$class}->AddParentReference($parentReference, $file, \%parentReferences))
        {
        $classes{$parent}->AddChild($class);

        $self->RebuildFilesFor($class, 0, 1, 0);
        $self->RebuildFilesFor($parent, 0, 1, 0);
        };

    $files{$file}->AddParentReference($class, $parentReference);

    if (defined $watchedFileName)
        {  $watchedFile->AddParentReference($class, $parentReference);  };
    };


#
#   Function: WatchFileForChanges
#
#   Watches a file for changes, which can then be applied by <AnalyzeChanges()>.  Definitions are not deleted via a DeleteClass()
#   function.  Instead, a file is watched for changes, reparsed, and then a comparison is made to look for definitions that
#   disappeared and any other relevant changes.
#
#   Parameters:
#
#       file - The <FileName> to watch.
#
sub WatchFileForChanges #(file)
    {
    my ($self, $file) = @_;

    $watchedFile = NaturalDocs::ClassHierarchy::File->New();
    $watchedFileName = $file;
    };


#
#   Function: AnalyzeChanges
#
#   Checks the watched file for any changes that occured since the last time is was parsed, and updates the hierarchy as
#   necessary.  Also sends any files that are affected to <NaturalDocs::Project->RebuildFile()>.
#
sub AnalyzeChanges
    {
    my ($self) = @_;

    # If the file didn't have any classes before, and it still doesn't, it wont be in %files.
    if (exists $files{$watchedFileName})
        {
        my @originalClasses = $files{$watchedFileName}->Classes();

        foreach my $originalClass (@originalClasses)
            {
            # If the class isn't there the second time around...
            if (!$watchedFile->HasClass($originalClass))
                {  $self->DeleteClass($watchedFileName, $originalClass);  }

            else
                {
                my @originalParents = $files{$watchedFileName}->ParentReferencesOf($originalClass);

                foreach my $originalParent (@originalParents)
                    {
                    # If the parent reference wasn't there the second time around...
                    if (!$watchedFile->HasParentReference($originalClass, $originalParent))
                        {  $self->DeleteParentReference($watchedFileName, $originalClass, $originalParent);  };
                    };
                };
            };
        };


    $watchedFile = undef;
    $watchedFileName = undef;
    };



###############################################################################
# Group: Information Functions


#
#   Function: ParentsOf
#   Returns a <SymbolString> array of the passed class' parents, or an empty array if none.  Note that not all of them may be
#   defined.
#
sub ParentsOf #(class)
    {
    my ($self, $class) = @_;

    if (exists $classes{$class})
        {  return $classes{$class}->Parents();  }
    else
        {  return ( );  };
    };

#
#   Function: ChildrenOf
#   Returns a <SymbolString> array of the passed class' children, or an empty array if none.  Note that not all of them may be
#   defined.
#
sub ChildrenOf #(class)
    {
    my ($self, $class) = @_;

    if (exists $classes{$class})
        {  return $classes{$class}->Children();  }
    else
        {  return ( );  };
    };



###############################################################################
# Group: Support Functions


#
#   Function: DeleteFile
#
#   Deletes a file and everything defined in it.
#
#   Parameters:
#
#       file - The <FileName>.
#
sub DeleteFile #(file)
    {
    my ($self, $file) = @_;

    if (!exists $files{$file})
        {  return;  };

    my @classes = $files{$file}->Classes();
    foreach my $class (@classes)
        {
        $self->DeleteClass($file, $class);
        };

    delete $files{$file};
    };

#
#   Function: DeleteClass
#
#   Deletes a class definition from a file.  Will also delete any parent references from this class and file.  Will rebuild any file
#   affected unless <dontRebuildFiles> is set.
#
#   Parameters:
#
#       file - The <FileName> that defines the class.
#       class - The class <SymbolString>.
#
sub DeleteClass #(file, class)
    {
    my ($self, $file, $class) = @_;

    my @parents = $files{$file}->ParentReferencesOf($class);
    foreach my $parent (@parents)
        {
        $self->DeleteParentReference($file, $class, $parent);
        };

    $files{$file}->DeleteClass($class);

    # If we're deleting the last definition of this class.
    if ($classes{$class}->DeleteDefinition($file))
        {
        if (!$classes{$class}->HasChildren())
            {
            delete $classes{$class};

            if (!$dontRebuildFiles)
                {  NaturalDocs::Project->RebuildFile($file);  };
            }
        else
            {  $self->RebuildFilesFor($class, 0, 1, 1);  };

        };
    };


#
#   Function: DeleteParentReference
#
#   Deletes a class' parent reference and returns whether it resulted in the loss of a parent class.  Will rebuild any file affected
#   unless <dontRebuildFiles> is set.
#
#   Parameters:
#
#       file - The <FileName> that defines the reference.
#       class - The class <SymbolString>.
#       reference - The parent <ReferenceString>.
#
#   Returns:
#
#       If the class lost a parent as a result of this, it will return its <SymbolString>.  It will return undef otherwise.
#
sub DeleteParentReference #(file, class, reference)
    {
    my ($self, $file, $class, $reference) = @_;

    if (!exists $classes{$class})
        {  return;  };

    $files{$file}->DeleteParentReference($class, $reference);

    my $deletedParent = $classes{$class}->DeleteParentReference($reference, $file, \%parentReferences);

    if (defined $deletedParent)
        {
        my $deletedParentObject = $classes{$deletedParent};

        $deletedParentObject->DeleteChild($class);

        $self->RebuildFilesFor($deletedParent, 0, 1, 0);
        $self->RebuildFilesFor($class, 0, 1, 0);

        if (!$deletedParentObject->HasChildren() && !$deletedParentObject->IsDefined())
            {
            delete $classes{$deletedParent};
            NaturalDocs::SymbolTable->DeleteReference(
                $self->ClassReferenceOf($class, NaturalDocs::Languages->LanguageOf($file)->Name()) );
            };

        return $deletedParent;
        };

    return undef;
    };


#
#   Function: ClassReferenceOf
#
#   Returns the <REFERENCE_CH_CLASS> <ReferenceString> of the passed class <SymbolString>.
#
sub ClassReferenceOf #(class, languageName)
    {
    my ($self, $class, $languageName) = @_;

    return NaturalDocs::ReferenceString->MakeFrom(::REFERENCE_CH_CLASS(), $class, $languageName, undef, undef,
                                                                            ::RESOLVE_ABSOLUTE() | ::RESOLVE_NOPLURAL());
    };


#
#   Function: RebuildFilesFor
#
#   Calls <NaturalDocs::Project->RebuildFile()> for every file defining the passed class, its parents, and/or its children.
#   Returns without doing anything if <dontRebuildFiles> is set.
#
#   Parameters:
#
#       class - The class <SymbolString>.
#       rebuildParents - Whether to rebuild the class' parents.
#       rebuildSelf - Whether to rebuild the class.
#       rebuildChildren - Whether to rebuild the class' children.
#
sub RebuildFilesFor #(class, rebuildParents, rebuildSelf, rebuildChildren)
    {
    my ($self, $class, $rebuildParents, $rebuildSelf, $rebuildChildren) = @_;

    if ($dontRebuildFiles)
        {  return;  };

    my @classesToBuild;

    if ($rebuildParents)
        {  @classesToBuild = $classes{$class}->Parents();  };
    if ($rebuildSelf)
        {  push @classesToBuild, $class;  };
    if ($rebuildChildren)
        {  push @classesToBuild, $classes{$class}->Children();  };

    foreach my $classToBuild (@classesToBuild)
        {
        my @definitions = $classes{$classToBuild}->Definitions();

        foreach my $definition (@definitions)
            {  NaturalDocs::Project->RebuildFile($definition);  };
        };
    };


1;
