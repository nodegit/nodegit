###############################################################################
#
#   Package: NaturalDocs::Project
#
###############################################################################
#
#   A package that manages information about the files in the source tree, as well as the list of files that have to be parsed
#   and built.
#
#   Usage and Dependencies:
#
#       - All the <Config and Data File Functions> are available immediately, except for the status functions.
#
#       - <ReparseEverything()> and <RebuildEverything()> are available immediately, because they may need to be called
#         after <LoadConfigFileInfo()> but before <LoadSourceFileInfo()>.
#
#       - Prior to <LoadConfigFileInfo()>, <NaturalDocs::Settings> must be initialized.
#
#       - After <LoadConfigFileInfo()>, the status <Config and Data File Functions> are available as well.
#
#       - Prior to <LoadSourceFileInfo()>, <NaturalDocs::Settings> and <NaturalDocs::Languages> must be initialized.
#
#       - After <LoadSourceFileInfo()>, the rest of the <Source File Functions> are available.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use NaturalDocs::Project::SourceFile;
use NaturalDocs::Project::ImageFile;

use strict;
use integer;

package NaturalDocs::Project;


###############################################################################
# Group: File Handles

#
#   handle: FH_FILEINFO
#
#   The file handle for the file information file, <FileInfo.nd>.
#

#
#   handle: FH_CONFIGFILEINFO
#
#   The file handle for the config file information file, <ConfigFileInfo.nd>.
#

#
#   handle: FH_IMAGEFILE
#
#   The file handle for determining the dimensions of image files.
#



###############################################################################
# Group: Source File Variables


#
#   hash: supportedFiles
#
#   A hash of all the supported files in the input directory.  The keys are the <FileNames>, and the values are
#   <NaturalDocs::Project::SourceFile> objects.
#
my %supportedFiles;

#
#   hash: filesToParse
#
#   An existence hash of all the <FileNames> that need to be parsed.
#
my %filesToParse;

#
#   hash: filesToBuild
#
#   An existence hash of all the <FileNames> that need to be built.
#
my %filesToBuild;

#
#   hash: filesToPurge
#
#   An existence hash of the <FileNames> that had Natural Docs content last time, but now either don't exist or no longer have
#   content.
#
my %filesToPurge;

#
#   hash: unbuiltFilesWithContent
#
#   An existence hash of all the <FileNames> that have Natural Docs content but are not part of <filesToBuild>.
#
my %unbuiltFilesWithContent;


# bool: reparseEverything
# Whether all the source files need to be reparsed.
my $reparseEverything;

# bool: rebuildEverything
# Whether all the source files need to be rebuilt.
my $rebuildEverything;

# hash: mostUsedLanguage
# The name of the most used language.  Doesn't include text files.
my $mostUsedLanguage;



###############################################################################
# Group: Configuration File Variables


#
#   hash: mainConfigFile
#
#   A hash mapping all the main configuration file names without paths to their <FileStatus>.  Prior to <LoadConfigFileInfo()>,
#   it serves as an existence hashref of the file names.
#
my %mainConfigFiles = ( 'Topics.txt' => 1, 'Languages.txt' => 1 );

#
#   hash: userConfigFiles
#
#   A hash mapping all the user configuration file names without paths to their <FileStatus>.  Prior to <LoadConfigFileInfo()>,
#   it serves as an existence hashref of the file names.
#
my %userConfigFiles = ( 'Topics.txt' => 1, 'Languages.txt' => 1, 'Menu.txt' => 1 );




###############################################################################
# Group: Image File Variables


#
#   hash: imageFileExtensions
#
#   An existence hash of all the file extensions for images.  Extensions are in all lowercase.
#
my %imageFileExtensions = ( 'jpg' => 1, 'jpeg' => 1, 'gif' => 1, 'png' => 1, 'bmp' => 1 );


#
#   hash: imageFiles
#
#   A hash of all the image files in the project.  The keys are the <FileNames> and the values are
#   <NaturalDocs::Project::ImageFiles>.
#
my %imageFiles;


#
#   hash: imageFilesToUpdate
#
#   An existence hash of all the image <FileNames> that need to be updated, either because they changed or they're new to the
#   project.
#
my %imageFilesToUpdate;


#
#   hash: imageFilesToPurge
#
#   An existence hash of all the image <FileNames> that need to be purged, either because the files no longer exist or because
#   they are no longer used.
#
my %imageFilesToPurge;


#
#   hash: insensitiveImageFiles
#
#   A hash that maps all lowercase image <FileNames> to their proper case as it would appear in <imageFiles>.  Used for
#   case insensitivity, obviously.
#
#   You can't just use all lowercase in <imageFiles> because both Linux and HTTP are case sensitive, so the original case must
#   be preserved.  We also want to allow separate entries for files that differ based only on case, so it goes to <imageFiles> first
#   where they can be distinguished and here only if there's no match.  Ties are broken by whichever is lower with cmp, because
#   it has to resolve consistently on all runs of the program.
#
my %insensitiveImageFiles;



###############################################################################
# Group: Files


