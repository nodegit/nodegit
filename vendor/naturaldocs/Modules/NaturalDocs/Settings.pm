###############################################################################
#
#   Package: NaturalDocs::Settings
#
###############################################################################
#
#   A package to handle the command line and various other program settings.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use Cwd ();

use NaturalDocs::Settings::BuildTarget;

use strict;
use integer;

package NaturalDocs::Settings;


###############################################################################
# Group: Information

=pod begin nd

    Topic: Usage and Dependencies

        - The <Constant Functions> can be called immediately.

        - Prior to initialization, <NaturalDocs::Builder> must have all its output packages registered.

        - To initialize, call <Load()>.  All functions except <InputDirectoryNameOf()> will then be available.

        - <GenerateDirectoryNames()> must be called before <InputDirectoryNameOf()> will work.  Currently it is called by
          <NaturalDocs::Menu->LoadAndUpdate()>.


    Architecture: Internal Overview

        - <Load()> first parses the command line, gathering all the settings and checking for errors.  All <NaturalDocs::Builder>
          packages must be registered before this is called because it needs their command line options.
          <NaturalDocs::Project->ReparseEverything()> and <NaturalDocs::Project->RebuildEverything()> are called right away if -r
          or -ro are used.

        - Output directories are *not* named at this point.  See <Named Directories>.

        - The previous settings from the last time Natural Docs was run are loaded and compared to the current settings.
          <NaturalDocs::Project->ReparseEverything()> and <NaturalDocs::Project->RebuildEverything()> are called if there are
          any differences that warrant it.

        - It then waits for <GenerateDirectoryNames()> to be called by <NaturalDocs::Menu>.  The reason for this is that the
          previous directory names are stored as hints in the menu file, for reasons explained in <Named Directories>.  Once that
          happens all the unnamed directories have names generated for them so everything is named.  The package is completely
          set up.

        - The input directories are stored in an array instead of a hash because the order they were declared in matters.  If two
          people use multiple input directories on separate computers without sharing a menu file, they should at least get consistent
          directory names by declaring them in the same order.


    Architecture: Named Directories

        Ever since Natural Docs introduced multiple input directories in 1.16, they've had to be named.  Since they don't necessarily
        extend from the same root anymore, they can't share an output directory without the risk of file name conflicts.  There was
        an early attempt at giving them actual names, but now they're just numbered from 1.

        Directory names aren't generated right away.  It waits for <Menu.txt> to load because that holds the obfuscated names from
        the last run.  <NaturalDocs::Menu> then calls <GenerateDirectoryNames()> and passes those along as hints.
        <GenerateDirectoryNames()> then applies them to any matches and generates new ones for any remaining.  This is done so
        that output page locations can remain consistent when built on multiple computers, so long as the menu file is shared.  I tend
        to think the menu file is the most likely configuration file to be shared.


    Architecture: Removed Directories

        Directories that were part of the previous run but aren't anymore are still stored in the package.  The primary reason, though
        there may be others, is file purging.  If an input directory is removed, all the output files that were generated from anything
        in it need to be removed.  To find out what the output file name was for a removed source file, it needs to be able to split it
        from it's original input directory and know what that directory was named.  If this didn't happen those output files would be
        orphaned, as was the case prior to 1.32.

=cut



###############################################################################
# Group: Variables


# handle: PREVIOUS_SETTINGS_FILEHANDLE
# The file handle used with <PreviousSettings.nd>.

# array: inputDirectories
# An array of input directories.
my @inputDirectories;

# array: inputDirectoryNames
# An array of the input directory names.  Each name corresponds to the directory of the same index in <inputDirectories>.
my @inputDirectoryNames;

# array: imageDirectories
# An array of image directories.
my @imageDirectories;

# array: imageDirectoryNames
# An array of the image directory names.  Each name corresponds to the directory of the same index in <imageDirectories>.
my @imageDirectoryNames;

# array: relativeImageDirectories
# An array of the relative paths for images.  The asterisks found in the command line are not present.
my @relativeImageDirectories;

# array: excludedInputDirectories
# An array of input directories to exclude.
my @excludedInputDirectories;

# array: removedInputDirectories
# An array of input directories that were once in the command line but are no longer.
my @removedInputDirectories;

# array: removedInputDirectoryNames
# An array of the removed input directories' names.  Each name corresponds to the directory of the same index in
# <removedInputDirectories>.
my @removedInputDirectoryNames;

# array: removedImageDirectories
# An array of image directories that were once in the command line but are no longer.
my @removedImageDirectories;

# array: removedImageDirectoryNames
# An array of the removed image directories' names.  Each name corresponds to the directory of the same index in
# <removedImageDirectories>.
my @removedImageDirectoryNames;

# var: projectDirectory
# The project directory.
my $projectDirectory;

# array: buildTargets
# An array of <NaturalDocs::Settings::BuildTarget>s.
my @buildTargets;

# var: documentedOnly
# Whether undocumented code aspects should be included in the output.
my $documentedOnly;

# int: tabLength
# The number of spaces in tabs.
my $tabLength;

# bool: noAutoGroup
# Whether auto-grouping is turned off.
my $noAutoGroup;

# bool: onlyFileTitles
# Whether source files should always use the file name as the title.
my $onlyFileTitles;

# bool: isQuiet
# Whether the script should be run in quiet mode or not.
my $isQuiet;

# bool: rebuildData
# WHether most data files should be ignored and rebuilt.
my $rebuildData;

# array: styles
# An array of style names to use, most important first.
my @styles;

# var: highlightCode
# Whether syntax highlighting should be applied to code tags.
my $highlightCode;

