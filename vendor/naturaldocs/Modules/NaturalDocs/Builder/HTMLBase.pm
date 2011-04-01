###############################################################################
#
#   Package: NaturalDocs::Builder::HTMLBase
#
###############################################################################
#
#   A base package for all the shared functionality in <NaturalDocs::Builder::HTML> and
#   <NaturalDocs::Builder::FramedHTML>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use Tie::RefHash;

use strict;
use integer;

package NaturalDocs::Builder::HTMLBase;

use base 'NaturalDocs::Builder::Base';

use NaturalDocs::DefineMembers 'MADE_EMPTY_SEARCH_RESULTS_PAGE', 'MadeEmptySearchResultsPage()',
                                                 'SetMadeEmptySearchResultsPage()';



###############################################################################
# Group: Object Variables


#
#   Constants: Members
#
#   The object is implemented as a blessed arrayref, with the follow constants as indexes.
#
#   MADE_EMPTY_SEARCH_RESULTS_PAGE - Whether the search results page for searches with no results was generated.
#

#
#   Constants: NDMarkupToHTML Styles
#
#   These are the styles used with <NDMarkupToHTML()>.
#
#   NDMARKUPTOHTML_GENERAL - General style.
#   NDMARKUPTOHTML_SUMMARY - For summaries.
#   NDMARKUPTOHTML_TOOLTIP - For tooltips.
#
use constant NDMARKUPTOHTML_GENERAL => undef;
use constant NDMARKUPTOHTML_SUMMARY => 1;
use constant NDMARKUPTOHTML_TOOLTIP => 2;



###############################################################################
# Group: Package Variables
# These variables are shared by all instances of the package so don't change them.


#
#   handle: FH_CSS_FILE
#
#   The file handle to use when updating CSS files.
#


#
#   Hash: abbreviations
#
#   An existence hash of acceptable abbreviations.  These are words that <AddDoubleSpaces()> won't put a second space
#   after when followed by period-whitespace-capital letter.  Yes, this is seriously over-engineered.
#
my %abbreviations = ( mr => 1, mrs => 1, ms => 1, dr => 1,
                                  rev => 1, fr => 1, 'i.e' => 1,
                                  maj => 1, gen => 1, pres => 1, sen => 1, rep => 1,
                                  n => 1, s => 1, e => 1, w => 1, ne => 1, se => 1, nw => 1, sw => 1 );

#
#   array: indexHeadings
#
#   An array of the headings of all the index sections.  First is for symbols, second for numbers, and the rest for each letter.
#
my @indexHeadings = ( '$#!', '0-9', 'A' .. 'Z' );

#
#   array: indexAnchors
#
#   An array of the HTML anchors of all the index sections.  First is for symbols, second for numbers, and the rest for each letter.
#
my @indexAnchors = ( 'Symbols', 'Numbers', 'A' .. 'Z' );

#
#   array: searchExtensions
#
#   An array of the search file name extensions for all the index sections.  First is for symbols, second for numbers, and the rest
#   for each letter.
#
my @searchExtensions = ( 'Symbols', 'Numbers', 'A' .. 'Z' );

#
#   bool: saidUpdatingCSSFile
#
#   Whether the status message "Updating CSS file..." has been displayed.  We only want to print it once, no matter how many
#   HTML-based targets we are building.
#
my $saidUpdatingCSSFile;

#
#   constant: ADD_HIDDEN_BREAKS
#
#   Just a synonym for "1" so that setting the flag on <StringToHTML()> is clearer in the calling code.
#
use constant ADD_HIDDEN_BREAKS => 1;


###############################################################################
# Group: ToolTip Package Variables
#
#   These variables are for the tooltip generation functions only.  Since they're reset on every call to <BuildContent()> and
#   <BuildIndexSections()>, and are only used by them and their support functions, they can be shared by all instances of the
#   package.

#
#   int: tooltipLinkNumber
#
#   A number used as part of the ID for each link that has a tooltip.  Should be incremented whenever one is made.
#
my $tooltipLinkNumber;

#
#   int: tooltipNumber
#
#   A number used as part of the ID for each tooltip.  Should be incremented whenever one is made.
#
my $tooltipNumber;

#
#   hash: tooltipSymbolsToNumbers
#
#   A hash that maps the tooltip symbols to their assigned numbers.
#
my %tooltipSymbolsToNumbers;

#
#   string: tooltipHTML
#
#   The generated tooltip HTML.
#
my $tooltipHTML;


###############################################################################
# Group: Menu Package Variables
#
#   These variables are for the menu generation functions only.  Since they're reset on every call to <BuildMenu()> and are
#   only used by it and its support functions, they can be shared by all instances of the package.
#


#
#   hash: prebuiltMenus
#
#   A hash that maps output directonies to menu HTML already built for it.  There will be no selection or JavaScript in the menus.
#
my %prebuiltMenus;


#
#   bool: menuNumbersAndLengthsDone
#
#   Set when the variables that only need to be calculated for the menu once are done.  This includes <menuGroupNumber>,
#   <menuLength>, <menuGroupLengths>, and <menuGroupNumbers>, and <menuRootLength>.
#
my $menuNumbersAndLengthsDone;


#
#   int: menuGroupNumber
#
#   The current menu group number.  Each time a group is created, this is incremented so that each one will be unique.
#
my $menuGroupNumber;


#
#   int: menuLength
#
#   The length of the entire menu, fully expanded.  The value is computed from the <Menu Length Constants>.
#
my $menuLength;


#
#   hash: menuGroupLengths
#
#   A hash of the length of each group, *not* including any subgroup contents.  The keys are references to each groups'
#   <NaturalDocs::Menu::Entry> object, and the values are their lengths computed from the <Menu Length Constants>.
#
my %menuGroupLengths;
tie %menuGroupLengths, 'Tie::RefHash';


#
#   hash: menuGroupNumbers
#
#   A hash of the number of each group, as managed by <menuGroupNumber>.  The keys are references to each groups'
#   <NaturalDocs::Menu::Entry> object, and the values are the number.
#
my %menuGroupNumbers;
tie %menuGroupNumbers, 'Tie::RefHash';


#
#   int: menuRootLength
#
#   The length of the top-level menu entries without expansion.  The value is computed from the <Menu Length Constants>.
#
my $menuRootLength;


#
#   constants: Menu Length Constants
#
#   Constants used to approximate the lengths of the menu or its groups.
#
#   MENU_TITLE_LENGTH       - The length of the title.
#   MENU_SUBTITLE_LENGTH - The length of the subtitle.
#   MENU_FILE_LENGTH         - The length of one file entry.
#   MENU_GROUP_LENGTH     - The length of one group entry.
#   MENU_TEXT_LENGTH        - The length of one text entry.
#   MENU_LINK_LENGTH        - The length of one link entry.
#
#   MENU_LENGTH_LIMIT    - The limit of the menu's length.  If the total length surpasses this limit, groups that aren't required
#                                       to be open to show the selection will default to closed on browsers that support it.
#
use constant MENU_TITLE_LENGTH => 3;
use constant MENU_SUBTITLE_LENGTH => 1;
use constant MENU_FILE_LENGTH => 1;
use constant MENU_GROUP_LENGTH => 2; # because it's a line and a blank space
use constant MENU_TEXT_LENGTH => 1;
use constant MENU_LINK_LENGTH => 1;
use constant MENU_INDEX_LENGTH => 1;

use constant MENU_LENGTH_LIMIT => 35;


###############################################################################
# Group: Image Package Variables
#
#   These variables are for the image generation functions only.  Since they're reset on every call to <BuildContent()>,
#   and are only used by it and its support functions, they can be shared by all instances of thepackage.


#
#   var: imageAnchorNumber
#   Incremented for each image link in the file that requires an anchor.
#
my $imageAnchorNumber;


#
#   var: imageContent
#
#   The actual embedded image HTML for all image links.  When generating an image link, the link HTML is returned and
#   the HTML for the target image is added here.  Periodically, such as after the end of the paragraph, this content should
#   be added to the page and the variable set to undef.
#
my $imageContent;



###############################################################################
# Group: Search Package Variables
#
#   These variables are for the search generation functions only.  Since they're reset on every call to <BuildIndexSections()> and
#   are only used by them and their support functions, they can be shared by all instances of the package.


#
#   hash: searchResultIDs
#
#   A hash mapping lowercase-only search result IDs to the number of times they've been used.  This is to work around an IE
#   bug where it won't correctly reference IDs if they differ only in case.
#
my %searchResultIDs;



###############################################################################
# Group: Object Functions


#
#   Function: New
#   Creates and returns a new object.
#
sub New
    {
    my $class = shift;

    my $object = $class->SUPER::New();
    $object->SetMadeEmptySearchResultsPage(0);

    return $object;
    };


# Function: MadeEmptySearchResultsPage
# Returns whether the empty search results page was created or not.

# Function: SetMadeEmptySearchResultsPage
# Sets whether the empty search results page was created or not.



###############################################################################
# Group: Implemented Interface Functions
#
#   The behavior of these functions is shared between HTML output formats.
#


#
#   Function: UpdateImage
#
#   Define this function to add or update the passed image in the output.
#
#   Parameters:
#
#       file - The image <FileName>
#
sub UpdateImage #(file)
    {
    my ($self, $file) = @_;

    my $outputFile = $self->OutputImageOf($file);
    my $outputDirectory = NaturalDocs::File->NoFileName($outputFile);

    if (!-d $outputDirectory)
        {  NaturalDocs::File->CreatePath($outputDirectory);  };

    NaturalDocs::File->Copy($file, $outputFile);
    };


#
#   Function: PurgeFiles
#
#   Deletes the output files associated with the purged source files.
#
sub PurgeFiles #(filesToPurge)
    {
    my ($self, $filesToPurge) = @_;

    # Combine directories into a hash to remove duplicate work.
    my %directoriesToPurge;

    foreach my $file (keys %$filesToPurge)
        {
        # It's possible that there may be files there that aren't in a valid input directory anymore.  They won't generate an output
        # file name so we need to check for undef.
        my $outputFile = $self->OutputFileOf($file);
        if (defined $outputFile)
            {
            unlink($outputFile);
            $directoriesToPurge{ NaturalDocs::File->NoFileName($outputFile) } = 1;
            };
        };

    foreach my $directory (keys %directoriesToPurge)
        {
        NaturalDocs::File->RemoveEmptyTree($directory, NaturalDocs::Settings->OutputDirectoryOf($self));
        };
    };


#
#   Function: PurgeIndexes
#
#   Deletes the output files associated with the purged source files.
#
#   Parameters:
#
#       indexes  - An existence hashref of the index types to purge.  The keys are the <TopicTypes> or * for the general index.
#
sub PurgeIndexes #(indexes)
    {
    my ($self, $indexes) = @_;

    foreach my $index (keys %$indexes)
        {
        $self->PurgeIndexFiles($index, undef, undef);
        };
    };


#
#   Function: PurgeImages
#
#   Define this function to make the package remove all output related to the passed image files.  These files are no longer used
#   by the documentation.
#
#   Parameters:
#
#       files - An existence hashref of the image <FileNames> to purge.
#
sub PurgeImages #(files)
    {
    my ($self, $filesToPurge) = @_;

    # Combine directories into a hash to remove duplicate work.
    my %directoriesToPurge;

    foreach my $file (keys %$filesToPurge)
        {
        # It's possible that there may be files there that aren't in a valid input directory anymore.  They won't generate an output
        # file name so we need to check for undef.
        my $outputFile = $self->OutputImageOf($file);
        if (defined $outputFile)
            {
            unlink($outputFile);
            $directoriesToPurge{ NaturalDocs::File->NoFileName($outputFile) } = 1;
            };
        };

    foreach my $directory (keys %directoriesToPurge)
        {
        NaturalDocs::File->RemoveEmptyTree($directory, NaturalDocs::Settings->OutputDirectoryOf($self));
        };
    };


#
#   Function: BeginBuild
#
#   Creates the necessary subdirectories in the output directory.
#
sub BeginBuild #(hasChanged)
    {
    my ($self, $hasChanged) = @_;

    foreach my $directory ( $self->JavaScriptDirectory(), $self->CSSDirectory(), $self->IndexDirectory(),
                                       $self->SearchResultsDirectory() )
        {
        if (!-d $directory)
            {  NaturalDocs::File->CreatePath($directory);  };
        };
    };


