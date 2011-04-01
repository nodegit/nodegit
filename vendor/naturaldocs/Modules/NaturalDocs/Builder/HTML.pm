###############################################################################
#
#   Package: NaturalDocs::Builder::HTML
#
###############################################################################
#
#   A package that generates output in HTML.
#
#   All functions are called with Package->Function() notation.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use strict;
use integer;

package NaturalDocs::Builder::HTML;

use base 'NaturalDocs::Builder::HTMLBase';


###############################################################################
# Group: Implemented Interface Functions


#
#   Function: INIT
#
#   Registers the package with <NaturalDocs::Builder>.
#
sub INIT
    {
    NaturalDocs::Builder->Add(__PACKAGE__);
    };


#
#   Function: CommandLineOption
#
#   Returns the option to follow -o to use this package.  In this case, "html".
#
sub CommandLineOption
    {
    return 'HTML';
    };


#
#   Function: BuildFile
#
#   Builds the output file from the parsed source file.
#
#   Parameters:
#
#       sourcefile       - The <FileName> of the source file.
#       parsedFile      - An arrayref of the source file as <NaturalDocs::Parser::ParsedTopic> objects.
#
sub BuildFile #(sourceFile, parsedFile)
    {
    my ($self, $sourceFile, $parsedFile) = @_;

    my $outputFile = $self->OutputFileOf($sourceFile);


    # 99.99% of the time the output directory will already exist, so this will actually be more efficient.  It only won't exist
    # if a new file was added in a new subdirectory and this is the first time that file was ever parsed.
    if (!open(OUTPUTFILEHANDLE, '>' . $outputFile))
        {
        NaturalDocs::File->CreatePath( NaturalDocs::File->NoFileName($outputFile) );

        open(OUTPUTFILEHANDLE, '>' . $outputFile)
            or die "Couldn't create output file " . $outputFile . "\n";
        };

    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');
    my $usePrettify = (NaturalDocs::Settings->HighlightCode() || NaturalDocs::Settings->HighlightAnonymous());


    print OUTPUTFILEHANDLE

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" '
            . '"http://www.w3.org/TR/REC-html40/strict.dtd">' . "\n\n"

        . '<html><head>'

            . '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<title>'
                . $self->BuildTitle($sourceFile)
            . '</title>'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($outputFile, $self->MainCSSFile(), 1) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->MainJavaScriptFile(), 1) . '">'
                . '</script>';

			if ($usePrettify)
				{
				print OUTPUTFILEHANDLE
	            '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->PrettifyJavaScriptFile(), 1) . '">'
	                . '</script>';
                }

            print OUTPUTFILEHANDLE
            '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->SearchDataJavaScriptFile(), 1) . '">'
                . '</script>'

	        . '</head><body class="ContentPage" onLoad="NDOnLoad();' . ($usePrettify ? 'prettyPrint();' : '') . '">'
            . $self->OpeningBrowserStyles()

            . $self->StandardComments()

            . "\n\n\n"
                . $self->BuildContent($sourceFile, $parsedFile)
            . "\n\n\n"
                . $self->BuildFooter()
            . "\n\n\n"
                . $self->BuildMenu($sourceFile, undef)
            . "\n\n\n"
                . $self->BuildToolTips()
            . "\n\n\n"
                . '<div id=MSearchResultsWindow>'
                    . '<iframe src="" frameborder=0 name=MSearchResults id=MSearchResults></iframe>'
                    . '<a href="javascript:searchPanel.CloseResultsWindow()" id=MSearchResultsWindowClose>Close</a>'
                . '</div>'
            . "\n\n\n"

            . $self->ClosingBrowserStyles()
        . '</body></html>';


    close(OUTPUTFILEHANDLE);
    };