# var: highlightAnonymous
# Whether syntax highlighting should be applied to anonymous code tags.
my $highlightAnonymous;


###############################################################################
# Group: Files


#
#   File: PreviousSettings.nd
#
#   Stores the previous command line settings.
#
#   Format:
#
#       > [BINARY_FORMAT]
#       > [VersionInt: app version]
#
#       The file starts with the standard <BINARY_FORMAT> <VersionInt> header.
#
#       > [UInt8: tab length]
#       > [UInt8: documented only (0 or 1)]
#       > [UInt8: no auto-group (0 or 1)]
#       > [UInt8: only file titles (0 or 1)]
#		> [UInt8: highlight code (0 or 1)]
#		> [UInt8: highlight anonymous (0 or 1)]
#       >
#       > [UInt8: number of input directories]
#       > [AString16: input directory] [AString16: input directory name] ...
#
#       A count of input directories, then that number of directory/name pairs.
#
#       > [UInt8: number of output targets]
#       > [AString16: output directory] [AString16: output format command line option] ...
#
#       A count of output targets, then that number of directory/format pairs.
#
#
#   Revisions:
#
#		1.51:
#
#			- Removed charset.
#
#		1.5:
#
#			- Added highlight code and highlight anonymous.
#
#       1.4:
#
#           - Added only file titles.
#
#       1.33:
#
#           - Added charset.
#
#       1.3:
#
#           - Removed headers-only, which was a 0/1 UInt8 after tab length.
#           - Change auto-group level (1 = no, 2 = yes, 3 = full only) to no auto-group (0 or 1).
#
#       1.22:
#
#           - Added auto-group level.
#
#       1.2:
#
#           - File was added to the project.  Prior to 1.2, it didn't exist.
#


###############################################################################
# Group: Action Functions

#
#   Function: Load
#
#   Loads and parses all settings from the command line and configuration files.  Will exit if the options are invalid or the syntax
#   reference was requested.
#
sub Load
    {
    my ($self) = @_;

    $self->ParseCommandLine();
    $self->LoadAndComparePreviousSettings();
    };


#
#   Function: Save
#
#   Saves all settings in configuration files to disk.
#
sub Save
    {
    my ($self) = @_;

    $self->SavePreviousSettings();
    };


#
#   Function: GenerateDirectoryNames
#
#   Generates names for each of the input and image directories, which can later be retrieved with <InputDirectoryNameOf()>
#   and <ImageDirectoryNameOf()>.
#
#   Parameters:
#
#       inputHints - A hashref of suggested input directory names, where the keys are the directories and the values are the names.
#                        These take precedence over anything generated.  You should include names for directories that are no longer in
#                        the command line.  This parameter may be undef.
#       imageHints - Same as inputHints, only for the image directories.
#
sub GenerateDirectoryNames #(hashref inputHints, hashref imageHints)
    {
    my ($self, $inputHints, $imageHints) = @_;

    my %usedInputNames;
    my %usedImageNames;


    if (defined $inputHints)
        {
        # First, we have to convert all non-numeric names to numbers, since they may come from a pre-1.32 menu file.  We do it
        # here instead of in NaturalDocs::Menu to keep the naming scheme centralized.

        my @names = values %$inputHints;
        my $hasNonNumeric;

        foreach my $name (@names)
            {
            if ($name !~ /^[0-9]+$/)
                {
                $hasNonNumeric = 1;
                last;
                };
            };


        if ($hasNonNumeric)
            {
            # Hash mapping old names to new names.
            my %conversion;

            # The sequential number to use.  Starts at two because we want 'default' to be one.
            my $currentNumber = 2;

            # If there's only one name, we set it to one no matter what it was set to before.
            if (scalar @names == 1)
                {  $conversion{$names[0]} = 1;  }
            else
                {
                # We sort the list first because we want the end result to be predictable.  This conversion could be happening on many
                # machines, and they may not all specify the input directories in the same order.  They need to all come up with the
                # same result.
                @names = sort @names;

                foreach my $name (@names)
                    {
                    if ($name eq 'default')
                        {  $conversion{$name} = 1;  }
                    else
                        {
                        $conversion{$name} = $currentNumber;
                        $currentNumber++;
                        };
                    };
                };

            # Convert them to the new names.
            foreach my $directory (keys %$inputHints)
                {
                $inputHints->{$directory} = $conversion{ $inputHints->{$directory} };
                };
            };


        # Now we apply all the names from the hints, and save any unused ones as removed directories.

        for (my $i = 0; $i < scalar @inputDirectories; $i++)
            {
            if (exists $inputHints->{$inputDirectories[$i]})
                {
                $inputDirectoryNames[$i] = $inputHints->{$inputDirectories[$i]};
                $usedInputNames{ $inputDirectoryNames[$i] } = 1;
                delete $inputHints->{$inputDirectories[$i]};
                };
            };


        # Any remaining hints are saved as removed directories.

        while (my ($directory, $name) = each %$inputHints)
            {
            push @removedInputDirectories, $directory;
            push @removedInputDirectoryNames, $name;
            };
        };


    if (defined $imageHints)
        {
        # Image directory names were never non-numeric, so there is no conversion.  Apply all the names from the hints.

        for (my $i = 0; $i < scalar @imageDirectories; $i++)
            {
            if (exists $imageHints->{$imageDirectories[$i]})
                {
                $imageDirectoryNames[$i] = $imageHints->{$imageDirectories[$i]};
                $usedImageNames{ $imageDirectoryNames[$i] } = 1;
                delete $imageHints->{$imageDirectories[$i]};
                };
            };


        # Any remaining hints are saved as removed directories.

        while (my ($directory, $name) = each %$imageHints)
            {
            push @removedImageDirectories, $directory;
            push @removedImageDirectoryNames, $name;
            };
        };


    # Now we generate names for anything remaining.

    my $inputCounter = 1;

    for (my $i = 0; $i < scalar @inputDirectories; $i++)
        {
        if (!defined $inputDirectoryNames[$i])
            {
            while (exists $usedInputNames{$inputCounter})
                {  $inputCounter++;  };

            $inputDirectoryNames[$i] = $inputCounter;
            $usedInputNames{$inputCounter} = 1;

            $inputCounter++;
            };
        };


    my $imageCounter = 1;

    for (my $i = 0; $i < scalar @imageDirectories; $i++)
        {
        if (!defined $imageDirectoryNames[$i])
            {
            while (exists $usedImageNames{$imageCounter})
                {  $imageCounter++;  };

            $imageDirectoryNames[$i] = $imageCounter;
            $usedImageNames{$imageCounter} = 1;

            $imageCounter++;
            };
        };
    };