#
#   Function: EndBuild
#
#   Synchronizes the projects CSS and JavaScript files.  Also generates the search data JavaScript file.
#
sub EndBuild #(hasChanged)
    {
    my ($self, $hasChanged) = @_;


    # Update the style sheets.

    my $styles = NaturalDocs::Settings->Styles();
    my $changed;

    my $cssDirectory = $self->CSSDirectory();
    my $mainCSSFile = $self->MainCSSFile();

    for (my $i = 0; $i < scalar @$styles; $i++)
        {
        my $outputCSSFile;

        if (scalar @$styles == 1)
            {  $outputCSSFile = $mainCSSFile;  }
        else
            {  $outputCSSFile = NaturalDocs::File->JoinPaths($cssDirectory, ($i + 1) . '.css');  };


        my $masterCSSFile = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->ProjectDirectory(), $styles->[$i] . '.css' );

        if (! -e $masterCSSFile)
            {  $masterCSSFile = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->StyleDirectory(), $styles->[$i] . '.css' );  };

        # We check both the date and the size in case the user switches between two styles which just happen to have the same
        # date.  Should rarely happen, but it might.
        if (! -e $outputCSSFile ||
            (stat($masterCSSFile))[9] != (stat($outputCSSFile))[9] ||
             -s $masterCSSFile != -s $outputCSSFile)
            {
            if (!NaturalDocs::Settings->IsQuiet() && !$saidUpdatingCSSFile)
                {
                print "Updating CSS file...\n";
                $saidUpdatingCSSFile = 1;
                };

            NaturalDocs::File->Copy($masterCSSFile, $outputCSSFile);

            $changed = 1;
            };
        };


    my $deleteFrom;

    if (scalar @$styles == 1)
        {  $deleteFrom = 1;  }
    else
        {  $deleteFrom = scalar @$styles + 1;  };

    for (;;)
        {
        my $file = NaturalDocs::File->JoinPaths($cssDirectory, $deleteFrom . '.css');

        if (! -e $file)
            {  last;  };

        unlink ($file);
        $deleteFrom++;

        $changed = 1;
        };


    if ($changed)
        {
        if (scalar @$styles > 1)
            {
            open(FH_CSS_FILE, '>' . $mainCSSFile);
		    binmode(FH_CSS_FILE, ':encoding(UTF-8)');

            for (my $i = 0; $i < scalar @$styles; $i++)
                {
                print FH_CSS_FILE '@import URL("' . ($i + 1) . '.css");' . "\n";
                };

            close(FH_CSS_FILE);
            };
        };



    # Update the JavaScript files

    my $jsMaster = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->JavaScriptDirectory(), 'NaturalDocs.js' );
    my $jsOutput = $self->MainJavaScriptFile();

    # We check both the date and the size in case the user switches between two styles which just happen to have the same
    # date.  Should rarely happen, but it might.
    if (! -e $jsOutput ||
        (stat($jsMaster))[9] != (stat($jsOutput))[9] ||
         -s $jsMaster != -s $jsOutput)
        {
        NaturalDocs::File->Copy($jsMaster, $jsOutput);
        };


    my $prettifyOutput = $self->PrettifyJavaScriptFile();

    if (NaturalDocs::Settings->HighlightCode() || NaturalDocs::Settings->HighlightAnonymous())
    	{
	    my $prettifyMaster = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->JavaScriptDirectory(), 'GooglePrettify.js' );

	    # We check both the date and the size in case the user switches between two styles which just happen to have the same
	    # date.  Should rarely happen, but it might.
	    if (! -e $prettifyOutput ||
	        (stat($prettifyMaster))[9] != (stat($prettifyOutput))[9] ||
	         -s $prettifyMaster != -s $prettifyOutput)
	        {
	        NaturalDocs::File->Copy($prettifyMaster, $prettifyOutput);
	        };
	    }
	elsif (-e $prettifyOutput)
		{
		unlink $prettifyOutput;
		}


    my @indexes = keys %{NaturalDocs::Menu->Indexes()};

    open(FH_INDEXINFOJS, '>' . NaturalDocs::File->JoinPaths( $self->JavaScriptDirectory(), 'searchdata.js'));
    binmode(FH_INDEXINFOJS, ':encoding(UTF-8)');

    print FH_INDEXINFOJS
    "var indexSectionsWithContent = {\n";

    for (my $i = 0; $i < scalar @indexes; $i++)
        {
        if ($i != 0)
            {  print FH_INDEXINFOJS ",\n";  };

        print FH_INDEXINFOJS '   "' . NaturalDocs::Topics->NameOfType($indexes[$i], 1, 1) . "\": {\n";

        my $content = NaturalDocs::SymbolTable->IndexSectionsWithContent($indexes[$i]);
        for (my $contentIndex = 0; $contentIndex < 28; $contentIndex++)
            {
            if ($contentIndex != 0)
                {  print FH_INDEXINFOJS ",\n";  };

            print FH_INDEXINFOJS '      "' . $searchExtensions[$contentIndex] . '": ' . ($content->[$contentIndex] ? 'true' : 'false');
            };

        print FH_INDEXINFOJS "\n      }";
        };

    print FH_INDEXINFOJS
    "\n   }";

    close(FH_INDEXINFOJS);
    };



###############################################################################
# Group: Section Functions


#
#   Function: BuildTitle
#
#   Builds and returns the HTML page title of a file.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to build the title of.
#
#   Returns:
#
#       The source file's title in HTML.
#
sub BuildTitle #(sourceFile)
    {
    my ($self, $sourceFile) = @_;

    # If we have a menu title, the page title is [menu title] - [file title].  Otherwise it is just [file title].

    my $title = NaturalDocs::Project->DefaultMenuTitleOf($sourceFile);

    my $menuTitle = NaturalDocs::Menu->Title();
    if (defined $menuTitle && $menuTitle ne $title)
        {  $title .= ' - ' . $menuTitle;  };

    $title = $self->StringToHTML($title);

    return $title;
    };

#
#   Function: BuildMenu
#
#   Builds and returns the side menu of a file.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to use if you're looking for a source file.
#       indexType - The index <TopicType> to use if you're looking for an index.
#
#       Both sourceFile and indexType may be undef.
#
#   Returns:
#
#       The side menu in HTML.
#
#   Dependencies:
#
#       - <Builder::HTML::UpdateFile()> and <Builder::HTML::UpdateIndex()> require this section to be surrounded with the exact
#         strings "<div id=Menu>" and "</div><!--Menu-->".
#       - This function depends on the way <BuildMenuSegment()> formats file and index entries.
#
sub BuildMenu #(FileName sourceFile, TopicType indexType) -> string htmlMenu
    {
    my ($self, $sourceFile, $indexType) = @_;

    if (!$menuNumbersAndLengthsDone)
        {
        $menuGroupNumber = 1;
        $menuLength = 0;
        %menuGroupLengths = ( );
        %menuGroupNumbers = ( );
        $menuRootLength = 0;
        };

    my $outputDirectory;

    if ($sourceFile)
        {  $outputDirectory = NaturalDocs::File->NoFileName( $self->OutputFileOf($sourceFile) );  }
    elsif ($indexType)
        {  $outputDirectory = NaturalDocs::File->NoFileName( $self->IndexFileOf($indexType) );  }
    else
        {  $outputDirectory = NaturalDocs::Settings->OutputDirectoryOf($self);  };


    # Comment needed for UpdateFile().
    my $output = '<div id=Menu>';


    if (!exists $prebuiltMenus{$outputDirectory})
        {
        my $segmentOutput;

        ($segmentOutput, $menuRootLength) =
            $self->BuildMenuSegment($outputDirectory, NaturalDocs::Menu->Content(), 1);

        my $titleOutput;

        my $menuTitle = NaturalDocs::Menu->Title();
        if (defined $menuTitle)
            {
            if (!$menuNumbersAndLengthsDone)
                {  $menuLength += MENU_TITLE_LENGTH;  };

            $menuRootLength += MENU_TITLE_LENGTH;

            $titleOutput .=
            '<div class=MTitle>'
                . $self->StringToHTML($menuTitle);

            my $menuSubTitle = NaturalDocs::Menu->SubTitle();
            if (defined $menuSubTitle)
                {
                if (!$menuNumbersAndLengthsDone)
                    {  $menuLength += MENU_SUBTITLE_LENGTH;  };

                $menuRootLength += MENU_SUBTITLE_LENGTH;

                $titleOutput .=
                '<div class=MSubTitle>'
                    . $self->StringToHTML($menuSubTitle)
                . '</div>';
                };

            $titleOutput .=
            '</div>';
            };

        my $searchOutput;

        if (scalar keys %{NaturalDocs::Menu->Indexes()})
            {
            $searchOutput =
            '<script type="text/javascript"><!--' . "\n"
                . 'var searchPanel = new SearchPanel("searchPanel", "' . $self->CommandLineOption() . '", '
                    . '"' . $self->MakeRelativeURL($outputDirectory, $self->SearchResultsDirectory()) . '");' . "\n"
            . '--></script>'

            . '<div id=MSearchPanel class=MSearchPanelInactive>'
                . '<input type=text id=MSearchField value=Search '
                    . 'onFocus="searchPanel.OnSearchFieldFocus(true)" onBlur="searchPanel.OnSearchFieldFocus(false)" '
                    . 'onKeyUp="searchPanel.OnSearchFieldChange()">'
                . '<select id=MSearchType '
                    . 'onFocus="searchPanel.OnSearchTypeFocus(true)" onBlur="searchPanel.OnSearchTypeFocus(false)" '
                    . 'onChange="searchPanel.OnSearchTypeChange()">';

                my @indexes = keys %{NaturalDocs::Menu->Indexes()};
                @indexes = sort
                    {
                    if ($a eq ::TOPIC_GENERAL())  {  return -1;  }
                    elsif ($b eq ::TOPIC_GENERAL())  {  return 1;  }
                    else  {  return (NaturalDocs::Topics->NameOfType($a, 1) cmp NaturalDocs::Topics->NameOfType($b, 1))  };
                    }  @indexes;

                foreach my $index (@indexes)
                    {
                    my ($name, $extra);
                    if ($index eq ::TOPIC_GENERAL())
                        {
                        $name = 'Everything';
                        $extra = ' id=MSearchEverything selected ';
                        }
                    else
                        {  $name = $self->ConvertAmpChars(NaturalDocs::Topics->NameOfType($index, 1));  }

                    $searchOutput .=
                    '<option ' . $extra . 'value="' . NaturalDocs::Topics->NameOfType($index, 1, 1) . '">'
                        . $name
                    . '</option>';
                    };

                $searchOutput .=
                '</select>'
            . '</div>';
            };

        $prebuiltMenus{$outputDirectory} = $titleOutput . $segmentOutput . $searchOutput;
        $output .= $titleOutput . $segmentOutput . $searchOutput;
        }
    else
        {  $output .= $prebuiltMenus{$outputDirectory};  };


    # Highlight the menu selection.

    if ($sourceFile)
        {
        # Dependency: This depends on how BuildMenuSegment() formats file entries.
        my $outputFile = $self->OutputFileOf($sourceFile);
        my $tag = '<div class=MFile><a href="' . $self->MakeRelativeURL($outputDirectory, $outputFile) . '">';
        my $tagIndex = index($output, $tag);

        if ($tagIndex != -1)
            {
            my $endIndex = index($output, '</a>', $tagIndex);

            substr($output, $endIndex, 4, '');
            substr($output, $tagIndex, length($tag), '<div class=MFile id=MSelected>');
            };
        }
    elsif ($indexType)
        {
        # Dependency: This depends on how BuildMenuSegment() formats index entries.
        my $outputFile = $self->IndexFileOf($indexType);
        my $tag = '<div class=MIndex><a href="' . $self->MakeRelativeURL($outputDirectory, $outputFile) . '">';
        my $tagIndex = index($output, $tag);

        if ($tagIndex != -1)
            {
            my $endIndex = index($output, '</a>', $tagIndex);

            substr($output, $endIndex, 4, '');
            substr($output, $tagIndex, length($tag), '<div class=MIndex id=MSelected>');
            };
        };


    # If the completely expanded menu is too long, collapse all the groups that aren't in the selection hierarchy or near the
    # selection.  By doing this instead of having them default to closed via CSS, any browser that doesn't support changing this at
    # runtime will keep the menu entirely open so that its still usable.

    if ($menuLength > MENU_LENGTH_LIMIT())
        {
        my $menuSelectionHierarchy = $self->GetMenuSelectionHierarchy($sourceFile, $indexType);

        my $toExpand = $self->ExpandMenu($sourceFile, $indexType, $menuSelectionHierarchy, $menuRootLength);

        $output .=

        '<script language=JavaScript><!--' . "\n"

        . 'HideAllBut([' . join(', ', @$toExpand) . '], ' . $menuGroupNumber . ');'

        . '// --></script>';
        };

    $output .= '</div><!--Menu-->';

    $menuNumbersAndLengthsDone = 1;

    return $output;
    };


#
#   Function: BuildMenuSegment
#
#   A recursive function to build a segment of the menu.  *Remember to reset the <Menu Package Variables> before calling this
#   for the first time.*
#
#   Parameters:
#
#       outputDirectory - The output directory the menu is being built for.
#       menuSegment - An arrayref specifying the segment of the menu to build.  Either pass the menu itself or the contents
#                               of a group.
#       topLevel - Whether the passed segment is the top level segment or not.
#
#   Returns:
#
#       The array ( menuHTML, length ).
#
#       menuHTML - The menu segment in HTML.
#       groupLength - The length of the group, *not* including the contents of any subgroups, as computed from the
#                            <Menu Length Constants>.
#
#   Dependencies:
#
#       - <BuildMenu()> depends on the way this function formats file and index entries.
#
sub BuildMenuSegment #(outputDirectory, menuSegment, topLevel)
    {
    my ($self, $outputDirectory, $menuSegment, $topLevel) = @_;

    my $output;
    my $groupLength = 0;

    foreach my $entry (@$menuSegment)
        {
        if ($entry->Type() == ::MENU_GROUP())
            {
            my ($entryOutput, $entryLength) =
                $self->BuildMenuSegment($outputDirectory, $entry->GroupContent());

            my $entryNumber;

            if (!$menuNumbersAndLengthsDone)
                {
                $entryNumber = $menuGroupNumber;
                $menuGroupNumber++;

                $menuGroupLengths{$entry} = $entryLength;
                $menuGroupNumbers{$entry} = $entryNumber;
                }
            else
                {  $entryNumber = $menuGroupNumbers{$entry};  };

            $output .=
            '<div class=MEntry>'
                . '<div class=MGroup>'

                    . '<a href="javascript:ToggleMenu(\'MGroupContent' . $entryNumber . '\')"'
                         . ($self->CommandLineOption() eq 'FramedHTML' ? ' target="_self"' : '') . '>'
                        . $self->StringToHTML($entry->Title())
                    . '</a>'

                    . '<div class=MGroupContent id=MGroupContent' . $entryNumber . '>'
                        . $entryOutput
                    . '</div>'

                . '</div>'
            . '</div>';

            $groupLength += MENU_GROUP_LENGTH;
            }

        elsif ($entry->Type() == ::MENU_FILE())
            {
            my $targetOutputFile = $self->OutputFileOf($entry->Target());

        # Dependency: BuildMenu() depends on how this formats file entries.
            $output .=
            '<div class=MEntry>'
                . '<div class=MFile>'
                    . '<a href="' . $self->MakeRelativeURL($outputDirectory, $targetOutputFile) . '">'
                        . $self->StringToHTML( $entry->Title(), ADD_HIDDEN_BREAKS)
                    . '</a>'
                . '</div>'
            . '</div>';

            $groupLength += MENU_FILE_LENGTH;
            }

        elsif ($entry->Type() == ::MENU_TEXT())
            {
            $output .=
            '<div class=MEntry>'
                . '<div class=MText>'
                    . $self->StringToHTML( $entry->Title() )
                . '</div>'
            . '</div>';

            $groupLength += MENU_TEXT_LENGTH;
            }

        elsif ($entry->Type() == ::MENU_LINK())
            {
            $output .=
            '<div class=MEntry>'
                . '<div class=MLink>'
                    . '<a href="' . $entry->Target() . '"' . ($self->CommandLineOption() eq 'FramedHTML' ? ' target="_top"' : '') . '>'
                        . $self->StringToHTML( $entry->Title() )
                    . '</a>'
                . '</div>'
            . '</div>';

            $groupLength += MENU_LINK_LENGTH;
            }

        elsif ($entry->Type() == ::MENU_INDEX())
            {
            my $indexFile = $self->IndexFileOf($entry->Target);

        # Dependency: BuildMenu() depends on how this formats index entries.
            $output .=
            '<div class=MEntry>'
                . '<div class=MIndex>'
                    . '<a href="' . $self->MakeRelativeURL( $outputDirectory, $self->IndexFileOf($entry->Target()) ) . '">'
                        . $self->StringToHTML( $entry->Title() )
                    . '</a>'
                . '</div>'
            . '</div>';

            $groupLength += MENU_INDEX_LENGTH;
            };
        };


    if (!$menuNumbersAndLengthsDone)
        {  $menuLength += $groupLength;  };

    return ($output, $groupLength);
    };


