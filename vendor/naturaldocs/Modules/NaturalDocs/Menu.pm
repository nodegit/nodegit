###############################################################################
#
#   Package: NaturalDocs::Menu
#
###############################################################################
#
#   A package handling the menu's contents and state.
#
#   Usage and Dependencies:
#
#       - The <Event Handlers> can be called by <NaturalDocs::Project> immediately.
#
#       - Prior to initialization, <NaturalDocs::Project> must be initialized, and all files that have been changed must be run
#         through <NaturalDocs::Parser->ParseForInformation()>.
#
#       - To initialize, call <LoadAndUpdate()>.  Afterwards, all other functions are available.  Also, <LoadAndUpdate()> will
#         call <NaturalDocs::Settings->GenerateDirectoryNames()>.
#
#       - To save the changes back to disk, call <Save()>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use Tie::RefHash;

use NaturalDocs::Menu::Entry;

use strict;
use integer;

package NaturalDocs::Menu;


#
#   Constants: Constants
#
#   MAXFILESINGROUP - The maximum number of file entries that can be present in a group before it becomes a candidate for
#                                  sub-grouping.
#   MINFILESINNEWGROUP - The minimum number of file entries that must be present in a group before it will be automatically
#                                        created.  This is *not* the number of files that must be in a group before it's deleted.
#
use constant MAXFILESINGROUP => 6;
use constant MINFILESINNEWGROUP => 3;


###############################################################################
# Group: Variables


#
#   bool: hasChanged
#
#   Whether the menu changed or not, regardless of why.
#
my $hasChanged;


#
#   Object: menu
#
#   The parsed menu file.  Is stored as a <MENU_GROUP> <NaturalDocs::Menu::Entry> object, with the top-level entries being
#   stored as the group's content.  This is done because it makes a number of functions simpler to implement, plus it allows group
#   flags to be set on the top-level.  However, it is exposed externally via <Content()> as an arrayref.
#
#   This structure will only contain objects for <MENU_FILE>, <MENU_GROUP>, <MENU_TEXT>, <MENU_LINK>, and
#   <MENU_INDEX> entries.  Other types, such as <MENU_TITLE>, are stored in variables such as <title>.
#
my $menu;

#
#   hash: defaultTitlesChanged
#
#   An existence hash of default titles that have changed, since <OnDefaultTitleChange()> will be called before
#   <LoadAndUpdate()>.  Collects them to be applied later.  The keys are the <FileNames>.
#
my %defaultTitlesChanged;

#
#   String: title
#
#   The title of the menu.
#
my $title;

#
#   String: subTitle
#
#   The sub-title of the menu.
#
my $subTitle;

#
#   String: footer
#
#   The footer for the documentation.
#
my $footer;

#
#   String: timestampText
#
#   The timestamp for the documentation, stored as the final output text.
#
my $timestampText;

#
#   String: timestampCode
#
#   The timestamp for the documentation, storted as the symbolic code.
#
my $timestampCode;

#
#   hash: indexes
#
#   An existence hash of all the defined index <TopicTypes> appearing in the menu.
#
my %indexes;

#
#   hash: previousIndexes
#
#   An existence hash of all the index <TopicTypes> that appeared in the menu last time.
#
my %previousIndexes;

#
#   hash: bannedIndexes
#
#   An existence hash of all the index <TopicTypes> that the user has manually deleted, and thus should not be added back to
#   the menu automatically.
#
my %bannedIndexes;


###############################################################################
# Group: Files

#
#   File: Menu.txt
#
#   The file used to generate the menu.
#
#   Format:
#
#       The file is plain text.  Blank lines can appear anywhere and are ignored.  Tags and their content must be completely
#       contained on one line with the exception of Group's braces.  All values in brackets below are encoded with entity characters.
#
#       > # [comment]
#
#       The file supports single-line comments via #.  They can appear alone on a line or after content.
#
#       > Format: [version]
#       > Title: [title]
#       > SubTitle: [subtitle]
#       > Footer: [footer]
#       > Timestamp: [timestamp code]
#
#       The file format version, menu title, subtitle, footer, and timestamp are specified as above.  Each can only be specified once,
#       with subsequent ones being ignored.  Subtitle is ignored if Title is not present.  Format must be the first entry in the file.  If
#       it's not present, it's assumed the menu is from version 0.95 or earlier, since it was added with 1.0.
#
#       The timestamp code is as follows.
#
#           m - Single digit month, where applicable.  January is "1".
#           mm - Always double digit month.  January is "01".
#           mon - Short month word.  January is "Jan".
#           month - Long month word.  January is "January".
#           d - Single digit day, where applicable.  1 is "1".
#           dd - Always double digit day.  1 is "01".
#           day - Day with text extension.  1 is "1st".
#           yy - Double digit year.  2006 is "06".
#           yyyy - Four digit year.  2006 is "2006".
#           year - Four digit year.  2006 is "2006".
#
#       Anything else is left literal in the output.
#
#       > File: [title] ([file name])
#       > File: [title] (auto-title, [file name])
#       > File: [title] (no auto-title, [file name])
#
#       Files are specified as above.  If there is only one input directory, file names are relative.  Otherwise they are absolute.
#       If "no auto-title" is specified, the title on the line is used.  If not, the title is ignored and the
#       default file title is used instead.  Auto-title defaults to on, so specifying "auto-title" is for compatibility only.
#
#       > Group: [title]
#       > Group: [title] { ... }
#
#       Groups are specified as above.  If no braces are specified, the group's content is everything that follows until the end of the
#       file, the next group (braced or unbraced), or the closing brace of a parent group.  Group braces are the only things in this
#       file that can span multiple lines.
#
#       There is no limitations on where the braces can appear.  The opening brace can appear after the group tag, on its own line,
#       or preceding another tag on a line.  Similarly, the closing brace can appear after another tag or on its own line.  Being
#       bitchy here would just get in the way of quick and dirty editing; the package will clean it up automatically when it writes it
#       back to disk.
#
#       > Text: [text]
#
#       Arbitrary text is specified as above.  As with other tags, everything must be contained on the same line.
#
#       > Link: [URL]
#       > Link: [title] ([URL])
#
#       External links can be specified as above.  If the titled form is not used, the URL is used as the title.
#
#       > Index: [name]
#       > [topic type name] Index: [name]
#
#       Indexes are specified as above.  The topic type names can be either singular or plural.  General is assumed if not specified.
#
#       > Don't Index: [topic type name]
#       > Don't Index: [topic type name], [topic type name], ...
#
#       The option above prevents indexes that exist but are not on the menu from being automatically added.
#
#       > Data: [number]([obscured data])
#
#       Used to store non-user editable data.
#
#       > Data: 1([obscured: [directory name]///[input directory]])
#
#       When there is more than one directory, these lines store the input directories used in the last run and their names.  This
#       allows menu files to be shared across machines since the names will be consistent and the directories can be used to convert
#       filenames to the local machine's paths.  We don't want this user-editable because they may think changing it changes the
#       input directories, when it doesn't.  Also, changing it without changing all the paths screws up resolving.
#
#       > Data: 2([obscured: [directory name])
#
#       When there is only one directory and its name is not "default", this stores the name.
#
#
#   Entities:
#
#       &amp; - Ampersand.
#       &lparen; - Left parenthesis.
#       &rparen; - Right parenthesis.
#       &lbrace; - Left brace.
#       &rbrace; - Right brace.
#
#
#   Revisions:
#
#       1.4:
#
#           - Added Timestamp property.
#           - Values are now encoded with entity characters.
#
#       1.3:
#
#           - File names are now relative again if there is only one input directory.
#           - Data: 2(...) added.
#           - Can't use synonyms like "copyright" for "footer" or "sub-title" for "subtitle".
#           - "Don't Index" line now requires commas to separate them, whereas it tolerated just spaces before.
#
#       1.16:
#
#           - File names are now absolute instead of relative.  Prior to 1.16 only one input directory was allowed, so they could be
#             relative.
#           - Data keywords introduced to store input directories and their names.
#
#       1.14:
#
#           - Renamed this file from NaturalDocs_Menu.txt to Menu.txt.
#
#       1.1:
#
#           - Added the "don't index" line.
#
#           This is also the point where indexes were automatically added and removed, so all index entries from prior revisions
#           were manually added and are not guaranteed to contain anything.
#
#       1.0:
#
#           - Added the format line.
#           - Added the "no auto-title" attribute.
#           - Changed the file entry default to auto-title.
#
#           This is also the point where auto-organization and better auto-titles were introduced.  All groups in prior revisions were
#           manually added, with the exception of a top-level Other group where new files were automatically added if there were
#           groups defined.
#
#       Break in support:
#
#           Releases prior to 1.0 are no longer supported.  Why?
#
#           - They don't have a Format: line, which is required by <NaturalDocs::ConfigFile>, although I could work around this
#             if I needed to.
#           - No significant number of downloads for pre-1.0 releases.
#           - Code simplification.  I don't have to bridge the conversion from manual-only menu organization to automatic.
#
#       0.9:
#
#           - Added index entries.
#

#
#   File: PreviousMenuState.nd
#
#   The file used to store the previous state of the menu so as to detect changes.
#
#
#   Format:
#
#   > [BINARY_FORMAT]
#   > [VersionInt: app version]
#
#   First is the standard <BINARY_FORMAT> <VersionInt> header.
#
#   > [UInt8: 0 (end group)]
#   > [UInt8: MENU_FILE] [UInt8: noAutoTitle] [AString16: title] [AString16: target]
#   > [UInt8: MENU_GROUP] [AString16: title]
#   > [UInt8: MENU_INDEX] [AString16: title] [AString16: topic type]
#   > [UInt8: MENU_LINK] [AString16: title] [AString16: url]
#   > [UInt8: MENU_TEXT] [AString16: text]
#
#   The first UInt8 of each following line is either zero or one of the <Menu Entry Types>.  What follows is contextual.
#
#   There are no entries for title, subtitle, or footer.  Only the entries present in <menu>.
#
#   See Also:
#
#       <File Format Conventions>
#
#   Dependencies:
#
#       - Because the type is represented by a UInt8, the <Menu Entry Types> must all be <= 255.
#
#   Revisions:
#
#       1.3:
#
#           - The topic type following the <MENU_INDEX> entries were changed from UInt8s to AString16s, since <TopicTypes>
#             were switched from integer constants to strings.  You can still convert the old to the new via
#             <NaturalDocs::Topics->TypeFromLegacy()>.
#
#       1.16:
#
#           - The file targets are now absolute.  Prior to 1.16, they were relative to the input directory since only one was allowed.
#
#       1.14:
#
#           - The file was renamed from NaturalDocs.m to PreviousMenuState.nd and moved into the Data subdirectory.
#
#       1.0:
#
#           - The file's format was completely redone.  Prior to 1.0, the file was a text file consisting of the app version and a line
#             which was a tab-separated list of the indexes present in the menu.  * meant the general index.
#
#       Break in support:
#
#           Pre-1.0 files are no longer supported.  There was no significant number of downloads for pre-1.0 releases, and this
#           eliminates a separate code path for them.
#
#       0.95:
#
#           - Change the file version to match the app version.  Prior to 0.95, the version line was 1.  Test for "1" instead of "1.0" to
#             distinguish.
#
#       0.9:
#
#           - The file was added to the project.  Prior to 0.9, it didn't exist.
#


###############################################################################
# Group: File Functions