###############################################################################
# Group: Information Functions


#
#   Function: InputDirectories
#
#   Returns an arrayref of input directories.  Do not change.
#
#   This will not return any removed input directories.
#
sub InputDirectories
    {  return \@inputDirectories;  };

#
#   Function: InputDirectoryNameOf
#
#   Returns the generated name of the passed input directory.  <GenerateDirectoryNames()> must be called once before this
#   function is available.
#
#   If a name for a removed input directory is available, it will be returned as well.
#
sub InputDirectoryNameOf #(directory)
    {
    my ($self, $directory) = @_;

    for (my $i = 0; $i < scalar @inputDirectories; $i++)
        {
        if ($directory eq $inputDirectories[$i])
            {  return $inputDirectoryNames[$i];  };
        };

    for (my $i = 0; $i < scalar @removedInputDirectories; $i++)
        {
        if ($directory eq $removedInputDirectories[$i])
            {  return $removedInputDirectoryNames[$i];  };
        };

    return undef;
    };


#
#   Function: SplitFromInputDirectory
#
#   Takes an input file name and returns the array ( inputDirectory, relativePath ).
#
#   If the file cannot be split from an input directory, it will try to do it with the removed input directories.
#
sub SplitFromInputDirectory #(file)
    {
    my ($self, $file) = @_;

    foreach my $directory (@inputDirectories, @removedInputDirectories)
        {
        if (NaturalDocs::File->IsSubPathOf($directory, $file))
            {  return ( $directory, NaturalDocs::File->MakeRelativePath($directory, $file) );  };
        };

    return ( );
    };


#
#   Function: ImageDirectories
#
#   Returns an arrayref of image directories.  Do not change.
#
#   This will not return any removed image directories.
#
sub ImageDirectories
    {  return \@imageDirectories;  };


#
#   Function: ImageDirectoryNameOf
#
#   Returns the generated name of the passed image or input directory.  <GenerateDirectoryNames()> must be called once before
#   this function is available.
#
#   If a name for a removed input or image directory is available, it will be returned as well.
#
sub ImageDirectoryNameOf #(directory)
    {
    my ($self, $directory) = @_;

    for (my $i = 0; $i < scalar @imageDirectories; $i++)
        {
        if ($directory eq $imageDirectories[$i])
            {  return $imageDirectoryNames[$i];  };
        };

    for (my $i = 0; $i < scalar @removedImageDirectories; $i++)
        {
        if ($directory eq $removedImageDirectories[$i])
            {  return $removedImageDirectoryNames[$i];  };
        };

    return undef;
    };


#
#   Function: SplitFromImageDirectory
#
#   Takes an input image file name and returns the array ( imageDirectory, relativePath ).
#
#   If the file cannot be split from an image directory, it will try to do it with the removed image directories.
#
sub SplitFromImageDirectory #(file)
    {
    my ($self, $file) = @_;

    foreach my $directory (@imageDirectories, @removedImageDirectories)
        {
        if (NaturalDocs::File->IsSubPathOf($directory, $file))
            {  return ( $directory, NaturalDocs::File->MakeRelativePath($directory, $file) );  };
        };

    return ( );
    };


#
#   Function: RelativeImageDirectories
#
#   Returns an arrayref of relative image directories.  Do not change.
#
sub RelativeImageDirectories
    {  return \@relativeImageDirectories;  };


# Function: ExcludedInputDirectories
# Returns an arrayref of input directories to exclude.  Do not change.
sub ExcludedInputDirectories
    {  return \@excludedInputDirectories;  };


# Function: BuildTargets
# Returns an arrayref of <NaturalDocs::Settings::BuildTarget>s.  Do not change.
sub BuildTargets
    {  return \@buildTargets;  };


#
#   Function: OutputDirectoryOf
#
#   Returns the output directory of a builder object.
#
#   Parameters:
#
#       object - The builder object, whose class is derived from <NaturalDocs::Builder::Base>.
#
#   Returns:
#
#       The builder directory, or undef if the object wasn't found..
#
sub OutputDirectoryOf #(object)
    {
    my ($self, $object) = @_;

    foreach my $buildTarget (@buildTargets)
        {
        if ($buildTarget->Builder() == $object)
            {  return $buildTarget->Directory();  };
        };

    return undef;
    };


# Function: Styles
# Returns an arrayref of the styles associated with the output.
sub Styles
    {  return \@styles;  };

