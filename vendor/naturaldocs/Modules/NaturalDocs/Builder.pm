###############################################################################
#
#   Package: NaturalDocs::Builder
#
###############################################################################
#
#   A package that takes parsed source file and builds the output for it.
#
#   Usage and Dependencies:
#
#       - <Add()> can be called immediately.
#       - <OutputPackages()> and <OutputPackageOf()> can be called once all sub-packages have been registered via <Add()>.
#         Since this is normally done in their INIT functions, they should be available to all normal functions immediately.
#
#       - Prior to calling <Run()>, <NaturalDocs::Settings>, <NaturalDocs::Project>, <NaturalDocs::Menu>, and
#         <NaturalDocs::Parser> must be initialized.  <NaturalDocs::Settings->GenerateDirectoryNames()> must be called.
#         <NaturalDocs::SymbolTable> and <NaturalDocs::ClassHierarchy> must be initialized and fully resolved.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use strict;
use integer;

use NaturalDocs::Builder::Base;
use NaturalDocs::Builder::HTML;
use NaturalDocs::Builder::FramedHTML;

package NaturalDocs::Builder;


###############################################################################
# Group: Variables

#
#   Array: outputPackages
#
#   An array of the output packages available for use.
#
my @outputPackages;


###############################################################################
# Group: Functions


#
#   Function: OutputPackages
#
#   Returns an arrayref of the output packages available for use.  The arrayref is not a copy of the data, so don't change it.
#
#   Add output packages to this list with the <Add()> function.
#
sub OutputPackages
    {  return \@outputPackages;  };


#
#   Function: OutputPackageOf
#
#   Returns the output package corresponding to the passed command line option, or undef if none.
#
sub OutputPackageOf #(commandLineOption)
    {
    my ($self, $commandLineOption) = @_;

    $commandLineOption = lc($commandLineOption);

    foreach my $package (@outputPackages)
        {
        if (lc($package->CommandLineOption()) eq $commandLineOption)
            {  return $package;  };
        };

    return undef;
    };



#
#   Function: Add
#
#   Adds an output package to those available for use.  All output packages must call this function in order to be recognized.
#
#   Parameters:
#
#       package - The package name.
#
sub Add #(package)
    {
    my ($self, $package) = @_;

    # Output packages shouldn't register themselves more than once, so we don't need to check for it.
    push @outputPackages, $package;
    };


