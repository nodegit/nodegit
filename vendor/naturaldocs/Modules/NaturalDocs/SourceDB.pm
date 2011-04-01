###############################################################################
#
#   Package: NaturalDocs::SourceDB
#
###############################################################################
#
#   SourceDB is an experimental package meant to unify the tracking of various elements in the source code.
#
#   Requirements:
#
#       - All extension packages must call <RegisterExtension()> before they can be used.
#
#
#   Architecture: The Idea
#
#       For quite a while Natural Docs only needed <SymbolTable>.  However, 1.3 introduced the <ClassHierarchy> package
#       which duplicated some of its functionality to track classes and parent references.  1.4 now needs <ImageReferenceTable>,
#       so this package was an attempt to isolate the common functionality so the wheel doesn't have to keep being rewritten as
#       the scope of Natural Docs expands.
#
#       SourceDB is designed around <Extensions> and items.  The purposefully vague "items" are anything in the source code
#       that we need to track the definitions of.  Extensions are the packages to track them, only they're derived from
#       <NaturalDocs::SourceDB::Extension> and registered with this package instead of being free standing and duplicating
#       functionality such as watched files.
#
#       The architecture on this package isn't comprehensive yet.  As more extensions are added or previously made free standing
#       packages are migrated to it it will expand to encompass them.  However, it's still experimental so this concept may
#       eventually be abandoned for something better instead.
#
#
#   Architecture: Assumptions
#
#       SourceDB is built around certain assumptions.
#
#       One item per file:
#
#           SourceDB assumes that only the first item per file with a particular item string is relevant.  For example, if two functions
#           have the exact same name, there's no way to link to the second one either in HTML or internally so it doesn't matter for
#           our purposes.  Likewise, if two references are exactly the same they go to the same target, so it doesn't matter whether
#           there's one or two or a thousand.  All that matters is that at least one reference exists in this file because you only need
#           to determine whether the entire file gets rebuilt.  If two items are different in some meaningful way, they should generate
#           different item strings.
#
#       Watched file parsing:
#
#           SourceDB assumes the parse method is that the information that was stored from Natural Docs' previous run is loaded, a
#           file is watched, that file is reparsed, and then <AnalyzeWatchedFileChanges()> is called.  When the file is reparsed all
#           items within it are added the same as if the file was never parsed before.
#
#           If there's a new item this time around, that's fine no matter what.  However, a changed item wouldn't normally be
#           recorded because the previous run's definition is seen as the first one and subsequent ones are ignored.  Also, deleted
#           items would normally not be recorded either because we're only adding.
#
#           The watched file method fixes this because everything is also added to a second, clean database specifically for the
#           watched file.  Because it starts clean, it always gets the first definition from the current parse which can then be
#           compared to the original by <AnalyzeWatchedFileChanges()>.  Because it starts clean you can also compare it to the
#           main database to see if anything was deleted, because it would appear in the main database but not the watched one.
#
#           This means that functions like <ChangeDefinition()> and <DeleteDefinition()> should only be called by
#           <AnalyzeWatchedFileChanges()>.  Externally only <AddDefinition()> should be called.  <DeleteItem()> is okay to be
#           called externally because entire items aren't managed by the watched file database, only definitions.
#
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


use NaturalDocs::SourceDB::Extension;
use NaturalDocs::SourceDB::Item;
use NaturalDocs::SourceDB::ItemDefinition;
use NaturalDocs::SourceDB::File;
use NaturalDocs::SourceDB::WatchedFileDefinitions;


package NaturalDocs::SourceDB;


###############################################################################
# Group: Types


#
#   Type: ExtensionID
#
#   A unique identifier for each <NaturalDocs::SourceDB> extension as given out by <RegisterExtension()>.
#



###############################################################################
# Group: Variables


#
#   array: extensions
#
#   An array of <NaturalDocs::SourceDB::Extension>-derived extensions, as added with <RegisterExtension()>.  The indexes
#   are the <ExtensionIDs> and the values are package references.
#
my @extensions;

#
#   array: extensionUsesDefinitionObjects
#
#   An array where the indexes are <ExtensionIDs> and the values are whether that extension uses its own definition class
#   derived from <NaturalDocs::SourceDB::ItemDefinition> or it just tracks their existence.
#
my @extensionUsesDefinitionObjects;



#
#   array: items
#
#   The array of source items.  The <ExtensionIDs> are the indexes, and the values are hashrefs mapping the item
#   string to <NaturalDocs::SourceDB::Item>-derived objects.  Hashrefs may be undef.
#
my @items;


#
#   hash: files
#
#   A hashref mapping source <FileNames> to <NaturalDocs::SourceDB::Files>.
#
my %files;