# Function: ProjectDirectory
# Returns the project directory.
sub ProjectDirectory
    {  return $projectDirectory;  };

# Function: ProjectDataDirectory
# Returns the project data directory.
sub ProjectDataDirectory
    {  return NaturalDocs::File->JoinPaths($projectDirectory, 'Data', 1);  };

# Function: StyleDirectory
# Returns the main style directory.
sub StyleDirectory
    {  return NaturalDocs::File->JoinPaths($FindBin::RealBin, 'Styles', 1);  };

# Function: JavaScriptDirectory
# Returns the main JavaScript directory.
sub JavaScriptDirectory
    {  return NaturalDocs::File->JoinPaths($FindBin::RealBin, 'JavaScript', 1);  };

# Function: ConfigDirectory
# Returns the main configuration directory.
sub ConfigDirectory
    {  return NaturalDocs::File->JoinPaths($FindBin::RealBin, 'Config', 1);  };

# Function: DocumentedOnly
# Returns whether undocumented code aspects should be included in the output.
sub DocumentedOnly
    {  return $documentedOnly;  };

# Function: TabLength
# Returns the number of spaces tabs should be expanded to.
sub TabLength
    {  return $tabLength;  };

# Function: NoAutoGroup
# Returns whether auto-grouping is turned off.
sub NoAutoGroup
    {  return $noAutoGroup;  };

# Function: OnlyFileTitles
# Returns whether source files should always use the file name as the title.
sub OnlyFileTitles
    {  return $onlyFileTitles;  };

# Function: IsQuiet
# Returns whether the script should be run in quiet mode or not.
sub IsQuiet
    {  return $isQuiet;  };

# Function: RebuildData
# Returns whether all data files should be ignored and rebuilt.
sub RebuildData
    {  return $rebuildData;  };

# Function: HighlightCode
# Returns whether to apply syntax highlighting (start code) sections.
sub HighlightCode
	{  return $highlightCode;  }

# Function: HighlightAnonymous
# Returns whether to apply syntax highlighting to anonymous code sections designated with :, >, or |.
sub HighlightAnonymous
	{  return $highlightAnonymous;  }


###############################################################################
# Group: Constant Functions

#
#   Function: AppVersion
#
#   Returns Natural Docs' version number as an integer.  Use <TextAppVersion()> to get a printable version.
#
sub AppVersion
    {
    my ($self) = @_;
    return NaturalDocs::Version->FromString($self->TextAppVersion());
    };

#
#   Function: TextAppVersion
#
#   Returns Natural Docs' version number as plain text.
#
sub TextAppVersion
    {
    return '1.51';
    };

#
#   Function: AppURL
#
#   Returns a string of the project's current web address.
#
sub AppURL
    {  return 'http://www.naturaldocs.org';  };



###############################################################################
# Group: Support Functions