#
#   Function: LoadAndUpdate
#
#   Loads the menu file from disk and updates it.  Will add, remove, rearrange, and remove auto-titling from entries as
#   necessary.  Will also call <NaturalDocs::Settings->GenerateDirectoryNames()>.
#
sub LoadAndUpdate
    {
    my ($self) = @_;

    my ($inputDirectoryNames, $relativeFiles, $onlyDirectoryName) = $self->LoadMenuFile();

    my $errorCount = NaturalDocs::ConfigFile->ErrorCount();
    if ($errorCount)
        {
        NaturalDocs::ConfigFile->PrintErrorsAndAnnotateFile();
        NaturalDocs::Error->SoftDeath('There ' . ($errorCount == 1 ? 'is an error' : 'are ' . $errorCount . ' errors')
                                                    . ' in ' . NaturalDocs::Project->UserConfigFile('Menu.txt'));
        };

    # If the menu has a timestamp and today is a different day than the last time Natural Docs was run, we have to count it as the
    # menu changing.
    if (defined $timestampCode)
        {
        my (undef, undef, undef, $currentDay, $currentMonth, $currentYear) = localtime();
        my (undef, undef, undef, $lastDay, $lastMonth, $lastYear) =
            localtime( (stat( NaturalDocs::Project->DataFile('PreviousMenuState.nd') ))[9] );
            # This should be okay if the previous menu state file doesn't exist.

        if ($currentDay != $lastDay || $currentMonth != $lastMonth || $currentYear != $lastYear)
            {  $hasChanged = 1;  };
        };


    if ($relativeFiles)
        {
        my $inputDirectory = $self->ResolveRelativeInputDirectories($onlyDirectoryName);

        if ($onlyDirectoryName)
            {  $inputDirectoryNames = { $inputDirectory => $onlyDirectoryName };  };
        }
    else
        {  $self->ResolveInputDirectories($inputDirectoryNames);  };

    NaturalDocs::Settings->GenerateDirectoryNames($inputDirectoryNames);

    my $filesInMenu = $self->FilesInMenu();

    my ($previousMenu, $previousIndexes, $previousFiles) = $self->LoadPreviousMenuStateFile();

    if (defined $previousIndexes)
        {  %previousIndexes = %$previousIndexes;  };

    if (defined $previousFiles)
        {  $self->LockUserTitleChanges($previousFiles);  };

    # Don't need these anymore.  We keep this level of detail because it may be used more in the future.
    $previousMenu = undef;
    $previousFiles = undef;
    $previousIndexes = undef;

    # We flag title changes instead of actually performing them at this point for two reasons.  First, contents of groups are still
    # subject to change, which would affect the generated titles.  Second, we haven't detected the sort order yet.  Changing titles
    # could make groups appear unalphabetized when they were beforehand.

    my $updateAllTitles;

    # If the menu file changed, we can't be sure which groups changed and which didn't without a comparison, which really isn't
    # worth the trouble.  So we regenerate all the titles instead.
    if (NaturalDocs::Project->UserConfigFileStatus('Menu.txt') == ::FILE_CHANGED())
        {  $updateAllTitles = 1;  }
    else
        {  $self->FlagAutoTitleChanges();  };

    # We add new files before deleting old files so their presence still affects the grouping.  If we deleted old files first, it could
    # throw off where to place the new ones.

    $self->AutoPlaceNewFiles($filesInMenu);

    my $numberRemoved = $self->RemoveDeadFiles();

    $self->CheckForTrashedMenu(scalar keys %$filesInMenu, $numberRemoved);

    # Don't ban indexes if they deleted Menu.txt.  They may have not deleted PreviousMenuState.nd and we don't want everything
    # to be banned because of it.
    if (NaturalDocs::Project->UserConfigFileStatus('Menu.txt') != ::FILE_DOESNTEXIST())
        {  $self->BanAndUnbanIndexes();  };

    # Index groups need to be detected before adding new ones.

    $self->DetectIndexGroups();

    $self->AddAndRemoveIndexes();

   # We wait until after new files are placed to remove dead groups because a new file may save a group.

    $self->RemoveDeadGroups();

    $self->CreateDirectorySubGroups();

    # We detect the sort before regenerating the titles so it doesn't get thrown off by changes.  However, we do it after deleting
    # dead entries and moving things into subgroups because their removal may bump it into a stronger sort category (i.e.
    # SORTFILESANDGROUPS instead of just SORTFILES.)  New additions don't factor into the sort.

    $self->DetectOrder($updateAllTitles);

    $self->GenerateAutoFileTitles($updateAllTitles);

    $self->ResortGroups($updateAllTitles);


    # Don't need this anymore.
    %defaultTitlesChanged = ( );
    };


#
#   Function: Save
#
#   Writes the changes to the menu files.
#
sub Save
    {
    my ($self) = @_;

    if ($hasChanged)
        {
        $self->SaveMenuFile();
        $self->SavePreviousMenuStateFile();
        };
    };


###############################################################################
# Group: Information Functions

#
#   Function: HasChanged
#
#   Returns whether the menu has changed or not.
#
sub HasChanged
    {  return $hasChanged;  };

#
#   Function: Content
#
#   Returns the parsed menu as an arrayref of <NaturalDocs::Menu::Entry> objects.  Do not change the arrayref.
#
#   The arrayref will only contain <MENU_FILE>, <MENU_GROUP>, <MENU_INDEX>, <MENU_TEXT>, and <MENU_LINK>
#   entries.  Entries such as <MENU_TITLE> are parsed out and are only accessible via functions such as <Title()>.
#
sub Content
    {  return $menu->GroupContent();  };

#
#   Function: Title
#
#   Returns the title of the menu, or undef if none.
#
sub Title
    {  return $title;  };

#
#   Function: SubTitle
#
#   Returns the sub-title of the menu, or undef if none.
#
sub SubTitle
    {  return $subTitle;  };

#
#   Function: Footer
#
#   Returns the footer of the documentation, or undef if none.
#
sub Footer
    {  return $footer;  };

#
#   Function: TimeStamp
#
#   Returns the timestamp text of the documentation, or undef if none.
#
sub TimeStamp
    {  return $timestampText;  };

#
#   Function: Indexes
#
#   Returns an existence hashref of all the index <TopicTypes> appearing in the menu.  Do not change the hashref.
#
sub Indexes
    {  return \%indexes;  };

#
#   Function: PreviousIndexes
#
#   Returns an existence hashref of all the index <TopicTypes> that previously appeared in the menu.  Do not change the
#   hashref.
#
sub PreviousIndexes
    {  return \%previousIndexes;  };


#
#   Function: FilesInMenu
#
#   Returns a hashref of all the files present in the menu.  The keys are the <FileNames>, and the values are references to their
#   <NaturalDocs::Menu::Entry> objects.
#
sub FilesInMenu
    {
    my ($self) = @_;

    my @groupStack = ( $menu );
    my $filesInMenu = { };

    while (scalar @groupStack)
        {
        my $currentGroup = pop @groupStack;
        my $currentGroupContent = $currentGroup->GroupContent();

        foreach my $entry (@$currentGroupContent)
            {
            if ($entry->Type() == ::MENU_GROUP())
                {  push @groupStack, $entry;  }
            elsif ($entry->Type() == ::MENU_FILE())
                {  $filesInMenu->{ $entry->Target() } = $entry;  };
            };
        };

    return $filesInMenu;
    };



###############################################################################
# Group: Event Handlers
#
#   These functions are called by <NaturalDocs::Project> only.  You don't need to worry about calling them.  For example, when
#   changing the default menu title of a file, you only need to call <NaturalDocs::Project->SetDefaultMenuTitle()>.  That function
#   will handle calling <OnDefaultTitleChange()>.


#
#   Function: OnDefaultTitleChange
#
#   Called by <NaturalDocs::Project> if the default menu title of a source file has changed.
#
#   Parameters:
#
#       file    - The source <FileName> that had its default menu title changed.
#
sub OnDefaultTitleChange #(file)
    {
    my ($self, $file) = @_;

    # Collect them for later.  We'll deal with them in LoadAndUpdate().

    $defaultTitlesChanged{$file} = 1;
    };



###############################################################################
# Group: Support Functions