#
#   object: watchedFile
#
#   When a file is being watched for changes, will be a <NaturalDocs::SourceDB::File> for that file.  Is undef otherwise.
#
#   When the file is parsed, items are added to both this and the version in <files>.  Thus afterwards we can compare the two to
#   see if any were deleted since the last time Natural Docs was run, because they would be in the <files> version but not this
#   one.
#
my $watchedFile;


#
#   string: watchedFileName
#
#   When a file is being watched for changes, will be the <FileName> of the file being watched.  Is undef otherwise.
#
my $watchedFileName;


#
#   object: watchedFileDefinitions
#
#   When a file is being watched for changes, will be a <NaturalDocs::SourceDB::WatchedFileDefinitions> object.  Is undef
#   otherwise.
#
#   When the file is parsed, items are added to both this and the version in <items>.  Since only the first definition is kept, this
#   will always have the definition info from the file whereas the version in <items> will have the first definition as of the last time
#   Natural Docs was run.  Thus they can be compared to see if the definitions of items that existed the last time around have
#   changed.
#
my $watchedFileDefinitions;



###############################################################################
# Group: Extension Functions


#
#   Function: RegisterExtension
#
#   Registers a <NaturalDocs::SourceDB::Extension>-derived package and returns a unique <ExtensionID> for it.  All extensions
#   must call this before they can be used.
#
#   Registration Order:
#
#       The order in which extensions register is important.  Whenever possible, items are added in the order their extensions
#       registered.  However, items are changed and deleted in the reverse order.  Take advantage of this to minimize
#       churn between extensions that are dependent on each other.
#
#       For example, when symbols are added or deleted they may cause references to be retargeted and thus their files need to
#       be rebuilt.  However, adding or deleting references never causes the symbols' files to be rebuilt.  So it makes sense that
#       symbols should be created before references, and that references should be deleted before symbols.
#
#   Parameters:
#
#       extension - The package or object of the extension.  Must be derived from <NaturalDocs::SourceDB::Extension>.
#       usesDefinitionObjects - Whether the extension uses its own class derived from <NaturalDocs::SourceDB::ItemDefinition>
#                                         or simply tracks each definitions existence.
#
#   Returns:
#
#       An <ExtensionID> unique to the extension.  This should be saved because it's required in functions such as <AddItem()>.
#
sub RegisterExtension #(package extension, bool usesDefinitionObjects) => ExtensionID
    {
    my ($self, $extension, $usesDefinitionObjects) = @_;

    push @extensions, $extension;
    push @extensionUsesDefinitionObjects, $usesDefinitionObjects;

    return scalar @extensions - 1;
    };




###############################################################################
# Group: File Functions


#
#   Function: Load
#
#   Loads the data of the source database and all the extensions.  Will call <NaturalDocs::SourceDB::Extension->Load()> for
#   all of them, unless there's a situation where all the source files are going to be reparsed anyway in which case it's not needed.
#
sub Load
    {
    my $self = shift;

    # No point loading if RebuildData is set.
    if (!NaturalDocs::Settings->RebuildData())
        {
        # If any load fails, stop loading the rest and just reparse all the source files.
        my $success = 1;

        for (my $extension = 0; $extension < scalar @extensions && $success; $extension++)
            {
            $success = $extensions[$extension]->Load();
            };

        if (!$success)
            {  NaturalDocs::Project->ReparseEverything();  };
        };
    };


#
#   Function: Save
#
#   Saves the data of the source database and all its extensions.  Will call <NaturalDocs::SourceDB::Extension->Save()> for all
#   of them.
#
sub Save
    {
    my $self = shift;

    for (my $extension = scalar @extensions - 1; $extension >= 0; $extension--)
        {
        $extensions[$extension]->Save();
        };
    };


#
#   Function: PurgeDeletedSourceFiles
#
#   Removes all data associated with deleted source files.
#
sub PurgeDeletedSourceFiles
    {
    my $self = shift;

    my $filesToPurge = NaturalDocs::Project->FilesToPurge();

    # Extension is the outermost loop because we want the extensions added last to have their definitions removed first to cause
    # the least amount of churn between interdependent extensions.
    for (my $extension = scalar @extensions - 1; $extension >= 0; $extension--)
        {
        foreach my $file (keys %$filesToPurge)
            {
            if (exists $files{$file})
                {
                my @items = $files{$file}->ListItems($extension);

                foreach my $item (@items)
                    {
                    $self->DeleteDefinition($extension, $item, $file);
                    };
                }; # file exists
            }; # each file
        }; # each extension
    };





###############################################################################
# Group: Item Functions