#
#   Function: ParseCommandLine
#
#   Parses and validates the command line.  Will cause the script to exit if the options ask for the syntax reference or
#   are invalid.
#
sub ParseCommandLine
    {
    my ($self) = @_;

    my %synonyms = ( 'input'    => '-i',
                                  'source' => '-i',
                                  'excludeinput' => '-xi',
                                  'excludesource' => '-xi',
                                  'images' => '-img',
                                  'output'  => '-o',
                                  'project' => '-p',
                                  'documentedonly' => '-do',
                                  'style'    => '-s',
                                  'rebuild' => '-r',
                                  'rebuildoutput' => '-ro',
                                  'tablength' => '-t',
                                  'quiet'    => '-q',
                                  'headersonly' => '-ho',
                                  'help'     => '-h',
                                  'autogroup' => '-ag',
                                  'noautogroup' => '-nag',
                                  'onlyfiletitles' => '-oft',
                                  'onlyfiletitle' => '-oft',
                                  'highlight' => '-hl',
                                  'highlighting' => '-hl' );


    my @errorMessages;

    my $valueRef;
    my $option;

    my @outputStrings;
    my @imageStrings;
    my $highlightString;


    # Sometimes $valueRef is set to $ignored instead of undef because we don't want certain errors to cause other,
    # unnecessary errors.  For example, if they set the input directory twice, we want to show that error and swallow the
    # specified directory without complaint.  Otherwise it would complain about the directory too as if it were random crap
    # inserted into the command line.
    my $ignored;

    my $index = 0;

    while ($index < scalar @ARGV)
        {
        my $arg = $ARGV[$index];

        if (substr($arg, 0, 1) eq '-')
            {
            $option = lc($arg);

            # Support options like -t2 as well as -t 2.
            if ($option =~ /^([^0-9]+)([0-9]+)$/)
                {
                $option = $1;
                splice(@ARGV, $index + 1, 0, $2);
                };

            # Convert long forms to short.
            if (substr($option, 1, 1) eq '-')
                {
                # Strip all dashes.
                my $newOption = $option;
                $newOption =~ tr/-//d;

                if (exists $synonyms{$newOption})
                    {  $option = $synonyms{$newOption};  }
                }

            if ($option eq '-i')
                {
                push @inputDirectories, undef;
                $valueRef = \$inputDirectories[-1];
                }
            elsif ($option eq '-xi')
                {
                push @excludedInputDirectories, undef;
                $valueRef = \$excludedInputDirectories[-1];
                }
            elsif ($option eq '-img')
                {
                push @imageStrings, undef;
                $valueRef = \$imageStrings[-1];
                }
            elsif ($option eq '-p')
                {
                if (defined $projectDirectory)
                    {
                    push @errorMessages, 'You cannot have more than one project directory.';
                    $valueRef = \$ignored;
                    }
                else
                    {  $valueRef = \$projectDirectory;  };
                }
            elsif ($option eq '-o')
                {
                push @outputStrings, undef;
                $valueRef = \$outputStrings[-1];
                }
            elsif ($option eq '-s')
                {
                $valueRef = \$styles[0];
                }
            elsif ($option eq '-t')
                {
                $valueRef = \$tabLength;
                }
            elsif ($option eq '-hl')
            	{
            	$valueRef = \$highlightString;
            	}
            elsif ($option eq '-ag')
                {
                push @errorMessages, 'The -ag setting is no longer supported.  You can use -nag (--no-auto-group) to turn off '
                                               . "auto-grouping, but there aren't multiple levels anymore.";
                $valueRef = \$ignored;
                }

            # Options that aren't followed by content.
            else
                {
                $valueRef = undef;

                if ($option eq '-r')
                    {
                    NaturalDocs::Project->ReparseEverything();
                    NaturalDocs::Project->RebuildEverything();
                    $rebuildData = 1;
                    }
                elsif ($option eq '-ro')
                    {
                    NaturalDocs::Project->RebuildEverything();
                    }
                elsif ($option eq '-do')
                    {  $documentedOnly = 1;  }
                elsif ($option eq '-oft')
                    {  $onlyFileTitles = 1;  }
                elsif ($option eq '-q')
                    {  $isQuiet = 1;  }
                elsif ($option eq '-ho')
                    {
                    push @errorMessages, 'The -ho setting is no longer supported.  You can have Natural Docs skip over the source file '
                                                   . 'extensions by editing Languages.txt in your project directory.';
                    }
                elsif ($option eq '-nag')
                    {  $noAutoGroup = 1;  }
                elsif ($option eq '-?' || $option eq '-h')
                    {
                    $self->PrintSyntax();
                    exit;
                    }
                else
                    {  push @errorMessages, 'Unrecognized option ' . $option;  };

                };

            }

        # Is a segment of text, not an option...
        else
            {
            if (defined $valueRef)
                {
                # We want to preserve spaces in paths.
                if (defined $$valueRef)
                    {  $$valueRef .= ' ';  };

                $$valueRef .= $arg;
                }

            else
                {
                push @errorMessages, 'Unrecognized element ' . $arg;
                };
            };

        $index++;
        };


    # Validate the style, if specified.

    if ($styles[0])
        {
        my @stylePieces = split(/ +/, $styles[0]);
        @styles = ( );

        while (scalar @stylePieces)
            {
            if (lc($stylePieces[0]) eq 'custom')
                {
                push @errorMessages, 'The "Custom" style setting is no longer supported.  Copy your custom style sheet to your '
                                               . 'project directory and you can refer to it with -s.';
                shift @stylePieces;
                }
            else
                {
                # People may use styles with spaces in them.  If a style doesn't exist, we need to join the pieces until we find one that
                # does or we run out of pieces.

                my $extras = 0;
                my $success;

                while ($extras < scalar @stylePieces)
                    {
                    my $style;

                    if (!$extras)
                        {  $style = $stylePieces[0];  }
                    else
                        {  $style = join(' ', @stylePieces[0..$extras]);  };

                    my $cssFile = NaturalDocs::File->JoinPaths( $self->StyleDirectory(), $style . '.css' );
                    if (-e $cssFile)
                        {
                        push @styles, $style;
                        splice(@stylePieces, 0, 1 + $extras);
                        $success = 1;
                        last;
                        }
                    else
                        {
                        $cssFile = NaturalDocs::File->JoinPaths( $self->ProjectDirectory(), $style . '.css' );

                        if (-e $cssFile)
                            {
                            push @styles, $style;
                            splice(@stylePieces, 0, 1 + $extras);
                            $success = 1;
                            last;
                            }
                        else
                            {  $extras++;  };
                        };
                    };

                if (!$success)
                    {
                    push @errorMessages, 'The style "' . $stylePieces[0] . '" does not exist.';
                    shift @stylePieces;
                    };
                };
            };
        }
    else
        {  @styles = ( 'Default' );  };


    # Decode and validate the output strings.

    my %outputDirectories;

    foreach my $outputString (@outputStrings)
        {
        my ($format, $directory) = split(/ /, $outputString, 2);

        if (!defined $directory)
            {  push @errorMessages, 'The -o option needs two parameters: -o [format] [directory]';  }
        else
            {
            if (!NaturalDocs::File->PathIsAbsolute($directory))
                {  $directory = NaturalDocs::File->JoinPaths(Cwd::cwd(), $directory, 1);  };

            $directory = NaturalDocs::File->CanonizePath($directory);

            if (! -e $directory || ! -d $directory)
                {
                # They may have forgotten the format portion and the directory name had a space in it.
                if (-e ($format . ' ' . $directory) && -d ($format . ' ' . $directory))
                    {
                    push @errorMessages, 'The -o option needs two parameters: -o [format] [directory]';
                    $format = undef;
                    }
                else
                    {  push @errorMessages, 'The output directory ' . $directory . ' does not exist.';  }
                }
            elsif (exists $outputDirectories{$directory})
                {  push @errorMessages, 'You cannot specify the output directory ' . $directory . ' more than once.';  }
            else
                {  $outputDirectories{$directory} = 1;  };

            if (defined $format)
                {
                my $builderPackage = NaturalDocs::Builder->OutputPackageOf($format);

                if (defined $builderPackage)
                    {
                    push @buildTargets,
                            NaturalDocs::Settings::BuildTarget->New($builderPackage->New(), $directory);
                    }
                else
                    {
                    push @errorMessages, 'The output format ' . $format . ' doesn\'t exist or is not installed.';
                    $valueRef = \$ignored;
                    };
                };
            };
        };

    if (!scalar @buildTargets)
        {  push @errorMessages, 'You did not specify an output directory.';  };


    # Decode and validate the image strings.

    foreach my $imageString (@imageStrings)
        {
        if ($imageString =~ /^ *\*/)
            {
            # The below NaturalDocs::File functions assume everything is canonized.
            $imageString = NaturalDocs::File->CanonizePath($imageString);

            my ($volume, $directoryString) = NaturalDocs::File->SplitPath($imageString, 1);
            my @directories = NaturalDocs::File->SplitDirectories($directoryString);

            shift @directories;

            $directoryString = NaturalDocs::File->JoinDirectories(@directories);
            push @relativeImageDirectories, NaturalDocs::File->JoinPath($volume, $directoryString);
            }
        else
            {
            if (!NaturalDocs::File->PathIsAbsolute($imageString))
                {  $imageString = NaturalDocs::File->JoinPaths(Cwd::cwd(), $imageString, 1);  };

            $imageString = NaturalDocs::File->CanonizePath($imageString);

            if (! -e $imageString || ! -d $imageString)
                {  push @errorMessages, 'The image directory ' . $imageString . ' does not exist.';  };

            push @imageDirectories, $imageString;
            };
        };


    # Make sure the input and project directories are specified, canonized, and exist.

    if (scalar @inputDirectories)
        {
        for (my $i = 0; $i < scalar @inputDirectories; $i++)
            {
            if (!NaturalDocs::File->PathIsAbsolute($inputDirectories[$i]))
                {  $inputDirectories[$i] = NaturalDocs::File->JoinPaths(Cwd::cwd(), $inputDirectories[$i], 1);  };

            $inputDirectories[$i] = NaturalDocs::File->CanonizePath($inputDirectories[$i]);

            if (! -e $inputDirectories[$i] || ! -d $inputDirectories[$i])
                {  push @errorMessages, 'The input directory ' . $inputDirectories[$i] . ' does not exist.';  };
            };
        }
    else
        {  push @errorMessages, 'You did not specify an input (source) directory.';  };

    if (defined $projectDirectory)
        {
        if (!NaturalDocs::File->PathIsAbsolute($projectDirectory))
            {  $projectDirectory = NaturalDocs::File->JoinPaths(Cwd::cwd(), $projectDirectory, 1);  };

        $projectDirectory = NaturalDocs::File->CanonizePath($projectDirectory);

        if (! -e $projectDirectory || ! -d $projectDirectory)
            {  push @errorMessages, 'The project directory ' . $projectDirectory . ' does not exist.';  };

        # Create the Data subdirectory if it doesn't exist.
        NaturalDocs::File->CreatePath( NaturalDocs::File->JoinPaths($projectDirectory, 'Data', 1) );
        }
    else
        {  push @errorMessages, 'You did not specify a project directory.';  };


    # Make sure the excluded input directories are canonized, and add the project and output directories to the list.

    for (my $i = 0; $i < scalar @excludedInputDirectories; $i++)
        {
        if (!NaturalDocs::File->PathIsAbsolute($excludedInputDirectories[$i]))
            {  $excludedInputDirectories[$i] = NaturalDocs::File->JoinPaths(Cwd::cwd(), $excludedInputDirectories[$i], 1);  };

        $excludedInputDirectories[$i] = NaturalDocs::File->CanonizePath($excludedInputDirectories[$i]);
        };

    push @excludedInputDirectories, $projectDirectory;

    foreach my $buildTarget (@buildTargets)
        {
        push @excludedInputDirectories, $buildTarget->Directory();
        };


    # Determine the tab length, and default to four if not specified.

    if (defined $tabLength)
        {
        if ($tabLength !~ /^[0-9]+$/)
            {  push @errorMessages, 'The tab length must be a number.';  };
        }
    else
        {  $tabLength = 4;  };


    # Decode and validate the highlight setting.

    if (defined $highlightString)
    	{
    	$highlightString = lc($highlightString);

    	if ($highlightString eq 'off')
    		{
    		$highlightCode = undef;
    		$highlightAnonymous = undef;
    		}
    	elsif ($highlightString eq 'code')
    		{
    		$highlightCode = 1;
    		$highlightAnonymous = undef;
    		}
    	elsif ($highlightString eq 'all')
    		{
    		$highlightCode = 1;
    		$highlightAnonymous = 1;
    		}
    	else
    		{  push @errorMessages, $highlightString . ' is not a valid value for --highlight.';  }
    	}
    else
    	{
    	$highlightCode = 1;
    	$highlightAnonymous = undef;
    	}


    # Exit with the error message if there was one.

    if (scalar @errorMessages)
        {
        print join("\n", @errorMessages) . "\nType NaturalDocs -h to see the syntax reference.\n";
        exit;
        };
    };