#
#   Function: LoadMenuFile
#
#   Loads and parses the menu file <Menu.txt>.  This will fill <menu>, <title>, <subTitle>, <footer>, <timestampText>,
#   <timestampCode>, <indexes>, and <bannedIndexes>.  If there are any errors in the file, they will be recorded with
#   <NaturalDocs::ConfigFile->AddError()>.
#
#   Returns:
#
#       The array ( inputDirectories, relativeFiles, onlyDirectoryName ) or an empty array if the file doesn't exist.
#
#       inputDirectories - A hashref of all the input directories and their names stored in the menu file.  The keys are the
#                                 directories and the values are their names.  Undef if none.
#       relativeFiles - Whether the menu uses relative file names.
#       onlyDirectoryName - The name of the input directory if there is only one.
#
sub LoadMenuFile
    {
    my ($self) = @_;

    my $inputDirectories = { };
    my $relativeFiles;
    my $onlyDirectoryName;

    # A stack of Menu::Entry object references as we move through the groups.
    my @groupStack;

    $menu = NaturalDocs::Menu::Entry->New(::MENU_GROUP(), undef, undef, undef);
    my $currentGroup = $menu;

    # Whether we're currently in a braceless group, since we'd have to find the implied end rather than an explicit one.
    my $inBracelessGroup;

    # Whether we're right after a group token, which is the only place there can be an opening brace.
    my $afterGroupToken;

    my $version;

    if ($version = NaturalDocs::ConfigFile->Open(NaturalDocs::Project->UserConfigFile('Menu.txt'), 1))
        {
        # We don't check if the menu file is from a future version because we can't just throw it out and regenerate it like we can
        # with other data files.  So we just keep going regardless.  Any syntactic differences will show up as errors.

        while (my ($keyword, $value, $comment) = NaturalDocs::ConfigFile->GetLine())
            {
            # Check for an opening brace after a group token.  This has to be separate from the rest of the code because the flag
            # needs to be reset after every line.
            if ($afterGroupToken)
                {
                $afterGroupToken = undef;

                if ($keyword eq '{')
                    {
                    $inBracelessGroup = undef;
                    next;
                    }
                else
                    {  $inBracelessGroup = 1;  };
                };


            # Now on to the real code.

            if ($keyword eq 'file')
                {
                my $flags = 0;

                if ($value =~ /^(.+)\(([^\(]+)\)$/)
                    {
                    my ($title, $file) = ($1, $2);

                    $title =~ s/ +$//;

                    # Check for auto-title modifier.
                    if ($file =~ /^((?:no )?auto-title, ?)(.+)$/i)
                        {
                        my $modifier;
                        ($modifier, $file) = ($1, $2);

                        if ($modifier =~ /^no/i)
                            {  $flags |= ::MENU_FILE_NOAUTOTITLE();  };
                        };

                    my $entry = NaturalDocs::Menu::Entry->New(::MENU_FILE(), $self->RestoreAmpChars($title),
                                                                                       $self->RestoreAmpChars($file), $flags);

                    $currentGroup->PushToGroup($entry);
                    }
                else
                    {  NaturalDocs::ConfigFile->AddError('File lines must be in the format "File: [title] ([location])"');  };
                }

            elsif ($keyword eq 'group')
                {
                # End a braceless group, if we were in one.
                if ($inBracelessGroup)
                    {
                    $currentGroup = pop @groupStack;
                    $inBracelessGroup = undef;
                    };

                my $entry = NaturalDocs::Menu::Entry->New(::MENU_GROUP(), $self->RestoreAmpChars($value), undef, undef);

                $currentGroup->PushToGroup($entry);

                push @groupStack, $currentGroup;
                $currentGroup = $entry;

                $afterGroupToken = 1;
                }


            elsif ($keyword eq '{')
                {
                NaturalDocs::ConfigFile->AddError('Opening braces are only allowed after Group tags.');
                }


            elsif ($keyword eq '}')
                {
                # End a braceless group, if we were in one.
                if ($inBracelessGroup)
                    {
                    $currentGroup = pop @groupStack;
                    $inBracelessGroup = undef;
                    };

                # End a braced group too.
                if (scalar @groupStack)
                    {  $currentGroup = pop @groupStack;  }
                else
                    {  NaturalDocs::ConfigFile->AddError('Unmatched closing brace.');  };
                }


            elsif ($keyword eq 'title')
                {
                if (!defined $title)
                    {  $title = $self->RestoreAmpChars($value);  }
                else
                    {  NaturalDocs::ConfigFile->AddError('Title can only be defined once.');  };
                }


            elsif ($keyword eq 'subtitle')
                {
                if (defined $title)
                    {
                    if (!defined $subTitle)
                        {  $subTitle = $self->RestoreAmpChars($value);  }
                    else
                        {  NaturalDocs::ConfigFile->AddError('SubTitle can only be defined once.');  };
                    }
                else
                    {  NaturalDocs::ConfigFile->AddError('Title must be defined before SubTitle.');  };
                }


            elsif ($keyword eq 'footer')
                {
                if (!defined $footer)
                    {  $footer = $self->RestoreAmpChars($value);  }
                else
                    {  NaturalDocs::ConfigFile->AddError('Footer can only be defined once.');  };
                }


            elsif ($keyword eq 'timestamp')
                {
                if (!defined $timestampCode)
                    {
                    $timestampCode = $self->RestoreAmpChars($value);
                    $self->GenerateTimestampText();
                    }
                else
                    {  NaturalDocs::ConfigFile->AddError('Timestamp can only be defined once.');  };
                }


            elsif ($keyword eq 'text')
                {
                $currentGroup->PushToGroup( NaturalDocs::Menu::Entry->New(::MENU_TEXT(), $self->RestoreAmpChars($value),
                                                                                                              undef, undef) );
                }


            elsif ($keyword eq 'link')
                {
                my ($title, $url);

                if ($value =~ /^([^\(\)]+?) ?\(([^\)]+)\)$/)
                    {
                    ($title, $url) = ($1, $2);
                    }
                elsif (defined $comment)
                    {
                    $value .= $comment;

                    if ($value =~ /^([^\(\)]+?) ?\(([^\)]+)\) ?(?:#.*)?$/)
                        {
                        ($title, $url) = ($1, $2);
                        };
                    };

                if ($title)
                    {
                    $currentGroup->PushToGroup( NaturalDocs::Menu::Entry->New(::MENU_LINK(), $self->RestoreAmpChars($title),
                                                                 $self->RestoreAmpChars($url), undef) );
                    }
                else
                    {  NaturalDocs::ConfigFile->AddError('Link lines must be in the format "Link: [title] ([url])"');  };
                }


            elsif ($keyword eq 'data')
                {
                $value =~ /^(\d)\((.*)\)$/;
                my ($number, $data) = ($1, $2);

                $data = NaturalDocs::ConfigFile->Unobscure($data);

                # The input directory naming convention changed with version 1.32, but NaturalDocs::Settings will handle that
                # automatically.

                if ($number == 1)
                    {
                    my ($dirName, $inputDir) = split(/\/\/\//, $data, 2);
                    $inputDirectories->{$inputDir} = $dirName;
                    }
                elsif ($number == 2)
                    {  $onlyDirectoryName = $data;  };
                # Ignore other numbers because it may be from a future format and we don't want to make the user delete it
                # manually.
                }

            elsif ($keyword eq "don't index")
                {
                my @indexes = split(/, ?/, $value);

                foreach my $index (@indexes)
                    {
                    my $indexType = NaturalDocs::Topics->TypeFromName( $self->RestoreAmpChars($index) );

                    if (defined $indexType)
                        {  $bannedIndexes{$indexType} = 1;  };
                    };
                }

            elsif ($keyword eq 'index')
                {
                my $entry = NaturalDocs::Menu::Entry->New(::MENU_INDEX(), $self->RestoreAmpChars($value),
                                                                                   ::TOPIC_GENERAL(), undef);
                $currentGroup->PushToGroup($entry);

                $indexes{::TOPIC_GENERAL()} = 1;
                }

            elsif (substr($keyword, -6) eq ' index')
                {
                my $index = substr($keyword, 0, -6);
                my ($indexType, $indexInfo) = NaturalDocs::Topics->NameInfo( $self->RestoreAmpChars($index) );

                if (defined $indexType)
                    {
                    if ($indexInfo->Index())
                        {
                        $indexes{$indexType} = 1;
                        $currentGroup->PushToGroup(
                            NaturalDocs::Menu::Entry->New(::MENU_INDEX(), $self->RestoreAmpChars($value), $indexType, undef) );
                        }
                    else
                        {
                        # If it's on the menu but isn't indexable, the topic setting may have changed out from under it.
                        $hasChanged = 1;
                        };
                    }
                else
                    {
                    NaturalDocs::ConfigFile->AddError($index . ' is not a valid index type.');
                    };
                }

            else
                {
                NaturalDocs::ConfigFile->AddError(ucfirst($keyword) . ' is not a valid keyword.');
                };
            };


        # End a braceless group, if we were in one.
        if ($inBracelessGroup)
            {
            $currentGroup = pop @groupStack;
            $inBracelessGroup = undef;
            };

        # Close up all open groups.
        my $openGroups = 0;
        while (scalar @groupStack)
            {
            $currentGroup = pop @groupStack;
            $openGroups++;
            };

        if ($openGroups == 1)
            {  NaturalDocs::ConfigFile->AddError('There is an unclosed group.');  }
        elsif ($openGroups > 1)
            {  NaturalDocs::ConfigFile->AddError('There are ' . $openGroups . ' unclosed groups.');  };


        if (!scalar keys %$inputDirectories)
            {
            $inputDirectories = undef;
            $relativeFiles = 1;
            };

        NaturalDocs::ConfigFile->Close();

        return ($inputDirectories, $relativeFiles, $onlyDirectoryName);
        }

    else
        {  return ( );  };
    };


#
#   Function: SaveMenuFile
#
#   Saves the current menu to <Menu.txt>.
#
sub SaveMenuFile
    {
    my ($self) = @_;

    open(MENUFILEHANDLE, '>' . NaturalDocs::Project->UserConfigFile('Menu.txt'))
        or die "Couldn't save menu file " . NaturalDocs::Project->UserConfigFile('Menu.txt') . "\n";


    print MENUFILEHANDLE
    "Format: " . NaturalDocs::Settings->TextAppVersion() . "\n\n\n";

    my $inputDirs = NaturalDocs::Settings->InputDirectories();


    if (defined $title)
        {
        print MENUFILEHANDLE 'Title: ' . $self->ConvertAmpChars($title) . "\n";

        if (defined $subTitle)
            {
            print MENUFILEHANDLE 'SubTitle: ' . $self->ConvertAmpChars($subTitle) . "\n";
            }
        else
            {
            print MENUFILEHANDLE
            "\n"
            . "# You can also add a sub-title to your menu like this:\n"
            . "# SubTitle: [subtitle]\n";
            };
        }
    else
        {
        print MENUFILEHANDLE
        "# You can add a title and sub-title to your menu like this:\n"
        . "# Title: [project name]\n"
        . "# SubTitle: [subtitle]\n";
        };

    print MENUFILEHANDLE "\n";

    if (defined $footer)
        {
        print MENUFILEHANDLE 'Footer: ' . $self->ConvertAmpChars($footer) . "\n";
        }
    else
        {
        print MENUFILEHANDLE
        "# You can add a footer to your documentation like this:\n"
        . "# Footer: [text]\n"
        . "# If you want to add a copyright notice, this would be the place to do it.\n";
        };

    if (defined $timestampCode)
        {
        print MENUFILEHANDLE 'Timestamp: ' . $self->ConvertAmpChars($timestampCode) . "\n";
        }
    else
        {
        print MENUFILEHANDLE
        "\n"
        . "# You can add a timestamp to your documentation like one of these:\n"
        . "# Timestamp: Generated on month day, year\n"
        . "# Timestamp: Updated mm/dd/yyyy\n"
        . "# Timestamp: Last updated mon day\n"
        . "#\n";
        };

    print MENUFILEHANDLE
        qq{#   m     - One or two digit month.  January is "1"\n}
        . qq{#   mm    - Always two digit month.  January is "01"\n}
        . qq{#   mon   - Short month word.  January is "Jan"\n}
        . qq{#   month - Long month word.  January is "January"\n}
        . qq{#   d     - One or two digit day.  1 is "1"\n}
        . qq{#   dd    - Always two digit day.  1 is "01"\n}
        . qq{#   day   - Day with letter extension.  1 is "1st"\n}
        . qq{#   yy    - Two digit year.  2006 is "06"\n}
        . qq{#   yyyy  - Four digit year.  2006 is "2006"\n}
        . qq{#   year  - Four digit year.  2006 is "2006"\n}

        . "\n";

    if (scalar keys %bannedIndexes)
        {
        print MENUFILEHANDLE

        "# These are indexes you deleted, so Natural Docs will not add them again\n"
        . "# unless you remove them from this line.\n"
        . "\n"
        . "Don't Index: ";

        my $first = 1;

        foreach my $index (keys %bannedIndexes)
            {
            if (!$first)
                {  print MENUFILEHANDLE ', ';  }
            else
                {  $first = undef;  };

            print MENUFILEHANDLE $self->ConvertAmpChars( NaturalDocs::Topics->NameOfType($index, 1), CONVERT_COMMAS() );
            };

        print MENUFILEHANDLE "\n\n";
        };


    # Remember to keep lines below eighty characters.

    print MENUFILEHANDLE
    "\n"
    . "# --------------------------------------------------------------------------\n"
    . "# \n"
    . "# Cut and paste the lines below to change the order in which your files\n"
    . "# appear on the menu.  Don't worry about adding or removing files, Natural\n"
    . "# Docs will take care of that.\n"
    . "# \n"
    . "# You can further organize the menu by grouping the entries.  Add a\n"
    . "# \"Group: [name] {\" line to start a group, and add a \"}\" to end it.\n"
    . "# \n"
    . "# You can add text and web links to the menu by adding \"Text: [text]\" and\n"
    . "# \"Link: [name] ([URL])\" lines, respectively.\n"
    . "# \n"
    . "# The formatting and comments are auto-generated, so don't worry about\n"
    . "# neatness when editing the file.  Natural Docs will clean it up the next\n"
    . "# time it is run.  When working with groups, just deal with the braces and\n"
    . "# forget about the indentation and comments.\n"
    . "# \n";

    if (scalar @$inputDirs > 1)
        {
        print MENUFILEHANDLE
        "# You can use this file on other computers even if they use different\n"
        . "# directories.  As long as the command line points to the same source files,\n"
        . "# Natural Docs will be able to correct the locations automatically.\n"
        . "# \n";
        };

    print MENUFILEHANDLE
    "# --------------------------------------------------------------------------\n"

    . "\n\n";


    $self->WriteMenuEntries($menu->GroupContent(), \*MENUFILEHANDLE, undef, (scalar @$inputDirs == 1));


    if (scalar @$inputDirs > 1)
        {
        print MENUFILEHANDLE
        "\n\n##### Do not change or remove these lines. #####\n";

        foreach my $inputDir (@$inputDirs)
            {
            print MENUFILEHANDLE
            'Data: 1(' . NaturalDocs::ConfigFile->Obscure( NaturalDocs::Settings->InputDirectoryNameOf($inputDir)
                                                                              . '///' . $inputDir ) . ")\n";
            };
        }
    elsif (lc(NaturalDocs::Settings->InputDirectoryNameOf($inputDirs->[0])) != 1)
        {
        print MENUFILEHANDLE
        "\n\n##### Do not change or remove this line. #####\n"
        . 'Data: 2(' . NaturalDocs::ConfigFile->Obscure( NaturalDocs::Settings->InputDirectoryNameOf($inputDirs->[0]) ) . ")\n";
        }

    close(MENUFILEHANDLE);
    };


#
#   Function: WriteMenuEntries
#
#   A recursive function to write the contents of an arrayref of <NaturalDocs::Menu::Entry> objects to disk.
#
#   Parameters:
#
#       entries          - The arrayref of menu entries to write.
#       fileHandle      - The handle to the output file.
#       indentChars   - The indentation _characters_ to add before each line.  It is not the number of characters, it is the characters
#                              themselves.  Use undef for none.
#       relativeFiles - Whether to use relative file names.
#
sub WriteMenuEntries #(entries, fileHandle, indentChars, relativeFiles)
    {
    my ($self, $entries, $fileHandle, $indentChars, $relativeFiles) = @_;
    my $lastEntryType;

    foreach my $entry (@$entries)
        {
        if ($entry->Type() == ::MENU_FILE())
            {
            my $fileName;

            if ($relativeFiles)
                {  $fileName = (NaturalDocs::Settings->SplitFromInputDirectory($entry->Target()))[1];  }
            else
                {  $fileName = $entry->Target();  };

            print $fileHandle $indentChars . 'File: ' . $self->ConvertAmpChars( $entry->Title(), CONVERT_PARENTHESIS() )
                                  . '  (' . ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE() ? 'no auto-title, ' : '')
                                  . $self->ConvertAmpChars($fileName) . ")\n";
            }
        elsif ($entry->Type() == ::MENU_GROUP())
            {
            if (defined $lastEntryType && $lastEntryType != ::MENU_GROUP())
                {  print $fileHandle "\n";  };

            print $fileHandle $indentChars . 'Group: ' . $self->ConvertAmpChars( $entry->Title() ) . "  {\n\n";
            $self->WriteMenuEntries($entry->GroupContent(), $fileHandle, '   ' . $indentChars, $relativeFiles);
            print $fileHandle '   ' . $indentChars . '}  # Group: ' . $self->ConvertAmpChars( $entry->Title() ) . "\n\n";
            }
        elsif ($entry->Type() == ::MENU_TEXT())
            {
            print $fileHandle $indentChars . 'Text: ' . $self->ConvertAmpChars( $entry->Title() ) . "\n";
            }
        elsif ($entry->Type() == ::MENU_LINK())
            {
            print $fileHandle $indentChars . 'Link: ' . $self->ConvertAmpChars( $entry->Title() ) . '  '
                                                        . '(' . $self->ConvertAmpChars( $entry->Target(), CONVERT_PARENTHESIS() ) . ')' . "\n";
            }
        elsif ($entry->Type() == ::MENU_INDEX())
            {
            my $type;
            if ($entry->Target() ne ::TOPIC_GENERAL())
                {
                $type = NaturalDocs::Topics->NameOfType($entry->Target()) . ' ';
                };

            print $fileHandle $indentChars . $self->ConvertAmpChars($type, CONVERT_COLONS()) . 'Index: '
                                                        . $self->ConvertAmpChars( $entry->Title() ) . "\n";
            };

        $lastEntryType = $entry->Type();
        };
    };


#
#   Function: LoadPreviousMenuStateFile
#
#   Loads and parses the previous menu state file.
#
#   Returns:
#
#       The array ( previousMenu, previousIndexes, previousFiles ) or an empty array if there was a problem with the file.
#
#       previousMenu - A <MENU_GROUP> <NaturalDocs::Menu::Entry> object, similar to <menu>, which contains the entire
#                              previous menu.
#       previousIndexes - An existence hashref of the index <TopicTypes> present in the previous menu.
#       previousFiles - A hashref of the files present in the previous menu.  The keys are the <FileNames>, and the entries are
#                             references to its object in previousMenu.
#
sub LoadPreviousMenuStateFile
    {
    my ($self) = @_;

    my $fileIsOkay;
    my $version;
    my $previousStateFileName = NaturalDocs::Project->DataFile('PreviousMenuState.nd');

    if (open(PREVIOUSSTATEFILEHANDLE, '<' . $previousStateFileName))
        {
        # See if it's binary.
        binmode(PREVIOUSSTATEFILEHANDLE);

        my $firstChar;
        read(PREVIOUSSTATEFILEHANDLE, $firstChar, 1);

        if ($firstChar == ::BINARY_FORMAT())
            {
            $version = NaturalDocs::Version->FromBinaryFile(\*PREVIOUSSTATEFILEHANDLE);

            # Only the topic type format has changed since switching to binary, and we support both methods.

            if (NaturalDocs::Version->CheckFileFormat($version))
                {  $fileIsOkay = 1;  }
            else
                {  close(PREVIOUSSTATEFILEHANDLE);  };
            }

        else # it's not in binary
            {  close(PREVIOUSSTATEFILEHANDLE);  };
        };

    if ($fileIsOkay)
        {
        if (NaturalDocs::Project->UserConfigFileStatus('Menu.txt') == ::FILE_CHANGED())
            {  $hasChanged = 1;  };


        my $menu = NaturalDocs::Menu::Entry->New(::MENU_GROUP(), undef, undef, undef);
        my $indexes = { };
        my $files = { };

        my @groupStack;
        my $currentGroup = $menu;
        my $raw;

        # [UInt8: type or 0 for end group]

        while (read(PREVIOUSSTATEFILEHANDLE, $raw, 1))
            {
            my ($type, $flags, $title, $titleLength, $target, $targetLength);
            $type = unpack('C', $raw);

            if ($type == 0)
                {  $currentGroup = pop @groupStack;  }

            elsif ($type == ::MENU_FILE())
                {
                # [UInt8: noAutoTitle] [AString16: title] [AString16: target]

                read(PREVIOUSSTATEFILEHANDLE, $raw, 3);
                (my $noAutoTitle, $titleLength) = unpack('Cn', $raw);

                if ($noAutoTitle)
                    {  $flags = ::MENU_FILE_NOAUTOTITLE();  };

                read(PREVIOUSSTATEFILEHANDLE, $title, $titleLength);
                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);

                $targetLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $target, $targetLength);
                }

            elsif ($type == ::MENU_GROUP())
                {
                # [AString16: title]

                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                $titleLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $title, $titleLength);
                }

            elsif ($type == ::MENU_INDEX())
                {
                # [AString16: title]

                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                $titleLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $title, $titleLength);

                if ($version >= NaturalDocs::Version->FromString('1.3'))
                    {
                    # [AString16: topic type]
                    read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                    $targetLength = unpack('n', $raw);

                    read(PREVIOUSSTATEFILEHANDLE, $target, $targetLength);
                    }
                else
                    {
                    # [UInt8: topic type (0 for general)]
                    read(PREVIOUSSTATEFILEHANDLE, $raw, 1);
                    $target = unpack('C', $raw);

                    $target = NaturalDocs::Topics->TypeFromLegacy($target);
                    };
                }

            elsif ($type == ::MENU_LINK())
                {
                # [AString16: title] [AString16: url]

                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                $titleLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $title, $titleLength);
                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                $targetLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $target, $targetLength);
                }

            elsif ($type == ::MENU_TEXT())
                {
                # [AString16: text]

                read(PREVIOUSSTATEFILEHANDLE, $raw, 2);
                $titleLength = unpack('n', $raw);

                read(PREVIOUSSTATEFILEHANDLE, $title, $titleLength);
                };


            # The topic type of the index may have been removed.

            if ( !($type == ::MENU_INDEX() && !NaturalDocs::Topics->IsValidType($target)) )
                {
                my $entry = NaturalDocs::Menu::Entry->New($type, $title, $target, ($flags || 0));
                $currentGroup->PushToGroup($entry);

                if ($type == ::MENU_FILE())
                    {
                    $files->{$target} = $entry;
                    }
                elsif ($type == ::MENU_GROUP())
                    {
                    push @groupStack, $currentGroup;
                    $currentGroup = $entry;
                    }
                elsif ($type == ::MENU_INDEX())
                    {
                    $indexes->{$target} = 1;
                    };
                };

            };

        close(PREVIOUSSTATEFILEHANDLE);

        return ($menu, $indexes, $files);
        }
    else
        {
        $hasChanged = 1;
        return ( );
        };
    };


#
#   Function: SavePreviousMenuStateFile
#
#   Saves changes to <PreviousMenuState.nd>.
#
sub SavePreviousMenuStateFile
    {
    my ($self) = @_;

    open (PREVIOUSSTATEFILEHANDLE, '>' . NaturalDocs::Project->DataFile('PreviousMenuState.nd'))
        or die "Couldn't save " . NaturalDocs::Project->DataFile('PreviousMenuState.nd') . ".\n";

    binmode(PREVIOUSSTATEFILEHANDLE);

    print PREVIOUSSTATEFILEHANDLE '' . ::BINARY_FORMAT();

    NaturalDocs::Version->ToBinaryFile(\*PREVIOUSSTATEFILEHANDLE, NaturalDocs::Settings->AppVersion());

    $self->WritePreviousMenuStateEntries($menu->GroupContent(), \*PREVIOUSSTATEFILEHANDLE);

    close(PREVIOUSSTATEFILEHANDLE);
    };


#
#   Function: WritePreviousMenuStateEntries
#
#   A recursive function to write the contents of an arrayref of <NaturalDocs::Menu::Entry> objects to disk.
#
#   Parameters:
#
#       entries          - The arrayref of menu entries to write.
#       fileHandle      - The handle to the output file.
#
sub WritePreviousMenuStateEntries #(entries, fileHandle)
    {
    my ($self, $entries, $fileHandle) = @_;

    foreach my $entry (@$entries)
        {
        if ($entry->Type() == ::MENU_FILE())
            {
            # We need to do length manually instead of using n/A in the template because it's not supported in earlier versions
            # of Perl.

            # [UInt8: MENU_FILE] [UInt8: noAutoTitle] [AString16: title] [AString16: target]
            print $fileHandle pack('CCnA*nA*', ::MENU_FILE(), ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE() ? 1 : 0),
                                                                length($entry->Title()), $entry->Title(),
                                                                length($entry->Target()), $entry->Target());
            }

        elsif ($entry->Type() == ::MENU_GROUP())
            {
            # [UInt8: MENU_GROUP] [AString16: title]
            print $fileHandle pack('CnA*', ::MENU_GROUP(), length($entry->Title()), $entry->Title());
            $self->WritePreviousMenuStateEntries($entry->GroupContent(), $fileHandle);
            print $fileHandle pack('C', 0);
            }

        elsif ($entry->Type() == ::MENU_INDEX())
            {
            # [UInt8: MENU_INDEX] [AString16: title] [AString16: topic type]
            print $fileHandle pack('CnA*nA*', ::MENU_INDEX(), length($entry->Title()), $entry->Title(),
                                                                                       length($entry->Target()), $entry->Target());
            }

        elsif ($entry->Type() == ::MENU_LINK())
            {
            # [UInt8: MENU_LINK] [AString16: title] [AString16: url]
            print $fileHandle pack('CnA*nA*', ::MENU_LINK(), length($entry->Title()), $entry->Title(),
                                                             length($entry->Target()), $entry->Target());
            }

        elsif ($entry->Type() == ::MENU_TEXT())
            {
            # [UInt8: MENU_TEXT] [AString16: hext]
            print $fileHandle pack('CnA*', ::MENU_TEXT(), length($entry->Title()), $entry->Title());
            };
        };

    };


#
#   Function: CheckForTrashedMenu
#
#   Checks the menu to see if a significant number of file entries didn't resolve to actual files, and if so, saves a backup of the
#   menu and issues a warning.
#
#   Parameters:
#
#       numberOriginallyInMenu - A count of how many file entries were in the menu orignally.
#       numberRemoved - A count of how many file entries were removed from the menu.
#
sub CheckForTrashedMenu #(numberOriginallyInMenu, numberRemoved)
    {
    my ($self, $numberOriginallyInMenu, $numberRemoved) = @_;

    no integer;

    if ( ($numberOriginallyInMenu >= 6 && $numberRemoved == $numberOriginallyInMenu) ||
         ($numberOriginallyInMenu >= 12 && ($numberRemoved / $numberOriginallyInMenu) >= 0.4) ||
         ($numberRemoved >= 15) )
        {
        my $backupFile = NaturalDocs::Project->UserConfigFile('Menu_Backup.txt');
        my $backupFileNumber = 1;

        while (-e $backupFile)
            {
            $backupFileNumber++;
            $backupFile = NaturalDocs::Project->UserConfigFile('Menu_Backup_' . $backupFileNumber . '.txt');
            };

        NaturalDocs::File->Copy( NaturalDocs::Project->UserConfigFile('Menu.txt'), $backupFile );

        print STDERR
        "\n"
        # GNU format.  See http://www.gnu.org/prep/standards_15.html
        . "NaturalDocs: warning: possible trashed menu\n"
        . "\n"
        . "   Natural Docs has detected that a significant number file entries in the\n"
        . "   menu did not resolve to actual files.  A backup of your original menu file\n"
        . "   has been saved as\n"
        . "\n"
        . "   " . $backupFile . "\n"
        . "\n"
        . "   - If you recently deleted a lot of files from your project, you can safely\n"
        . "     ignore this message.  They have been deleted from the menu as well.\n"
        . "   - If you recently rearranged your source tree, you may want to restore your\n"
        . "     menu from the backup and do a search and replace to preserve your layout.\n"
        . "     Otherwise the position of any moved files will be reset.\n"
        . "   - If neither of these is the case, you may have gotten the -i parameter\n"
        . "     wrong in the command line.  You should definitely restore the backup and\n"
        . "     try again, because otherwise every file in your menu will be reset.\n"
        . "\n";
        };

    use integer;
    };


#
#   Function: GenerateTimestampText
#
#   Generates <timestampText> from <timestampCode> with the current date.
#
sub GenerateTimestampText
    {
    my $self = shift;

    my @longMonths = ( 'January', 'February', 'March', 'April', 'May', 'June',
                                   'July', 'August', 'September', 'October', 'November', 'December' );
    my @shortMonths = ( 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sept', 'Oct', 'Nov', 'Dec' );

    my (undef, undef, undef, $day, $month, $year) = localtime();
    $year += 1900;

    my $longDay;
    if ($day % 10 == 1 && $day != 11)
        {  $longDay = $day . 'st';  }
    elsif ($day % 10 == 2 && $day != 12)
        {  $longDay = $day . 'nd';  }
    elsif ($day % 10 == 3 && $day != 13)
        {  $longDay = $day . 'rd';  }
    else
        {  $longDay = $day . 'th';  };


    $timestampText = $timestampCode;

    $timestampText =~ s/(?<![a-z])month(?![a-z])/$longMonths[$month]/i;
    $timestampText =~ s/(?<![a-z])mon(?![a-z])/$shortMonths[$month]/i;
    $timestampText =~ s/(?<![a-z])mm(?![a-z])/sprintf('%02d', $month + 1)/ie;
    $timestampText =~ s/(?<![a-z])m(?![a-z])/$month + 1/ie;

    $timestampText =~ s/(?<![a-z])day(?![a-z])/$longDay/i;
    $timestampText =~ s/(?<![a-z])dd(?![a-z])/sprintf('%02d', $day)/ie;
    $timestampText =~ s/(?<![a-z])d(?![a-z])/$day/i;

    $timestampText =~ s/(?<![a-z])(?:year|yyyy)(?![a-z])/$year/i;
    $timestampText =~ s/(?<![a-z])yy(?![a-z])/sprintf('%02d', $year % 100)/ie;
    };


use constant CONVERT_PARENTHESIS => 0x01;
use constant CONVERT_COMMAS => 0x02;
use constant CONVERT_COLONS => 0x04;

#
#   Function: ConvertAmpChars
#   Replaces certain characters in the string with their entities and returns it.
#
#   Parameters:
#
#       text - The text to convert.
#       flags - The flags of any additional characters to convert.
#
#   Flags:
#
#       - CONVERT_PARENTHESIS
#       - CONVERT_COMMAS
#       - CONVERT_COLONS
#
#   Returns:
#
#       The string with the amp chars converted.
#
sub ConvertAmpChars #(string text, int flags) => string
    {
    my ($self, $text, $flags) = @_;

    $text =~ s/&/&amp;/g;
    $text =~ s/\{/&lbrace;/g;
    $text =~ s/\}/&rbrace;/g;

    if ($flags & CONVERT_PARENTHESIS())
        {
        $text =~ s/\(/&lparen;/g;
        $text =~ s/\)/&rparen;/g;
        };
    if ($flags & CONVERT_COMMAS())
        {
        $text =~ s/\,/&comma;/g;
        };
    if ($flags & CONVERT_COLONS())
        {
        $text =~ s/\:/&colon;/g;
        };

    return $text;
    };


#
#   Function: RestoreAmpChars
#   Replaces entity characters in the string with their original characters and returns it.  This will restore all amp chars regardless
#   of the flags passed to <ConvertAmpChars()>.
#
sub RestoreAmpChars #(string text) => string
    {
    my ($self, $text) = @_;

    $text =~ s/&lparen;/(/gi;
    $text =~ s/&rparen;/)/gi;
    $text =~ s/&lbrace;/{/gi;
    $text =~ s/&rbrace;/}/gi;
    $text =~ s/&comma;/,/gi;
    $text =~ s/&amp;/&/gi;
    $text =~ s/&colon;/:/gi;

    return $text;
    };



###############################################################################
# Group: Auto-Adjustment Functions


#
#   Function: ResolveInputDirectories
#
#   Detects if the input directories in the menu file match those in the command line, and if not, tries to resolve them.  This allows
#   menu files to work across machines, since the absolute paths won't be the same but the relative ones should be.
#
#   Parameters:
#
#       inputDirectoryNames - A hashref of the input directories appearing in the menu file, or undef if none.  The keys are the
#                                        directories, and the values are their names.  May be undef.
#
sub ResolveInputDirectories #(inputDirectoryNames)
    {
    my ($self, $menuDirectoryNames) = @_;


    # Determine which directories don't match the command line, if any.

    my $inputDirectories = NaturalDocs::Settings->InputDirectories();
    my @unresolvedMenuDirectories;

    foreach my $menuDirectory (keys %$menuDirectoryNames)
        {
        my $found;

        foreach my $inputDirectory (@$inputDirectories)
            {
            if ($menuDirectory eq $inputDirectory)
                {
                $found = 1;
                last;
                };
            };

        if (!$found)
            {  push @unresolvedMenuDirectories, $menuDirectory;  };
        };

    # Quit if everything matches up, which should be the most common case.
    if (!scalar @unresolvedMenuDirectories)
        {  return;  };

    # Poop.  See which input directories are still available.

    my @unresolvedInputDirectories;

    foreach my $inputDirectory (@$inputDirectories)
        {
        if (!exists $menuDirectoryNames->{$inputDirectory})
            {  push @unresolvedInputDirectories, $inputDirectory;  };
        };

    # Quit if there are none.  This means an input directory is in the menu that isn't in the command line.  Natural Docs should
    # proceed normally and let the files be deleted.
    if (!scalar @unresolvedInputDirectories)
        {
        $hasChanged = 1;
        return;
        };

    # The index into menuDirectoryScores is the same as in unresolvedMenuDirectories.  The index into each arrayref within it is
    # the same as in unresolvedInputDirectories.
    my @menuDirectoryScores;
    for (my $i = 0; $i < scalar @unresolvedMenuDirectories; $i++)
        {  push @menuDirectoryScores, [ ];  };


    # Now plow through the menu, looking for files that have an unresolved base.

    my @menuGroups = ( $menu );

    while (scalar @menuGroups)
        {
        my $currentGroup = pop @menuGroups;
        my $currentGroupContent = $currentGroup->GroupContent();

        foreach my $entry (@$currentGroupContent)
            {
            if ($entry->Type() == ::MENU_GROUP())
                {
                push @menuGroups, $entry;
                }
            elsif ($entry->Type() == ::MENU_FILE())
                {
                # Check if it uses an unresolved base.
                for (my $i = 0; $i < scalar @unresolvedMenuDirectories; $i++)
                    {
                    if (NaturalDocs::File->IsSubPathOf($unresolvedMenuDirectories[$i], $entry->Target()))
                        {
                        my $relativePath = NaturalDocs::File->MakeRelativePath($unresolvedMenuDirectories[$i], $entry->Target());
                        $self->ResolveFile($relativePath, \@unresolvedInputDirectories, $menuDirectoryScores[$i]);
                        last;
                        };
                    };
                };
            };
        };


    # Now, create an array of score objects.  Each score object is the three value arrayref [ from, to, score ].  From and To are the
    # conversion options and are the indexes into unresolvedInput/MenuDirectories.  We'll sort this array by score to get the best
    # possible conversions.  Yes, really.
    my @scores;

    for (my $menuIndex = 0; $menuIndex < scalar @unresolvedMenuDirectories; $menuIndex++)
        {
        for (my $inputIndex = 0; $inputIndex < scalar @unresolvedInputDirectories; $inputIndex++)
            {
            if ($menuDirectoryScores[$menuIndex]->[$inputIndex])
                {
                push @scores, [ $menuIndex, $inputIndex, $menuDirectoryScores[$menuIndex]->[$inputIndex] ];
                };
            };
        };

    @scores = sort { $b->[2] <=> $a->[2] } @scores;


    # Now we determine what goes where.
    my @menuDirectoryConversions;

    foreach my $scoreObject (@scores)
        {
        if (!defined $menuDirectoryConversions[ $scoreObject->[0] ])
            {
            $menuDirectoryConversions[ $scoreObject->[0] ] = $unresolvedInputDirectories[ $scoreObject->[1] ];
            };
        };


    # Now, FINALLY, we do the conversion.  Note that not every menu directory may have a conversion defined.

    @menuGroups = ( $menu );

    while (scalar @menuGroups)
        {
        my $currentGroup = pop @menuGroups;
        my $currentGroupContent = $currentGroup->GroupContent();

        foreach my $entry (@$currentGroupContent)
            {
            if ($entry->Type() == ::MENU_GROUP())
                {
                push @menuGroups, $entry;
                }
            elsif ($entry->Type() == ::MENU_FILE())
                {
                # Check if it uses an unresolved base.
                for (my $i = 0; $i < scalar @unresolvedMenuDirectories; $i++)
                    {
                    if (NaturalDocs::File->IsSubPathOf($unresolvedMenuDirectories[$i], $entry->Target()) &&
                        defined $menuDirectoryConversions[$i])
                        {
                        my $relativePath = NaturalDocs::File->MakeRelativePath($unresolvedMenuDirectories[$i], $entry->Target());
                        $entry->SetTarget( NaturalDocs::File->JoinPaths($menuDirectoryConversions[$i], $relativePath) );
                        last;
                        };
                    };
                };
            };
        };


    # Whew.

    $hasChanged = 1;
    };


#
#   Function: ResolveRelativeInputDirectories
#
#   Resolves relative input directories to the input directories available.
#
sub ResolveRelativeInputDirectories
    {
    my ($self) = @_;

    my $inputDirectories = NaturalDocs::Settings->InputDirectories();
    my $resolvedInputDirectory;

    if (scalar @$inputDirectories == 1)
        {  $resolvedInputDirectory = $inputDirectories->[0];  }
    else
        {
        my @score;

        # Plow through the menu, looking for files and scoring them.

        my @menuGroups = ( $menu );

        while (scalar @menuGroups)
            {
            my $currentGroup = pop @menuGroups;
            my $currentGroupContent = $currentGroup->GroupContent();

            foreach my $entry (@$currentGroupContent)
                {
                if ($entry->Type() == ::MENU_GROUP())
                    {
                    push @menuGroups, $entry;
                    }
                elsif ($entry->Type() == ::MENU_FILE())
                    {
                    $self->ResolveFile($entry->Target(), $inputDirectories, \@score);
                    };
                };
            };

        # Determine the best match.

        my $bestScore = 0;
        my $bestIndex = 0;

        for (my $i = 0; $i < scalar @$inputDirectories; $i++)
            {
            if ($score[$i] > $bestScore)
                {
                $bestScore = $score[$i];
                $bestIndex = $i;
                };
            };

        $resolvedInputDirectory = $inputDirectories->[$bestIndex];
        };


    # Okay, now that we have our resolved directory, update everything.

    my @menuGroups = ( $menu );

    while (scalar @menuGroups)
        {
        my $currentGroup = pop @menuGroups;
        my $currentGroupContent = $currentGroup->GroupContent();

        foreach my $entry (@$currentGroupContent)
            {
            if ($entry->Type() == ::MENU_GROUP())
                {  push @menuGroups, $entry;  }
            elsif ($entry->Type() == ::MENU_FILE())
                {
                $entry->SetTarget( NaturalDocs::File->JoinPaths($resolvedInputDirectory, $entry->Target()) );
                };
            };
        };

    if (scalar @$inputDirectories > 1)
        {  $hasChanged = 1;  };

    return $resolvedInputDirectory;
    };


#
#   Function: ResolveFile
#
#   Tests a relative path against a list of directories.  Adds one to the score of each base where there is a match.
#
#   Parameters:
#
#       relativePath - The relative file name to test.
#       possibleBases - An arrayref of bases to test it against.
#       possibleBaseScores - An arrayref of scores to adjust.  The score indexes should correspond to the base indexes.
#
sub ResolveFile #(relativePath, possibleBases, possibleBaseScores)
    {
    my ($self, $relativePath, $possibleBases, $possibleBaseScores) = @_;

    for (my $i = 0; $i < scalar @$possibleBases; $i++)
        {
        if (-e NaturalDocs::File->JoinPaths($possibleBases->[$i], $relativePath))
            {  $possibleBaseScores->[$i]++;  };
        };
    };


#
#   Function: LockUserTitleChanges
#
#   Detects if the user manually changed any file titles, and if so, automatically locks them with <MENU_FILE_NOAUTOTITLE>.
#
#   Parameters:
#
#       previousMenuFiles - A hashref of the files from the previous menu state.  The keys are the <FileNames>, and the values are
#                                    references to their <NaturalDocs::Menu::Entry> objects.
#
sub LockUserTitleChanges #(previousMenuFiles)
    {
    my ($self, $previousMenuFiles) = @_;

    my @groupStack = ( $menu );
    my $groupEntry;

    while (scalar @groupStack)
        {
        $groupEntry = pop @groupStack;

        foreach my $entry (@{$groupEntry->GroupContent()})
            {

            # If it's an unlocked file entry
            if ($entry->Type() == ::MENU_FILE() && ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE()) == 0)
                {
                my $previousEntry = $previousMenuFiles->{$entry->Target()};

                # If the previous entry was also unlocked and the titles are different, the user changed the title.  Automatically lock it.
                if (defined $previousEntry && ($previousEntry->Flags() & ::MENU_FILE_NOAUTOTITLE()) == 0 &&
                    $entry->Title() ne $previousEntry->Title())
                    {
                    $entry->SetFlags($entry->Flags() | ::MENU_FILE_NOAUTOTITLE());
                    $hasChanged = 1;
                    };
                }

            elsif ($entry->Type() == ::MENU_GROUP())
                {
                push @groupStack, $entry;
                };

            };
        };
    };


#
#   Function: FlagAutoTitleChanges
#
#   Finds which files have auto-titles that changed and flags their groups for updating with <MENU_GROUP_UPDATETITLES> and
#   <MENU_GROUP_UPDATEORDER>.
#
sub FlagAutoTitleChanges
    {
    my ($self) = @_;

    my @groupStack = ( $menu );
    my $groupEntry;

    while (scalar @groupStack)
        {
        $groupEntry = pop @groupStack;

        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_FILE() && ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE()) == 0 &&
                exists $defaultTitlesChanged{$entry->Target()})
                {
                $groupEntry->SetFlags($groupEntry->Flags() | ::MENU_GROUP_UPDATETITLES() | ::MENU_GROUP_UPDATEORDER());
                $hasChanged = 1;
                }
            elsif ($entry->Type() == ::MENU_GROUP())
                {
                push @groupStack, $entry;
                };
            };
        };
    };


#
#   Function: AutoPlaceNewFiles
#
#   Adds files to the menu that aren't already on it, attempting to guess where they belong.
#
#   New files are placed after a dummy <MENU_ENDOFORIGINAL> entry so that they don't affect the detected order.  Also, the
#   groups they're placed in get <MENU_GROUP_UPDATETITLES>, <MENU_GROUP_UPDATESTRUCTURE>, and
#   <MENU_GROUP_UPDATEORDER> flags.
#
#   Parameters:
#
#       filesInMenu - An existence hash of all the <FileNames> present in the menu.
#
sub AutoPlaceNewFiles #(fileInMenu)
    {
    my ($self, $filesInMenu) = @_;

    my $files = NaturalDocs::Project->FilesWithContent();

    my $directories;

    foreach my $file (keys %$files)
        {
        if (!exists $filesInMenu->{$file})
            {
            # This is done on demand because new files shouldn't be added very often, so this will save time.
            if (!defined $directories)
                {  $directories = $self->MatchDirectoriesAndGroups();  };

            my $targetGroup;
            my $fileDirectoryString = (NaturalDocs::File->SplitPath($file))[1];

            $targetGroup = $directories->{$fileDirectoryString};

            if (!defined $targetGroup)
                {
                # Okay, if there's no exact match, work our way down.

                my @fileDirectories = NaturalDocs::File->SplitDirectories($fileDirectoryString);

                do
                    {
                    pop @fileDirectories;
                    $targetGroup = $directories->{ NaturalDocs::File->JoinDirectories(@fileDirectories) };
                    }
                while (!defined $targetGroup && scalar @fileDirectories);

                if (!defined $targetGroup)
                    {  $targetGroup = $menu;  };
                };

            $targetGroup->MarkEndOfOriginal();
            $targetGroup->PushToGroup( NaturalDocs::Menu::Entry->New(::MENU_FILE(), undef, $file, undef) );

            $targetGroup->SetFlags( $targetGroup->Flags() | ::MENU_GROUP_UPDATETITLES() |
                                                 ::MENU_GROUP_UPDATESTRUCTURE() | ::MENU_GROUP_UPDATEORDER() );

            $hasChanged = 1;
            };
        };
    };


#
#   Function: MatchDirectoriesAndGroups
#
#   Determines which groups files in certain directories should be placed in.
#
#   Returns:
#
#       A hashref.  The keys are the directory names, and the values are references to the group objects they should be placed in.
#
#       This only repreesents directories that currently have files on the menu, so it shouldn't be assumed that every possible
#       directory will exist.  To match, you should first try to match the directory, and then strip the deepest directories one by
#       one until there's a match or there's none left.  If there's none left, use the root group <menu>.
#
sub MatchDirectoriesAndGroups
    {
    my ($self) = @_;

    # The keys are the directory names, and the values are hashrefs.  For the hashrefs, the keys are the group objects, and the
    # values are the number of files in them from that directory.  In other words,
    # $directories{$directory}->{$groupEntry} = $count;
    my %directories;
    # Note that we need to use Tie::RefHash to use references as keys.  Won't work otherwise.  Also, not every Perl distro comes
    # with Tie::RefHash::Nestable, so we can't rely on that.

    # We're using an index instead of pushing and popping because we want to save a list of the groups in the order they appear
    # to break ties.
    my @groups = ( $menu );
    my $groupIndex = 0;


    # Count the number of files in each group that appear in each directory.

    while ($groupIndex < scalar @groups)
        {
        my $groupEntry = $groups[$groupIndex];

        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_GROUP())
                {
                push @groups, $entry;
                }
            elsif ($entry->Type() == ::MENU_FILE())
                {
                my $directory = (NaturalDocs::File->SplitPath($entry->Target()))[1];

                if (!exists $directories{$directory})
                    {
                    my $subHash = { };
                    tie %$subHash, 'Tie::RefHash';
                    $directories{$directory} = $subHash;
                    };

                if (!exists $directories{$directory}->{$groupEntry})
                    {  $directories{$directory}->{$groupEntry} = 1;  }
                else
                    {  $directories{$directory}->{$groupEntry}++;  };
                };
            };

        $groupIndex++;
        };


    # Determine which group goes with which directory, breaking ties by using whichever group appears first.

    my $finalDirectories = { };

    while (my ($directory, $directoryGroups) = each %directories)
        {
        my $bestGroup;
        my $bestCount = 0;
        my %tiedGroups;  # Existence hash

        while (my ($group, $count) = each %$directoryGroups)
            {
            if ($count > $bestCount)
                {
                $bestGroup = $group;
                $bestCount = $count;
                %tiedGroups = ( );
                }
            elsif ($count == $bestCount)
                {
                $tiedGroups{$group} = 1;
                };
            };

        # Break ties.
        if (scalar keys %tiedGroups)
            {
            $tiedGroups{$bestGroup} = 1;

            foreach my $group (@groups)
                {
                if (exists $tiedGroups{$group})
                    {
                    $bestGroup = $group;
                    last;
                    };
                };
            };


        $finalDirectories->{$directory} = $bestGroup;
        };


    return $finalDirectories;
    };