#
#   Function: BuildIndex
#
#   Builds an index for the passed type.
#
#   Parameters:
#
#       type  - The <TopicType> to limit the index to, or undef if none.
#
sub BuildIndex #(type)
    {
    my ($self, $type) = @_;

    my $indexTitle = $self->IndexTitleOf($type);

    my $startIndexPage =

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" '
            . '"http://www.w3.org/TR/REC-html40/strict.dtd">' . "\n\n"

        . '<html><head>'

            . '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<title>'
                . $indexTitle;

                if (defined NaturalDocs::Menu->Title())
                    {  $startIndexPage .= ' - ' . $self->StringToHTML(NaturalDocs::Menu->Title());  };

            $startIndexPage .=
            '</title>'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($self->IndexDirectory(),
                                                                                                                       $self->MainCSSFile()) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($self->IndexDirectory(),
                                                                                                        $self->MainJavaScriptFile()) . '"></script>'
            . '<script language=JavaScript src="' . $self->MakeRelativeURL($self->IndexDirectory(),
                                                                                                        $self->SearchDataJavaScriptFile()) . '">'
                . '</script>'

        . '</head><body class="IndexPage" onLoad="NDOnLoad()">'
            . $self->OpeningBrowserStyles()

        . $self->StandardComments()

        . "\n\n\n"

        . '<div id=Index>'
            . '<div class=IPageTitle>'
                . $indexTitle
            . '</div>';

    my $endIndexPage =
            '</div><!--Index-->'

            . "\n\n\n"
                . $self->BuildFooter()
            . "\n\n\n"
                . $self->BuildMenu(undef, $type)
            . "\n\n\n"
                . '<div id=MSearchResultsWindow>'
                    . '<iframe src="" frameborder=0 name=MSearchResults id=MSearchResults></iframe>'
                    . '<a href="javascript:searchPanel.CloseResultsWindow()" id=MSearchResultsWindowClose>Close</a>'
                . '</div>'
            . "\n\n\n"

            . $self->ClosingBrowserStyles()
        . '</body></html>';


    my $startSearchResultsPage =

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" '
            . '"http://www.w3.org/TR/REC-html40/strict.dtd">' . "\n\n"

        . '<html><head>'

            . '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($self->SearchResultsDirectory(),
                                                                                                                       $self->MainCSSFile()) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($self->SearchResultsDirectory(),
                                                                                                        $self->MainJavaScriptFile()) . '"></script>'

        . '</head><body class="PopupSearchResultsPage" onLoad="NDOnLoad()">'
            . $self->OpeningBrowserStyles()

        . $self->StandardComments()

        . "\n\n\n"

        . '<div id=Index>';


    my $endSearchResultsPage =
        '</div>'
        . $self->ClosingBrowserStyles()
   . '</body></html>';

    my $indexContent = NaturalDocs::SymbolTable->Index($type);
    my $pageCount = $self->BuildIndexPages($type, $indexContent, $startIndexPage, $endIndexPage,
                                                                  $startSearchResultsPage, $endSearchResultsPage);
    $self->PurgeIndexFiles($type, $indexContent, $pageCount + 1);
    };


#
#   Function: UpdateMenu
#
#   Updates the menu in all the output files that weren't rebuilt.  Also generates index.html.
#
sub UpdateMenu
    {
    my $self = shift;


    # Update the menu on unbuilt files.

    my $filesToUpdate = NaturalDocs::Project->UnbuiltFilesWithContent();

    foreach my $sourceFile (keys %$filesToUpdate)
        {
        $self->UpdateFile($sourceFile);
        };


    # Update the menu on unchanged index files.

    my $indexes = NaturalDocs::Menu->Indexes();

    foreach my $index (keys %$indexes)
        {
        if (!NaturalDocs::SymbolTable->IndexChanged($index))
            {
            $self->UpdateIndex($index);
            };
        };


    # Update index.html

    my $firstMenuEntry = $self->FindFirstFile();
    my $indexFile = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'index.html' );

    # We have to check because it's possible that there may be no files with Natural Docs content and thus no files on the menu.
    if (defined $firstMenuEntry)
        {
        open(INDEXFILEHANDLE, '>' . $indexFile)
            or die "Couldn't create output file " . $indexFile . ".\n";

	    binmode(INDEXFILEHANDLE, ':encoding(UTF-8)');

        print INDEXFILEHANDLE
        '<html><head>'
             . '<meta http-equiv="Refresh" CONTENT="0; URL='
                 . $self->MakeRelativeURL( NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'index.html'),
                                                        $self->OutputFileOf($firstMenuEntry->Target()), 1 ) . '">'
        . '</head></html>';

        close INDEXFILEHANDLE;
        }

    elsif (-e $indexFile)
        {
        unlink($indexFile);
        };
    };