#
#   File: FileInfo.nd
#
#   An index of the state of the files as of the last parse.  Used to determine if files were added, deleted, or changed.
#
#   Format:
#
#       The format is a text file.
#
#       > [VersionInt: app version]
#
#       The beginning of the file is the <VersionInt> it was generated with.
#
#       > [most used language name]
#
#       Next is the name of the most used language in the source tree.  Does not include text files.
#
#       Each following line is
#
#       > [file name] tab [last modification time] tab [has ND content (0 or 1)] tab [default menu title] \n
#
#   Revisions:
#
#       1.3:
#
#           - The line following the <VersionInt>, which was previously the last modification time of <Menu.txt>, was changed to
#             the name of the most used language.
#
#       1.16:
#
#           - File names are now absolute.  Prior to 1.16, they were relative to the input directory since only one was allowed.
#
#       1.14:
#
#           - The file was renamed from NaturalDocs.files to FileInfo.nd and moved into the Data subdirectory.
#
#       0.95:
#
#           - The file version was changed to match the program version.  Prior to 0.95, the version line was 1.  Test for "1" instead
#             of "1.0" to distinguish.
#


#
#   File: ConfigFileInfo.nd
#
#   An index of the state of the config files as of the last parse.
#
#   Format:
#
#       > [BINARY_FORMAT]
#       > [VersionInt: app version]
#
#       First is the standard <BINARY_FORMAT> <VersionInt> header.
#
#       > [UInt32: last modification time of menu]
#       > [UInt32: last modification of main topics file]
#       > [UInt32: last modification of user topics file]
#       > [UInt32: last modification of main languages file]
#       > [UInt32: last modification of user languages file]
#
#       Next are the last modification times of various configuration files as UInt32s in the standard Unix format.
#
#
#   Revisions:
#
#       1.3:
#
#           - The file was added to Natural Docs.  Previously the last modification of <Menu.txt> was stored in <FileInfo.nd>, and
#             <Topics.txt> and <Languages.txt> didn't exist.
#


#
#   File: ImageFileInfo.nd
#
#   An index of the state of the image files as of the last parse.
#
#   Format:
#
#       > [Standard Binary Header]
#
#       First is the standard binary file header as defined by <NaturalDocs::BinaryFile>.
#
#       > [AString16: file name or undef]
#       > [UInt32: last modification time]
#       > [UInt8: was used]
#
#       This section is repeated until the file name is null.  The last modification times are UInt32s in the standard Unix format.
#
#
#   Revisions:
#
#       1.4:
#
#           - The file was added to Natural Docs.
#



###############################################################################
# Group: File Functions