#
#   Function: PrintSyntax
#
#   Prints the syntax reference.
#
sub PrintSyntax
    {
    my ($self) = @_;

    # Make sure all line lengths are under 80 characters.

    print

    "Natural Docs, version " . $self->TextAppVersion() . "\n"
    . $self->AppURL() . "\n"
    . "This program is licensed under version 3 of the AGPL\n"
    . "Refer to License.txt for the complete details\n"
    . "--------------------------------------\n"
    . "\n"
    . "Syntax:\n"
    . "\n"
    . "    NaturalDocs -i [input (source) directory]\n"
    . "               (-i [input (source) directory] ...)\n"
    . "                -o [output format] [output directory]\n"
    . "               (-o [output format] [output directory] ...)\n"
    . "                -p [project directory]\n"
    . "                [options]\n"
    . "\n"
    . "Examples:\n"
    . "\n"
    . "    NaturalDocs -i C:\\My Project\\Source -o HTML C:\\My Project\\Docs\n"
    . "                -p C:\\My Project\\Natural Docs\n"
    . "    NaturalDocs -i /src/project -o HTML /doc/project\n"
    . "                -p /etc/naturaldocs/project -s Small -q\n"
    . "\n"
    . "Required Parameters:\n"
    . "\n"
    . " -i [dir]\n--input [dir]\n--source [dir]\n"
    . "     Specifies an input (source) directory.  Required.\n"
    . "     Can be specified multiple times.\n"
    . "\n"
    . " -o [fmt] [dir]\n--output [fmt] [dir]\n"
    . "    Specifies an output format and directory.  Required.\n"
    . "    Can be specified multiple times, but only once per directory.\n"
    . "    Possible output formats:\n";

    $self->PrintOutputFormats('    - ');

    print
    "\n"
    . " -p [dir]\n--project [dir]\n"
    . "    Specifies the project directory.  Required.\n"
    . "    There needs to be a unique project directory for every source directory.\n"
    . "\n"
    . "Optional Parameters:\n"
    . "\n"
    . " -s [style] ([style] [style] ...)\n--style [style] ([style] [style] ...)\n"
    . "    Specifies the CSS style when building HTML output.  If multiple styles are\n"
    . "    specified, they will all be included in the order given.\n"
    . "\n"
    . " -img [image directory]\n--image [image directory]\n"
    . "    Specifies an image directory.  Can be specified multiple times.\n"
    . "    Start with * to specify a relative directory, as in -img */images.\n"
    . "\n"
    . " -do\n--documented-only\n"
    . "    Specifies only documented code aspects should be included in the output.\n"
    . "\n"
    . " -t [len]\n--tab-length [len]\n"
    . "    Specifies the number of spaces tabs should be expanded to.  This only needs\n"
    . "    to be set if you use tabs in example code and text diagrams.  Defaults to 4.\n"
    . "\n"
    . " -xi [dir]\n--exclude-input [dir]\n--exclude-source [dir]\n"
    . "    Excludes an input (source) directory from the documentation.\n"
    . "    Automatically done for the project and output directories.  Can\n"
    . "    be specified multiple times.\n"
    . "\n"
    . " -nag\n--no-auto-group\n"
    . "    Turns off auto-grouping completely.\n"
    . "\n"
    . " -oft\n--only-file-titles\n"
    . "    Source files will only use the file name as the title.\n"
    . "\n"
    . " -hl [option]\n--highlight [option]\n"
    . "    Specifies when syntax highlighting should be applied.  Defaults to code.\n"
    . "    off  - No syntax highlighting is applied.\n"
    . "    code - Syntax highlighting is only applied to prototypes and (start code)\n"
    . "           segments.\n"
    . "    all  - Systax highlighting is applied to prototypes, (start code) segments,\n"
    . "           and lines starting with >, :, or |."
    . "\n"
    . " -r\n--rebuild\n"
    . "    Rebuilds all output and data files from scratch.\n"
    . "    Does not affect the menu file.\n"
    . "\n"
    . " -ro\n--rebuild-output\n"
    . "    Rebuilds all output files from scratch.\n"
    . "\n"
    . " -q\n--quiet\n"
    . "    Suppresses all non-error output.\n"
    . "\n"
    . " -?\n -h\n--help\n"
    . "    Displays this syntax reference.\n";
    };