#
#   Function: AddItem
#
#   Adds the passed item to the database.  This will not work if the item string already exists.  The item added should *not*
#   already have definitions attached.  Only use this to add blank items and then call <AddDefinition()> instead.
#
#   Parameters:
#
#       extension - An <ExtensionID>.
#       itemString - The string serving as the item identifier.
#       item - An object derived from <NaturalDocs::SourceDB::Item>.
#
#   Returns:
#
#       Whether the item was added, that is, whether it was the first time this item was added.
#
sub AddItem #(ExtensionID extension, string itemString, NaturalDocs::SourceDB::Item item) => bool
    {
    my ($self, $extension, $itemString, $item) = @_;

    if (!defined $items[$extension])
        {  $items[$extension] = { };  };

    if (!exists $items[$extension]->{$itemString})
        {
        if ($item->HasDefinitions())
            {  die "Tried to add an item to SourceDB that already had definitions.";  };

        $items[$extension]->{$itemString} = $item;
        return 1;
        };

    return 0;
    };


#
#   Function: GetItem
#
#   Returns the <NaturalDocs::SourceDB::Item>-derived object for the passed <ExtensionID> and item string, or undef if there
#   is none.
#
sub GetItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extensionID, $itemString) = @_;

    if (defined $items[$extensionID])
        {  return $items[$extensionID]->{$itemString};  }
    else
        {  return undef;  };
    };


#
#   Function: DeleteItem
#
#   Deletes the record of the passed <ExtensionID> and item string.  Do *not* delete items that still have definitions.  Use
#   <DeleteDefinition()> first.
#
#   Parameters:
#
#       extension - The <ExtensionID>.
#       itemString - The item's identifying string.
#
#   Returns:
#
#       Whether it was successful, meaning whether an entry existed for it.
#
sub DeleteItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $items[$extension] && exists $items[$extension]->{$itemString})
        {
        if ($items[$extension]->{$itemString}->HasDefinitions())
            {  die "Tried to delete an item from SourceDB that still has definitions.";  };

        delete $items[$extension]->{$itemString};
        return 1;
        }
    else
        {  return 0;  };
    };


#
#   Function: HasItem
#
#   Returns whether there is an item defined for the passed <ExtensionID> and item string.
#
sub HasItem #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    if (defined $items[$extension])
        {  return (exists $items[$extension]->{$itemString});  }
    else
        {  return 0;  };
    };


#
#   Function: GetAllItemsHashRef
#
#   Returns a hashref of all the items defined for an extension.  *Do not change the contents.*  The keys are the item strings and
#   the values are <NaturalDocs::SourceDB::Items> or derived classes.
#
sub GetAllItemsHashRef #(ExtensionID extension) => hashref
    {
    my ($self, $extension) = @_;
    return $items[$extension];
    };



###############################################################################
# Group: Definition Functions


#
#   Function: AddDefinition
#
#   Adds a definition to an item.  Assumes the item was already created with <AddItem()>.  If there's already a definition for this
#   file in the item, the new definition will be ignored.
#
#   Parameters:
#
#       extension - The <ExtensionID>.
#       itemString - The item string.
#       file - The <FileName> the definition is in.
#       definition - If you're using a custom <NaturalDocs::SourceDB::ItemDefinition> class, you must include an object for it here.
#                       Otherwise this parameter is ignored.
#
#   Returns:
#
#       Whether the definition was added, which is to say, whether this was the first definition for the passed <FileName>.
#
sub AddDefinition #(ExtensionID extension, string itemString, FileName file, optional NaturalDocs::SourceDB::ItemDefinition definition) => bool
    {
    my ($self, $extension, $itemString, $file, $definition) = @_;


    # Items

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  die "Tried to add a definition to an undefined item in SourceDB.";  };

    if (!$extensionUsesDefinitionObjects[$extension])
        {  $definition = 1;  };

    my $result = $item->AddDefinition($file, $definition);


    # Files

    if (!exists $files{$file})
        {  $files{$file} = NaturalDocs::SourceDB::File->New();  };

    $files{$file}->AddItem($extension, $itemString);


    # Watched File

    if ($self->WatchingFileForChanges())
        {
        $watchedFile->AddItem($extension, $itemString);

        if ($extensionUsesDefinitionObjects[$extension])
            {  $watchedFileDefinitions->AddDefinition($extension, $itemString, $definition);  };
        };


    return $result;
    };


#
#   Function: ChangeDefinition
#
#   Changes the definition of an item.  This function is only used for extensions that use custom
#   <NaturalDocs::SourceDB::ItemDefinition>-derived classes.
#
#   Parameters:
#
#       extension - The <ExtensionID>.
#       itemString - The item string.
#       file - The <FileName> the definition is in.
#       definition - The definition, which must be an object derived from <NaturalDocs::SourceDB::ItemDefinition>.
#
sub ChangeDefinition #(ExtensionID extension, string itemString, FileName file, NaturalDocs::SourceDB::ItemDefinition definition)
    {
    my ($self, $extension, $itemString, $file, $definition) = @_;

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  die "Tried to change the definition of an undefined item in SourceDB.";  };

    if (!$extensionUsesDefinitionObjects[$extension])
        {  die "Tried to change the definition of an item in an extension that doesn't use definition objects in SourceDB.";  };

    if (!$item->HasDefinition($file))
        {  die "Tried to change a definition that doesn't exist in SourceDB.";  };

    $item->ChangeDefinition($file, $definition);
    $extensions[$extension]->OnChangedDefinition($itemString, $file);
    };