#
#   Function: RemoveDeadFiles
#
#   Removes files from the menu that no longer exist or no longer have Natural Docs content.
#
#   Returns:
#
#       The number of file entries removed.
#
sub RemoveDeadFiles
    {
    my ($self) = @_;

    my @groupStack = ( $menu );
    my $numberRemoved = 0;

    my $filesWithContent = NaturalDocs::Project->FilesWithContent();

    while (scalar @groupStack)
        {
        my $groupEntry = pop @groupStack;
        my $groupContent = $groupEntry->GroupContent();

        my $index = 0;
        while ($index < scalar @$groupContent)
            {
            if ($groupContent->[$index]->Type() == ::MENU_FILE() &&
                !exists $filesWithContent->{ $groupContent->[$index]->Target() } )
                {
                $groupEntry->DeleteFromGroup($index);

                $groupEntry->SetFlags( $groupEntry->Flags() | ::MENU_GROUP_UPDATETITLES() |
                                                   ::MENU_GROUP_UPDATESTRUCTURE() );
                $numberRemoved++;
                $hasChanged = 1;
                }

            elsif ($groupContent->[$index]->Type() == ::MENU_GROUP())
                {
                push @groupStack, $groupContent->[$index];
                $index++;
                }

            else
                {  $index++;  };
            };
        };

    return $numberRemoved;
    };