#
#   Function: Run
#
#   Runs the build process.  This must be called *every time* Natural Docs is run, regardless of whether any source files changed
#   or not.  Some output packages have dependencies on files outside of the source tree that need to be checked.
#
#   Since there are multiple stages to the build process, this function will handle its own status messages.  There's no need to print
#   "Building files..." or something similar beforehand.
#
sub Run
    {
    my ($self) = @_;


    # Determine what we're doing.

    my $buildTargets = NaturalDocs::Settings->BuildTargets();

    my $filesToBuild = NaturalDocs::Project->FilesToBuild();
    my $numberOfFilesToBuild = (scalar keys %$filesToBuild) * (scalar @$buildTargets);

    my $filesToPurge = NaturalDocs::Project->FilesToPurge();
    my $numberOfFilesToPurge = (scalar keys %$filesToPurge) * (scalar @$buildTargets);

    my $imagesToUpdate = NaturalDocs::Project->ImageFilesToUpdate();
    my $numberOfImagesToUpdate = (scalar keys %$imagesToUpdate) * (scalar @$buildTargets);

    my $imagesToPurge = NaturalDocs::Project->ImageFilesToPurge();
    my $numberOfImagesToPurge = (scalar keys %$imagesToPurge) * (scalar @$buildTargets);

    my %indexesToBuild;
    my %indexesToPurge;

    my $currentIndexes = NaturalDocs::Menu->Indexes();
    my $previousIndexes = NaturalDocs::Menu->PreviousIndexes();

    foreach my $index (keys %$currentIndexes)
        {
        if (NaturalDocs::SymbolTable->IndexChanged($index) || !exists $previousIndexes->{$index})
            {
            $indexesToBuild{$index} = 1;
            };
        };

    # All indexes that still exist should have been deleted.
    foreach my $index (keys %$previousIndexes)
        {
        if (!exists $currentIndexes->{$index})
            {
            $indexesToPurge{$index} = 1;
            };
        };

    my $numberOfIndexesToBuild = (scalar keys %indexesToBuild) * (scalar @$buildTargets);
    my $numberOfIndexesToPurge = (scalar keys %indexesToPurge) * (scalar @$buildTargets);


    # Start the build process

    foreach my $buildTarget (@$buildTargets)
        {
        $buildTarget->Builder()->BeginBuild( $numberOfFilesToBuild || $numberOfFilesToPurge ||
                                                               $numberOfImagesToUpdate || $numberOfImagesToPurge ||
                                                               $numberOfIndexesToBuild || $numberOfIndexesToPurge ||
                                                               NaturalDocs::Menu->HasChanged() );
        };

    if ($numberOfFilesToPurge)
        {
        NaturalDocs::StatusMessage->Start('Purging ' . $numberOfFilesToPurge
                                                          . ' file' . ($numberOfFilesToPurge > 1 ? 's' : '') . '...',
                                                             scalar @$buildTargets);

        foreach my $buildTarget (@$buildTargets)
            {
            $buildTarget->Builder()->PurgeFiles($filesToPurge);
            NaturalDocs::StatusMessage->CompletedItem();
            };
        };

    if ($numberOfIndexesToPurge)
        {
        NaturalDocs::StatusMessage->Start('Purging ' . $numberOfIndexesToPurge
                                                           . ' index' . ($numberOfIndexesToPurge > 1 ? 'es' : '') . '...',
                                                             scalar @$buildTargets);

        foreach my $buildTarget (@$buildTargets)
            {
            $buildTarget->Builder()->PurgeIndexes(\%indexesToPurge);
            NaturalDocs::StatusMessage->CompletedItem();
            };
        };

    if ($numberOfImagesToPurge)
        {
        NaturalDocs::StatusMessage->Start('Purging ' . $numberOfImagesToPurge
                                                          . ' image' . ($numberOfImagesToPurge > 1 ? 's' : '') . '...',
                                                             scalar @$buildTargets);

        foreach my $buildTarget (@$buildTargets)
            {
            $buildTarget->Builder()->PurgeImages($imagesToPurge);
            NaturalDocs::StatusMessage->CompletedItem();
            };
        };

    if ($numberOfFilesToBuild)
        {
        NaturalDocs::StatusMessage->Start('Building ' . $numberOfFilesToBuild
                                                           . ' file' . ($numberOfFilesToBuild > 1 ? 's' : '') . '...',
                                                             $numberOfFilesToBuild);

        foreach my $file (keys %$filesToBuild)
            {
            my $parsedFile = NaturalDocs::Parser->ParseForBuild($file);

            NaturalDocs::Error->OnStartBuilding($file);

            foreach my $buildTarget (@$buildTargets)
                {
                $buildTarget->Builder()->BuildFile($file, $parsedFile);
                NaturalDocs::StatusMessage->CompletedItem();
                };

            NaturalDocs::Error->OnEndBuilding($file);
            };
        };

    if ($numberOfIndexesToBuild)
        {
        NaturalDocs::StatusMessage->Start('Building ' . $numberOfIndexesToBuild
                                                          . ' index' . ($numberOfIndexesToBuild > 1 ? 'es' : '') . '...',
                                                             $numberOfIndexesToBuild);

        foreach my $index (keys %indexesToBuild)
            {
            foreach my $buildTarget (@$buildTargets)
                {
                $buildTarget->Builder()->BuildIndex($index);
                NaturalDocs::StatusMessage->CompletedItem();
                };
            };
        };

    if ($numberOfImagesToUpdate)
        {
        NaturalDocs::StatusMessage->Start('Updating ' . $numberOfImagesToUpdate
                                                          . ' image' . ($numberOfImagesToUpdate > 1 ? 's' : '') . '...',
                                                             $numberOfImagesToUpdate);

        foreach my $image (keys %$imagesToUpdate)
            {
            foreach my $buildTarget (@$buildTargets)
                {
                $buildTarget->Builder()->UpdateImage($image);
                NaturalDocs::StatusMessage->CompletedItem();
                };
            };
        };

    if (NaturalDocs::Menu->HasChanged())
        {
        if (!NaturalDocs::Settings->IsQuiet())
            {  print "Updating menu...\n";  };

        foreach my $buildTarget (@$buildTargets)
            {  $buildTarget->Builder()->UpdateMenu();  };
        };

    foreach my $buildTarget (@$buildTargets)
        {
        $buildTarget->Builder()->EndBuild($numberOfFilesToBuild || $numberOfFilesToPurge ||
                                                           $numberOfIndexesToBuild || $numberOfIndexesToPurge ||
                                                           $numberOfImagesToUpdate || $numberOfImagesToPurge ||
                                                           NaturalDocs::Menu->HasChanged());
        };
    };


1;