#
#   Function: GetDefinition
#
#   If the extension uses custom <NaturalDocs::SourceDB::ItemDefinition> classes, returns it for the passed definition or undef
#   if it doesn't exist.  Otherwise returns whether it exists.
#
sub GetDefinition #(ExtensionID extension, string itemString, FileName file) => NaturalDocs::SourceDB::ItemDefinition or bool
    {
    my ($self, $extension, $itemString, $file) = @_;

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  return undef;  };

    return $item->GetDefinition($file);
    };


#
#   Function: DeleteDefinition
#
#   Removes the definition for the passed item.  Returns whether it was successful, meaning whether a definition existed for that
#   file.
#
sub DeleteDefinition #(ExtensionID extension, string itemString, FileName file) => bool
    {
    my ($self, $extension, $itemString, $file) = @_;

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  return 0;  };

    my $result = $item->DeleteDefinition($file);

    if ($result)
        {
        $files{$file}->DeleteItem($extension, $itemString);
        $extensions[$extension]->OnDeletedDefinition($itemString, $file, !$item->HasDefinitions());
        };

    return $result;
    };


#
#   Function: HasDefinitions
#
#   Returns whether there are any definitions for this item.
#
sub HasDefinitions #(ExtensionID extension, string itemString) => bool
    {
    my ($self, $extension, $itemString) = @_;

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  return 0;  };

    return $item->HasDefinitions();
    };


#
#   Function: HasDefinition
#
#   Returns whether there is a definition for the passed <FileName>.
#
sub HasDefinition #(ExtensionID extension, string itemString, FileName file) => bool
    {
    my ($self, $extension, $itemString, $file) = @_;

    my $item = $self->GetItem($extension, $itemString);

    if (!defined $item)
        {  return 0;  };

    return $item->HasDefinition($file);
    };



###############################################################################
# Group: Watched File Functions


#
#   Function: WatchFileForChanges
#
#   Begins watching a file for changes.  Only one file at a time can be watched.
#
#   This should be called before a file is parsed so the file info goes both into the main database and the watched file info.
#   Afterwards you call <AnalyzeWatchedFileChanges()> so item deletions and definition changes can be detected.
#
#   Parameters:
#
#       filename - The <FileName> to watch.
#
sub WatchFileForChanges #(FileName filename)
    {
    my ($self, $filename) = @_;

    $watchedFileName = $filename;
    $watchedFile = NaturalDocs::SourceDB::File->New();
    $watchedFileDefinitions = NaturalDocs::SourceDB::WatchedFileDefinitions->New();
    };


#
#   Function: WatchingFileForChanges
#
#   Returns whether we're currently watching a file for changes or not.
#
sub WatchingFileForChanges # => bool
    {
    my $self = shift;
    return defined $watchedFileName;
    };


#
#   Function: AnalyzeWatchedFileChanges
#
#   Analyzes the watched file for changes.  Will delete and change definitions as necessary.
#
sub AnalyzeWatchedFileChanges
    {
    my $self = shift;

    if (!$self->WatchingFileForChanges())
        {  die "Tried to analyze watched file for changes in SourceDB when no file was being watched.";  };
    if (!$files{$watchedFileName})
        {  return;  };


    # Process extensions last registered to first.

    for (my $extension = scalar @extensions - 1; $extension >= 0; $extension--)
        {
        my @items = $files{$watchedFileName}->ListItems($extension);

        foreach my $item (@items)
            {
            if ($watchedFile->HasItem($extension, $item))
                {
                if ($extensionUsesDefinitionObjects[$extension])
                    {
                    my $originalDefinition = $items[$extension]->GetDefinition($watchedFileName);
                    my $watchedDefinition = $watchedFileDefinitions->GetDefinition($extension, $item);

                    if (!$originalDefinition->Compare($watchedDefinition))
                        {  $self->ChangeDefinition($extension, $item, $watchedFileName, $watchedDefinition);  };
                    }
                }
            else # !$watchedFile->HasItem($item)
                {
                $self->DeleteDefinition($extension, $item, $watchedFileName);
                };
            };
        };


    $watchedFile = undef;
    $watchedFileName = undef;
    $watchedFileDefinitions = undef;
    };


1;
