###############################################################################
#
#   Package: NaturalDocs::File
#
###############################################################################
#
#   A package to manage file access across platforms.  Incorporates functions from various standard File:: packages, but more
#   importantly, works around the glorious suckage present in File::Spec, at least in version 0.82 and earlier.  Read the "Why oh
#   why?" sections for why this package was necessary.
#
#   Usage and Dependencies:
#
#       - The package doesn't depend on any other Natural Docs packages and is ready to use immediately.
#
#       - All functions except <CanonizePath()> assume that all parameters are canonized.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use File::Spec ();
use File::Path ();
use File::Copy ();

use strict;
use integer;

package NaturalDocs::File;


#
#   Function: CheckCompatibility
#
#   Checks if the standard packages required by this one are up to snuff and dies if they aren't.  This is done because I can't
#   tell which versions of File::Spec have splitpath just by the version numbers.
#
sub CheckCompatibility
    {
    my ($self) = @_;

    eval {
        File::Spec->splitpath('');
    };

    if ($@)
        {
        NaturalDocs::Error->SoftDeath("Natural Docs requires a newer version of File::Spec than you have.  "
                                                    . "You must either upgrade it or upgrade Perl.");
        };
    };


###############################################################################
# Group: Path String Functions


#
#   Function: CanonizePath
#
#   Takes a path and returns a logically simplified version of it.
#
#   Why oh why?:
#
#       Because File::Spec->canonpath doesn't strip quotes on Windows.  So if you pass in "a b\c" or "a b"\c, they still end up as
#       different strings even though they're logically the same.
#
#       It also doesn't remove things like "..", so "a/b/../c" doesn't simplify to "a/c" like it should.
#
sub CanonizePath #(path)
    {
    my ($self, $path) = @_;

    if ($::OSNAME eq 'MSWin32')
        {
        # We don't have to use a smarter algorithm for dropping quotes because they're invalid characters for actual file and
        # directory names.
        $path =~ s/\"//g;
        };

    $path = File::Spec->canonpath($path);

    # Condense a/b/../c into a/c.

    my $upDir = File::Spec->updir();
    if (index($path, $upDir) != -1)
        {
        my ($volume, $directoryString, $file) = $self->SplitPath($path);
        my @directories = $self->SplitDirectories($directoryString);

        my $i = 1;
        while ($i < scalar @directories)
            {
            if ($i > 0 && $directories[$i] eq $upDir && $directories[$i - 1] ne $upDir)
                {
                splice(@directories, $i - 1, 2);
                $i--;
                }
            else
                {  $i++;  };
            };

        $directoryString = $self->JoinDirectories(@directories);
        $path = $self->JoinPath($volume, $directoryString, $file);
        };

    return $path;
    };


#
#   Function: PathIsAbsolute
#
#   Returns whether the passed path is absolute.
#
sub PathIsAbsolute #(path)
    {
    my ($self, $path) = @_;
    return File::Spec->file_name_is_absolute($path);
    };


#
#   Function: JoinPath
#
#   Creates a path from its elements.
#
#   Parameters:
#
#       volume - The volume, such as the drive letter on Windows.  Undef if none.
#       dirString - The directory string.  Create with <JoinDirectories()> if necessary.
#       file - The file name, or undef if none.
#
#   Returns:
#
#       The joined path.
#
sub JoinPath #(volume, dirString, $file)
    {
    my ($self, $volume, $dirString, $file) = @_;
    return File::Spec->catpath($volume, $dirString, $file);
    };


#
#   Function: JoinPaths
#
#   Joins two paths.
#
#   Parameters:
#
#       basePath       - May be a relative path, an absolute path, or undef.
#       extraPath      - May be a relative path, a file, a relative path and file together, or undef.
#       noFileInExtra - Set this to true if extraPath is a relative path only, and doesn't have a file.
#
#   Returns:
#
#       The joined path.
#
#   Why oh why?:
#
#       Because nothing in File::Spec will simply slap two paths together.  They have to be split up for catpath/file, and rel2abs
#       requires the base to be absolute.
#
sub JoinPaths #(basePath, extraPath, noFileInExtra)
    {
    my ($self, $basePath, $extraPath, $noFileInExtra) = @_;

    # If both are undef, it will return undef, which is what we want.
    if (!defined $basePath)
        {  return $extraPath;  }
    elsif (!defined $extraPath)
        {  return $basePath;  };

    my ($baseVolume, $baseDirString, $baseFile) = File::Spec->splitpath($basePath, 1);
    my ($extraVolume, $extraDirString, $extraFile) = File::Spec->splitpath($extraPath, $noFileInExtra);

    my @baseDirectories = $self->SplitDirectories($baseDirString);
    my @extraDirectories = $self->SplitDirectories($extraDirString);

    my $fullDirString = $self->JoinDirectories(@baseDirectories, @extraDirectories);

    my $fullPath = File::Spec->catpath($baseVolume, $fullDirString, $extraFile);

    return $self->CanonizePath($fullPath);
    };