#
#   Function: PrintOutputFormats
#
#   Prints all the possible output formats that can be specified with -o.  Each one will be placed on its own line.
#
#   Parameters:
#
#       prefix - Characters to prefix each one with, such as for indentation.
#
sub PrintOutputFormats #(prefix)
    {
    my ($self, $prefix) = @_;

    my $outputPackages = NaturalDocs::Builder::OutputPackages();

    foreach my $outputPackage (@$outputPackages)
        {
        print $prefix . $outputPackage->CommandLineOption() . "\n";
        };
    };


#
#   Function: LoadAndComparePreviousSettings
#
#   Loads <PreviousSettings.nd> and compares the values there with those in the command line.  If differences require it,
#   sets <rebuildData> and/or <rebuildOutput>.
#
sub LoadAndComparePreviousSettings
    {
    my ($self) = @_;

    my $fileIsOkay;

    if (!NaturalDocs::Settings->RebuildData())
        {
        my $version;

        if (NaturalDocs::BinaryFile->OpenForReading( NaturalDocs::Project->DataFile('PreviousSettings.nd'),
                                                                           NaturalDocs::Version->FromString('1.51') ))
            {  $fileIsOkay = 1;  };
        };

    if (!$fileIsOkay)
        {
        # We need to reparse everything because --documented-only may have changed.
        # We need to rebuild everything because --tab-length may have changed.
        NaturalDocs::Project->ReparseEverything();
        NaturalDocs::Project->RebuildEverything();
        }
    else
        {
        my $raw;

        # [UInt8: tab expansion]
        # [UInt8: documented only (0 or 1)]
        # [UInt8: no auto-group (0 or 1)]
        # [UInt8: only file titles (0 or 1)]
        # [UInt8: highlight code (0 or 1)]
        # [UInt8: highlight anonymous (0 or 1)]

        my $prevTabLength = NaturalDocs::BinaryFile->GetUInt8();
        my $prevDocumentedOnly = NaturalDocs::BinaryFile->GetUInt8();
        my $prevNoAutoGroup = NaturalDocs::BinaryFile->GetUInt8();
        my $prevOnlyFileTitles = NaturalDocs::BinaryFile->GetUInt8();
        my $prevHighlightCode = NaturalDocs::BinaryFile->GetUInt8();
        my $prevHighlightAnonymous = NaturalDocs::BinaryFile->GetUInt8();

        if ($prevDocumentedOnly == 0)
            {  $prevDocumentedOnly = undef;  };
        if ($prevNoAutoGroup == 0)
            {  $prevNoAutoGroup = undef;  };
        if ($prevOnlyFileTitles == 0)
            {  $prevOnlyFileTitles = undef;  };
        if ($prevHighlightCode == 0)
            {  $prevHighlightCode = undef;  };
        if ($prevHighlightAnonymous == 0)
            {  $prevHighlightAnonymous = undef;  };

        if ($prevTabLength != $self->TabLength() ||
        	$prevHighlightCode != $self->HighlightCode() ||
        	$prevHighlightAnonymous != $self->HighlightAnonymous())
            {
            NaturalDocs::Project->RebuildEverything();
            };

        if ($prevDocumentedOnly != $self->DocumentedOnly() ||
            $prevNoAutoGroup != $self->NoAutoGroup() ||
            $prevOnlyFileTitles != $self->OnlyFileTitles())
            {
            NaturalDocs::Project->ReparseEverything();
            };


        # [UInt8: number of input directories]

        my $inputDirectoryCount = NaturalDocs::BinaryFile->GetUInt8();

        while ($inputDirectoryCount)
            {
            # [AString16: input directory] [AString16: input directory name] ...

            my $inputDirectory = NaturalDocs::BinaryFile->GetAString16();
            my $inputDirectoryName = NaturalDocs::BinaryFile->GetAString16();

            # Not doing anything with this for now.

            $inputDirectoryCount--;
            };


        # [UInt8: number of output targets]

        my $outputTargetCount = NaturalDocs::BinaryFile->GetUInt8();

        # Keys are the directories, values are the command line options.
        my %previousOutputDirectories;

        while ($outputTargetCount)
            {
            # [AString16: output directory] [AString16: output format command line option] ...

            my $outputDirectory = NaturalDocs::BinaryFile->GetAString16();
            my $outputCommand = NaturalDocs::BinaryFile->GetAString16();

            $previousOutputDirectories{$outputDirectory} = $outputCommand;

            $outputTargetCount--;
            };

        # Check if any targets were added to the command line, or if their formats changed.  We don't care if targets were
        # removed.
        my $buildTargets = $self->BuildTargets();

        foreach my $buildTarget (@$buildTargets)
            {
            if (!exists $previousOutputDirectories{$buildTarget->Directory()} ||
                $buildTarget->Builder()->CommandLineOption() ne $previousOutputDirectories{$buildTarget->Directory()})
                {
                NaturalDocs::Project->RebuildEverything();
                last;
                };
            };

        NaturalDocs::BinaryFile->Close();
        };
    };