#
#   Function: BuildContent
#
#   Builds and returns the main page content.
#
#   Parameters:
#
#       sourceFile - The source <FileName>.
#       parsedFile - The parsed source file as an arrayref of <NaturalDocs::Parser::ParsedTopic> objects.
#
#   Returns:
#
#       The page content in HTML.
#
sub BuildContent #(sourceFile, parsedFile)
    {
    my ($self, $sourceFile, $parsedFile) = @_;

    $self->ResetToolTips();
    $imageAnchorNumber = 1;
    $imageContent = undef;

    my $output = '<div id=Content>';
    my $i = 0;

    while ($i < scalar @$parsedFile)
        {
        my $anchor = $self->SymbolToHTMLSymbol($parsedFile->[$i]->Symbol());

        my $scope = NaturalDocs::Topics->TypeInfo($parsedFile->[$i]->Type())->Scope();


        # The anchors are closed, but not around the text, so the :hover CSS style won't accidentally kick in.

        my $headerType;

        if ($i == 0)
            {  $headerType = 'h1';  }
        elsif ($scope == ::SCOPE_START() || $scope == ::SCOPE_END())
            {  $headerType = 'h2';  }
        else
            {  $headerType = 'h3';  };

        $output .=

        '<div class="C' . NaturalDocs::Topics->NameOfType($parsedFile->[$i]->Type(), 0, 1) . '">'
            . '<div class=CTopic' . ($i == 0 ? ' id=MainTopic' : '') . '>'

                . '<' . $headerType . ' class=CTitle>'
                    . '<a name="' . $anchor . '"></a>'
                    . $self->StringToHTML( $parsedFile->[$i]->Title(), ADD_HIDDEN_BREAKS)
                . '</' . $headerType . '>';


        my $hierarchy;
        if (NaturalDocs::Topics->TypeInfo( $parsedFile->[$i]->Type() )->ClassHierarchy())
            {
            $hierarchy = $self->BuildClassHierarchy($sourceFile, $parsedFile->[$i]->Symbol());
            };

        my $summary;
        if ($i == 0 || $scope == ::SCOPE_START() || $scope == ::SCOPE_END())
            {
            $summary .= $self->BuildSummary($sourceFile, $parsedFile, $i);
            };

        my $hasBody;
        if (defined $hierarchy || defined $summary ||
            defined $parsedFile->[$i]->Prototype() || defined $parsedFile->[$i]->Body())
            {
            $output .= '<div class=CBody>';
            $hasBody = 1;
            };

        $output .= $hierarchy;

        if (defined $parsedFile->[$i]->Prototype())
            {
            $output .= $self->BuildPrototype($parsedFile->[$i]->Type(), $parsedFile->[$i]->Prototype(), $sourceFile);
            };

        if (defined $parsedFile->[$i]->Body())
            {
            $output .= $self->NDMarkupToHTML( $sourceFile, $parsedFile->[$i]->Body(), $parsedFile->[$i]->Symbol(),
                                                                  $parsedFile->[$i]->Package(), $parsedFile->[$i]->Type(),
                                                                  $parsedFile->[$i]->Using() );
            };

        $output .= $summary;


        if ($hasBody)
            {  $output .= '</div>';  };

        $output .=
            '</div>' # CTopic
        . '</div>' # CType
        . "\n\n";

        $i++;
        };

    $output .= '</div><!--Content-->';

    return $output;
    };


#
#   Function: BuildSummary
#
#   Builds a summary, either for the entire file or the current class/section.
#
#   Parameters:
#
#       sourceFile - The source <FileName> the summary appears in.
#
#       parsedFile - A reference to the parsed source file.
#
#       index   - The index into the parsed file to start at.  If undef or zero, it builds a summary for the entire file.  If it's the
#                    index of a <TopicType> that starts or ends a scope, it builds a summary for that scope
#
#   Returns:
#
#       The summary in HTML.
#
sub BuildSummary #(sourceFile, parsedFile, index)
    {
    my ($self, $sourceFile, $parsedFile, $index) = @_;
    my $completeSummary;

    if (!defined $index || $index == 0)
        {
        $index = 0;
        $completeSummary = 1;
        }
    else
        {
        # Skip the scope entry.
        $index++;
        };

    if ($index + 1 >= scalar @$parsedFile)
        {  return undef;  };


    my $scope = NaturalDocs::Topics->TypeInfo($parsedFile->[$index]->Type())->Scope();

    # Return nothing if there's only one entry.
    if (!$completeSummary && ($scope == ::SCOPE_START() || $scope == ::SCOPE_END()) )
        {  return undef;  };


    my $indent = 0;
    my $inGroup;

    my $isMarked = 0;

    my $output =
    '<!--START_ND_SUMMARY-->'
    . '<div class=Summary><div class=STitle>Summary</div>'

        # Not all browsers get table padding right, so we need a div to apply the border.
        . '<div class=SBorder>'
        . '<table border=0 cellspacing=0 cellpadding=0 class=STable>';

        while ($index < scalar @$parsedFile)
            {
            my $topic = $parsedFile->[$index];
            my $scope = NaturalDocs::Topics->TypeInfo($topic->Type())->Scope();

            if (!$completeSummary && ($scope == ::SCOPE_START() || $scope == ::SCOPE_END()) )
                {  last;  };


            # Remove modifiers as appropriate for the current entry.

            if ($scope == ::SCOPE_START() || $scope == ::SCOPE_END())
                {
                $indent = 0;
                $inGroup = 0;
                $isMarked = 0;
                }
            elsif ($topic->Type() eq ::TOPIC_GROUP())
                {
                if ($inGroup)
                    {  $indent--;  };

                $inGroup = 0;
                $isMarked = 0;
                };


            $output .=
             '<tr class="S' . ($index == 0 ? 'Main' : NaturalDocs::Topics->NameOfType($topic->Type(), 0, 1))
                . ($indent ? ' SIndent' . $indent : '') . ($isMarked ? ' SMarked' : '') .'">'
                . '<td class=SEntry>';

           # Add the entry itself.

            my $toolTipProperties;

            # We only want a tooltip here if there's a protoype.  Otherwise it's redundant.

            if (defined $topic->Prototype())
                {
                my $tooltipID = $self->BuildToolTip($topic->Symbol(), $sourceFile, $topic->Type(),
                                                                     $topic->Prototype(), $topic->Summary());
                $toolTipProperties = $self->BuildToolTipLinkProperties($tooltipID);
                };

            $output .=
            '<a href="#' . $self->SymbolToHTMLSymbol($parsedFile->[$index]->Symbol()) . '" ' . $toolTipProperties . '>'
                . $self->StringToHTML( $parsedFile->[$index]->Title(), ADD_HIDDEN_BREAKS)
            . '</a>';


            $output .=
            '</td><td class=SDescription>';

            if (defined $parsedFile->[$index]->Body())
                {
                $output .= $self->NDMarkupToHTML($sourceFile, $parsedFile->[$index]->Summary(),
                                                                     $parsedFile->[$index]->Symbol(), $parsedFile->[$index]->Package(),
                                                                     $parsedFile->[$index]->Type(), $parsedFile->[$index]->Using(),
                                                                     NDMARKUPTOHTML_SUMMARY);
                };


            $output .=
            '</td></tr>';


            # Prepare the modifiers for the next entry.

            if ($scope == ::SCOPE_START() || $scope == ::SCOPE_END())
                {
                $indent = 1;
                $inGroup = 0;
                }
            elsif ($topic->Type() eq ::TOPIC_GROUP())
                {
                if (!$inGroup)
                    {
                    $indent++;
                    $inGroup = 1;
                    };
                };

            $isMarked ^= 1;
            $index++;
            };

        $output .=
        '</table>'
    . '</div>' # Body
    . '</div>' # Summary
    . "<!--END_ND_SUMMARY-->";

    return $output;
    };


#
#   Function: BuildPrototype
#
#   Builds and returns the prototype as HTML.
#
#   Parameters:
#
#       type - The <TopicType> the prototype is from.
#       prototype - The prototype to format.
#       file - The <FileName> the prototype was defined in.
#
#   Returns:
#
#       The prototype in HTML.
#
sub BuildPrototype #(type, prototype, file)
    {
    my ($self, $type, $prototype, $file) = @_;

    my $language = NaturalDocs::Languages->LanguageOf($file);
    my $prototypeObject = $language->ParsePrototype($type, $prototype);

    my $output;

    if ($prototypeObject->OnlyBeforeParameters())
        {
        $output =
        # A blockquote to scroll it if it's too long.
        '<blockquote>'
            # A surrounding table as a hack to make the div form-fit.
            . '<table border=0 cellspacing=0 cellpadding=0 class="Prototype"><tr>'
            	. '<td' . (NaturalDocs::Settings->HighlightCode() ? ' class="prettyprint"' : '') . '>'
	                . $self->ConvertAmpChars($prototypeObject->BeforeParameters())
	            . '</td>'
	        . '</tr></table>'
        . '</blockquote>';
        }

    else
        {
        my $params = $prototypeObject->Parameters();
        my $beforeParams = $prototypeObject->BeforeParameters();
        my $afterParams = $prototypeObject->AfterParameters();

	    my $highlightClass = (NaturalDocs::Settings->HighlightCode() ? ' prettyprint ' : '');

        # Determine what features the prototype has and its length.

        my ($hasType, $hasTypePrefix, $hasNamePrefix, $hasDefaultValue, $hasDefaultValuePrefix);
        my $maxParamLength = 0;

        foreach my $param (@$params)
            {
            my $paramLength = length($param->Name());

            if ($param->Type())
                {
                $hasType = 1;
                $paramLength += length($param->Type()) + 1;
                };
            if ($param->TypePrefix())
                {
                $hasTypePrefix = 1;
                $paramLength += length($param->TypePrefix()) + 1;
                };
            if ($param->NamePrefix())
                {
                $hasNamePrefix = 1;
                $paramLength += length($param->NamePrefix());
                };
            if ($param->DefaultValue())
                {
                $hasDefaultValue = 1;

                # The length of the default value part is either the longest word, or 1/3 the total, whichever is longer.  We do this
                # because we don't want parameter lines wrapping to more than three lines, and there's no guarantee that the line will
                # wrap at all.  There's a small possibility that it could still wrap to four lines with this code, but we don't need to go
                # crazy(er) here.

                my $thirdLength = length($param->DefaultValue()) / 3;

                my @words = split(/ +/, $param->DefaultValue());
                my $maxWordLength = 0;

                foreach my $word (@words)
                    {
                    if (length($word) > $maxWordLength)
                        {  $maxWordLength = length($word);  };
                    };

                $paramLength += ($maxWordLength > $thirdLength ? $maxWordLength : $thirdLength) + 1;
                };
            if ($param->DefaultValuePrefix())
                {
                $hasDefaultValuePrefix = 1;
                $paramLength += length($param->DefaultValuePrefix()) + 1;
                };

            if ($paramLength > $maxParamLength)
                {  $maxParamLength = $paramLength;  };
            };

        my $useCondensed = (length($beforeParams) + $maxParamLength + length($afterParams) > 80 ? 1 : 0);
        my $parameterColumns = 1 + $hasType + $hasTypePrefix + $hasNamePrefix +
                                               $hasDefaultValue + $hasDefaultValuePrefix + $useCondensed;

        $output =
        '<blockquote><table border=0 cellspacing=0 cellpadding=0 class="Prototype"><tr><td>'

            # Stupid hack to get it to work right in IE.
            . '<table border=0 cellspacing=0 cellpadding=0><tr>'

            . '<td class="PBeforeParameters ' . $highlightClass . '"' . ($useCondensed ? 'colspan=' . $parameterColumns : 'nowrap') . '>'
                . $self->ConvertAmpChars($beforeParams);

                if ($beforeParams && $beforeParams !~ /[\(\[\{\<]$/)
                    {  $output .= '&nbsp;';  };

            $output .=
            '</td>';

            for (my $i = 0; $i < scalar @$params; $i++)
                {
                if ($useCondensed)
                    {
                    $output .= '</tr><tr><td>&nbsp;&nbsp;&nbsp;</td>';
                    }
                elsif ($i > 0)
                    {
                    # Go to the next row and and skip the BeforeParameters cell.
                    $output .= '</tr><tr><td></td>';
                    };

                if ($language->TypeBeforeParameter())
                    {
                    if ($hasTypePrefix)
                        {
                        my $htmlTypePrefix = $self->ConvertAmpChars($params->[$i]->TypePrefix());
                        $htmlTypePrefix =~ s/ $/&nbsp;/;

                        $output .=
                        '<td class="PTypePrefix ' . $highlightClass . '" nowrap>'
                            . $htmlTypePrefix
                        . '</td>';
                        };

                    if ($hasType)
                        {
                        $output .=
                        '<td class="PType ' . $highlightClass . '" nowrap>'
                            . $self->ConvertAmpChars($params->[$i]->Type()) . '&nbsp;'
                        . '</td>';
                        };

                    if ($hasNamePrefix)
                        {
                        $output .=
                        '<td class="PParameterPrefix ' . $highlightClass . '" nowrap>'
                            . $self->ConvertAmpChars($params->[$i]->NamePrefix())
                        . '</td>';
                        };

                    $output .=
                    '<td class="PParameter ' . $highlightClass . '" nowrap' . ($useCondensed && !$hasDefaultValue ? ' width=100%' : '') . '>'
                        . $self->ConvertAmpChars($params->[$i]->Name())
                    . '</td>';
                    }

                else # !$language->TypeBeforeParameter()
                    {
                    $output .=
                    '<td class="PParameter ' . $highlightClass . '" nowrap>'
                        . $self->ConvertAmpChars( $params->[$i]->NamePrefix() . $params->[$i]->Name() )
                    . '</td>';

                    if ($hasType || $hasTypePrefix)
                        {
                        my $typePrefix = $params->[$i]->TypePrefix();
                        if ($typePrefix)
                            {  $typePrefix .= ' ';  };

                        $output .=
                        '<td class="PType ' . $highlightClass . '" nowrap' . ($useCondensed && !$hasDefaultValue ? ' width=100%' : '') . '>'
                            . '&nbsp;' . $self->ConvertAmpChars( $typePrefix . $params->[$i]->Type() )
                        . '</td>';
                        };
                    };

                if ($hasDefaultValuePrefix)
                    {
                    $output .=
                    '<td class="PDefaultValuePrefix ' . $highlightClass . '">'

                       . '&nbsp;' . $self->ConvertAmpChars( $params->[$i]->DefaultValuePrefix() ) . '&nbsp;'
                    . '</td>';
                    };

                if ($hasDefaultValue)
                    {
                    $output .=
                    '<td class="PDefaultValue ' . $highlightClass . '" width=100%>'
                        . ($hasDefaultValuePrefix ? '' : '&nbsp;') . $self->ConvertAmpChars( $params->[$i]->DefaultValue() )
                    . '</td>';
                    };
                };

            if ($useCondensed)
                {  $output .= '</tr><tr>';  };

            $output .=
            '<td class="PAfterParameters ' . $highlightClass . '"' . ($useCondensed ? 'colspan=' . $parameterColumns : 'nowrap') . '>'
                 . $self->ConvertAmpChars($afterParams);

                if ($afterParams && $afterParams !~ /^[\)\]\}\>]/)
                    {  $output .= '&nbsp;';  };

            $output .=
            '</td>'
        . '</tr></table>'

        # Hack.
        . '</td></tr></table></blockquote>';
       };

    return $output;
    };