#
#   Function: SplitPath
#
#   Takes a path and returns its elements.
#
#   Parameters:
#
#       path - The path to split.
#       noFile - Set to true if the path doesn't have a file at the end.
#
#   Returns:
#
#       The array ( volume, directoryString, file ).  If any don't apply, they will be undef.  Use <SplitDirectories()> to split the
#       directory string if desired.
#
#   Why oh Why?:
#
#       Because File::Spec->splitpath may leave a trailing slash/backslash/whatever on the directory string, which makes
#       it a bit hard to match it with results from File::Spec->catdir.
#
sub SplitPath #(path, noFile)
    {
    my ($self, $path, $noFile) = @_;

    my @segments = File::Spec->splitpath($path, $noFile);

    if (!length $segments[0])
        {  $segments[0] = undef;  };
    if (!length $segments[2])
        {  $segments[2] = undef;  };

    $segments[1] = File::Spec->catdir( File::Spec->splitdir($segments[1]) );

    return @segments;
    };


#
#   Function: JoinDirectories
#
#   Creates a directory string from an array of directory names.
#
#   Parameters:
#
#       directory - A directory name.  There may be as many of these as desired.
#
sub JoinDirectories #(directory, directory, ...)
    {
    my ($self, @directories) = @_;
    return File::Spec->catdir(@directories);
    };


#
#   Function: SplitDirectories
#
#   Takes a string of directories and returns an array of its elements.
#
#   Why oh why?:
#
#       Because File::Spec->splitdir might leave an empty element at the end of the array, which screws up both joining in
#       <ConvertToURL> and navigation in <MakeRelativePath>.
#
sub SplitDirectories #(directoryString)
    {
    my ($self, $directoryString) = @_;

    my @directories = File::Spec->splitdir($directoryString);

    if (!length $directories[-1])
        {  pop @directories;  };

    return @directories;
    };


#
#   Function: MakeRelativePath
#
#   Takes two paths and returns a relative path between them.
#
#   Parameters:
#
#       basePath    - The starting path.  May be relative or absolute, so long as the target path is as well.
#       targetPath  - The target path.  May be relative or absolute, so long as the base path is as well.
#
#       If both paths are relative, they are assumed to be relative to the same base.
#
#   Returns:
#
#       The target path relative to base.
#
#   Why oh why?:
#
#       First, there's nothing that gives a relative path between two relative paths.
#
#       Second, if target and base are absolute but on different volumes, File::Spec->abs2rel creates a totally non-functional
#       relative path.  It should return the target as is, since there is no relative path.
#
#       Third, File::Spec->abs2rel between absolute paths on the same volume, at least on Windows, leaves the drive letter
#       on.  So abs2rel('a:\b\c\d', 'a:\b') returns 'a:c\d' instead of the expected 'c\d'.  That makes no sense whatsoever.  It's
#       not like it was designed to handle only directory names, either; the documentation says 'path' and the code seems to
#       explicitly handle it.  There's just an 'unless' in there that tacks on the volume, defeating the purpose of a *relative* path
#       and making the function worthless.
#
sub MakeRelativePath #(basePath, targetPath)
    {
    my ($self, $basePath, $targetPath) = @_;

    my ($baseVolume, $baseDirString, $baseFile) = $self->SplitPath($basePath, 1);
    my ($targetVolume, $targetDirString, $targetFile) = $self->SplitPath($targetPath);

    # If the volumes are different, there is no possible relative path.
    if ($targetVolume ne $baseVolume)
        {  return $targetPath;  };

    my @baseDirectories = $self->SplitDirectories($baseDirString);
    my @targetDirectories = $self->SplitDirectories($targetDirString);

    # Skip the parts of the path that are the same.
    while (scalar @baseDirectories && @targetDirectories && $baseDirectories[0] eq $targetDirectories[0])
        {
        shift @baseDirectories;
        shift @targetDirectories;
        };

    # Back out of the base path until it reaches where they were similar.
    for (my $i = 0; $i < scalar @baseDirectories; $i++)
        {
        unshift @targetDirectories, File::Spec->updir();
        };

    $targetDirString = $self->JoinDirectories(@targetDirectories);

    return File::Spec->catpath(undef, $targetDirString, $targetFile);
    };