#
#   Function: BanAndUnbanIndexes
#
#   Adjusts the indexes that are banned depending on if the user added or deleted any.
#
sub BanAndUnbanIndexes
    {
    my ($self) = @_;

    # Unban any indexes that are present, meaning the user added them back manually without deleting the ban.
    foreach my $index (keys %indexes)
        {  delete $bannedIndexes{$index};  };

    # Ban any indexes that were in the previous menu but not the current, meaning the user manually deleted them.  However,
    # don't do this if the topic isn't indexable, meaning they changed the topic type rather than the menu.
    foreach my $index (keys %previousIndexes)
        {
        if (!exists $indexes{$index} && NaturalDocs::Topics->TypeInfo($index)->Index())
            {  $bannedIndexes{$index} = 1;  };
        };
    };


#
#   Function: AddAndRemoveIndexes
#
#   Automatically adds and removes index entries on the menu as necessary.  <DetectIndexGroups()> should be called
#   beforehand.
#
sub AddAndRemoveIndexes
    {
    my ($self) = @_;

    my %validIndexes;
    my @allIndexes = NaturalDocs::Topics->AllIndexableTypes();

    foreach my $index (@allIndexes)
        {
        # Strip the banned indexes first so it's potentially less work for SymbolTable.
        if (!exists $bannedIndexes{$index})
            {  $validIndexes{$index} = 1;  };
        };

    %validIndexes = %{NaturalDocs::SymbolTable->HasIndexes(\%validIndexes)};


    # Delete dead indexes and find the best index group.

    my @groupStack = ( $menu );

    my $bestIndexGroup;
    my $bestIndexCount = 0;

    while (scalar @groupStack)
        {
        my $currentGroup = pop @groupStack;
        my $index = 0;

        my $currentIndexCount = 0;

        while ($index < scalar @{$currentGroup->GroupContent()})
            {
            my $entry = $currentGroup->GroupContent()->[$index];

            if ($entry->Type() == ::MENU_INDEX())
                {
                $currentIndexCount++;

                if ($currentIndexCount > $bestIndexCount)
                    {
                    $bestIndexCount = $currentIndexCount;
                    $bestIndexGroup = $currentGroup;
                    };

                # Remove it if it's dead.

                if (!exists $validIndexes{ $entry->Target() })
                    {
                    $currentGroup->DeleteFromGroup($index);
                    delete $indexes{ $entry->Target() };
                    $hasChanged = 1;
                    }
                else
                    {  $index++;  };
                }

            else
                {
                if ($entry->Type() == ::MENU_GROUP())
                    {  push @groupStack, $entry;  };

                $index++;
                };
            };
        };


    # Now add the new indexes.

    foreach my $index (keys %indexes)
        {  delete $validIndexes{$index};  };

    if (scalar keys %validIndexes)
        {
        # Add a group if there are no indexes at all.

        if ($bestIndexCount == 0)
            {
            $menu->MarkEndOfOriginal();

            my $newIndexGroup = NaturalDocs::Menu::Entry->New(::MENU_GROUP(), 'Index', undef,
                                                                                              ::MENU_GROUP_ISINDEXGROUP());
            $menu->PushToGroup($newIndexGroup);

            $bestIndexGroup = $newIndexGroup;
            $menu->SetFlags( $menu->Flags() | ::MENU_GROUP_UPDATEORDER() | ::MENU_GROUP_UPDATESTRUCTURE() );
            };

        # Add the new indexes.

        $bestIndexGroup->MarkEndOfOriginal();
        my $isIndexGroup = $bestIndexGroup->Flags() & ::MENU_GROUP_ISINDEXGROUP();

        foreach my $index (keys %validIndexes)
            {
            my $title;

            if ($isIndexGroup)
                {
                if ($index eq ::TOPIC_GENERAL())
                    {  $title = 'Everything';  }
                else
                    {  $title = NaturalDocs::Topics->NameOfType($index, 1);  };
                }
            else
                {
                $title = NaturalDocs::Topics->NameOfType($index) . ' Index';
                };

            my $newEntry = NaturalDocs::Menu::Entry->New(::MENU_INDEX(), $title, $index, undef);
            $bestIndexGroup->PushToGroup($newEntry);

            $indexes{$index} = 1;
            };

        $bestIndexGroup->SetFlags( $bestIndexGroup->Flags() |
                                                   ::MENU_GROUP_UPDATEORDER() | ::MENU_GROUP_UPDATESTRUCTURE() );
        $hasChanged = 1;
        };
    };