#
#   Function: SavePreviousSettings
#
#   Saves the settings into <PreviousSettings.nd>.
#
sub SavePreviousSettings
    {
    my ($self) = @_;

    NaturalDocs::BinaryFile->OpenForWriting(  NaturalDocs::Project->DataFile('PreviousSettings.nd') );

    # [UInt8: tab length]
    # [UInt8: documented only (0 or 1)]
    # [UInt8: no auto-group (0 or 1)]
    # [UInt8: only file titles (0 or 1)]
    # [UInt8: highlight code (0 or 1)]
    # [UInt8: highlight anonymous (0 or 1)]
    # [UInt8: number of input directories]

    my $inputDirectories = $self->InputDirectories();

    NaturalDocs::BinaryFile->WriteUInt8($self->TabLength());
    NaturalDocs::BinaryFile->WriteUInt8($self->DocumentedOnly() ? 1 : 0);
    NaturalDocs::BinaryFile->WriteUInt8($self->NoAutoGroup() ? 1 : 0);
    NaturalDocs::BinaryFile->WriteUInt8($self->OnlyFileTitles() ? 1 : 0);
    NaturalDocs::BinaryFile->WriteUInt8($self->HighlightCode() ? 1 : 0);
    NaturalDocs::BinaryFile->WriteUInt8($self->HighlightAnonymous() ? 1 : 0);
    NaturalDocs::BinaryFile->WriteUInt8(scalar @$inputDirectories);

    foreach my $inputDirectory (@$inputDirectories)
        {
        my $inputDirectoryName = $self->InputDirectoryNameOf($inputDirectory);

        # [AString16: input directory] [AString16: input directory name] ...
        NaturalDocs::BinaryFile->WriteAString16($inputDirectory);
        NaturalDocs::BinaryFile->WriteAString16($inputDirectoryName);
        };

    # [UInt8: number of output targets]

    my $buildTargets = $self->BuildTargets();
    NaturalDocs::BinaryFile->WriteUInt8(scalar @$buildTargets);

    foreach my $buildTarget (@$buildTargets)
        {
        # [AString16: output directory] [AString16: output format command line option] ...
        NaturalDocs::BinaryFile->WriteAString16( $buildTarget->Directory() );
        NaturalDocs::BinaryFile->WriteAString16( $buildTarget->Builder()->CommandLineOption() );
        };

    NaturalDocs::BinaryFile->Close();
    };


1;