#
#   Function: LoadSourceFileInfo
#
#   Loads the project file from disk and compares it against the files in the input directory.  Project is loaded from
#   <FileInfo.nd>.  New and changed files will be added to <FilesToParse()>, and if they have content,
#   <FilesToBuild()>.
#
#   Will call <NaturalDocs::Languages->OnMostUsedLanguageKnown()> if <MostUsedLanguage()> changes.
#
#   Returns:
#
#       Returns whether the project was changed in any way.
#
sub LoadSourceFileInfo
    {
    my ($self) = @_;

    $self->GetAllSupportedFiles();
    NaturalDocs::Languages->OnMostUsedLanguageKnown();

    my $fileIsOkay;
    my $version;
    my $hasChanged;
    my $lineReader;

    if (open(FH_FILEINFO, '<' . $self->DataFile('FileInfo.nd')))
        {
        $lineReader = NaturalDocs::LineReader->New(\*FH_FILEINFO);

        # Check if the file is in the right format.
        $version = NaturalDocs::Version->FromString($lineReader->Get());

        # The project file need to be rebuilt for 1.16.  The source files need to be reparsed and the output files rebuilt for 1.51.
        # We'll tolerate the difference between 1.16 and 1.3 in the loader.

        if (NaturalDocs::Version->CheckFileFormat( $version, NaturalDocs::Version->FromString('1.16') ))
            {
            $fileIsOkay = 1;

            if (!NaturalDocs::Version->CheckFileFormat( $version, NaturalDocs::Version->FromString('1.51') ))
                {
                $reparseEverything = 1;
                $rebuildEverything = 1;
                $hasChanged = 1;
                };
            }
        else
            {
            close(FH_FILEINFO);
            $hasChanged = 1;
            };
        };


    if ($fileIsOkay)
        {
        my %indexedFiles;


        my $line = $lineReader->Get();

        # Prior to 1.3 it was the last modification time of Menu.txt, which we ignore and treat as though the most used language
        # changed.  Prior to 1.32 the settings didn't transfer over correctly to Menu.txt so we need to behave that way again.
        if ($version < NaturalDocs::Version->FromString('1.32') || lc($mostUsedLanguage) ne lc($line))
            {
            $reparseEverything = 1;
            NaturalDocs::SymbolTable->RebuildAllIndexes();
            };


        # Parse the rest of the file.

        while ($line = $lineReader->Get())
            {
            my ($file, $modification, $hasContent, $menuTitle) = split(/\t/, $line, 4);

            # If the file no longer exists...
            if (!exists $supportedFiles{$file})
                {
                if ($hasContent)
                    {  $filesToPurge{$file} = 1;  };

                $hasChanged = 1;
                }

            # If the file still exists...
            else
                {
                $indexedFiles{$file} = 1;

                # If the file changed...
                if ($supportedFiles{$file}->LastModified() != $modification)
                    {
                    $supportedFiles{$file}->SetStatus(::FILE_CHANGED());
                    $filesToParse{$file} = 1;

                    # If the file loses its content, this will be removed by SetHasContent().
                    if ($hasContent)
                        {  $filesToBuild{$file} = 1;  };

                    $hasChanged = 1;
                    }

                # If the file has not changed...
                else
                    {
                    my $status;

                    if ($rebuildEverything && $hasContent)
                        {
                        $status = ::FILE_CHANGED();

                        # If the file loses its content, this will be removed by SetHasContent().
                        $filesToBuild{$file} = 1;
                        $hasChanged = 1;
                        }
                    else
                        {
                        $status = ::FILE_SAME();

                        if ($hasContent)
                            {  $unbuiltFilesWithContent{$file} = 1;  };
                        };

                    if ($reparseEverything)
                        {
                        $status = ::FILE_CHANGED();

                        $filesToParse{$file} = 1;
                        $hasChanged = 1;
                        };

                    $supportedFiles{$file}->SetStatus($status);
                    };

                $supportedFiles{$file}->SetHasContent($hasContent);
                $supportedFiles{$file}->SetDefaultMenuTitle($menuTitle);
                };
            };

        close(FH_FILEINFO);


        # Check for added files.

        if (scalar keys %supportedFiles > scalar keys %indexedFiles)
            {
            foreach my $file (keys %supportedFiles)
                {
                if (!exists $indexedFiles{$file})
                    {
                    $supportedFiles{$file}->SetStatus(::FILE_NEW());
                    $supportedFiles{$file}->SetDefaultMenuTitle($file);
                    $supportedFiles{$file}->SetHasContent(undef);
                    $filesToParse{$file} = 1;
                    # It will be added to filesToBuild if HasContent gets set to true when it's parsed.
                    $hasChanged = 1;
                    };
                };
            };
        }

    # If something's wrong with FileInfo.nd, everything is new.
    else
        {
        foreach my $file (keys %supportedFiles)
            {
            $supportedFiles{$file}->SetStatus(::FILE_NEW());
            $supportedFiles{$file}->SetDefaultMenuTitle($file);
            $supportedFiles{$file}->SetHasContent(undef);
            $filesToParse{$file} = 1;
            # It will be added to filesToBuild if HasContent gets set to true when it's parsed.
            };

        $hasChanged = 1;
        };


    # There are other side effects, so we need to call this.
    if ($rebuildEverything)
        {  $self->RebuildEverything();  };


    return $hasChanged;
    };


#
#   Function: SaveSourceFileInfo
#
#   Saves the source file info to disk.  Everything is saved in <FileInfo.nd>.
#
sub SaveSourceFileInfo
    {
    my ($self) = @_;

    open(FH_FILEINFO, '>' . $self->DataFile('FileInfo.nd'))
        or die "Couldn't save project file " . $self->DataFile('FileInfo.nd') . "\n";

    NaturalDocs::Version->ToTextFile(\*FH_FILEINFO, NaturalDocs::Settings->AppVersion());

    print FH_FILEINFO $mostUsedLanguage . "\n";

    while (my ($fileName, $file) = each %supportedFiles)
        {
        print FH_FILEINFO $fileName . "\t"
                              . $file->LastModified() . "\t"
                              . ($file->HasContent() || '0') . "\t"
                              . $file->DefaultMenuTitle() . "\n";
        };

    close(FH_FILEINFO);
    };