#
#   Function: BuildFooter
#
#   Builds and returns the HTML footer for the page.
#
#   Parameters:
#
#       multiline - Whether it should be formatted on multiple lines or not.
#
#   Dependencies:
#
#       <Builder::HTML::UpdateFile()> and <Builder::HTML::UpdateIndex()> require this section to be surrounded with the exact
#       strings "<div id=Footer>" and "</div><!--Footer-->".
#
sub BuildFooter #(bool multiline)
    {
    my ($self, $multiline) = @_;

    my $footer = NaturalDocs::Menu->Footer();
    my $timestamp = NaturalDocs::Menu->TimeStamp();
    my $divider;

    if ($multiline)
        {  $divider = '</p><p>';  }
    else
        {  $divider = '&nbsp; &middot;&nbsp; ';  };


    my $output = '<div id=Footer>';
    if ($multiline)
        {  $output .= '<p>';  };

    if (defined $footer)
        {
        $footer =~ s/\(c\)/&copy;/gi;
        $footer =~ s/\(tm\)/&trade;/gi;
        $footer =~ s/\(r\)/&reg;/gi;

        $output .= $footer . $divider;
        };

    if (defined $timestamp)
        {
        $output .= $timestamp . $divider;
        };

    $output .=
    '<a href="' . NaturalDocs::Settings->AppURL() . '">'
        . 'Generated by Natural Docs'
    . '</a>';

    if ($multiline)
        {  $output .= '</p>';  };

    $output .=
    '</div><!--Footer-->';

    return $output;
    };


#
#   Function: BuildToolTip
#
#   Builds the HTML for a symbol's tooltip and stores it in <tooltipHTML>.
#
#   Parameters:
#
#       symbol - The target <SymbolString>.
#       file - The <FileName> the target's defined in.
#       type - The symbol <TopicType>.
#       prototype - The target prototype, or undef for none.
#       summary - The target summary, or undef for none.
#
#   Returns:
#
#       If a tooltip is necessary for the link, returns the tooltip ID.  If not, returns undef.
#
sub BuildToolTip #(symbol, file, type, prototype, summary)
    {
    my ($self, $symbol, $file, $type, $prototype, $summary) = @_;

    if (defined $prototype || defined $summary)
        {
        my $htmlSymbol = $self->SymbolToHTMLSymbol($symbol);
        my $number = $tooltipSymbolsToNumbers{$htmlSymbol};

        if (!defined $number)
            {
            $number = $tooltipNumber;
            $tooltipNumber++;

            $tooltipSymbolsToNumbers{$htmlSymbol} = $number;

            $tooltipHTML .=
            '<div class=CToolTip id="tt' . $number . '">'
                . '<div class=C' . NaturalDocs::Topics->NameOfType($type, 0, 1) . '>';

            if (defined $prototype)
                {
                $tooltipHTML .= $self->BuildPrototype($type, $prototype, $file);
                };

            if (defined $summary)
                {
                # The fact that we don't have scope or using shouldn't matter because links shouldn't be included in the style anyway.
                $summary = $self->NDMarkupToHTML($file, $summary, undef, undef, $type, undef, NDMARKUPTOHTML_TOOLTIP);
                $tooltipHTML .= $summary;
                };

            $tooltipHTML .=
                '</div>'
            . '</div>';
            };

        return 'tt' . $number;
        }
    else
        {  return undef;  };
    };

#
#   Function: BuildToolTips
#
#   Builds and returns the tooltips for the page in HTML.
#
sub BuildToolTips
    {
    my $self = shift;
    return "\n<!--START_ND_TOOLTIPS-->\n" . $tooltipHTML . "<!--END_ND_TOOLTIPS-->\n\n";
    };

#
#   Function: BuildClassHierarchy
#
#   Builds and returns a class hierarchy diagram for the passed class, if applicable.
#
#   Parameters:
#
#       file - The source <FileName>.
#       class - The class <SymbolString> to build the hierarchy of.
#
sub BuildClassHierarchy #(file, symbol)
    {
    my ($self, $file, $symbol) = @_;

    my @parents = NaturalDocs::ClassHierarchy->ParentsOf($symbol);
    @parents = sort { ::StringCompare($a, $b) } @parents;

    my @children = NaturalDocs::ClassHierarchy->ChildrenOf($symbol);
    @children = sort { ::StringCompare($a, $b) } @children;

    if (!scalar @parents && !scalar @children)
        {  return undef;  };

    my $output =
    '<div class=ClassHierarchy>';

    if (scalar @parents)
        {
        $output .='<table border=0 cellspacing=0 cellpadding=0><tr><td>';

        foreach my $parent (@parents)
            {
            $output .= $self->BuildClassHierarchyEntry($file, $parent, 'CHParent', 1);
            };

        $output .= '</td></tr></table><div class=CHIndent>';
        };

    $output .=
    '<table border=0 cellspacing=0 cellpadding=0><tr><td>'
        . $self->BuildClassHierarchyEntry($file, $symbol, 'CHCurrent', undef)
    . '</td></tr></table>';

    if (scalar @children)
        {
        $output .=
        '<div class=CHIndent>'
            . '<table border=0 cellspacing=0 cellpadding=0><tr><td>';

        if (scalar @children <= 5)
            {
            for (my $i = 0; $i < scalar @children; $i++)
                {  $output .= $self->BuildClassHierarchyEntry($file, $children[$i], 'CHChild', 1);  };
            }
        else
            {
            for (my $i = 0; $i < 4; $i++)
                {  $output .= $self->BuildClassHierarchyEntry($file, $children[$i], 'CHChild', 1);  };

           $output .= '<div class=CHChildNote><div class=CHEntry>' . (scalar @children - 4) . ' other children</div></div>';
            };

        $output .=
        '</td></tr></table>'
        . '</div>';
        };

    if (scalar @parents)
        {  $output .= '</div>';  };

    $output .=
    '</div>';

    return $output;
    };


#
#   Function: BuildClassHierarchyEntry
#
#   Builds and returns a single class hierarchy entry.
#
#   Parameters:
#
#       file - The source <FileName>.
#       symbol - The class <SymbolString> whose entry is getting built.
#       style - The style to apply to the entry, such as <CHParent>.
#       link - Whether to build a link for this class or not.  When building the selected class' entry, this should be false.  It will
#               automatically handle whether the symbol is defined or not.
#
sub BuildClassHierarchyEntry #(file, symbol, style, link)
    {
    my ($self, $file, $symbol, $style, $link) = @_;

    my @identifiers = NaturalDocs::SymbolString->IdentifiersOf($symbol);
    my $name = join(NaturalDocs::Languages->LanguageOf($file)->PackageSeparator(), @identifiers);
    $name = $self->StringToHTML($name);

    my $output = '<div class=' . $style . '><div class=CHEntry>';

    if ($link)
        {
        my $target = NaturalDocs::SymbolTable->Lookup($symbol, $file);

        if (defined $target)
            {
            my $targetFile;

            if ($target->File() ne $file)
                {  $targetFile = $self->MakeRelativeURL( $self->OutputFileOf($file), $self->OutputFileOf($target->File()), 1 );  };
            # else leave it undef

            my $targetTooltipID = $self->BuildToolTip($symbol, $target->File(), $target->Type(),
                                                                          $target->Prototype(), $target->Summary());

            my $toolTipProperties = $self->BuildToolTipLinkProperties($targetTooltipID);

            $output .= '<a href="' . $targetFile . '#' . $self->SymbolToHTMLSymbol($symbol) . '" '
                            . 'class=L' . NaturalDocs::Topics->NameOfType($target->Type(), 0, 1) . ' ' . $toolTipProperties . '>'
                            . $name . '</a>';
            }
        else
            {  $output .= $name;  };
        }
    else
        {  $output .= $name;  };

    $output .= '</div></div>';
    return $output;
    };


#
#   Function: OpeningBrowserStyles
#
#   Returns the JavaScript that will add opening browser styles if necessary.
#
sub OpeningBrowserStyles
    {
    my $self = shift;

    return

    '<script language=JavaScript><!--' . "\n"

        # IE 4 and 5 don't understand 'undefined', so you can't say '!= undefined'.
        . 'if (browserType) {'
            . 'document.write("<div class=" + browserType + ">");'
            . 'if (browserVer) {'
                . 'document.write("<div class=" + browserVer + ">"); }'
            . '}'

    . '// --></script>';
    };


#
#   Function: ClosingBrowserStyles
#
#   Returns the JavaScript that will close browser styles if necessary.
#
sub ClosingBrowserStyles
    {
    my $self = shift;

    return

    '<script language=JavaScript><!--' . "\n"

        . 'if (browserType) {'
            . 'if (browserVer) {'
                . 'document.write("</div>"); }'
            . 'document.write("</div>");'
            . '}'

    . '// --></script>';
    };


#
#   Function: StandardComments
#
#   Returns the standard HTML comments that should be included in every generated file.  This includes <IEWebMark()>, so this
#   really is required for proper functionality.
#
sub StandardComments
    {
    my $self = shift;

    return "\n\n"

        . '<!--  Generated by Natural Docs, version ' . NaturalDocs::Settings->TextAppVersion() . ' -->' . "\n"
        . '<!--  ' . NaturalDocs::Settings->AppURL() . '  -->' . "\n\n"
        . $self->IEWebMark() . "\n\n";
    };


#
#   Function: IEWebMark
#
#   Returns the HTML comment necessary to get around the security warnings in IE starting with Windows XP Service Pack 2.
#
#   With this mark, the HTML page is treated as if it were in the Internet security zone instead of the Local Machine zone.  This
#   prevents the lockdown on scripting that causes an error message to appear with each page.
#
#   More Information:
#
#       - http://www.microsoft.com/technet/prodtechnol/winxppro/maintain/sp2brows.mspx#EHAA
#       - http://www.phdcc.com/xpsp2.htm#markoftheweb
#
sub IEWebMark
    {
    my $self = shift;

    return '<!-- saved from url=(0026)http://www.naturaldocs.org -->';
    };



###############################################################################
# Group: Index Functions