#
#   Function: IsSubPathOf
#
#   Returns whether the path is a descendant of another path.
#
#   Parameters:
#
#       base - The base path to test against.
#       path - The possible subpath to test.
#
#   Returns:
#
#       Whether path is a descendant of base.
#
sub IsSubPathOf #(base, path)
    {
    my ($self, $base, $path) = @_;

    # This is a quick test that should find a false quickly.
    if ($base eq substr($path, 0, length($base)))
        {
        # This doesn't guarantee true, because it could be "C:\A B" and "C:\A B C\File".  So we test for it by seeing if the last
        # directory in base is the same as the equivalent directory in path.

        my ($baseVolume, $baseDirString, $baseFile) = NaturalDocs::File->SplitPath($base, 1);
        my @baseDirectories = NaturalDocs::File->SplitDirectories($baseDirString);

        my ($pathVolume, $pathDirString, $pathFile) = NaturalDocs::File->SplitPath($path);
        my @pathDirectories = NaturalDocs::File->SplitDirectories($pathDirString);

        return ( $baseDirectories[-1] eq $pathDirectories[ scalar @baseDirectories - 1 ] );
        }
    else
        {  return undef;  };
    };


#
#   Function: ConvertToURL
#
#   Takes a relative path and converts it from the native format to a relative URL.  Note that it _doesn't_ convert special characters
#   to amp chars.
#
sub ConvertToURL #(path)
    {
    my ($self, $path) = @_;

    my ($pathVolume, $pathDirString, $pathFile) = $self->SplitPath($path);
    my @pathDirectories = $self->SplitDirectories($pathDirString);

    my $i = 0;
    while ($i < scalar @pathDirectories && $pathDirectories[$i] eq File::Spec->updir())
        {
        $pathDirectories[$i] = '..';
        $i++;
        };

    return join('/', @pathDirectories, $pathFile);
    };


#
#   Function: NoUpwards
#
#   Takes an array of directory entries and returns one without all the entries that refer to the parent directory, such as '.' and '..'.
#
sub NoUpwards #(array)
    {
    my ($self, @array) = @_;
    return File::Spec->no_upwards(@array);
    };


#
#   Function: NoFileName
#
#   Takes a path and returns a version without the file name.  Useful for sending paths to <CreatePath()>.
#
sub NoFileName #(path)
    {
    my ($self, $path) = @_;

    my ($pathVolume, $pathDirString, $pathFile) = File::Spec->splitpath($path);

    return File::Spec->catpath($pathVolume, $pathDirString, undef);
    };


#
#   Function: NoExtension
#
#   Returns the path without an extension.
#
sub NoExtension #(path)
    {
    my ($self, $path) = @_;

    my $extension = $self->ExtensionOf($path);

    if ($extension)
        {  $path = substr($path, 0, length($path) - length($extension) - 1);  };

    return $path;
    };


#
#   Function: ExtensionOf
#
#   Returns the extension of the passed path, or undef if none.
#
sub ExtensionOf #(path)
    {
    my ($self, $path) = @_;

    my ($pathVolume, $pathDirString, $pathFile) = File::Spec->splitpath($path);

    # We need the leading dot in the regex so files that start with a dot but don't have an extension count as extensionless files.
    if ($pathFile =~ /.\.([^\.]+)$/)
        {  return $1;  }
    else
        {  return undef;  };
    };


#
#   Function: IsCaseSensitive
#
#   Returns whether the current platform has case-sensitive paths.
#
sub IsCaseSensitive
    {
    return !(File::Spec->case_tolerant());
    };



###############################################################################
# Group: Disk Functions


#
#   Function: CreatePath
#
#   Creates a directory tree corresponding to the passed path, regardless of how many directories do or do not already exist.
#   Do _not_ include a file name in the path.  Use <NoFileName()> first if you need to.
#
sub CreatePath #(path)
    {
    my ($self, $path) = @_;
    File::Path::mkpath($path);
    };


#
#   Function: RemoveEmptyTree
#
#   Removes an empty directory tree.  The passed directory will be removed if it's empty, and it will keep removing its parents
#   until it reaches one that's not empty or a set limit.
#
#   Parameters:
#
#       path - The path to start from.  It will try to remove this directory and work it's way down.
#       limit - The path to stop at if it doesn't find any non-empty directories first.  This path will *not* be removed.
#
sub RemoveEmptyTree #(path, limit)
    {
    my ($self, $path, $limit) = @_;

    my ($volume, $directoryString) = $self->SplitPath($path, 1);
    my @directories = $self->SplitDirectories($directoryString);

    my $directory = $path;

    while (-d $directory && $directory ne $limit)
        {
        opendir FH_ND_FILE, $directory;
        my @entries = readdir FH_ND_FILE;
        closedir FH_ND_FILE;

        @entries = $self->NoUpwards(@entries);

        if (scalar @entries || !rmdir($directory))
            {  last;  };

        pop @directories;
        $directoryString = $self->JoinDirectories(@directories);
        $directory = $self->JoinPath($volume, $directoryString);
        };
    };


#
#   Function: Copy
#
#   Copies a file from one path to another.  If the destination file exists, it is overwritten.
#
#   Parameters:
#
#       source       - The file to copy.
#       destination - The destination to copy to.
#
#   Returns:
#
#       Whether it succeeded
#
sub Copy #(source, destination) => bool
    {
    my ($self, $source, $destination) = @_;
    return File::Copy::copy($source, $destination);
    };


1;