#
#   Function: LoadConfigFileInfo
#
#   Loads the config file info from disk.
#
sub LoadConfigFileInfo
    {
    my ($self) = @_;

    my $fileIsOkay;
    my $version;
    my $fileName = NaturalDocs::Project->DataFile('ConfigFileInfo.nd');

    if (open(FH_CONFIGFILEINFO, '<' . $fileName))
        {
        # See if it's binary.
        binmode(FH_CONFIGFILEINFO);

        my $firstChar;
        read(FH_CONFIGFILEINFO, $firstChar, 1);

        if ($firstChar == ::BINARY_FORMAT())
            {
            $version = NaturalDocs::Version->FromBinaryFile(\*FH_CONFIGFILEINFO);

            # It hasn't changed since being introduced.

            if (NaturalDocs::Version->CheckFileFormat($version))
                {  $fileIsOkay = 1;  }
            else
                {  close(FH_CONFIGFILEINFO);  };
            }

        else # it's not in binary
            {  close(FH_CONFIGFILEINFO);  };
        };

    my @configFiles = ( $self->UserConfigFile('Menu.txt'), \$userConfigFiles{'Menu.txt'},
                                 $self->MainConfigFile('Topics.txt'), \$mainConfigFiles{'Topics.txt'},
                                 $self->UserConfigFile('Topics.txt'), \$userConfigFiles{'Topics.txt'},
                                 $self->MainConfigFile('Languages.txt'), \$mainConfigFiles{'Languages.txt'},
                                 $self->UserConfigFile('Languages.txt'), \$userConfigFiles{'Languages.txt'} );

    if ($fileIsOkay)
        {
        my $raw;

        read(FH_CONFIGFILEINFO, $raw, 20);
        my @configFileDates = unpack('NNNNN', $raw);

        while (scalar @configFiles)
            {
            my $file = shift @configFiles;
            my $fileStatus = shift @configFiles;
            my $fileDate = shift @configFileDates;

            if (-e $file)
                {
                if ($fileDate == (stat($file))[9])
                    {  $$fileStatus = ::FILE_SAME();  }
                else
                    {  $$fileStatus = ::FILE_CHANGED();  };
                }
            else
                {  $$fileStatus = ::FILE_DOESNTEXIST();  };
            };

        close(FH_CONFIGFILEINFO);
        }
    else # !$fileIsOkay
        {
        while (scalar @configFiles)
            {
            my $file = shift @configFiles;
            my $fileStatus = shift @configFiles;

            if (-e $file)
                {  $$fileStatus = ::FILE_CHANGED();  }
            else
                {  $$fileStatus = ::FILE_DOESNTEXIST();  };
            };
        };

    if ($userConfigFiles{'Menu.txt'} == ::FILE_SAME() && $rebuildEverything)
        {  $userConfigFiles{'Menu.txt'} = ::FILE_CHANGED();  };
    };


#
#   Function: SaveConfigFileInfo
#
#   Saves the config file info to disk.  You *must* save all other config files first, such as <Menu.txt> and <Topics.txt>.
#
sub SaveConfigFileInfo
    {
    my ($self) = @_;

    open (FH_CONFIGFILEINFO, '>' . NaturalDocs::Project->DataFile('ConfigFileInfo.nd'))
        or die "Couldn't save " . NaturalDocs::Project->DataFile('ConfigFileInfo.nd') . ".\n";

    binmode(FH_CONFIGFILEINFO);

    print FH_CONFIGFILEINFO '' . ::BINARY_FORMAT();

    NaturalDocs::Version->ToBinaryFile(\*FH_CONFIGFILEINFO, NaturalDocs::Settings->AppVersion());

    print FH_CONFIGFILEINFO pack('NNNNN', (stat($self->UserConfigFile('Menu.txt')))[9],
                                                                (stat($self->MainConfigFile('Topics.txt')))[9],
                                                                (stat($self->UserConfigFile('Topics.txt')))[9],
                                                                (stat($self->MainConfigFile('Languages.txt')))[9],
                                                                (stat($self->UserConfigFile('Languages.txt')))[9] );

    close(FH_CONFIGFILEINFO);
    };


#
#   Function: LoadImageFileInfo
#
#   Loads the image file info from disk.
#
sub LoadImageFileInfo
    {
    my ($self) = @_;

    my $version = NaturalDocs::BinaryFile->OpenForReading( NaturalDocs::Project->DataFile('ImageFileInfo.nd') );
    my $fileIsOkay;

    if (defined $version)
        {
        # It hasn't changed since being introduced.

        if (NaturalDocs::Version->CheckFileFormat($version))
            {  $fileIsOkay = 1;  }
        else
            {  NaturalDocs::BinaryFile->Close();  };
        };

    if ($fileIsOkay)
        {
        # [AString16: file name or undef]

        while (my $imageFile = NaturalDocs::BinaryFile->GetAString16())
            {
            # [UInt32: last modified]
            # [UInt8: was used]

            my $lastModified = NaturalDocs::BinaryFile->GetUInt32();
            my $wasUsed = NaturalDocs::BinaryFile->GetUInt8();

            my $imageFileObject = $imageFiles{$imageFile};

            # If there's an image file in ImageFileInfo.nd that no longer exists...
            if (!$imageFileObject)
                {
                $imageFileObject = NaturalDocs::Project::ImageFile->New($lastModified, ::FILE_DOESNTEXIST(), $wasUsed);
                $imageFiles{$imageFile} = $imageFileObject;

                if ($wasUsed)
                    {  $imageFilesToPurge{$imageFile} = 1;  };
                }
            else
                {
                $imageFileObject->SetWasUsed($wasUsed);

                # This will be removed if it gets any references.
                if ($wasUsed)
                    {  $imageFilesToPurge{$imageFile} = 1;  };

                if ($imageFileObject->LastModified() == $lastModified && !$rebuildEverything)
                    {  $imageFileObject->SetStatus(::FILE_SAME());  }
                else
                    {  $imageFileObject->SetStatus(::FILE_CHANGED());  };
                };
            };

        NaturalDocs::BinaryFile->Close();
        }

    else # !$fileIsOkay
        {
        $self->RebuildEverything();
        };
    };