#
#   Function: BuildIndexPages
#
#   Builds an index file or files.
#
#   Parameters:
#
#       type - The <TopicType> the index is limited to, or undef for none.
#       indexSections  - An arrayref of sections, each section being an arrayref <NaturalDocs::SymbolTable::IndexElement>
#                               objects.  The first section is for symbols, the second for numbers, and the rest for A through Z.
#       beginIndexPage - All the content of the HTML page up to where the index content should appear.
#       endIndexPage - All the content of the HTML page past where the index should appear.
#       beginSearchResultsPage - All the content of the HTML page up to where the search results content should appear.
#       endSearchResultsPage - All the content of the HTML page past where the search results content should appear.
#
#   Returns:
#
#       The number of pages in the index.
#
sub BuildIndexPages #(TopicType type, NaturalDocs::SymbolTable::IndexElement[] indexSections, string beginIndexPage, string endIndexPage, string beginSearchResultsPage, string endSearchResultsPage) => int
    {
    my ($self, $type, $indexSections, $beginIndexPage, $endIndexPage, $beginSearchResultsPage, $endSearchResultsPage) = @_;


    # Build the content.

    my ($indexHTMLSections, $tooltipHTMLSections, $searchResultsHTMLSections) = $self->BuildIndexSections($indexSections);


    # Generate the search result pages.

   for (my $i = 0; $i < 28; $i++)
        {
        if ($searchResultsHTMLSections->[$i])
            {
            my $searchResultsFileName = $self->SearchResultsFileOf($type, $searchExtensions[$i]);

            open(INDEXFILEHANDLE, '>' . $searchResultsFileName)
                or die "Couldn't create output file " . $searchResultsFileName . ".\n";

		    binmode(INDEXFILEHANDLE, ':encoding(UTF-8)');

            print INDEXFILEHANDLE

            $beginSearchResultsPage

            . '<div class=SRStatus id=Loading>Loading...</div>'

            . '<table border=0 cellspacing=0 cellpadding=0>'
                . $searchResultsHTMLSections->[$i]
            . '</table>'

            . '<div class=SRStatus id=Searching>Searching...</div>'
            . '<div class=SRStatus id=NoMatches>No Matches</div>'

            . '<script type="text/javascript"><!--' . "\n"
                . 'document.getElementById("Loading").style.display="none";' . "\n"
                . 'document.getElementById("NoMatches").style.display="none";' . "\n"

                . 'var searchResults = new SearchResults("searchResults", "' . $self->CommandLineOption() . '");' . "\n"
                . 'searchResults.Search();' . "\n"
            . '--></script>'

            . $endSearchResultsPage;

            close(INDEXFILEHANDLE);
            };
        };

    if (!$self->MadeEmptySearchResultsPage())
        {
        my $emptySearchResultsFileName = NaturalDocs::File->JoinPaths( $self->SearchResultsDirectory(), 'NoResults.html' );

        open(INDEXFILEHANDLE, '>' . $emptySearchResultsFileName)
            or die "Couldn't create output file " . $emptySearchResultsFileName . ".\n";

	    binmode(INDEXFILEHANDLE, ':encoding(UTF-8)');

        print INDEXFILEHANDLE

        $beginSearchResultsPage
        . '<div class=SRStatus id=NoMatches>No Matches</div>'
        . $endSearchResultsPage;

        close(INDEXFILEHANDLE);

        $self->SetMadeEmptySearchResultsPage(1);
        };


    # Generate the index pages.

    my $page = 1;
    my $pageSize = 0;
    my @pageLocations;

    # The maximum page size acceptable before starting a new page.  Note that this doesn't include beginPage and endPage,
    # because we don't want something like a large menu screwing up the calculations.
    use constant PAGESIZE_LIMIT => 50000;


    # File the pages.

    for (my $i = 0; $i < scalar @$indexHTMLSections; $i++)
        {
        if (!defined $indexHTMLSections->[$i])
            {  next;  };

        $pageSize += length($indexHTMLSections->[$i]) + length($tooltipHTMLSections->[$i]);
        $pageLocations[$i] = $page;

        if ($pageSize + length($indexHTMLSections->[$i+1]) + length($tooltipHTMLSections->[$i+1]) > PAGESIZE_LIMIT)
            {
            $page++;
            $pageSize = 0;
            };
        };


    # Build the pages.

    my $indexFileName;
    $page = -1;
    my $oldPage = -1;
    my $tooltips;
    my $firstHeading;

    for (my $i = 0; $i < scalar @$indexHTMLSections; $i++)
        {
        if (!defined $indexHTMLSections->[$i])
            {  next;  };

        $page = $pageLocations[$i];

        # Switch files if we need to.

        if ($page != $oldPage)
            {
            if ($oldPage != -1)
                {
                print INDEXFILEHANDLE '</table>' . $tooltips . $endIndexPage;
                close(INDEXFILEHANDLE);
                $tooltips = undef;
                };

            $indexFileName = $self->IndexFileOf($type, $page);

            open(INDEXFILEHANDLE, '>' . $indexFileName)
                or die "Couldn't create output file " . $indexFileName . ".\n";

		    binmode(INDEXFILEHANDLE, ':encoding(UTF-8)');

            print INDEXFILEHANDLE $beginIndexPage . $self->BuildIndexNavigationBar($type, $page, \@pageLocations)
                                              . '<table border=0 cellspacing=0 cellpadding=0>';

            $oldPage = $page;
            $firstHeading = 1;
            };

        print INDEXFILEHANDLE
        '<tr>'
            . '<td class=IHeading' . ($firstHeading ? ' id=IFirstHeading' : '') . '>'
                . '<a name="' . $indexAnchors[$i] . '"></a>'
                 . $indexHeadings[$i]
            . '</td>'
            . '<td></td>'
        . '</tr>'

        . $indexHTMLSections->[$i];

        $firstHeading = 0;
        $tooltips .= $tooltipHTMLSections->[$i];
        };

    if ($page != -1)
        {
        print INDEXFILEHANDLE '</table>' . $tooltips . $endIndexPage;
        close(INDEXFILEHANDLE);
        }

    # Build a dummy page so there's something at least.
    else
        {
        $indexFileName = $self->IndexFileOf($type, 1);

        open(INDEXFILEHANDLE, '>' . $indexFileName)
            or die "Couldn't create output file " . $indexFileName . ".\n";

    	binmode(INDEXFILEHANDLE, ':encoding(UTF-8)');

        print INDEXFILEHANDLE
            $beginIndexPage
            . $self->BuildIndexNavigationBar($type, 1, \@pageLocations)
            . 'There are no entries in the ' . lc( NaturalDocs::Topics->NameOfType($type) ) . ' index.'
            . $endIndexPage;

        close(INDEXFILEHANDLE);
        };


    return $page;
    };


#
#   Function: BuildIndexSections
#
#   Builds and returns the index and search results sections in HTML.
#
#   Parameters:
#
#       index  - An arrayref of sections, each section being an arrayref <NaturalDocs::SymbolTable::IndexElement> objects.
#                   The first section is for symbols, the second for numbers, and the rest for A through Z.
#
#   Returns:
#
#       The arrayref ( indexSections, tooltipSections, searchResultsSections ).
#
#       Index 0 is the symbols, index 1 is the numbers, and each following index is A through Z.  The content of each section
#       is its HTML, or undef if there is nothing for that section.
#
sub BuildIndexSections #(NaturalDocs::SymbolTable::IndexElement[] index) => ( string[], string[], string[] )
    {
    my ($self, $indexSections) = @_;

    $self->ResetToolTips();
    %searchResultIDs = ( );

    my $contentSections = [ ];
    my $tooltipSections = [ ];
    my $searchResultsSections = [ ];

    for (my $section = 0; $section < scalar @$indexSections; $section++)
        {
        if (defined $indexSections->[$section])
            {
            my $total = scalar @{$indexSections->[$section]};

            for (my $i = 0; $i < $total; $i++)
                {
                my $id;

                if ($i == 0)
                    {
                    if ($total == 1)
                        {  $id = 'IOnlySymbolPrefix';  }
                    else
                        {  $id = 'IFirstSymbolPrefix';  };
                    }
                elsif ($i == $total - 1)
                    {  $id = 'ILastSymbolPrefix';  };

                my ($content, $searchResult) = $self->BuildIndexElement($indexSections->[$section]->[$i], $id);
                $contentSections->[$section] .= $content;
                $searchResultsSections->[$section] .= $searchResult;
                };

            $tooltipSections->[$section] .= $self->BuildToolTips();
            $self->ResetToolTips(1);
            };
        };


    return ( $contentSections, $tooltipSections, $searchResultsSections );
    };


#
#   Function: BuildIndexElement
#
#   Converts a <NaturalDocs::SymbolTable::IndexElement> to HTML and returns it.  It will handle all sub-elements automatically.
#
#   Parameters:
#
#       element - The <NaturalDocs::SymbolTable::IndexElement> to build.
#       cssID - The CSS ID to apply to the prefix.
#
#   Recursion-Only Parameters:
#
#       These parameters are used internally for recursion, and should not be set.
#
#       symbol - If the element is below symbol level, the <SymbolString> to use.
#       package - If the element is below package level, the package <SymbolString> to use.
#       hasPackage - Whether the element is below package level.  Is necessary because package may need to be undef.
#
#   Returns:
#
#       The array ( indexHTML, searchResultHTML ) which is the element in the respective HTML forms.
#
sub BuildIndexElement #(NaturalDocs::SymbolTable::IndexElement element, string cssID, SymbolString symbol, SymbolString package, bool hasPackage) => ( string, string )
    {
    my ($self, $element, $cssID, $symbol, $package, $hasPackage) = @_;


    # If we're doing a file sub-index entry...

    if ($hasPackage)
        {
        my ($inputDirectory, $relativePath) = NaturalDocs::Settings->SplitFromInputDirectory($element->File());

        return $self->BuildIndexLink($self->StringToHTML($relativePath, ADD_HIDDEN_BREAKS), $symbol,
                                                                                 $package, $element->File(), $element->Type(),
                                                                                 $element->Prototype(), $element->Summary(), 'IFile');
        }


    # If we're doing a package sub-index entry...

    elsif (defined $symbol)

        {
        my $text;

        if ($element->Package())
            {
            $text = NaturalDocs::SymbolString->ToText($element->Package(), $element->PackageSeparator());
            $text = $self->StringToHTML($text, ADD_HIDDEN_BREAKS);
            }
        else
            {  $text = 'Global';  };

        if (!$element->HasMultipleFiles())
            {
            return $self->BuildIndexLink($text, $symbol, $element->Package(), $element->File(), $element->Type(),
                                                      $element->Prototype(), $element->Summary(), 'IParent');
            }

        else
            {
            my $indexHTML =
            '<span class=IParent>'
                . $text
            . '</span>'
            . '<div class=ISubIndex>';

            my $searchResultHTML = $indexHTML;

            my $fileElements = $element->File();
            foreach my $fileElement (@$fileElements)
                {
                my ($i, $s) = $self->BuildIndexElement($fileElement, $cssID, $symbol, $element->Package(), 1);
                $indexHTML .= $i;
                $searchResultHTML .= $s;
                };

            $indexHTML .= '</div>';
            $searchResultHTML .= '</div>';

            return ($indexHTML, $searchResultHTML);
            };
        }


    # If we're doing a top-level symbol entry...

    else
        {
        my $symbolText = $self->StringToHTML($element->SortableSymbol(), ADD_HIDDEN_BREAKS);
        my $symbolPrefix = $self->StringToHTML($element->IgnoredPrefix());
        my $searchResultID = $self->StringToSearchResultID($element->SortableSymbol());

        my $indexHTML =
        '<tr>'
            . '<td class=ISymbolPrefix' . ($cssID ? ' id=' . $cssID : '') . '>'
                . ($symbolPrefix || '&nbsp;')
            . '</td><td class=IEntry>';

        my $searchResultsHTML =
        '<div class=SRResult id=' . $searchResultID . '><div class=IEntry>';

            if ($symbolPrefix)
                {  $searchResultsHTML .= '<span class=ISymbolPrefix>' . $symbolPrefix . '</span>';  };

        if (!$element->HasMultiplePackages())
            {
            my $packageText;

            if (defined $element->Package())
                {
                $packageText = NaturalDocs::SymbolString->ToText($element->Package(), $element->PackageSeparator());
                $packageText = $self->StringToHTML($packageText, ADD_HIDDEN_BREAKS);
                };

            if (!$element->HasMultipleFiles())
                {
                my ($i, $s) =
                    $self->BuildIndexLink($symbolText, $element->Symbol(), $element->Package(), $element->File(),
                                                     $element->Type(), $element->Prototype(), $element->Summary(), 'ISymbol');
                $indexHTML .= $i;
                $searchResultsHTML .= $s;

                if (defined $packageText)
                    {
                    $indexHTML .=
                    ', <span class=IParent>'
                        . $packageText
                    . '</span>';

                    $searchResultsHTML .=
                    ', <span class=IParent>'
                        . $packageText
                    . '</span>';
                    };
                }
            else # hasMultipleFiles but not multiplePackages
                {
                $indexHTML .=
                '<span class=ISymbol>'
                    . $symbolText
                . '</span>';

                $searchResultsHTML .=
                q{<a href="javascript:searchResults.Toggle('} . $searchResultID . q{')" class=ISymbol>}
                    . $symbolText
                . '</a>';

                my $output;

                if (defined $packageText)
                    {
                    $output .=
                    ', <span class=IParent>'
                        . $packageText
                    . '</span>';
                    };

                $output .=
                '<div class=ISubIndex>';

                $indexHTML .= $output;
                $searchResultsHTML .= $output;

                my $fileElements = $element->File();
                foreach my $fileElement (@$fileElements)
                    {
                    my ($i, $s) = $self->BuildIndexElement($fileElement, $cssID, $element->Symbol(), $element->Package(), 1);
                    $indexHTML .= $i;
                    $searchResultsHTML .= $s;
                    };

                $indexHTML .= '</div>';
                $searchResultsHTML .= '</div>';
                };
            }

        else # hasMultiplePackages
            {
            $indexHTML .=
            '<span class=ISymbol>'
                . $symbolText
            . '</span>'
            . '<div class=ISubIndex>';

            $searchResultsHTML .=
            q{<a href="javascript:searchResults.Toggle('} . $searchResultID . q{')" class=ISymbol>}
                . $symbolText
            . '</a>'
            . '<div class=ISubIndex>';

            my $packageElements = $element->Package();
            foreach my $packageElement (@$packageElements)
                {
                my ($i, $s) = $self->BuildIndexElement($packageElement, $cssID, $element->Symbol());
                $indexHTML .= $i;
                $searchResultsHTML .= $s;
                };

            $indexHTML .= '</div>';
            $searchResultsHTML .= '</div>';
            };

        $indexHTML .= '</td></tr>';
        $searchResultsHTML .= '</div></div>';

        return ($indexHTML, $searchResultsHTML);
        };
    };


#
#   Function: BuildIndexLink
#
#   Builds and returns the HTML associated with an index link.  The HTML will be the a href tag, the text, and the closing tag.
#
#   Parameters:
#
#       text - The text of the link *in HTML*.  Use <IndexSymbolToHTML()> if necessary.
#       symbol - The partial <SymbolString> to link to.
#       package - The package <SymbolString> of the symbol.
#       file - The <FileName> the symbol is defined in.
#       type - The <TopicType> of the symbol.
#       prototype - The prototype of the symbol, or undef if none.
#       summary - The summary of the symbol, or undef if none.
#       style - The CSS style to apply to the link.
#
#   Returns:
#
#       The array ( indexHTML, searchResultHTML ) which is the link in the respective forms.
#
sub BuildIndexLink #(string text, SymbolString symbol, SymbolString package, FileName file, TopicType type, string prototype, string summary, string style) => ( string, string )
    {
    my ($self, $text, $symbol, $package, $file, $type, $prototype, $summary, $style) = @_;

    $symbol = NaturalDocs::SymbolString->Join($package, $symbol);

    my $targetTooltipID = $self->BuildToolTip($symbol, $file, $type, $prototype, $summary);
    my $toolTipProperties = $self->BuildToolTipLinkProperties($targetTooltipID);

    my $indexHTML = '<a href="' . $self->MakeRelativeURL( $self->IndexDirectory(), $self->OutputFileOf($file) )
                                         . '#' . $self->SymbolToHTMLSymbol($symbol) . '" ' . $toolTipProperties . ' '
                                . 'class=' . $style . '>' . $text . '</a>';
    my $searchResultHTML = '<a href="' . $self->MakeRelativeURL( $self->SearchResultsDirectory(), $self->OutputFileOf($file) )
                                         . '#' . $self->SymbolToHTMLSymbol($symbol) . '" '
                                         . ($self->CommandLineOption eq 'HTML' ? 'target=_parent ' : '')
                                . 'class=' . $style . '>' . $text . '</a>';

    return ($indexHTML, $searchResultHTML);
    };