#
#   Function: RemoveDeadGroups
#
#   Removes groups with less than two entries.  It will always remove empty groups, and it will remove groups with one entry if it
#   has the <MENU_GROUP_UPDATESTRUCTURE> flag.
#
sub RemoveDeadGroups
    {
    my ($self) = @_;

    my $index = 0;

    while ($index < scalar @{$menu->GroupContent()})
        {
        my $entry = $menu->GroupContent()->[$index];

        if ($entry->Type() == ::MENU_GROUP())
            {
            my $removed = $self->RemoveIfDead($entry, $menu, $index);

            if (!$removed)
                {  $index++;  };
            }
        else
            {  $index++;  };
        };
    };


#
#   Function: RemoveIfDead
#
#   Checks a group and all its sub-groups for life and remove any that are dead.  Empty groups are removed, and groups with one
#   entry and the <MENU_GROUP_UPDATESTRUCTURE> flag have their entry moved to the parent group.
#
#   Parameters:
#
#       groupEntry - The group to check for possible deletion.
#       parentGroupEntry - The parent group to move the single entry to if necessary.
#       parentGroupIndex - The index of the group in its parent.
#
#   Returns:
#
#       Whether the group was removed or not.
#
sub RemoveIfDead #(groupEntry, parentGroupEntry, parentGroupIndex)
    {
    my ($self, $groupEntry, $parentGroupEntry, $parentGroupIndex) = @_;


    # Do all sub-groups first, since their deletions will affect our UPDATESTRUCTURE flag and content count.

    my $index = 0;
    while ($index < scalar @{$groupEntry->GroupContent()})
        {
        my $entry = $groupEntry->GroupContent()->[$index];

        if ($entry->Type() == ::MENU_GROUP())
            {
            my $removed = $self->RemoveIfDead($entry, $groupEntry, $index);

            if (!$removed)
                {  $index++;  };
            }
        else
            {  $index++;  };
        };


    # Now check ourself.

    my $count = scalar @{$groupEntry->GroupContent()};
    if ($groupEntry->Flags() & ::MENU_GROUP_HASENDOFORIGINAL())
        {  $count--;  };

    if ($count == 0)
        {
        $parentGroupEntry->DeleteFromGroup($parentGroupIndex);

        $parentGroupEntry->SetFlags( $parentGroupEntry->Flags() | ::MENU_GROUP_UPDATESTRUCTURE() );

        $hasChanged = 1;
        return 1;
        }
    elsif ($count == 1 && ($groupEntry->Flags() & ::MENU_GROUP_UPDATESTRUCTURE()) )
        {
        my $onlyEntry = $groupEntry->GroupContent()->[0];
        if ($onlyEntry->Type() == ::MENU_ENDOFORIGINAL())
            {  $onlyEntry = $groupEntry->GroupContent()->[1];  };

        $parentGroupEntry->DeleteFromGroup($parentGroupIndex);

        $parentGroupEntry->MarkEndOfOriginal();
        $parentGroupEntry->PushToGroup($onlyEntry);

        $parentGroupEntry->SetFlags( $parentGroupEntry->Flags() | ::MENU_GROUP_UPDATETITLES() |
                                                     ::MENU_GROUP_UPDATEORDER() | ::MENU_GROUP_UPDATESTRUCTURE() );

        $hasChanged = 1;
        return 1;
        }
    else
        {  return undef;  };
    };


#
#   Function: DetectIndexGroups
#
#   Finds groups that are primarily used for indexes and gives them the <MENU_GROUP_ISINDEXGROUP> flag.
#
sub DetectIndexGroups
    {
    my ($self) = @_;

    my @groupStack = ( $menu );

    while (scalar @groupStack)
        {
        my $groupEntry = pop @groupStack;

        my $isIndexGroup = -1;  # -1: Can't tell yet.  0: Can't be an index group.  1: Is an index group so far.

        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_INDEX())
                {
                if ($isIndexGroup == -1)
                    {  $isIndexGroup = 1;  };
                }

            # Text is tolerated, but it still needs at least one index entry.
            elsif ($entry->Type() != ::MENU_TEXT())
                {
                $isIndexGroup = 0;

                if ($entry->Type() == ::MENU_GROUP())
                    {  push @groupStack, $entry;  };
                };
            };

        if ($isIndexGroup == 1)
            {
            $groupEntry->SetFlags( $groupEntry->Flags() | ::MENU_GROUP_ISINDEXGROUP() );
            };
        };
    };