#
#   Function: SaveImageFileInfo
#
#   Saves the image file info to disk.
#
sub SaveImageFileInfo
    {
    my $self = shift;

    NaturalDocs::BinaryFile->OpenForWriting( NaturalDocs::Project->DataFile('ImageFileInfo.nd') );

    while (my ($imageFile, $imageFileInfo) = each %imageFiles)
        {
        if ($imageFileInfo->Status() != ::FILE_DOESNTEXIST())
            {
            # [AString16: file name or undef]
            # [UInt32: last modification time]
            # [UInt8: was used]

            NaturalDocs::BinaryFile->WriteAString16($imageFile);
            NaturalDocs::BinaryFile->WriteUInt32($imageFileInfo->LastModified());
            NaturalDocs::BinaryFile->WriteUInt8( ($imageFileInfo->ReferenceCount() > 0 ? 1 : 0) );
            };
        };

    NaturalDocs::BinaryFile->WriteAString16(undef);
    NaturalDocs::BinaryFile->Close();
    };


#
#   Function: MigrateOldFiles
#
#   If the project uses the old file names used prior to 1.14, it converts them to the new file names.
#
sub MigrateOldFiles
    {
    my ($self) = @_;

    my $projectDirectory = NaturalDocs::Settings->ProjectDirectory();

    # We use the menu file as a test to see if we're using the new format.
    if (-e NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs_Menu.txt'))
        {
        # The Data subdirectory would have been created by NaturalDocs::Settings.

        rename( NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs_Menu.txt'), $self->UserConfigFile('Menu.txt') );

        if (-e NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.sym'))
            {  rename( NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.sym'), $self->DataFile('SymbolTable.nd') );  };

        if (-e NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.files'))
            {  rename( NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.files'), $self->DataFile('FileInfo.nd') );  };

        if (-e NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.m'))
            {  rename( NaturalDocs::File->JoinPaths($projectDirectory, 'NaturalDocs.m'), $self->DataFile('PreviousMenuState.nd') );  };
        };
    };



###############################################################################
# Group: Config and Data File Functions


#
#   Function: MainConfigFile
#
#   Returns the full path to the passed main configuration file.  Pass the file name only.
#
sub MainConfigFile #(string file)
    {
    my ($self, $file) = @_;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->ConfigDirectory(), $file );
    };

#
#   Function: MainConfigFileStatus
#
#   Returns the <FileStatus> of the passed main configuration file.  Pass the file name only.
#
sub MainConfigFileStatus #(string file)
    {
    my ($self, $file) = @_;
    return $mainConfigFiles{$file};
    };

#
#   Function: UserConfigFile
#
#   Returns the full path to the passed user configuration file.  Pass the file name only.
#
sub UserConfigFile #(string file)
    {
    my ($self, $file) = @_;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->ProjectDirectory(), $file );
    };

#
#   Function: UserConfigFileStatus
#
#   Returns the <FileStatus> of the passed user configuration file.  Pass the file name only.
#
sub UserConfigFileStatus #(string file)
    {
    my ($self, $file) = @_;
    return $userConfigFiles{$file};
    };

#
#   Function: DataFile
#
#   Returns the full path to the passed data file.  Pass the file name only.
#
sub DataFile #(string file)
    {
    my ($self, $file) = @_;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->ProjectDataDirectory(), $file );
    };




###############################################################################
# Group: Source File Functions


# Function: FilesToParse
# Returns an existence hashref of the <FileNames> to parse.  This is not a copy of the data, so don't change it.
sub FilesToParse
    {  return \%filesToParse;  };

# Function: FilesToBuild
# Returns an existence hashref of the <FileNames> to build.  This is not a copy of the data, so don't change it.
sub FilesToBuild
    {  return \%filesToBuild;  };

# Function: FilesToPurge
# Returns an existence hashref of the <FileNames> that had content last time, but now either don't anymore or were deleted.
# This is not a copy of the data, so don't change it.
sub FilesToPurge
    {  return \%filesToPurge;  };

#
#   Function: RebuildFile
#
#   Adds the file to the list of files to build.  This function will automatically filter out files that don't have Natural Docs content and
#   files that are part of <FilesToPurge()>.  If this gets called on a file and that file later gets Natural Docs content, it will be added.
#
#   Parameters:
#
#       file - The <FileName> to build or rebuild.
#
sub RebuildFile #(file)
    {
    my ($self, $file) = @_;

    # We don't want to add it to the build list if it doesn't exist, doesn't have Natural Docs content, or it's going to be purged.
    # If it wasn't parsed yet and will later be found to have ND content, it will be added by SetHasContent().
    if (exists $supportedFiles{$file} && !exists $filesToPurge{$file} && $supportedFiles{$file}->HasContent())
        {
        $filesToBuild{$file} = 1;

        if (exists $unbuiltFilesWithContent{$file})
            {  delete $unbuiltFilesWithContent{$file};  };
        };
    };


#
#   Function: ReparseEverything
#
#   Adds all supported files to the list of files to parse.  This does not necessarily mean these files are going to be rebuilt.
#
sub ReparseEverything
    {
    my ($self) = @_;

    if (!$reparseEverything)
        {
        foreach my $file (keys %supportedFiles)
            {
            $filesToParse{$file} = 1;
            };

        $reparseEverything = 1;
        };
    };


#
#   Function: RebuildEverything
#
#   Adds all supported files to the list of files to build.  This does not necessarily mean these files are going to be reparsed.
#
sub RebuildEverything
    {
    my ($self) = @_;

    foreach my $file (keys %unbuiltFilesWithContent)
        {
        $filesToBuild{$file} = 1;
        };

    %unbuiltFilesWithContent = ( );
    $rebuildEverything = 1;

    NaturalDocs::SymbolTable->RebuildAllIndexes();

    if ($userConfigFiles{'Menu.txt'} == ::FILE_SAME())
        {  $userConfigFiles{'Menu.txt'} = ::FILE_CHANGED();  };

    while (my ($imageFile, $imageObject) = each %imageFiles)
        {
        if ($imageObject->ReferenceCount())
            {  $imageFilesToUpdate{$imageFile} = 1;  };
        };
    };


# Function: UnbuiltFilesWithContent
# Returns an existence hashref of the <FileNames> that have Natural Docs content but are not part of <FilesToBuild()>.  This is
# not a copy of the data so don't change it.
sub UnbuiltFilesWithContent
    {  return \%unbuiltFilesWithContent;  };

# Function: FilesWithContent
# Returns and existence hashref of the <FileNames> that have Natural Docs content.
sub FilesWithContent
    {
    # Don't keep this one internally, but there's an easy way to make it.
    return { %filesToBuild, %unbuiltFilesWithContent };
    };


#
#   Function: HasContent
#
#   Returns whether the <FileName> contains Natural Docs content.
#
sub HasContent #(file)
    {
    my ($self, $file) = @_;

    if (exists $supportedFiles{$file})
        {  return $supportedFiles{$file}->HasContent();  }
    else
        {  return undef;  };
    };


#
#   Function: SetHasContent
#
#   Sets whether the <FileName> has Natural Docs content or not.
#
sub SetHasContent #(file, hasContent)
    {
    my ($self, $file, $hasContent) = @_;

    if (exists $supportedFiles{$file} && $supportedFiles{$file}->HasContent() != $hasContent)
        {
        # If the file now has content...
        if ($hasContent)
            {
            $filesToBuild{$file} = 1;
            }

        # If the file's content has been removed...
        else
            {
            delete $filesToBuild{$file};  # may not be there
            $filesToPurge{$file} = 1;
            };

        $supportedFiles{$file}->SetHasContent($hasContent);
        };
    };


#
#   Function: StatusOf
#
#   Returns the <FileStatus> of the passed <FileName>.
#
sub StatusOf #(file)
    {
    my ($self, $file) = @_;

    if (exists $supportedFiles{$file})
        {  return $supportedFiles{$file}->Status();  }
    else
        {  return ::FILE_DOESNTEXIST();  };
    };


#
#   Function: DefaultMenuTitleOf
#
#   Returns the default menu title of the <FileName>.  If one isn't specified, it returns the <FileName>.
#
sub DefaultMenuTitleOf #(file)
    {
    my ($self, $file) = @_;

    if (exists $supportedFiles{$file})
        {  return $supportedFiles{$file}->DefaultMenuTitle();  }
    else
        {  return $file;  };
    };


#
#   Function: SetDefaultMenuTitle
#
#   Sets the <FileName's> default menu title.
#
sub SetDefaultMenuTitle #(file, menuTitle)
    {
    my ($self, $file, $menuTitle) = @_;

    if (exists $supportedFiles{$file} && $supportedFiles{$file}->DefaultMenuTitle() ne $menuTitle)
        {
        $supportedFiles{$file}->SetDefaultMenuTitle($menuTitle);
        NaturalDocs::Menu->OnDefaultTitleChange($file);
        };
    };


#
#   Function: MostUsedLanguage
#
#   Returns the name of the most used language in the source trees.  Does not include text files.
#
sub MostUsedLanguage
    {  return $mostUsedLanguage;  };




###############################################################################
# Group: Image File Functions


#
#   Function: ImageFileExists
#   Returns whether the passed image file exists.
#
sub ImageFileExists #(FileName file) => bool
    {
    my ($self, $file) = @_;

    if (!exists $imageFiles{$file})
        {  $file = $insensitiveImageFiles{lc($file)};  };

    return (exists $imageFiles{$file} && $imageFiles{$file}->Status() != ::FILE_DOESNTEXIST());
    };


#
#   Function: ImageFileDimensions
#   Returns the dimensions of the passed image file as the array ( width, height ).  Returns them both as undef if it cannot be
#   determined.
#
sub ImageFileDimensions #(FileName file) => (int, int)
    {
    my ($self, $file) = @_;

    if (!exists $imageFiles{$file})
        {  $file = $insensitiveImageFiles{lc($file)};  };

    my $object = $imageFiles{$file};
    if (!$object)
        {  die "Tried to get the dimensions of an image that doesn't exist.";  };

    if ($object->Width() == -1)
        {  $self->DetermineImageDimensions($file);  };

    return ($object->Width(), $object->Height());
    };


#
#   Function: ImageFileCapitalization
#   Returns the properly capitalized version of the passed image <FileName>.  Image file paths are treated as case insensitive
#   regardless of whether the underlying operating system is or not, so we have to make sure the final version matches the
#   capitalization of the actual file.
#
sub ImageFileCapitalization #(FileName file) => FileName
    {
    my ($self, $file) = @_;

    if (exists $imageFiles{$file})
        {  return $file;  }
    elsif (exists $insensitiveImageFiles{lc($file)})
        {  return $insensitiveImageFiles{lc($file)};  }
    else
        {  die "Tried to get the capitalization of an image file that doesn't exist.";  };
    };


#
#   Function: AddImageFileReference
#   Adds a reference to the passed image <FileName>.
#
sub AddImageFileReference #(FileName imageFile)
    {
    my ($self, $imageFile) = @_;

    if (!exists $imageFiles{$imageFile})
        {  $imageFile = $insensitiveImageFiles{lc($imageFile)};  };

    my $imageFileInfo = $imageFiles{$imageFile};

    if ($imageFileInfo == undef || $imageFileInfo->Status() == ::FILE_DOESNTEXIST())
        {  die "Tried to add a reference to a non-existant image file.";  };

    if ($imageFileInfo->AddReference() == 1)
        {
        delete $imageFilesToPurge{$imageFile};

        if (!$imageFileInfo->WasUsed() ||
            $imageFileInfo->Status() == ::FILE_NEW() ||
            $imageFileInfo->Status() == ::FILE_CHANGED())
            {  $imageFilesToUpdate{$imageFile} = 1;  };
        };
    };


#
#   Function: DeleteImageFileReference
#   Deletes a reference from the passed image <FileName>.
#
sub DeleteImageFileReference #(FileName imageFile)
    {
    my ($self, $imageFile) = @_;

    if (!exists $imageFiles{$imageFile})
        {  $imageFile = $insensitiveImageFiles{lc($imageFile)};  };

    if (!exists $imageFiles{$imageFile})
        {  die "Tried to delete a reference to a non-existant image file.";  };

    if ($imageFiles{$imageFile}->DeleteReference() == 0)
        {
        delete $imageFilesToUpdate{$imageFile};

        if ($imageFiles{$imageFile}->WasUsed())
            {  $imageFilesToPurge{$imageFile} = 1;  };
        };
    };


#
#   Function: ImageFilesToUpdate
#   Returns an existence hashref of image <FileNames> that need to be updated.  *Do not change.*
#
sub ImageFilesToUpdate
    {  return \%imageFilesToUpdate;  };


#
#   Function: ImageFilesToPurge
#   Returns an existence hashref of image <FileNames> that need to be updated.  *Do not change.*
#
sub ImageFilesToPurge
    {  return \%imageFilesToPurge;  };



###############################################################################
# Group: Support Functions

#
#   Function: GetAllSupportedFiles
#
#   Gets all the supported files in the passed directory and its subdirectories and puts them into <supportedFiles>.  The only
#   attribute that will be set is <NaturalDocs::Project::SourceFile->LastModified()>.  Also sets <mostUsedLanguage>.
#
sub GetAllSupportedFiles
    {
    my ($self) = @_;

    my @directories = @{NaturalDocs::Settings->InputDirectories()};
    my $isCaseSensitive = NaturalDocs::File->IsCaseSensitive();

    # Keys are language names, values are counts.
    my %languageCounts;


    # Make an existence hash of excluded directories.

    my %excludedDirectories;
    my $excludedDirectoryArrayRef = NaturalDocs::Settings->ExcludedInputDirectories();

    foreach my $excludedDirectory (@$excludedDirectoryArrayRef)
        {
        if ($isCaseSensitive)
            {  $excludedDirectories{$excludedDirectory} = 1;  }
        else
            {  $excludedDirectories{lc($excludedDirectory)} = 1;  };
        };


    my $imagesOnly;
    my $language;

    while (scalar @directories)
        {
        my $directory = pop @directories;

        opendir DIRECTORYHANDLE, $directory;
        my @entries = readdir DIRECTORYHANDLE;
        closedir DIRECTORYHANDLE;

        @entries = NaturalDocs::File->NoUpwards(@entries);

        foreach my $entry (@entries)
            {
            my $fullEntry = NaturalDocs::File->JoinPaths($directory, $entry);

            # If an entry is a directory, recurse.
            if (-d $fullEntry)
                {
                # Join again with the noFile flag set in case the platform handles them differently.
                $fullEntry = NaturalDocs::File->JoinPaths($directory, $entry, 1);

                if ($isCaseSensitive)
                    {
                    if (!exists $excludedDirectories{$fullEntry})
                        {  push @directories, $fullEntry;  };
                    }
                else
                    {
                    if (!exists $excludedDirectories{lc($fullEntry)})
                        {  push @directories, $fullEntry;  };
                    };
                }

            # Otherwise add it if it's a supported extension.
            else
                {
                my $extension = NaturalDocs::File->ExtensionOf($entry);

                if (exists $imageFileExtensions{lc($extension)})
                    {
                    my $fileObject = NaturalDocs::Project::ImageFile->New( (stat($fullEntry))[9], ::FILE_NEW(), 0 );
                    $imageFiles{$fullEntry} = $fileObject;

                    my $lcFullEntry = lc($fullEntry);

                    if (!exists $insensitiveImageFiles{$lcFullEntry} ||
                        ($fullEntry cmp $insensitiveImageFiles{$lcFullEntry}) < 0)
                        {
                        $insensitiveImageFiles{$lcFullEntry} = $fullEntry;
                        };
                    }
                elsif (!$imagesOnly && ($language = NaturalDocs::Languages->LanguageOf($fullEntry)) )
                    {
                    my $fileObject = NaturalDocs::Project::SourceFile->New();
                    $fileObject->SetLastModified(( stat($fullEntry))[9] );
                    $supportedFiles{$fullEntry} = $fileObject;

                    $languageCounts{$language->Name()}++;
                    };
                };
            };


        # After we run out of source directories, add the image directories.

        if (scalar @directories == 0 && !$imagesOnly)
            {
            $imagesOnly = 1;
            @directories = @{NaturalDocs::Settings->ImageDirectories()};
            };
        };


    my $topCount = 0;

    while (my ($language, $count) = each %languageCounts)
        {
        if ($count > $topCount && $language ne 'Text File')
            {
            $topCount = $count;
            $mostUsedLanguage = $language;
            };
        };
    };


#
#   Function: DetermineImageDimensions
#
#   Attempts to determine the dimensions of the passed image and apply them to their object in <imageFiles>.  Will set them to
#   undef if they can't be determined.
#
sub DetermineImageDimensions #(FileName imageFile)
    {
    my ($self, $imageFile) = @_;

    my $imageFileObject = $imageFiles{$imageFile};
    if (!defined $imageFileObject)
        {  die "Tried to determine image dimensions of a file with no object.";  };

    my $extension = lc( NaturalDocs::File->ExtensionOf($imageFile) );
    my ($width, $height);

    if ($imageFileExtensions{$extension})
        {
        open(FH_IMAGEFILE, '<' . $imageFile)
            or die 'Could not open ' . $imageFile . "\n";
        binmode(FH_IMAGEFILE);

        my $raw;

        if ($extension eq 'gif')
            {
            read(FH_IMAGEFILE, $raw, 6);

            if ($raw eq 'GIF87a' || $raw eq 'GIF89a')
                {
                read(FH_IMAGEFILE, $raw, 4);
                ($width, $height) = unpack('vv', $raw);
                };
            }

        elsif ($extension eq 'png')
            {
            read(FH_IMAGEFILE, $raw, 8);

            if ($raw eq "\x89PNG\x0D\x0A\x1A\x0A")
                {
                seek(FH_IMAGEFILE, 4, 1);
                read(FH_IMAGEFILE, $raw, 4);

                if ($raw eq 'IHDR')
                    {
                    read(FH_IMAGEFILE, $raw, 8);
                    ($width, $height) = unpack('NN', $raw);
                    };
                };
            }

        elsif ($extension eq 'bmp')
            {
            read(FH_IMAGEFILE, $raw, 2);

            if ($raw eq 'BM')
                {
                seek(FH_IMAGEFILE, 16, 1);
                read(FH_IMAGEFILE, $raw, 8);

                ($width, $height) = unpack('VV', $raw);
                };
            }

        elsif ($extension eq 'jpg' || $extension eq 'jpeg')
            {
            read(FH_IMAGEFILE, $raw, 2);
            my $isOkay = ($raw eq "\xFF\xD8");

            while ($isOkay)
                {
                read(FH_IMAGEFILE, $raw, 4);
                my ($marker, $code, $length) = unpack('CCn', $raw);

                $isOkay = ($marker eq 0xFF);

                if ($isOkay)
                    {
                    if ($code >= 0xC0 && $code <= 0xC3)
                        {
                        read(FH_IMAGEFILE, $raw, 5);
                        ($height, $width) = unpack('xnn', $raw);
                        last;
                        }

                    else
                        {
                        $isOkay = seek(FH_IMAGEFILE, $length - 2, 1);
                        };
                    };
                };
            };

        close(FH_IMAGEFILE);
        };


    # Sanity check the values.  Although images can theoretically be bigger than 5000, most won't.  The worst that happens in this
    # case is just that they don't get length and width values in the output anyway.
    if ($width > 0 && $width < 5000 && $height > 0 && $height < 5000)
        {  $imageFileObject->SetDimensions($width, $height);  }
    else
        {  $imageFileObject->SetDimensions(undef, undef);  };
    };


1;