#
#   Function: BuildIndexNavigationBar
#
#   Builds a navigation bar for a page of the index.
#
#   Parameters:
#
#       type - The <TopicType> of the index, or undef for general.
#       page - The page of the index the navigation bar is for.
#       locations - An arrayref of the locations of each section.  Index 0 is for the symbols, index 1 for the numbers, and the rest
#                       for each letter.  The values are the page numbers where the sections are located.
#
sub BuildIndexNavigationBar #(type, page, locations)
    {
    my ($self, $type, $page, $locations) = @_;

    my $output = '<div class=INavigationBar>';

    for (my $i = 0; $i < scalar @indexHeadings; $i++)
        {
        if ($i != 0)
            {  $output .= ' &middot; ';  };

        if (defined $locations->[$i])
            {
            $output .= '<a href="';

            if ($locations->[$i] != $page)
                {  $output .= $self->RelativeIndexFileOf($type, $locations->[$i]);  };

            $output .= '#' . $indexAnchors[$i] . '">' . $indexHeadings[$i] . '</a>';
            }
        else
            {
            $output .= $indexHeadings[$i];
            };
        };

    $output .= '</div>';

    return $output;
    };



###############################################################################
# Group: File Functions


#
#   Function: PurgeIndexFiles
#
#   Removes all or some of the output files for an index.
#
#   Parameters:
#
#       type  - The index <TopicType>.
#       indexSections  - An arrayref of sections, each section being an arrayref <NaturalDocs::SymbolTable::IndexElement>
#                               objects.  The first section is for symbols, the second for numbers, and the rest for A through Z.  May be
#                               undef.
#       startingPage - If defined, only pages starting with this number will be removed.  Otherwise all pages will be removed.
#
sub PurgeIndexFiles #(TopicType type, optional NaturalDocs::SymbolTable::IndexElement[] indexSections, optional int startingPage)
    {
    my ($self, $type, $indexSections, $page) = @_;

    # First the regular index pages.

    if (!defined $page)
        {  $page = 1;  };

    for (;;)
        {
        my $file = $self->IndexFileOf($type, $page);

        if (-e $file)
            {
            unlink($file);
            $page++;
            }
        else
            {
            last;
            };
        };


    # Next the search results.

    for (my $i = 0; $i < 28; $i++)
        {
        if (!$indexSections || !$indexSections->[$i])
            {
            my $file = $self->SearchResultsFileOf($type, $searchExtensions[$i]);

            if (-e $file)
                {  unlink($file);  };
            };
        };
    };


#
#   Function: OutputFileOf
#
#   Returns the output file name of the source file.  Will be undef if it is not a file from a valid input directory.
#
sub OutputFileOf #(sourceFile)
    {
    my ($self, $sourceFile) = @_;

    my ($inputDirectory, $relativeSourceFile) = NaturalDocs::Settings->SplitFromInputDirectory($sourceFile);
    if (!defined $inputDirectory)
        {  return undef;  };

    my $outputDirectory = NaturalDocs::Settings->OutputDirectoryOf($self);
    my $inputDirectoryName = NaturalDocs::Settings->InputDirectoryNameOf($inputDirectory);

    $outputDirectory = NaturalDocs::File->JoinPaths( $outputDirectory,
                                                                            'files' . ($inputDirectoryName != 1 ? $inputDirectoryName : ''), 1 );

    # We need to change any extensions to dashes because Apache will think file.pl.html is a script.
    # We also need to add a dash if the file doesn't have an extension so there'd be no conflicts with index.html,
    # FunctionIndex.html, etc.

    if (!($relativeSourceFile =~ tr/./-/))
        {  $relativeSourceFile .= '-';  };

    $relativeSourceFile =~ tr/ &?(){};#/_/;
    $relativeSourceFile .= '.html';

    return NaturalDocs::File->JoinPaths($outputDirectory, $relativeSourceFile);
    };


#
#   Function: OutputImageOf
#
#   Returns the output image file name of the source image file.  Will be undef if it is not a file from a valid input directory.
#
sub OutputImageOf #(sourceImageFile)
    {
    my ($self, $sourceImageFile) = @_;

    my $outputDirectory = NaturalDocs::Settings->OutputDirectoryOf($self);
    my $topLevelDirectory;

    my ($inputDirectory, $relativeImageFile) = NaturalDocs::Settings->SplitFromInputDirectory($sourceImageFile);

    if (defined $inputDirectory)
        {
        my $inputDirectoryName = NaturalDocs::Settings->InputDirectoryNameOf($inputDirectory);
        $topLevelDirectory = 'files' . ($inputDirectoryName != 1 ? $inputDirectoryName : '');
        }
    else
        {
        ($inputDirectory, $relativeImageFile) = NaturalDocs::Settings->SplitFromImageDirectory($sourceImageFile);

        if (!defined $inputDirectory)
            {  return undef;  };

        my $inputDirectoryName = NaturalDocs::Settings->ImageDirectoryNameOf($inputDirectory);
        $topLevelDirectory = 'images' . ($inputDirectoryName != 1 ? $inputDirectoryName : '');
        }


    $outputDirectory = NaturalDocs::File->JoinPaths($outputDirectory, $topLevelDirectory, 1);

    $relativeImageFile =~ tr/ /_/;

    return NaturalDocs::File->JoinPaths($outputDirectory, $relativeImageFile);
    };


#
#   Function: IndexDirectory
#
#   Returns the directory of the index files.
#
sub IndexDirectory
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'index', 1);
    };


#
#   Function: IndexFileOf
#
#   Returns the output file name of the index file.
#
#   Parameters:
#
#       type  - The <TopicType> of the index.
#       page  - The page number.  Undef is the same as one.
#
sub IndexFileOf #(type, page)
    {
    my ($self, $type, $page) = @_;
    return NaturalDocs::File->JoinPaths( $self->IndexDirectory(), $self->RelativeIndexFileOf($type, $page) );
    };


#
#   Function: RelativeIndexFileOf
#
#   Returns the output file name of the index file, relative to other index files.
#
#   Parameters:
#
#       type  - The <TopicType> of the index.
#       page  - The page number.  Undef is the same as one.
#
sub RelativeIndexFileOf #(type, page)
    {
    my ($self, $type, $page) = @_;
    return NaturalDocs::Topics->NameOfType($type, 1, 1) . (defined $page && $page != 1 ? $page : '') . '.html';
    };


#
#   Function: SearchResultsDirectory
#
#   Returns the directory of the search results files.
#
sub SearchResultsDirectory
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'search', 1);
    };


#
#   Function: SearchResultsFileOf
#
#   Returns the output file name of the search result file.
#
#   Parameters:
#
#       type  - The <TopicType> of the index.
#       extra - The string to add to the end of the file name, such as "A" or "Symbols".
#
sub SearchResultsFileOf #(TopicType type, string extra)
    {
    my ($self, $type, $extra) = @_;

    my $fileName = NaturalDocs::Topics->NameOfType($type, 1, 1) . $extra . '.html';

    return NaturalDocs::File->JoinPaths( $self->SearchResultsDirectory(), $fileName );
    };


#
#   Function: CSSDirectory
#
#   Returns the directory of the CSS files.
#
sub CSSDirectory
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'styles', 1);
    };


#
#   Function: MainCSSFile
#
#   Returns the location of the main CSS file.
#
sub MainCSSFile
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( $self->CSSDirectory(), 'main.css' );
    };


#
#   Function: JavaScriptDirectory
#
#   Returns the directory of the JavaScript files.
#
sub JavaScriptDirectory
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'javascript', 1);
    };


#
#   Function: MainJavaScriptFile
#
#   Returns the location of the main JavaScript file.
#
sub MainJavaScriptFile
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( $self->JavaScriptDirectory(), 'main.js' );
    };


#
#   Function: PrettifyJavaScriptFile
#
#   Returns the location of the Google Prettify JavaScript file.
#
sub PrettifyJavaScriptFile
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( $self->JavaScriptDirectory(), 'prettify.js' );
    };


#
#   Function: SearchDataJavaScriptFile
#
#   Returns the location of the search data JavaScript file.
#
sub SearchDataJavaScriptFile
    {
    my $self = shift;
    return NaturalDocs::File->JoinPaths( $self->JavaScriptDirectory(), 'searchdata.js' );
    };



###############################################################################
# Group: Support Functions


#
#   Function: IndexTitleOf
#
#   Returns the page title of the index file.
#
#   Parameters:
#
#       type  - The type of index.
#
sub IndexTitleOf #(type)
    {
    my ($self, $type) = @_;

    return ($type eq ::TOPIC_GENERAL() ? '' : NaturalDocs::Topics->NameOfType($type) . ' ') . 'Index';
    };

#
#   Function: MakeRelativeURL
#
#   Returns a relative path between two files in the output tree and returns it in URL format.
#
#   Parameters:
#
#       baseFile    - The base <FileName> in local format, *not* in URL format.
#       targetFile  - The target <FileName> of the link in local format, *not* in URL format.
#       baseHasFileName - Whether baseFile has a file name attached or is just a path.
#
#   Returns:
#
#       The relative URL to the target.
#
sub MakeRelativeURL #(FileName baseFile, FileName targetFile, bool baseHasFileName) -> string relativeURL
    {
    my ($self, $baseFile, $targetFile, $baseHasFileName) = @_;

    if ($baseHasFileName)
        {  $baseFile = NaturalDocs::File->NoFileName($baseFile)  };

    my $relativePath = NaturalDocs::File->MakeRelativePath($baseFile, $targetFile);

    return $self->ConvertAmpChars( NaturalDocs::File->ConvertToURL($relativePath) );
    };