#
#   Function: CreateDirectorySubGroups
#
#   Where possible, creates sub-groups based on directories for any long groups that have <MENU_GROUP_UPDATESTRUCTURE>
#   set.  Clears the flag afterwards on groups that are short enough to not need any more sub-groups, but leaves it for the rest.
#
sub CreateDirectorySubGroups
    {
    my ($self) = @_;

    my @groupStack = ( $menu );

    foreach my $groupEntry (@groupStack)
        {
        if ($groupEntry->Flags() & ::MENU_GROUP_UPDATESTRUCTURE())
            {
            # Count the number of files.

            my $fileCount = 0;

            foreach my $entry (@{$groupEntry->GroupContent()})
                {
                if ($entry->Type() == ::MENU_FILE())
                    {  $fileCount++;  };
                };


            if ($fileCount > MAXFILESINGROUP)
                {
                my @sharedDirectories = $self->SharedDirectoriesOf($groupEntry);
                my $unsharedIndex = scalar @sharedDirectories;

                # The keys are the first directory entries after the shared ones, and the values are the number of files that are in
                # that directory.  Files that don't have subdirectories after the shared directories aren't included because they shouldn't
                # be put in a subgroup.
                my %directoryCounts;

                foreach my $entry (@{$groupEntry->GroupContent()})
                    {
                    if ($entry->Type() == ::MENU_FILE())
                        {
                        my @entryDirectories = NaturalDocs::File->SplitDirectories( (NaturalDocs::File->SplitPath($entry->Target()))[1] );

                        if (scalar @entryDirectories > $unsharedIndex)
                            {
                            my $unsharedDirectory = $entryDirectories[$unsharedIndex];

                            if (!exists $directoryCounts{$unsharedDirectory})
                                {  $directoryCounts{$unsharedDirectory} = 1;  }
                            else
                                {  $directoryCounts{$unsharedDirectory}++;  };
                            };
                        };
                    };


                # Now create the subgroups.

                # The keys are the first directory entries after the shared ones, and the values are the groups for those files to be
                # put in.  There will only be entries for the groups with at least MINFILESINNEWGROUP files.
                my %directoryGroups;

                while (my ($directory, $count) = each %directoryCounts)
                    {
                    if ($count >= MINFILESINNEWGROUP)
                        {
                        my $newGroup = NaturalDocs::Menu::Entry->New( ::MENU_GROUP(), ucfirst($directory), undef,
                                                                                                   ::MENU_GROUP_UPDATETITLES() |
                                                                                                   ::MENU_GROUP_UPDATEORDER() );

                        if ($count > MAXFILESINGROUP)
                            {  $newGroup->SetFlags( $newGroup->Flags() | ::MENU_GROUP_UPDATESTRUCTURE());  };

                        $groupEntry->MarkEndOfOriginal();
                        push @{$groupEntry->GroupContent()}, $newGroup;

                        $directoryGroups{$directory} = $newGroup;
                        $fileCount -= $count;
                        };
                    };


                # Now fill the subgroups.

                if (scalar keys %directoryGroups)
                    {
                    my $afterOriginal;
                    my $index = 0;

                    while ($index < scalar @{$groupEntry->GroupContent()})
                        {
                        my $entry = $groupEntry->GroupContent()->[$index];

                        if ($entry->Type() == ::MENU_FILE())
                            {
                            my @entryDirectories =
                                NaturalDocs::File->SplitDirectories( (NaturalDocs::File->SplitPath($entry->Target()))[1] );

                            my $unsharedDirectory = $entryDirectories[$unsharedIndex];

                            if (exists $directoryGroups{$unsharedDirectory})
                                {
                                my $targetGroup = $directoryGroups{$unsharedDirectory};

                                if ($afterOriginal)
                                    {  $targetGroup->MarkEndOfOriginal();  };
                                $targetGroup->PushToGroup($entry);

                                $groupEntry->DeleteFromGroup($index);
                                }
                            else
                                {  $index++;  };
                            }

                        elsif ($entry->Type() == ::MENU_ENDOFORIGINAL())
                            {
                            $afterOriginal = 1;
                            $index++;
                            }

                        elsif ($entry->Type() == ::MENU_GROUP())
                            {
                            # See if we need to relocate this group.

                            my @groupDirectories = $self->SharedDirectoriesOf($entry);

                            # The group's shared directories must be at least two levels deeper than the current.  If the first level deeper
                            # is a new group, move it there because it's a subdirectory of that one.
                            if (scalar @groupDirectories - scalar @sharedDirectories >= 2)
                                {
                                my $unsharedDirectory = $groupDirectories[$unsharedIndex];

                                if (exists $directoryGroups{$unsharedDirectory} &&
                                    $directoryGroups{$unsharedDirectory} != $entry)
                                    {
                                    my $targetGroup = $directoryGroups{$unsharedDirectory};

                                    if ($afterOriginal)
                                        {  $targetGroup->MarkEndOfOriginal();  };
                                    $targetGroup->PushToGroup($entry);

                                    $groupEntry->DeleteFromGroup($index);

                                    # We need to retitle the group if it has the name of the unshared directory.

                                    my $oldTitle = $entry->Title();
                                    $oldTitle =~ s/ +//g;
                                    $unsharedDirectory =~ s/ +//g;

                                    if (lc($oldTitle) eq lc($unsharedDirectory))
                                        {
                                        $entry->SetTitle($groupDirectories[$unsharedIndex + 1]);
                                        };
                                    }
                                else
                                    {  $index++;  };
                                }
                            else
                                {  $index++;  };
                            }

                        else
                            {  $index++;  };
                        };

                    $hasChanged = 1;

                    if ($fileCount <= MAXFILESINGROUP)
                        {  $groupEntry->SetFlags( $groupEntry->Flags() & ~::MENU_GROUP_UPDATESTRUCTURE() );  };

                    $groupEntry->SetFlags( $groupEntry->Flags() | ::MENU_GROUP_UPDATETITLES() |
                                                                                         ::MENU_GROUP_UPDATEORDER() );
                    };

                };  # If group has >MAXFILESINGROUP files
            };  # If group has UPDATESTRUCTURE


        # Okay, now go through all the subgroups.  We do this after the above so that newly created groups can get subgrouped
        # further.

        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_GROUP())
                {  push @groupStack, $entry;  };
            };

        };  # For each group entry
    };


#
#   Function: DetectOrder
#
#   Detects the order of the entries in all groups that have the <MENU_GROUP_UPDATEORDER> flag set.  Will set one of the
#   <MENU_GROUP_FILESSORTED>, <MENU_GROUP_FILESANDGROUPSSORTED>, <MENU_GROUP_EVERYTHINGSORTED>, or
#   <MENU_GROUP_UNSORTED> flags.  It will always go for the most comprehensive sort possible, so if a group only has one
#   entry, it will be flagged as <MENU_GROUP_EVERYTHINGSORTED>.
#
#   <DetectIndexGroups()> should be called beforehand, as the <MENU_GROUP_ISINDEXGROUP> flag affects how the order is
#   detected.
#
#   The sort detection stops if it reaches a <MENU_ENDOFORIGINAL> entry, so new entries can be added to the end while still
#   allowing the original sort to be detected.
#
#   Parameters:
#
#       forceAll - If set, the order will be detected for all groups regardless of whether <MENU_GROUP_UPDATEORDER> is set.
#
sub DetectOrder #(forceAll)
    {
    my ($self, $forceAll) = @_;
    my @groupStack = ( $menu );

    while (scalar @groupStack)
        {
        my $groupEntry = pop @groupStack;
        my $index = 0;


        # First detect the sort.

        if ($forceAll || ($groupEntry->Flags() & ::MENU_GROUP_UPDATEORDER()) )
            {
            my $order = ::MENU_GROUP_EVERYTHINGSORTED();

            my $lastFile;
            my $lastFileOrGroup;

            while ($index < scalar @{$groupEntry->GroupContent()} &&
                     $groupEntry->GroupContent()->[$index]->Type() != ::MENU_ENDOFORIGINAL() &&
                     $order != ::MENU_GROUP_UNSORTED())
                {
                my $entry = $groupEntry->GroupContent()->[$index];


                # Ignore the last entry if it's an index group.  We don't want it to affect the sort.

                if ($index + 1 == scalar @{$groupEntry->GroupContent()} &&
                    $entry->Type() == ::MENU_GROUP() && ($entry->Flags() & ::MENU_GROUP_ISINDEXGROUP()) )
                    {
                    # Ignore.

                    # This is an awkward code construct, basically working towards an else instead of using an if, but the code just gets
                    # too hard to read otherwise.  The compiled code should work out to roughly the same thing anyway.
                    }


                # Ignore the first entry if it's the general index in an index group.  We don't want it to affect the sort.

                elsif ($index == 0 && ($groupEntry->Flags() & ::MENU_GROUP_ISINDEXGROUP()) &&
                        $entry->Type() == ::MENU_INDEX() && $entry->Target() eq ::TOPIC_GENERAL() )
                    {
                    # Ignore.
                    }


                # Degenerate the sort.

                else
                    {

                    if ($order == ::MENU_GROUP_EVERYTHINGSORTED() && $index > 0 &&
                        ::StringCompare($entry->Title(), $groupEntry->GroupContent()->[$index - 1]->Title()) < 0)
                        {  $order = ::MENU_GROUP_FILESANDGROUPSSORTED();  };

                    if ($order == ::MENU_GROUP_FILESANDGROUPSSORTED() &&
                        ($entry->Type() == ::MENU_FILE() || $entry->Type() == ::MENU_GROUP()) &&
                        defined $lastFileOrGroup && ::StringCompare($entry->Title(), $lastFileOrGroup->Title()) < 0)
                        {  $order = ::MENU_GROUP_FILESSORTED();  };

                    if ($order == ::MENU_GROUP_FILESSORTED() &&
                        $entry->Type() == ::MENU_FILE() && defined $lastFile &&
                        ::StringCompare($entry->Title(), $lastFile->Title()) < 0)
                        {  $order = ::MENU_GROUP_UNSORTED();  };

                    };


                # Set the lastX parameters for comparison and add sub-groups to the stack.

                if ($entry->Type() == ::MENU_FILE())
                    {
                    $lastFile = $entry;
                    $lastFileOrGroup = $entry;
                    }
                elsif ($entry->Type() == ::MENU_GROUP())
                    {
                    $lastFileOrGroup = $entry;
                    push @groupStack, $entry;
                    };

                $index++;
                };

            $groupEntry->SetFlags($groupEntry->Flags() | $order);
            };


        # Find any subgroups in the remaining entries.

        while ($index < scalar @{$groupEntry->GroupContent()})
            {
            my $entry = $groupEntry->GroupContent()->[$index];

            if ($entry->Type() == ::MENU_GROUP())
                {  push @groupStack, $entry;  };

            $index++;
            };
        };
    };


#
#   Function: GenerateAutoFileTitles
#
#   Creates titles for the unlocked file entries in all groups that have the <MENU_GROUP_UPDATETITLES> flag set.  It clears the
#   flag afterwards so it can be used efficiently for multiple sweeps.
#
#   Parameters:
#
#       forceAll - If set, forces all the unlocked file titles to update regardless of whether the group has the
#                     <MENU_GROUP_UPDATETITLES> flag set.
#
sub GenerateAutoFileTitles #(forceAll)
    {
    my ($self, $forceAll) = @_;

    my @groupStack = ( $menu );

    while (scalar @groupStack)
        {
        my $groupEntry = pop @groupStack;

        if ($forceAll || ($groupEntry->Flags() & ::MENU_GROUP_UPDATETITLES()) )
            {
            # Find common prefixes and paths to strip from the default menu titles.

            my @sharedDirectories = $self->SharedDirectoriesOf($groupEntry);
            my $noSharedDirectories = (scalar @sharedDirectories == 0);

            my @sharedPrefixes;
            my $noSharedPrefixes;

            foreach my $entry (@{$groupEntry->GroupContent()})
                {
                if ($entry->Type() == ::MENU_FILE())
                    {
                    # Find the common prefixes among all file entries that are unlocked and don't use the file name as their default title.

                    my $defaultTitle = NaturalDocs::Project->DefaultMenuTitleOf($entry->Target());

                    if (!$noSharedPrefixes && ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE()) == 0 &&
                        $defaultTitle ne $entry->Target())
                        {
                        # If the filename is part of the title, separate it off so no part of it gets included as a common prefix.  This would
                        # happen if there's a group with only one file in it (Project.h => h) or only files that differ by extension
                        # (Project.h, Project.cpp => h, cpp) and people labeled them manually (// File: Project.h).
                        my $filename = (NaturalDocs::File->SplitPath($entry->Target()))[2];
                        my $filenamePart;

                        if ( length $defaultTitle >= length $filename &&
                             lc(substr($defaultTitle, 0 - length($filename))) eq lc($filename) )
                            {
                            $filenamePart = substr($defaultTitle, 0 - length($filename));
                            $defaultTitle = substr($defaultTitle, 0, 0 - length($filename));
                            };


                        my @entryPrefixes = split(/(\.|::|->)/, $defaultTitle);

                        # Remove potential leading undef/empty string.
                        if (!length $entryPrefixes[0])
                            {  shift @entryPrefixes;  };

                        # Remove last entry.  Something has to exist for the title.  If we already separated off the filename, that will be
                        # it instead.
                        if (!$filenamePart)
                            {  pop @entryPrefixes;  };

                        if (!scalar @entryPrefixes)
                            {  $noSharedPrefixes = 1;  }
                        elsif (!scalar @sharedPrefixes)
                            {  @sharedPrefixes = @entryPrefixes;  }
                        elsif ($entryPrefixes[0] ne $sharedPrefixes[0])
                            {  $noSharedPrefixes = 1;  }

                        # If both arrays have entries, and the first is shared...
                        else
                            {
                            my $index = 1;

                            while ($index < scalar @sharedPrefixes && $entryPrefixes[$index] eq $sharedPrefixes[$index])
                                {  $index++;  };

                            if ($index < scalar @sharedPrefixes)
                                {  splice(@sharedPrefixes, $index);  };
                            };
                        };

                    };  # if entry is MENU_FILE
                };  # foreach entry in group content.


            if (!scalar @sharedPrefixes)
                {  $noSharedPrefixes = 1;  };


            # Update all the menu titles of unlocked file entries.

            foreach my $entry (@{$groupEntry->GroupContent()})
                {
                if ($entry->Type() == ::MENU_FILE() && ($entry->Flags() & ::MENU_FILE_NOAUTOTITLE()) == 0)
                    {
                    my $title = NaturalDocs::Project->DefaultMenuTitleOf($entry->Target());

                    if ($title eq $entry->Target())
                        {
                        my ($volume, $directoryString, $file) = NaturalDocs::File->SplitPath($entry->Target());
                        my @directories = NaturalDocs::File->SplitDirectories($directoryString);

                        if (!$noSharedDirectories)
                            {  splice(@directories, 0, scalar @sharedDirectories);  };

                        # directory\...\directory\file.ext

                        if (scalar @directories > 2)
                            {  @directories = ( $directories[0], '...', $directories[-1] );  };

                        $directoryString = NaturalDocs::File->JoinDirectories(@directories);
                        $title = NaturalDocs::File->JoinPaths($directoryString, $file);
                        }

                    else
                        {
                        my $filename = (NaturalDocs::File->SplitPath($entry->Target()))[2];
                        my $filenamePart;

                        if ( length $title >= length $filename &&
                             lc(substr($title, 0 - length($filename))) eq lc($filename) )
                            {
                            $filenamePart = substr($title, 0 - length($filename));
                            $title = substr($title, 0, 0 - length($filename));
                            };

                        my @segments = split(/(::|\.|->)/, $title);
                        if (!length $segments[0])
                            {  shift @segments;  };

                        if ($filenamePart)
                            {  push @segments, $filenamePart;  };

                        if (!$noSharedPrefixes)
                            {  splice(@segments, 0, scalar @sharedPrefixes);  };

                        # package...package::target

                        if (scalar @segments > 5)
                            {  splice(@segments, 1, scalar @segments - 4, '...');  };

                        $title = join('', @segments);
                        };

                    $entry->SetTitle($title);
                    };  # If entry is an unlocked file
                };  # Foreach entry

            $groupEntry->SetFlags( $groupEntry->Flags() & ~::MENU_GROUP_UPDATETITLES() );

            };  # If updating group titles

        # Now find any subgroups.
        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_GROUP())
                {  push @groupStack, $entry;  };
            };
        };

    };


