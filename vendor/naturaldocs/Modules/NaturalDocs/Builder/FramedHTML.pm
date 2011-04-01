###############################################################################
#
#   Package: NaturalDocs::Builder::FramedHTML
#
###############################################################################
#
#   A package that generates output in HTML with frames.
#
#   All functions are called with Package->Function() notation.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


use strict;
use integer;

package NaturalDocs::Builder::FramedHTML;

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
    return 'FramedHTML';
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

        # IE 6 doesn't like any doctype here at all.  Add one (strict or transitional doesn't matter) and it makes the page slightly too
        # wide for the frame.  Mozilla and Opera handle it like champs either way because they Don't Suck(tm).

        # '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" '
        # . '"http://www.w3.org/TR/REC-html40/loose.dtd">' . "\n\n"

        '<html><head>'

        	. '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<title>'
                . $self->BuildTitle($sourceFile)
            . '</title>'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($outputFile, $self->MainCSSFile(), 1) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->MainJavaScriptFile(), 1) . '"></script>';

            if ($usePrettify)
            	{
            	print OUTPUTFILEHANDLE
	            '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->PrettifyJavaScriptFile(), 1) . '">'
	            . '</script>';
            	}

        print OUTPUTFILEHANDLE
        '</head><body class="FramedContentPage" onLoad="NDOnLoad();' . ($usePrettify ? 'prettyPrint();' : '') . '">'
            . $self->OpeningBrowserStyles()

            . $self->StandardComments()

            . "\n\n\n"
                . $self->BuildContent($sourceFile, $parsedFile)
            . "\n\n\n"

            . $self->BuildToolTips()

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
    my $indexFile = $self->IndexFileOf($type);

    my $startIndexPage =

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" '
            . '"http://www.w3.org/TR/REC-html40/loose.dtd">' . "\n\n"

        . '<html><head>'

        	. '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<title>';

            if (defined NaturalDocs::Menu->Title())
                {  $startIndexPage .= $self->StringToHTML(NaturalDocs::Menu->Title()) . ' - ';  };

                $startIndexPage .=
                $indexTitle
            . '</title>'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($indexFile, $self->MainCSSFile(), 1) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($indexFile, $self->MainJavaScriptFile(), 1) . '"></script>'

        . '</head><body class="FramedIndexPage" onLoad="NDOnLoad()">'
            . $self->OpeningBrowserStyles()

            . "\n\n\n"
                . $self->StandardComments()
            . "\n\n\n"
                . '<div id=Index>'
                    . '<div class=IPageTitle>'
                        . $indexTitle
                    . '</div>';


    my $endIndexPage =

                    '</div><!--Index-->'
                . "\n\n\n"

                . $self->ClosingBrowserStyles()

       . '</body></html>';

    my $startSearchResultsPage =

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" '
            . '"http://www.w3.org/TR/REC-html40/loose.dtd">' . "\n\n"

        . '<html><head>'

            . '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($indexFile, $self->MainCSSFile(), 1) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($indexFile, $self->MainJavaScriptFile(), 1) . '"></script>'
            . '<script language=JavaScript src="' . $self->MakeRelativeURL($indexFile, $self->SearchDataJavaScriptFile(), 1) . '">'
                . '</script>'

        . '</head><body class="FramedSearchResultsPage" onLoad="NDOnLoad()">'
            . $self->OpeningBrowserStyles()

            . "\n\n\n"
                . $self->StandardComments()
            . "\n\n\n"

                . '<div id=Index>'
                    . '<div class=IPageTitle>'
                        . 'Search Results'
                    . '</div>';

    my $endSearchResultsPage =

                    '</div><!--Index-->'
                . "\n\n\n"

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
#   Builds the menu file.  Also generates index.html.
#
sub UpdateMenu
    {
    my $self = shift;

    my $outputDirectory = NaturalDocs::Settings->OutputDirectoryOf($self);
    my $outputFile = NaturalDocs::File->JoinPaths($outputDirectory, 'menu.html');


    open(OUTPUTFILEHANDLE, '>' . $outputFile)
        or die "Couldn't create output file " . $outputFile . "\n";

    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');

    my $title = 'Menu';
    if (defined $title)
        {  $title .= ' - ' . NaturalDocs::Menu->Title();  };

    $title = $self->StringToHTML($title);


    print OUTPUTFILEHANDLE

        '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" '
            . '"http://www.w3.org/TR/REC-html40/loose.dtd">' . "\n\n"

        . '<html><head>'

          	. '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

            . '<title>'
                . $title
            . '</title>'

            . '<base target="Content">'

            . '<link rel="stylesheet" type="text/css" href="' . $self->MakeRelativeURL($outputFile, $self->MainCSSFile(), 1) . '">'

            . '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->MainJavaScriptFile(), 1) . '"></script>'
            . '<script language=JavaScript src="' . $self->MakeRelativeURL($outputFile, $self->SearchDataJavaScriptFile(), 1) . '">'
                . '</script>'

        . '</head><body class="FramedMenuPage" onLoad="NDOnLoad()">'
            . $self->OpeningBrowserStyles()

            . $self->StandardComments()

            . "\n\n\n"
                . $self->BuildMenu(undef, undef)
            . "\n\n\n"
                . $self->BuildFooter(1)
            . "\n\n\n"

            . $self->ClosingBrowserStyles()
        . '</body></html>';


    close(OUTPUTFILEHANDLE);


    # Update index.html

    my $firstMenuEntry = $self->FindFirstFile();
    my $indexFile = NaturalDocs::File->JoinPaths( NaturalDocs::Settings->OutputDirectoryOf($self), 'index.html' );

    # We have to check because it's possible that there may be no files with Natural Docs content and thus no files on the menu.
    if (defined $firstMenuEntry)
        {
        open(INDEXFILEHANDLE, '>' . $indexFile)
            or die "Couldn't create output file " . $indexFile . ".\n";

	    binmode(OUTPUTFILEHANDLE, ':encoding(UTF-8)');

        print INDEXFILEHANDLE

            '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Frameset//EN" '
                . '"http://www.w3.org/TR/REC-html40/frameset.dtd">'

            . '<html>'

                . '<head>'

                    . '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'

                    . '<title>'
                        . $self->StringToHTML(NaturalDocs::Menu->Title())
                    . '</title>'

                . '</head>'

                . $self->StandardComments()

                . '<frameset cols="185,*">'
                    . '<frame name=Menu src="menu.html">'
                    . '<frame name=Content src="'
                        . $self->MakeRelativeURL($indexFile, $self->OutputFileOf($firstMenuEntry->Target()), 1) . '">'
                . '</frameset>'

                . '<noframes>'
                    . 'This documentation was designed for use with frames.  However, you can still use it by '
                    . '<a href="menu.html">starting from the menu page</a>.'
                    . "<script language=JavaScript><!--\n"
                        . 'location.href="menu.html";'
                    . "\n// --></script>"
                . '</noframes>'

            . '</html>';

        close INDEXFILEHANDLE;
        }

    elsif (-e $indexFile)
        {
        unlink($indexFile);
        };
    };


1;