#
#   Function: StringToHTML
#
#   Converts a text string to HTML.  Does not apply paragraph tags or accept formatting tags.
#
#   Parameters:
#
#       string - The string to convert.
#       addHiddenBreaks - Whether to add hidden breaks to the string.  You can use <ADD_HIDDEN_BREAKS> for this parameter
#                                   if you want to make the calling code clearer.
#
#   Returns:
#
#       The string in HTML.
#
sub StringToHTML #(string, addHiddenBreaks)
    {
    my ($self, $string, $addHiddenBreaks) = @_;

    $string =~ s/&/&amp;/g;
    $string =~ s/</&lt;/g;
    $string =~ s/>/&gt;/g;

    # Me likey the fancy quotes.  They work in IE 4+, Mozilla, and Opera 5+.  We've already abandoned NS4 with the CSS
    # styles, so might as well.
    $string =~ s/^\'/&lsquo;/gm;
    $string =~ s/([\ \(\[\{])\'/$1&lsquo;/g;
    $string =~ s/\'/&rsquo;/g;

    $string =~ s/^\"/&ldquo;/gm;
    $string =~ s/([\ \(\[\{])\"/$1&ldquo;/g;
    $string =~ s/\"/&rdquo;/g;

    # Me likey the double spaces too.  As you can probably tell, I like print-formatting better than web-formatting.  The indented
    # paragraphs without blank lines in between them do become readable when you have fancy quotes and double spaces too.
    $string = $self->AddDoubleSpaces($string);

    if ($addHiddenBreaks)
        {  $string = $self->AddHiddenBreaks($string);  };

    return $string;
    };


#
#   Function: SymbolToHTMLSymbol
#
#   Converts a <SymbolString> to a HTML symbol, meaning one that is safe to include in anchor and link tags.  You don't need
#   to pass the result to <ConvertAmpChars()>.
#
sub SymbolToHTMLSymbol #(symbol)
    {
    my ($self, $symbol) = @_;

    my @identifiers = NaturalDocs::SymbolString->IdentifiersOf($symbol);
    my $htmlSymbol = join('.', @identifiers);

    # If only Mozilla was nice about putting special characters in URLs like IE and Opera are, I could leave spaces in and replace
    # "<>& with their amp chars.  But alas, Mozilla shows them as %20, etc. instead.  It would have made for nice looking URLs.
    $htmlSymbol =~ tr/ \"<>\?&%/_/d;

    return $htmlSymbol;
    };


#
#   Function: StringToSearchResultID
#
#   Takes a text string and translates it into something that can be used as a CSS ID.
#
#   Parameters:
#
#       string - The string to convert
#       dontIncrement - If set, it reuses the last generated ID.  Otherwise it generates a new one if it matches a previously
#                               generated one in a case-insensitive way.
#
sub StringToSearchResultID #(string string, bool dontIncrement = 0) => string
    {
    my ($self, $string, $dontIncrement) = @_;

    $string =~ s/\_/_und/g;
    $string =~ s/ +/_spc/g;

    my %translation = ( '~' => '_til', '!' => '_exc', '@' => '_att', '#' => '_num', '$' => '_dol', '%' => '_pct', '^' => '_car',
                                  '&' => '_amp', '*' => '_ast', '(' => '_lpa', ')' => '_rpa', '-' => '_min', '+' => '_plu', '=' => '_equ',
                                  '{' => '_lbc', '}' => '_rbc', '[' => '_lbk', ']' => '_rbk', ':' => '_col', ';' => '_sco', '"' => '_quo',
                                  '\'' => '_apo', '<' => '_lan', '>' => '_ran', ',' => '_com', '.' => '_per', '?' => '_que', '/' => '_sla' );

    $string =~ s/([\~\!\@\#\$\%\^\&\*\(\)\-\+\=\{\}\[\]\:\;\"\'\<\>\,\.\?\/])/$translation{$1}/ge;
    $string =~ s/[^a-z0-9_]/_zzz/gi;

    my $number = $searchResultIDs{lc($string)};

    if (!$number)
        {  $number = 1;  }
    elsif (!$dontIncrement)
        {  $number++;  };

    $searchResultIDs{lc($string)} = $number;

    return 'SR' . ($number == 1 ? '' : $number) . '_' . $string;
    };


#
#   Function: NDMarkupToHTML
#
#   Converts a block of <NDMarkup> to HTML.
#
#   Parameters:
#
#       sourceFile - The source <FileName> the <NDMarkup> appears in.
#       text    - The <NDMarkup> text to convert.
#       symbol - The topic <SymbolString> the <NDMarkup> appears in.
#       package  - The package <SymbolString> the <NDMarkup> appears in.
#       type - The <TopicType> the <NDMarkup> appears in.
#       using - An arrayref of scope <SymbolStrings> the <NDMarkup> also has access to, or undef if none.
#       style - Set to one of the <NDMarkupToHTML Styles> or leave undef for general.
#
#   Returns:
#
#       The text in HTML.
#
sub NDMarkupToHTML #(sourceFile, text, symbol, package, type, using, style)
    {
    my ($self, $sourceFile, $text, $symbol, $package, $type, $using, $style) = @_;

    my $dlSymbolBehavior;

    if ($type eq ::TOPIC_ENUMERATION())
        {  $dlSymbolBehavior = NaturalDocs::Languages->LanguageOf($sourceFile)->EnumValues();  }
   elsif (NaturalDocs::Topics->TypeInfo($type)->Scope() == ::SCOPE_ALWAYS_GLOBAL())
        {  $dlSymbolBehavior = ::ENUM_GLOBAL();  }
    else
        {  $dlSymbolBehavior = ::ENUM_UNDER_PARENT();  };

    my $output;
    my $inCode;

    my @splitText = split(/(<\/?code(?: type="[^"]+")?>)/, $text);

    while (scalar @splitText)
        {
        $text = shift @splitText;

        if ($text =~ /<code type="([^"]+)">/)
            {
            my $codeType = $1;

            my $highlight = ( ($codeType eq "code" && NaturalDocs::Settings->HighlightCode()) ||
            						  ($codeType eq "anonymous" && NaturalDocs::Settings->HighlightAnonymous()) );

            $output .= '<blockquote><pre' . ($highlight ? ' class="prettyprint"' : '') . '>';
            $inCode = 1;
            }
        elsif ($text eq '</code>')
            {
            $output .= '</pre></blockquote>';
            $inCode = undef;
            }
        elsif ($inCode)
            {
            # Leave line breaks in.
            $output .= $text;
            }
        else
            {
            # Format non-code text.

            # Convert linked images.
            if ($text =~ /<img mode=\"link\"/)
                {
                if ($style == NDMARKUPTOHTML_GENERAL)
                    {
                    # Split by tags we would want to see the linked images appear after.  For example, an image link appearing in
                    # the middle of a paragraph would appear after the end of that paragraph.
                    my @imageBlocks = split(/(<p>.*?<\/p>|<dl>.*?<\/dl>|<ul>.*?<\/ul>)/, $text);
                    $text = undef;

                    foreach my $imageBlock (@imageBlocks)
                        {
                        $imageBlock =~ s{<img mode=\"link\" target=\"([^\"]*)\" original=\"([^\"]*)\">}
                                                {$self->BuildImage($sourceFile, 'link', $1, $2)}ge;

                        $text .= $imageBlock . $imageContent;
                        $imageContent = undef;
                        };
                    }

                # Use only the text for tooltips and summaries.
                else
                    {
                    $text =~ s{<img mode=\"link\" target=\"[^\"]*\" original=\"([^\"]*)\">}{$1}g;
                    };
                };

            # Convert quotes to fancy quotes.  This has to be done before links because some of them may have JavaScript
            # attributes that use the apostrophe character.
            $text =~ s/^\'/&lsquo;/gm;
            $text =~ s/([\ \(\[\{])\'/$1&lsquo;/g;
            $text =~ s/\'/&rsquo;/g;

            $text =~ s/^&quot;/&ldquo;/gm;
            $text =~ s/([\ \(\[\{])&quot;/$1&ldquo;/g;
            $text =~ s/&quot;/&rdquo;/g;

            # Resolve and convert links, except for tooltips.
            if ($style != NDMARKUPTOHTML_TOOLTIP)
                {
                $text =~ s{<link target=\"([^\"]*)\" name=\"([^\"]*)\" original=\"([^\"]*)\">}
                               {$self->BuildTextLink($1, $2, $3, $package, $using, $sourceFile)}ge;
                $text =~ s/<url target=\"([^\"]*)\" name=\"([^\"]*)\">/$self->BuildURLLink($1, $2)/ge;
                }
            else
                {
                $text =~ s{<link target=\"[^\"]*\" name=\"([^\"]*)\" original=\"[^\"]*\">}{$1}g;
                $text =~ s{<url target=\"[^\"]*\" name=\"([^\"]*)\">}{$1}g;
                };

            # We do full e-mail links anyway just so the obfuscation remains.
            $text =~ s/<email target=\"([^\"]*)\" name=\"([^\"]*)\">/$self->BuildEMailLink($1, $2)/ge;


            # Convert inline images, but only for the general style.
            if ($style == NDMARKUPTOHTML_GENERAL)
                {
                $text =~ s{<img mode=\"inline\" target=\"([^\"]*)\" original=\"([^\"]*)\">}
                               {$self->BuildImage($sourceFile, 'inline', $1, $2)}ge;
                }
            else
                {
                $text =~ s{<img mode=\"inline\" target=\"[^\"]*\" original=\"([^\"]*)\">}{$1}g;
                };

            # Copyright symbols.  Prevent conversion when part of (a), (b), (c) lists.
            if ($text !~ /\(a\)/i)
                {  $text =~ s/\(c\)/&copy;/gi;  };

            # Trademark symbols.
            $text =~ s/\(tm\)/&trade;/gi;
            $text =~ s/\(r\)/&reg;/gi;

            # Add double spaces too.
            $text = $self->AddDoubleSpaces($text);

            # Headings
            $text =~ s/<h>/<h4 class=CHeading>/g;
            $text =~ s/<\/h>/<\/h4>/g;

            # Description Lists
            $text =~ s/<dl>/<table border=0 cellspacing=0 cellpadding=0 class=CDescriptionList>/g;
            $text =~ s/<\/dl>/<\/table>/g;

            $text =~ s/<de>/<tr><td class=CDLEntry>/g;
            $text =~ s/<\/de>/<\/td>/g;

            if ($dlSymbolBehavior == ::ENUM_GLOBAL())
                {  $text =~ s/<ds>([^<]+)<\/ds>/$self->MakeDescriptionListSymbol(undef, $1)/ge;  }
            elsif ($dlSymbolBehavior == ::ENUM_UNDER_PARENT())
                {  $text =~ s/<ds>([^<]+)<\/ds>/$self->MakeDescriptionListSymbol($package, $1)/ge;  }
            else # ($dlSymbolBehavior == ::ENUM_UNDER_TYPE())
                {  $text =~ s/<ds>([^<]+)<\/ds>/$self->MakeDescriptionListSymbol($symbol, $1)/ge;  }

            sub MakeDescriptionListSymbol #(package, text)
                {
                my ($self, $package, $text) = @_;

                $text = NaturalDocs::NDMarkup->RestoreAmpChars($text);
                my $symbol = NaturalDocs::SymbolString->FromText($text);

                if (defined $package)
                    {  $symbol = NaturalDocs::SymbolString->Join($package, $symbol);  };

                return
                '<tr>'
                    . '<td class=CDLEntry>'
                        # The anchors are closed, but not around the text, to prevent the :hover CSS style from kicking in.
                        . '<a name="' . $self->SymbolToHTMLSymbol($symbol) . '"></a>'
                        . $text
                    . '</td>';
                };

            $text =~ s/<dd>/<td class=CDLDescription>/g;
            $text =~ s/<\/dd>/<\/td><\/tr>/g;

            $output .= $text;
            };
        };

    return $output;
    };


#
#   Function: BuildTextLink
#
#   Creates a HTML link to a symbol, if it exists.
#
#   Parameters:
#
#       target  - The link text.
#       name - The link name.
#       original - The original text as it appears in the source.
#       package  - The package <SymbolString> the link appears in, or undef if none.
#       using - An arrayref of additional scope <SymbolStrings> the link has access to, or undef if none.
#       sourceFile  - The <FileName> the link appears in.
#
#       Target, name, and original are assumed to still have <NDMarkup> amp chars.
#
#   Returns:
#
#       The link in HTML, including tags.  If the link doesn't resolve to anything, returns the HTML that should be substituted for it.
#
sub BuildTextLink #(target, name, original, package, using, sourceFile)
    {
    my ($self, $target, $name, $original, $package, $using, $sourceFile) = @_;

    my $plainTarget = $self->RestoreAmpChars($target);

    my $symbol = NaturalDocs::SymbolString->FromText($plainTarget);
    my $symbolTarget = NaturalDocs::SymbolTable->References(::REFERENCE_TEXT(), $symbol, $package, $using, $sourceFile);

    if (defined $symbolTarget)
        {
        my $symbolTargetFile;

        if ($symbolTarget->File() ne $sourceFile)
            {
            $symbolTargetFile = $self->MakeRelativeURL( $self->OutputFileOf($sourceFile),
                                                                               $self->OutputFileOf($symbolTarget->File()), 1 );
            };
        # else leave it undef

        my $symbolTargetTooltipID = $self->BuildToolTip($symbolTarget->Symbol(), $sourceFile, $symbolTarget->Type(),
                                                                                 $symbolTarget->Prototype(), $symbolTarget->Summary());

        my $toolTipProperties = $self->BuildToolTipLinkProperties($symbolTargetTooltipID);

        return '<a href="' . $symbolTargetFile . '#' . $self->SymbolToHTMLSymbol($symbolTarget->Symbol()) . '" '
                    . 'class=L' . NaturalDocs::Topics->NameOfType($symbolTarget->Type(), 0, 1) . ' ' . $toolTipProperties . '>'
                        . $name
                    . '</a>';
        }
    else
        {
        return $original;
        };
    };


#
#   Function: BuildURLLink
#
#   Creates a HTML link to an external URL.  Long URLs will have hidden breaks to allow them to wrap.
#
#   Parameters:
#
#       target - The URL to link to.
#       name - The label of the link.
#
#       Both are assumed to still have <NDMarkup> amp chars.
#
#   Returns:
#
#       The HTML link, complete with tags.
#
sub BuildURLLink #(target, name)
    {
    my ($self, $target, $name) = @_;

    # Don't restore amp chars on the target.

    if (length $name < 50 || $name ne $target)
        {  return '<a href="' . $target . '" class=LURL target=_top>' . $name . '</a>';  };

    my @segments = split(/([\,\/]|&amp;)/, $target);
    my $output = '<a href="' . $target . '" class=LURL target=_top>';

    # Get past the first batch of slashes, since we don't want to break on things like http://.

    $output .= $segments[0];

    my $i = 1;
    while ($i < scalar @segments && ($segments[$i] eq '/' || !$segments[$i]))
        {
        $output .= $segments[$i];
        $i++;
        };

    # Now break on each one of those symbols.

    while ($i < scalar @segments)
        {
        if ($segments[$i] eq ',' || $segments[$i] eq '/' || $segments[$i] eq '&amp;')
            {  $output .= '<wbr>';  };

        $output .= $segments[$i];
        $i++;
        };

    $output .= '</a>';
    return $output;
    };


#
#   Function: BuildEMailLink
#
#   Creates a HTML link to an e-mail address.  The address will be transparently munged to protect it (hopefully) from spambots.
#
#   Parameters:
#
#       target  - The e-mail address.
#       name - The label of the link.
#
#       Both are assumed to still have <NDMarkup> amp chars.
#
#   Returns:
#
#       The HTML e-mail link, complete with tags.
#
sub BuildEMailLink #(target, name)
    {
    my ($self, $target, $name) = @_;
    my @splitAddress;


    # Hack the address up.  We want two user pieces and two host pieces.

    my ($user, $host) = split(/\@/, $self->RestoreAmpChars($target));

    my $userSplit = length($user) / 2;

    push @splitAddress, NaturalDocs::NDMarkup->ConvertAmpChars( substr($user, 0, $userSplit) );
    push @splitAddress, NaturalDocs::NDMarkup->ConvertAmpChars( substr($user, $userSplit) );

    push @splitAddress, '@';

    my $hostSplit = length($host) / 2;

    push @splitAddress, NaturalDocs::NDMarkup->ConvertAmpChars( substr($host, 0, $hostSplit) );
    push @splitAddress, NaturalDocs::NDMarkup->ConvertAmpChars( substr($host, $hostSplit) );


    # Now put it back together again.  We'll use spans to split the text transparently and JavaScript to split and join the link.

    my $output =
    "<a href=\"#\" onClick=\"location.href='mai' + 'lto:' + '" . join("' + '", @splitAddress) . "'; return false;\" class=LEMail>";

    if ($name eq $target)
        {
        $output .=
        $splitAddress[0] . '<span style="display: none">.nosp@m.</span>' . $splitAddress[1]
        . '<span>@</span>'
        . $splitAddress[3] . '<span style="display: none">.nosp@m.</span>' . $splitAddress[4];
        }
    else
        {  $output .= $name;  };

    $output .= '</a>';
    return $output;
    };


#
#   Function: BuildImage
#
#   Builds the HTML for an image.
#
#   Parameters:
#
#       sourceFile - The source <FileName> this image appears in.
#       mode - Either "inline" or "link".
#       target - The target.
#       original - The original text.
#
#       All are assumed to still have <NDMarkup> amp chars.
#
#   Returns:
#
#       The result in HTML.  If the mode was "link", the target image's HTML is added to <imageContent>.
#
sub BuildImage #(sourceFile, mode, target, original)
    {
    my ($self, $sourceFile, $mode, $target, $original) = @_;

    my $targetNoAmp = $self->RestoreAmpChars($target);

    my $image = NaturalDocs::ImageReferenceTable->GetReferenceTarget($sourceFile, $targetNoAmp);

    if ($image)
        {
        my ($width, $height) = NaturalDocs::Project->ImageFileDimensions($image);

        if ($mode eq 'inline')
            {
            return
            '<img src="' . $self->MakeRelativeURL($self->OutputFileOf($sourceFile),
                                                                       $self->OutputImageOf($image), 1) . '"'

            . ($width && $height ? ' width="' . $width . '" height="' . $height . '"' : '')
            . '>';
            }
        else # link
            {
            # Make the text a little more friendly in the output by removing any folders and file extensions.
            # (see images/Table1.gif) will be turned into (see Table1).
            my $originalNoAmp = $self->RestoreAmpChars($original);
            my $targetIndex = index($originalNoAmp, $targetNoAmp);
            my ($shortTarget, $shortTargetNoAmp, $shortOriginal);

            if ($targetIndex != -1)
                {
                $shortTargetNoAmp = (NaturalDocs::File->SplitPath($targetNoAmp))[2];
                $shortTargetNoAmp = NaturalDocs::File->NoExtension($shortTargetNoAmp);

                substr($originalNoAmp, $targetIndex, length($targetNoAmp), $shortTargetNoAmp);

                $shortOriginal = NaturalDocs::NDMarkup->ConvertAmpChars($originalNoAmp);
                $shortTarget = NaturalDocs::NDMarkup->ConvertAmpChars($shortTargetNoAmp);
                };

            my $output =
            '<a href="#Image' . $imageAnchorNumber . '" class=CImageLink>'
                . ($shortOriginal || $original)
            . '</a>';

            $imageContent .=
            '<blockquote>'
            . '<div class=CImage>'
                . '<a name="Image' . $imageAnchorNumber . '"></a>'
                . '<div class=CImageCaption>' . ($shortTarget || $target) . '</div>'
                . '<img src="' . $self->MakeRelativeURL($self->OutputFileOf($sourceFile),
                                                                           $self->OutputImageOf($image), 1) . '"'

                . ($width && $height ? ' width="' . $width . '" height="' . $height . '"' : '')
                . '>'

            . '</div></blockquote>';

            $imageAnchorNumber++;
            return $output;
            };
        }
    else # !$image
        {
        if ($mode eq 'inline')
            {  return '<p>' . $original . '</p>';  }
        else #($mode eq 'link')
            {  return $original;  };
        };
    };


#
#   Function: BuildToolTipLinkProperties
#
#   Returns the properties that should go in the link tag to add a tooltip to it.  Because the function accepts undef, you can
#   call it without checking if <BuildToolTip()> returned undef or not.
#
#   Parameters:
#
#       toolTipID - The ID of the tooltip.  If undef, the function will return undef.
#
#   Returns:
#
#       The properties that should be put in the link tag, or undef if toolTipID wasn't specified.
#
sub BuildToolTipLinkProperties #(toolTipID)
    {
    my ($self, $toolTipID) = @_;

    if (defined $toolTipID)
        {
        my $currentNumber = $tooltipLinkNumber;
        $tooltipLinkNumber++;

        return 'id=link' . $currentNumber . ' '
                . 'onMouseOver="ShowTip(event, \'' . $toolTipID . '\', \'link' . $currentNumber . '\')" '
                . 'onMouseOut="HideTip(\'' . $toolTipID . '\')"';
        }
    else
        {  return undef;  };
    };


#
#   Function: AddDoubleSpaces
#
#   Adds second spaces after the appropriate punctuation with &nbsp; so they show up in HTML.  They don't occur if there isn't at
#   least one space after the punctuation, so things like class.member notation won't be affected.
#
#   Parameters:
#
#       text - The text to convert.
#
#   Returns:
#
#       The text with double spaces as necessary.
#
sub AddDoubleSpaces #(text)
    {
    my ($self, $text) = @_;

    # Question marks and exclamation points get double spaces unless followed by a lowercase letter.

    $text =~ s/  ([^\ \t\r\n] [\!\?])  # Must appear after a non-whitespace character to apply.

                      (&quot;|&[lr][sd]quo;|[\'\"\]\}\)]?)  # Tolerate closing quotes, parenthesis, etc.
                      ((?:<[^>]+>)*)  # Tolerate tags

                      \   # The space
                      (?![a-z])  # Not followed by a lowercase character.

                   /$1$2$3&nbsp;\ /gx;


    # Periods get double spaces if it's not followed by a lowercase letter.  However, if it's followed by a capital letter and the
    # preceding word is in the list of acceptable abbreviations, it won't get the double space.  Yes, I do realize I am seriously
    # over-engineering this.

    $text =~ s/  ([^\ \t\r\n]+)  # The word prior to the period.

                      \.

                      (&quot;|&[lr][sd]quo;|[\'\"\]\}\)]?)  # Tolerate closing quotes, parenthesis, etc.
                      ((?:<[^>]+>)*)  # Tolerate tags

                      \   # The space
                      ([^a-z])   # The next character, if it's not a lowercase letter.

                  /$1 . '.' . $2 . $3 . MaybeExpand($1, $4) . $4/gex;

    sub MaybeExpand #(leadWord, nextLetter)
        {
        my ($leadWord, $nextLetter) = @_;

        if ($nextLetter =~ /^[A-Z]$/ && exists $abbreviations{ lc($leadWord) } )
            { return ' '; }
        else
            { return '&nbsp; '; };
        };

    return $text;
    };


#
#   Function: ConvertAmpChars
#
#   Converts certain characters to their HTML amp char equivalents.
#
#   Parameters:
#
#       text - The text to convert.
#
#   Returns:
#
#       The converted text.
#
sub ConvertAmpChars #(text)
    {
    my ($self, $text) = @_;

    $text =~ s/&/&amp;/g;
    $text =~ s/\"/&quot;/g;
    $text =~ s/</&lt;/g;
    $text =~ s/>/&gt;/g;

    return $text;
    };


#
#   Function: RestoreAmpChars
#
#   Restores all amp characters to their original state.  This works with both <NDMarkup> amp chars and fancy quotes.
#
#   Parameters:
#
#       text - The text to convert.
#
#   Returns:
#
#       The converted text.
#
sub RestoreAmpChars #(text)
    {
    my ($self, $text) = @_;

    $text = NaturalDocs::NDMarkup->RestoreAmpChars($text);
    $text =~ s/&[lr]squo;/\'/g;
    $text =~ s/&[lr]dquo;/\"/g;

    return $text;
    };


#
#   Function: AddHiddenBreaks
#
#   Adds hidden breaks to symbols.  Puts them after symbol and directory separators so long names won't screw up the layout.
#
#   Parameters:
#
#       string - The string to break.
#
#   Returns:
#
#       The string with hidden breaks.
#
sub AddHiddenBreaks #(string)
    {
    my ($self, $string) = @_;

    # \.(?=.{5,}) instead of \. so file extensions don't get breaks.
    # :+ instead of :: because Mac paths are separated by a : and we want to get those too.

    $string =~ s/(\w(?:\.(?=.{5,})|:+|->|\\|\/))(\w)/$1 . '<wbr>' . $2/ge;

    return $string;
    };


#
#   Function: FindFirstFile
#
#   A function that finds and returns the first file entry in the menu, or undef if none.
#
sub FindFirstFile
    {
    # Hidden parameter: arrayref
    # Used for recursion only.

    my ($self, $arrayref) = @_;

    if (!defined $arrayref)
        {  $arrayref = NaturalDocs::Menu->Content();  };

    foreach my $entry (@$arrayref)
        {
        if ($entry->Type() == ::MENU_FILE())
            {
            return $entry;
            }
        elsif ($entry->Type() == ::MENU_GROUP())
            {
            my $result = $self->FindFirstFile($entry->GroupContent());
            if (defined $result)
                {  return $result;  };
            };
        };

    return undef;
    };


#
#   Function: ExpandMenu
#
#   Determines which groups should be expanded.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to use if you're looking for a source file.
#       indexType - The index <TopicType> to use if you're looking for an index.
#       selectionHierarchy - The <FileName> the menu is being built for.  Does not have to be on the menu itself.
#       rootLength - The length of the menu's root group, *not* including the contents of subgroups.
#
#   Returns:
#
#       An arrayref of all the group numbers that should be expanded.  At minimum, it will contain the numbers of the groups
#       present in <menuSelectionHierarchy>, though it may contain more.
#
sub ExpandMenu #(FileName sourceFile, TopicType indexType, NaturalDocs::Menu::Entry[] selectionHierarchy, int rootLength) -> int[] groupsToExpand
    {
    my ($self, $sourceFile, $indexType, $menuSelectionHierarchy, $rootLength) = @_;

    my $toExpand = [ ];


    # First expand everything in the selection hierarchy.

    my $length = $rootLength;

    foreach my $entry (@$menuSelectionHierarchy)
        {
        $length += $menuGroupLengths{$entry};
        push @$toExpand, $menuGroupNumbers{$entry};
        };


    # Now do multiple passes of group expansion as necessary.  We start from bottomIndex and expand outwards.  We stop going
    # in a direction if a group there is too long -- we do not skip over it and check later groups as well.  However, if one direction
    # stops, the other can keep going.

    my $pass = 1;
    my $hasSubGroups;

    while ($length < MENU_LENGTH_LIMIT)
        {
        my $content;
        my $topIndex;
        my $bottomIndex;


        if ($pass == 1)
            {
            # First pass, we expand the selection's siblings.

            if (scalar @$menuSelectionHierarchy)
                {  $content = $menuSelectionHierarchy->[0]->GroupContent();  }
            else
                {  $content = NaturalDocs::Menu->Content();  };

            $bottomIndex = 0;

            while ($bottomIndex < scalar @$content &&
                     !($content->[$bottomIndex]->Type() == ::MENU_FILE() &&
                       $content->[$bottomIndex]->Target() eq $sourceFile) &&
                     !($content->[$bottomIndex]->Type() != ::MENU_INDEX() &&
                       $content->[$bottomIndex]->Target() eq $indexType) )
                {  $bottomIndex++;  };

            if ($bottomIndex == scalar @$content)
                {  $bottomIndex = 0;  };
            $topIndex = $bottomIndex - 1;
            }

        elsif ($pass == 2)
            {
            # If the section we just expanded had no sub-groups, do another pass trying to expand the parent's sub-groups.  The
            # net effect is that groups won't collapse as much unnecessarily.  Someone can click on a file in a sub-group and the
            # groups in the parent will stay open.

            if (!$hasSubGroups && scalar @$menuSelectionHierarchy)
                {
                if (scalar @$menuSelectionHierarchy > 1)
                    {  $content = $menuSelectionHierarchy->[1]->GroupContent();  }
                else
                    {  $content = NaturalDocs::Menu->Content();  };

                $bottomIndex = 0;

                while ($bottomIndex < scalar @$content &&
                         $content->[$bottomIndex] != $menuSelectionHierarchy->[0])
                    {  $bottomIndex++;  };

                $topIndex = $bottomIndex - 1;
                $bottomIndex++;  # Increment past our own group.
                $hasSubGroups = undef;
                }
            else
                {  last;  };
            }

        # No more passes.
        else
            {  last;  };


        while ( ($topIndex >= 0 || $bottomIndex < scalar @$content) && $length < MENU_LENGTH_LIMIT)
            {
            # We do the bottom first.

            while ($bottomIndex < scalar @$content && $content->[$bottomIndex]->Type() != ::MENU_GROUP())
                {  $bottomIndex++;  };

            if ($bottomIndex < scalar @$content)
                {
                my $bottomEntry = $content->[$bottomIndex];
                $hasSubGroups = 1;

                if ($length + $menuGroupLengths{$bottomEntry} <= MENU_LENGTH_LIMIT)
                    {
                    $length += $menuGroupLengths{$bottomEntry};
                    push @$toExpand, $menuGroupNumbers{$bottomEntry};
                    $bottomIndex++;
                    }
                else
                    {  $bottomIndex = scalar @$content;  };
                };

            # Top next.

            while ($topIndex >= 0 && $content->[$topIndex]->Type() != ::MENU_GROUP())
                {  $topIndex--;  };

            if ($topIndex >= 0)
                {
                my $topEntry = $content->[$topIndex];
                $hasSubGroups = 1;

                if ($length + $menuGroupLengths{$topEntry} <= MENU_LENGTH_LIMIT)
                    {
                    $length += $menuGroupLengths{$topEntry};
                    push @$toExpand, $menuGroupNumbers{$topEntry};
                    $topIndex--;
                    }
                else
                    {  $topIndex = -1;  };
                };
            };


        $pass++;
        };

    return $toExpand;
    };


#
#   Function: GetMenuSelectionHierarchy
#
#   Finds the sequence of menu groups that contain the current selection.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to use if you're looking for a source file.
#       indexType - The index <TopicType> to use if you're looking for an index.
#
#   Returns:
#
#       An arrayref of the <NaturalDocs::Menu::Entry> objects of each group surrounding the selected menu item.  First entry is the
#       group immediately encompassing it, and each subsequent entry works its way towards the outermost group.
#
sub GetMenuSelectionHierarchy #(FileName sourceFile, TopicType indexType) -> NaturalDocs::Menu::Entry[] selectionHierarchy
    {
    my ($self, $sourceFile, $indexType) = @_;

    my $hierarchy = [ ];

    $self->FindMenuSelection($sourceFile, $indexType, $hierarchy, NaturalDocs::Menu->Content());

    return $hierarchy;
    };


#
#   Function: FindMenuSelection
#
#   A recursive function that deterimes if it or any of its sub-groups has the menu selection.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to use if you're looking for a source file.
#       indexType - The index <TopicType> to use if you're looking for an index.
#       hierarchyRef - A reference to the menu selection hierarchy.
#       entries - An arrayref of <NaturalDocs::Menu::Entries> to search.
#
#   Returns:
#
#       Whether this group or any of its subgroups had the selection.  If true, it will add any subgroups to the menu selection
#       hierarchy but not itself.  This prevents the topmost entry from being added.
#
sub FindMenuSelection #(FileName sourceFile, TopicType indexType, NaturalDocs::Menu::Entry[] hierarchyRef, NaturalDocs::Menu::Entry[] entries) -> bool hasSelection
    {
    my ($self, $sourceFile, $indexType, $hierarchyRef, $entries) = @_;

    foreach my $entry (@$entries)
        {
        if ($entry->Type() == ::MENU_GROUP())
            {
            # If the subgroup has the selection...
            if ( $self->FindMenuSelection($sourceFile, $indexType, $hierarchyRef, $entry->GroupContent()) )
                {
                push @$hierarchyRef, $entry;
                return 1;
                };
            }

        elsif ($entry->Type() == ::MENU_FILE())
            {
            if ($sourceFile eq $entry->Target())
                {  return 1;  };
            }

        elsif ($entry->Type() == ::MENU_INDEX())
            {
            if ($indexType eq $entry->Target)
                {  return 1;  };
            };
        };

    return 0;
    };


#
#   Function: ResetToolTips
#
#   Resets the <ToolTip Package Variables> for a new page.
#
#   Parameters:
#
#       samePage  - Set this flag if there's the possibility that the next batch of tooltips may be on the same page as the last.
#
sub ResetToolTips #(samePage)
    {
    my ($self, $samePage) = @_;

    if (!$samePage)
        {
        $tooltipLinkNumber = 1;
        $tooltipNumber = 1;
        };

    $tooltipHTML = undef;
    %tooltipSymbolsToNumbers = ( );
    };


1;