###############################################################################
# Group: Support Functions


#
#   Function: UpdateFile
#
#   Updates an output file.  Replaces the menu, HTML title, and footer.  It opens the output file, makes the changes, and saves it
#   back to disk, which is much quicker than rebuilding the file from scratch if these were the only things that changed.
#
#   Parameters:
#
#       sourceFile - The source <FileName>.
#
#   Dependencies:
#
#       - Requires <Builder::BuildMenu()> to surround its content with the exact strings "<div id=Menu>" and "</div><!--Menu-->".
#       - Requires <Builder::BuildFooter()> to surround its content with the exact strings "<div id=Footer>" and
#         "</div><!--Footer-->".
#
sub UpdateFile #(sourceFile)
    {
    my ($self, $sourceFile) = @_;

    my $outputFile = $self->OutputFileOf($sourceFile);

    if (open(OUTPUTFILEHANDLE, '<' . $outputFile))
        {
        my $content;

	    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');
        read(OUTPUTFILEHANDLE, $content, -s OUTPUTFILEHANDLE);
        close(OUTPUTFILEHANDLE);


        $content =~ s{<title>[^<]*<\/title>}{'<title>' . $self->BuildTitle($sourceFile) . '</title>'}e;

        $content =~ s/<div id=Menu>.*?<\/div><!--Menu-->/$self->BuildMenu($sourceFile, undef)/es;

        $content =~ s/<div id=Footer>.*?<\/div><!--Footer-->/$self->BuildFooter()/e;


        open(OUTPUTFILEHANDLE, '>' . $outputFile);
	    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');
        print OUTPUTFILEHANDLE $content;
        close(OUTPUTFILEHANDLE);
        };
    };


#
#   Function: UpdateIndex
#
#   Updates an index's output file.  Replaces the menu and footer.  It opens the output file, makes the changes, and saves it
#   back to disk, which is much quicker than rebuilding the file from scratch if these were the only things that changed.
#
#   Parameters:
#
#       type - The index <TopicType>, or undef if none.
#
sub UpdateIndex #(type)
    {
    my ($self, $type) = @_;

    my $page = 1;

    my $outputFile = $self->IndexFileOf($type, $page);

    my $newMenu = $self->BuildMenu(undef, $type);
    my $newFooter = $self->BuildFooter();

    while (-e $outputFile)
        {
        open(OUTPUTFILEHANDLE, '<' . $outputFile)
            or die "Couldn't open output file " . $outputFile . ".\n";

        my $content;

	    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');
        read(OUTPUTFILEHANDLE, $content, -s OUTPUTFILEHANDLE);
        close(OUTPUTFILEHANDLE);


        $content =~ s/<div id=Menu>.*?<\/div><!--Menu-->/$newMenu/es;

        $content =~ s/<div id=Footer>.*<\/div><!--Footer-->/$newFooter/e;


        open(OUTPUTFILEHANDLE, '>' . $outputFile)
            or die "Couldn't save output file " . $outputFile . ".\n";

	    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');
        print OUTPUTFILEHANDLE $content;
        close(OUTPUTFILEHANDLE);

        $page++;
        $outputFile = $self->IndexFileOf($type, $page);
        };
    };


1;