#
#   Function: ResortGroups
#
#   Resorts all groups that have <MENU_GROUP_UPDATEORDER> set.  Assumes <DetectOrder()> and <GenerateAutoFileTitles()>
#   have already been called.  Will clear the flag and any <MENU_ENDOFORIGINAL> entries on reordered groups.
#
#   Parameters:
#
#       forceAll - If set, resorts all groups regardless of whether <MENU_GROUP_UPDATEORDER> is set.
#
sub ResortGroups #(forceAll)
    {
    my ($self, $forceAll) = @_;
    my @groupStack = ( $menu );

    while (scalar @groupStack)
        {
        my $groupEntry = pop @groupStack;

        if ($forceAll || ($groupEntry->Flags() & ::MENU_GROUP_UPDATEORDER()) )
            {
            my $newEntriesIndex;


            # Strip the ENDOFORIGINAL.

            if ($groupEntry->Flags() & ::MENU_GROUP_HASENDOFORIGINAL())
                {
                $newEntriesIndex = 0;

                while ($newEntriesIndex < scalar @{$groupEntry->GroupContent()} &&
                         $groupEntry->GroupContent()->[$newEntriesIndex]->Type() != ::MENU_ENDOFORIGINAL() )
                    {  $newEntriesIndex++;  };

                $groupEntry->DeleteFromGroup($newEntriesIndex);

                $groupEntry->SetFlags( $groupEntry->Flags() & ~::MENU_GROUP_HASENDOFORIGINAL() );
                }
            else
                {  $newEntriesIndex = -1;  };


            # Strip the exceptions.

            my $trailingIndexGroup;
            my $leadingGeneralIndex;

            if ( ($groupEntry->Flags() & ::MENU_GROUP_ISINDEXGROUP()) &&
                 $groupEntry->GroupContent()->[0]->Type() == ::MENU_INDEX() &&
                 $groupEntry->GroupContent()->[0]->Target() eq ::TOPIC_GENERAL() )
                {
                $leadingGeneralIndex = shift @{$groupEntry->GroupContent()};
                if ($newEntriesIndex != -1)
                    {  $newEntriesIndex--;  };
                }

            elsif (scalar @{$groupEntry->GroupContent()} && $newEntriesIndex != 0)
                {
                my $lastIndex;

                if ($newEntriesIndex != -1)
                    {  $lastIndex = $newEntriesIndex - 1;  }
                else
                    {  $lastIndex = scalar @{$groupEntry->GroupContent()} - 1;  };

                if ($groupEntry->GroupContent()->[$lastIndex]->Type() == ::MENU_GROUP() &&
                    ( $groupEntry->GroupContent()->[$lastIndex]->Flags() & ::MENU_GROUP_ISINDEXGROUP() ) )
                    {
                    $trailingIndexGroup = $groupEntry->GroupContent()->[$lastIndex];
                    $groupEntry->DeleteFromGroup($lastIndex);

                    if ($newEntriesIndex != -1)
                        {  $newEntriesIndex++;  };
                    };
                };


            # If there weren't already exceptions, strip them from the new entries.

            if ( (!defined $trailingIndexGroup || !defined $leadingGeneralIndex) && $newEntriesIndex != -1)
                {
                my $index = $newEntriesIndex;

                while ($index < scalar @{$groupEntry->GroupContent()})
                    {
                    my $entry = $groupEntry->GroupContent()->[$index];

                    if (!defined $trailingIndexGroup &&
                        $entry->Type() == ::MENU_GROUP() && ($entry->Flags() & ::MENU_GROUP_ISINDEXGROUP()) )
                        {
                        $trailingIndexGroup = $entry;
                        $groupEntry->DeleteFromGroup($index);
                        }
                    elsif (!defined $leadingGeneralIndex && ($groupEntry->Flags() & ::MENU_GROUP_ISINDEXGROUP()) &&
                            $entry->Type() == ::MENU_INDEX() && !defined $entry->Target())
                        {
                        $leadingGeneralIndex = $entry;
                        $groupEntry->DeleteFromGroup($index);
                        }
                    else
                        {  $index++;  };
                    };
                };


            # If there's no order, we still want to sort the new additions.

            if ($groupEntry->Flags() & ::MENU_GROUP_UNSORTED())
                {
                if ($newEntriesIndex != -1)
                    {
                    my @newEntries =
                        @{$groupEntry->GroupContent()}[$newEntriesIndex..scalar @{$groupEntry->GroupContent()} - 1];

                    @newEntries = sort { $self->CompareEntries($a, $b) } @newEntries;

                    foreach my $newEntry (@newEntries)
                        {
                        $groupEntry->GroupContent()->[$newEntriesIndex] = $newEntry;
                        $newEntriesIndex++;
                        };
                    };
                }

            elsif ($groupEntry->Flags() & ::MENU_GROUP_EVERYTHINGSORTED())
                {
                @{$groupEntry->GroupContent()} = sort { $self->CompareEntries($a, $b) } @{$groupEntry->GroupContent()};
                }

            elsif ( ($groupEntry->Flags() & ::MENU_GROUP_FILESSORTED()) ||
                     ($groupEntry->Flags() & ::MENU_GROUP_FILESANDGROUPSSORTED()) )
                {
                my $groupContent = $groupEntry->GroupContent();
                my @newEntries;

                if ($newEntriesIndex != -1)
                    {  @newEntries = splice( @$groupContent, $newEntriesIndex );  };


                # First resort the existing entries.

                # A couple of support functions.  They're defined here instead of spun off into their own functions because they're only
                # used here and to make them general we would need to add support for the other sort options.

                sub IsIncludedInSort #(groupEntry, entry)
                    {
                    my ($self, $groupEntry, $entry) = @_;

                    return ($entry->Type() == ::MENU_FILE() ||
                                ( $entry->Type() == ::MENU_GROUP() &&
                                    ($groupEntry->Flags() & ::MENU_GROUP_FILESANDGROUPSSORTED()) ) );
                    };

                sub IsSorted #(groupEntry)
                    {
                    my ($self, $groupEntry) = @_;
                    my $lastApplicable;

                    foreach my $entry (@{$groupEntry->GroupContent()})
                        {
                        # If the entry is applicable to the sort order...
                        if ($self->IsIncludedInSort($groupEntry, $entry))
                            {
                            if (defined $lastApplicable)
                                {
                                if ($self->CompareEntries($entry, $lastApplicable) < 0)
                                    {  return undef;  };
                                };

                            $lastApplicable = $entry;
                            };
                        };

                    return 1;
                    };


                # There's a good chance it's still sorted.  They should only become unsorted if an auto-title changes.
                if (!$self->IsSorted($groupEntry))
                    {
                    # Crap.  Okay, method one is to sort each group of continuous sortable elements.  There's a possibility that doing
                    # this will cause the whole to become sorted again.  We try this first, even though it isn't guaranteed to succeed,
                    # because it will restore the sort without moving any unsortable entries.

                    # Copy it because we'll need the original if this fails.
                    my @originalGroupContent = @$groupContent;

                    my $index = 0;
                    my $startSortable = 0;

                    while (1)
                        {
                        # If index is on an unsortable entry or the end of the array...
                        if ($index == scalar @$groupContent || !$self->IsIncludedInSort($groupEntry, $groupContent->[$index]))
                            {
                            # If we have at least two sortable entries...
                            if ($index - $startSortable >= 2)
                                {
                                # Sort them.
                                my @sortableEntries = @{$groupContent}[$startSortable .. $index - 1];
                                @sortableEntries = sort { $self->CompareEntries($a, $b) } @sortableEntries;
                                foreach my $sortableEntry (@sortableEntries)
                                    {
                                    $groupContent->[$startSortable] = $sortableEntry;
                                    $startSortable++;
                                    };
                                };

                            if ($index == scalar @$groupContent)
                                {  last;  };

                            $startSortable = $index + 1;
                            };

                        $index++;
                        };

                    if (!$self->IsSorted($groupEntry))
                        {
                        # Crap crap.  Okay, now we do a full sort but with potential damage to the original structure.  Each unsortable
                        # element is locked to the next sortable element.  We sort the sortable elements, bringing all the unsortable
                        # pieces with them.

                        my @pieces = ( [ ] );
                        my $currentPiece = $pieces[0];

                        foreach my $entry (@originalGroupContent)
                            {
                            push @$currentPiece, $entry;

                            # If the entry is sortable...
                            if ($self->IsIncludedInSort($groupEntry, $entry))
                                {
                                $currentPiece = [ ];
                                push @pieces, $currentPiece;
                                };
                            };

                        my $lastUnsortablePiece;

                        # If the last entry was sortable, we'll have an empty piece at the end.  Drop it.
                        if (scalar @{$pieces[-1]} == 0)
                            {  pop @pieces;  }

                        # If the last entry wasn't sortable, the last piece won't end with a sortable element.  Save it, but remove it
                        # from the list.
                        else
                            {  $lastUnsortablePiece = pop @pieces;  };

                        # Sort the list.
                        @pieces = sort { $self->CompareEntries( $a->[-1], $b->[-1] ) } @pieces;

                        # Copy it back to the original.
                        if (defined $lastUnsortablePiece)
                            {  push @pieces, $lastUnsortablePiece;  };

                        my $index = 0;

                        foreach my $piece (@pieces)
                            {
                            foreach my $entry (@{$piece})
                                {
                                $groupEntry->GroupContent()->[$index] = $entry;
                                $index++;
                                };
                            };
                        };
                    };


                # Okay, the orginal entries are sorted now.  Sort the new entries and apply.

                if (scalar @newEntries)
                    {
                    @newEntries = sort { $self->CompareEntries($a, $b) } @newEntries;
                    my @originalEntries = @$groupContent;
                    @$groupContent = ( );

                    while (1)
                        {
                        while (scalar @originalEntries && !$self->IsIncludedInSort($groupEntry, $originalEntries[0]))
                            {  push @$groupContent, (shift @originalEntries);  };

                        if (!scalar @originalEntries || !scalar @newEntries)
                            {  last;  };

                        while (scalar @newEntries && $self->CompareEntries($newEntries[0], $originalEntries[0]) < 0)
                            {  push @$groupContent, (shift @newEntries);  };

                        push @$groupContent, (shift @originalEntries);

                        if (!scalar @originalEntries || !scalar @newEntries)
                            {  last;  };
                        };

                    if (scalar @originalEntries)
                        {  push @$groupContent, @originalEntries;  }
                    elsif (scalar @newEntries)
                        {  push @$groupContent, @newEntries;  };
                    };
                };


            # Now re-add the exceptions.

            if (defined $leadingGeneralIndex)
                {
                unshift @{$groupEntry->GroupContent()}, $leadingGeneralIndex;
                };

            if (defined $trailingIndexGroup)
                {
                $groupEntry->PushToGroup($trailingIndexGroup);
                };

            };

        foreach my $entry (@{$groupEntry->GroupContent()})
            {
            if ($entry->Type() == ::MENU_GROUP())
                {  push @groupStack, $entry;  };
            };
        };
    };


#
#   Function: CompareEntries
#
#   A comparison function for use in sorting.  Compares the two entries by their titles with <StringCompare()>, but in the case
#   of a tie, puts <MENU_FILE> entries above <MENU_GROUP> entries.
#
sub CompareEntries #(a, b)
    {
    my ($self, $a, $b) = @_;

    my $result = ::StringCompare($a->Title(), $b->Title());

    if ($result == 0)
        {
        if ($a->Type() == ::MENU_FILE() && $b->Type() == ::MENU_GROUP())
            {  $result = -1;  }
        elsif ($a->Type() == ::MENU_GROUP() && $b->Type() == ::MENU_FILE())
            {  $result = 1;  };
        };

    return $result;
    };


#
#   Function: SharedDirectoriesOf
#
#   Returns an array of all the directories shared by the files in the group.  If none, returns an empty array.
#
sub SharedDirectoriesOf #(group)
    {
    my ($self, $groupEntry) = @_;
    my @sharedDirectories;

    foreach my $entry (@{$groupEntry->GroupContent()})
        {
        if ($entry->Type() == ::MENU_FILE())
            {
            my @entryDirectories = NaturalDocs::File->SplitDirectories( (NaturalDocs::File->SplitPath($entry->Target()))[1] );

            if (!scalar @sharedDirectories)
                {  @sharedDirectories = @entryDirectories;  }
            else
                {  ::ShortenToMatchStrings(\@sharedDirectories, \@entryDirectories);  };

            if (!scalar @sharedDirectories)
                {  last;  };
            };
        };

    return @sharedDirectories;
    };


1;
