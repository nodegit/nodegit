###############################################################################
#
#   Package: NaturalDocs::Parser
#
###############################################################################
#
#   A package that coordinates source file parsing between the <NaturalDocs::Languages::Base>-derived objects and its own
#   sub-packages such as <NaturalDocs::Parser::Native>.  Also handles sending symbols to <NaturalDocs::SymbolTable> and
#   other generic topic processing.
#
#   Usage and Dependencies:
#
#       - Prior to use, <NaturalDocs::Settings>, <NaturalDocs::Languages>, <NaturalDocs::Project>, <NaturalDocs::SymbolTable>,
#         and <NaturalDocs::ClassHierarchy> must be initialized.  <NaturalDocs::SymbolTable> and <NaturalDocs::ClassHierarchy>
#         do not have to be fully resolved.
#
#       - Aside from that, the package is ready to use right away.  It does not have its own initialization function.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use NaturalDocs::Parser::ParsedTopic;
use NaturalDocs::Parser::Native;
use NaturalDocs::Parser::JavaDoc;

use strict;
use integer;

package NaturalDocs::Parser;



###############################################################################
# Group: Variables


#
#   var: sourceFile
#
#   The source <FileName> currently being parsed.
#
my $sourceFile;

#
#   var: language
#
#   The language object for the file, derived from <NaturalDocs::Languages::Base>.
#
my $language;

#
#   Array: parsedFile
#
#   An array of <NaturalDocs::Parser::ParsedTopic> objects.
#
my @parsedFile;


#
#   bool: parsingForInformation
#   Whether <ParseForInformation()> was called.  If false, then <ParseForBuild()> was called.
#
my $parsingForInformation;



###############################################################################
# Group: Functions

#
#   Function: ParseForInformation
#
#   Parses the input file for information.  Will update the information about the file in <NaturalDocs::SymbolTable> and
#   <NaturalDocs::Project>.
#
#   Parameters:
#
#       file - The <FileName> to parse.
#
sub ParseForInformation #(file)
    {
    my ($self, $file) = @_;
    $sourceFile = $file;

    $parsingForInformation = 1;

    # Watch this parse so we detect any changes.
    NaturalDocs::SymbolTable->WatchFileForChanges($sourceFile);
    NaturalDocs::ClassHierarchy->WatchFileForChanges($sourceFile);
    NaturalDocs::SourceDB->WatchFileForChanges($sourceFile);

    my $defaultMenuTitle = $self->Parse();

    foreach my $topic (@parsedFile)
        {
        # Add a symbol for the topic.

        my $type = $topic->Type();
        if ($type eq ::TOPIC_ENUMERATION())
            {  $type = ::TOPIC_TYPE();  };

        NaturalDocs::SymbolTable->AddSymbol($topic->Symbol(), $sourceFile, $type,
                                                                   $topic->Prototype(), $topic->Summary());


        # You can't put the function call directly in a while with a regex.  It has to sit in a variable to work.
        my $body = $topic->Body();


        # If it's a list or enum topic, add a symbol for each description list entry.

        if ($topic->IsList() || $topic->Type() eq ::TOPIC_ENUMERATION())
            {
            # We'll hijack the enum constants to apply to non-enum behavior too.
            my $behavior;

            if ($topic->Type() eq ::TOPIC_ENUMERATION())
                {
                $type = ::TOPIC_CONSTANT();
                $behavior = $language->EnumValues();
                }
            elsif (NaturalDocs::Topics->TypeInfo($topic->Type())->Scope() == ::SCOPE_ALWAYS_GLOBAL())
                {
                $behavior = ::ENUM_GLOBAL();
                }
            else
                {
                $behavior = ::ENUM_UNDER_PARENT();
                };

            while ($body =~ /<ds>([^<]+)<\/ds><dd>(.*?)<\/dd>/g)
                {
                my ($listTextSymbol, $listSummary) = ($1, $2);

                $listTextSymbol = NaturalDocs::NDMarkup->RestoreAmpChars($listTextSymbol);
                my $listSymbol = NaturalDocs::SymbolString->FromText($listTextSymbol);

                if ($behavior == ::ENUM_UNDER_PARENT())
                    {  $listSymbol = NaturalDocs::SymbolString->Join($topic->Package(), $listSymbol);  }
                elsif ($behavior == ::ENUM_UNDER_TYPE())
                    {  $listSymbol = NaturalDocs::SymbolString->Join($topic->Symbol(), $listSymbol);  };

                NaturalDocs::SymbolTable->AddSymbol($listSymbol, $sourceFile, $type, undef,
                                                                           $self->GetSummaryFromDescriptionList($listSummary));
                };
            };


        # Add references in the topic.

        while ($body =~ /<link target=\"([^\"]*)\" name=\"[^\"]*\" original=\"[^\"]*\">/g)
            {
            my $linkText = NaturalDocs::NDMarkup->RestoreAmpChars($1);
            my $linkSymbol = NaturalDocs::SymbolString->FromText($linkText);

            NaturalDocs::SymbolTable->AddReference(::REFERENCE_TEXT(), $linkSymbol,
                                                                           $topic->Package(), $topic->Using(), $sourceFile);
            };


        # Add images in the topic.

        while ($body =~ /<img mode=\"[^\"]*\" target=\"([^\"]+)\" original=\"[^\"]*\">/g)
            {
            my $target = NaturalDocs::NDMarkup->RestoreAmpChars($1);
            NaturalDocs::ImageReferenceTable->AddReference($sourceFile, $target);
            };
        };

    # Handle any changes to the file.
    NaturalDocs::ClassHierarchy->AnalyzeChanges();
    NaturalDocs::SymbolTable->AnalyzeChanges();
    NaturalDocs::SourceDB->AnalyzeWatchedFileChanges();

    # Update project on the file's characteristics.
    my $hasContent = (scalar @parsedFile > 0);

    NaturalDocs::Project->SetHasContent($sourceFile, $hasContent);
    if ($hasContent)
        {  NaturalDocs::Project->SetDefaultMenuTitle($sourceFile, $defaultMenuTitle);  };

    # We don't need to keep this around.
    @parsedFile = ( );
    };


#
#   Function: ParseForBuild
#
#   Parses the input file for building, returning it as a <NaturalDocs::Parser::ParsedTopic> arrayref.
#
#   Note that all new and changed files should be parsed for symbols via <ParseForInformation()> before calling this function on
#   *any* file.  The reason is that <NaturalDocs::SymbolTable> needs to know about all the symbol definitions and references to
#   resolve them properly.
#
#   Parameters:
#
#       file - The <FileName> to parse for building.
#
#   Returns:
#
#       An arrayref of the source file as <NaturalDocs::Parser::ParsedTopic> objects.
#
sub ParseForBuild #(file)
    {
    my ($self, $file) = @_;
    $sourceFile = $file;

    $parsingForInformation = undef;

    $self->Parse();

    return \@parsedFile;
    };




###############################################################################
# Group: Interface Functions


#
#   Function: OnComment
#
#   The function called by <NaturalDocs::Languages::Base>-derived objects when their parsers encounter a comment
#   suitable for documentation.
#
#   Parameters:
#
#       commentLines - An arrayref of the comment's lines.  The language's comment symbols should be converted to spaces,
#                               and there should be no line break characters at the end of each line.  *The original memory will be
#                               changed.*
#       lineNumber - The line number of the first of the comment lines.
#       isJavaDoc - Whether the comment is in JavaDoc format.
#
#   Returns:
#
#       The number of topics created by this comment, or zero if none.
#
sub OnComment #(string[] commentLines, int lineNumber, bool isJavaDoc)
    {
    my ($self, $commentLines, $lineNumber, $isJavaDoc) = @_;

    $self->CleanComment($commentLines);

    # We check if it's definitely Natural Docs content first.  This overrides all else, since it's possible that a comment could start
    # with a topic line yet have something that looks like a JavaDoc tag.  Natural Docs wins in this case.
    if (NaturalDocs::Parser::Native->IsMine($commentLines, $isJavaDoc))
        {  return NaturalDocs::Parser::Native->ParseComment($commentLines, $isJavaDoc, $lineNumber, \@parsedFile);  }

    elsif (NaturalDocs::Parser::JavaDoc->IsMine($commentLines, $isJavaDoc))
        {  return NaturalDocs::Parser::JavaDoc->ParseComment($commentLines, $isJavaDoc, $lineNumber, \@parsedFile);  }

    # If the content is ambiguous and it's a JavaDoc-styled comment, treat it as Natural Docs content.
    elsif ($isJavaDoc)
        {  return NaturalDocs::Parser::Native->ParseComment($commentLines, $isJavaDoc, $lineNumber, \@parsedFile);  }
    };


#
#   Function: OnClass
#
#   A function called by <NaturalDocs::Languages::Base>-derived objects when their parsers encounter a class declaration.
#
#   Parameters:
#
#       class - The <SymbolString> of the class encountered.
#
sub OnClass #(class)
    {
    my ($self, $class) = @_;

    if ($parsingForInformation)
        {  NaturalDocs::ClassHierarchy->AddClass($sourceFile, $class);  };
    };


#
#   Function: OnClassParent
#
#   A function called by <NaturalDocs::Languages::Base>-derived objects when their parsers encounter a declaration of
#   inheritance.
#
#   Parameters:
#
#       class - The <SymbolString> of the class we're in.
#       parent - The <SymbolString> of the class it inherits.
#       scope - The package <SymbolString> that the reference appeared in.
#       using - An arrayref of package <SymbolStrings> that the reference has access to via "using" statements.
#       resolvingFlags - Any <Resolving Flags> to be used when resolving the reference.  <RESOLVE_NOPLURAL> is added
#                              automatically since that would never apply to source code.
#
sub OnClassParent #(class, parent, scope, using, resolvingFlags)
    {
    my ($self, $class, $parent, $scope, $using, $resolvingFlags) = @_;

    if ($parsingForInformation)
        {
        NaturalDocs::ClassHierarchy->AddParentReference($sourceFile, $class, $parent, $scope, $using,
                                                                                   $resolvingFlags | ::RESOLVE_NOPLURAL());
        };
    };



###############################################################################
# Group: Support Functions


#   Function: Parse
#
#   Opens the source file and parses process.  Most of the actual parsing is done in <NaturalDocs::Languages::Base->ParseFile()>
#   and <OnComment()>, though.
#
#   *Do not call externally.*  Rather, call <ParseForInformation()> or <ParseForBuild()>.
#
#   Returns:
#
#       The default menu title of the file.  Will be the <FileName> if nothing better is found.
#
sub Parse
    {
    my ($self) = @_;

    NaturalDocs::Error->OnStartParsing($sourceFile);

    $language = NaturalDocs::Languages->LanguageOf($sourceFile);
    NaturalDocs::Parser::Native->Start();
    @parsedFile = ( );

    my ($autoTopics, $scopeRecord) = $language->ParseFile($sourceFile, \@parsedFile);


    $self->AddToClassHierarchy();

    $self->BreakLists();

    if (defined $autoTopics)
        {
        if (defined $scopeRecord)
            {  $self->RepairPackages($autoTopics, $scopeRecord);  };

        $self->MergeAutoTopics($language, $autoTopics);
        };

    $self->RemoveRemainingHeaderlessTopics();


    # We don't need to do this if there aren't any auto-topics because the only package changes would be implied by the comments.
    if (defined $autoTopics)
        {  $self->AddPackageDelineators();  };

    if (!NaturalDocs::Settings->NoAutoGroup())
        {  $self->MakeAutoGroups($autoTopics);  };


    # Set the menu title.

    my $defaultMenuTitle = $sourceFile;

    if (scalar @parsedFile)
        {
        my $addFileTitle;

        if (NaturalDocs::Settings->OnlyFileTitles())
            {
            # We still want to use the title from the topics if the first one is a file.
            if ($parsedFile[0]->Type() eq ::TOPIC_FILE())
                {  $addFileTitle = 0;  }
            else
                {  $addFileTitle = 1;  };
            }
        elsif (scalar @parsedFile == 1 || NaturalDocs::Topics->TypeInfo( $parsedFile[0]->Type() )->PageTitleIfFirst())
            {  $addFileTitle = 0;  }
        else
            {  $addFileTitle = 1;  };

        if (!$addFileTitle)
            {
            $defaultMenuTitle = $parsedFile[0]->Title();
            }
        else
            {
            # If the title ended up being the file name, add a leading section for it.

            unshift @parsedFile,
                       NaturalDocs::Parser::ParsedTopic->New(::TOPIC_FILE(), (NaturalDocs::File->SplitPath($sourceFile))[2],
                                                                                  undef, undef, undef, undef, undef, 1, undef);
            };
        };

    NaturalDocs::Error->OnEndParsing($sourceFile);

    return $defaultMenuTitle;
    };


#
#   Function: CleanComment
#
#   Removes any extraneous formatting and whitespace from the comment.  Eliminates comment boxes, horizontal lines, trailing
#   whitespace from lines, and expands all tab characters.  It keeps leading whitespace, though, since it may be needed for
#   example code, and blank lines, since the original line numbers are needed.
#
#   Parameters:
#
#       commentLines  - An arrayref of the comment lines to clean.  *The original memory will be changed.*  Lines should have the
#                                language's comment symbols replaced by spaces and not have a trailing line break.
#
sub CleanComment #(commentLines)
    {
    my ($self, $commentLines) = @_;

    use constant DONT_KNOW => 0;
    use constant IS_UNIFORM => 1;
    use constant IS_UNIFORM_IF_AT_END => 2;
    use constant IS_NOT_UNIFORM => 3;

    my $leftSide = DONT_KNOW;
    my $rightSide = DONT_KNOW;
    my $leftSideChar;
    my $rightSideChar;

    my $index = 0;
    my $tabLength = NaturalDocs::Settings->TabLength();

    while ($index < scalar @$commentLines)
        {
        # Strip trailing whitespace from the original.

        $commentLines->[$index] =~ s/[ \t]+$//;


        # Expand tabs in the original.  This method is almost six times faster than Text::Tabs' method.

        my $tabIndex = index($commentLines->[$index], "\t");

        while ($tabIndex != -1)
            {
            substr( $commentLines->[$index], $tabIndex, 1, ' ' x ($tabLength - ($tabIndex % $tabLength)) );
            $tabIndex = index($commentLines->[$index], "\t", $tabIndex);
            };


        # Make a working copy and strip leading whitespace as well.  This has to be done after tabs are expanded because
        # stripping indentation could change how far tabs are expanded.

        my $line = $commentLines->[$index];
        $line =~ s/^ +//;

        # If the line is blank...
        if (!length $line)
            {
            # If we have a potential vertical line, this only acceptable if it's at the end of the comment.
            if ($leftSide == IS_UNIFORM)
                {  $leftSide = IS_UNIFORM_IF_AT_END;  };
            if ($rightSide == IS_UNIFORM)
                {  $rightSide = IS_UNIFORM_IF_AT_END;  };
            }

        # If there's at least four symbols in a row, it's a horizontal line.  The second regex supports differing edge characters.  It
        # doesn't matter if any of this matches the left and right side symbols.  The length < 256 is a sanity check, because that
        # regexp has caused the perl regexp engine to choke on an insane line someone sent me from an automatically generated
        # file.  It had over 10k characters on the first line, and most of them were 0x00.
        elsif ($line =~ /^([^a-zA-Z0-9 ])\1{3,}$/ ||
                (length $line < 256 && $line =~ /^([^a-zA-Z0-9 ])\1*([^a-zA-Z0-9 ])\2{3,}([^a-zA-Z0-9 ])\3*$/) )
            {
            # Ignore it.  This has no effect on the vertical line detection.  We want to keep it in the output though in case it was
            # in a code section.
            }

        # If the line is not blank or a horizontal line...
        else
            {
            # More content means any previous blank lines are no longer tolerated in vertical line detection.  They are only
            # acceptable at the end of the comment.

            if ($leftSide == IS_UNIFORM_IF_AT_END)
                {  $leftSide = IS_NOT_UNIFORM;  };
            if ($rightSide == IS_UNIFORM_IF_AT_END)
                {  $rightSide = IS_NOT_UNIFORM;  };


            # Detect vertical lines.  Lines are only lines if they are followed by whitespace or a connected horizontal line.
            # Otherwise we may accidentally detect lines from short comments that just happen to have every first or last
            # character the same.

            if ($leftSide != IS_NOT_UNIFORM)
                {
                if ($line =~ /^([^a-zA-Z0-9])\1*(?: |$)/)
                    {
                    if ($leftSide == DONT_KNOW)
                        {
                        $leftSide = IS_UNIFORM;
                        $leftSideChar = $1;
                        }
                    else # ($leftSide == IS_UNIFORM)  Other choices already ruled out.
                        {
                        if ($leftSideChar ne $1)
                            {  $leftSide = IS_NOT_UNIFORM;  };
                        };
                    }
                # We'll tolerate the lack of symbols on the left on the first line, because it may be a
                # /* Function: Whatever
                #  * Description.
                #  */
                # comment which would have the leading /* blanked out.
                elsif ($index != 0)
                    {
                    $leftSide = IS_NOT_UNIFORM;
                    };
                };

            if ($rightSide != IS_NOT_UNIFORM)
                {
                if ($line =~ / ([^a-zA-Z0-9])\1*$/)
                    {
                    if ($rightSide == DONT_KNOW)
                        {
                        $rightSide = IS_UNIFORM;
                        $rightSideChar = $1;
                        }
                    else # ($rightSide == IS_UNIFORM)  Other choices already ruled out.
                        {
                        if ($rightSideChar ne $1)
                            {  $rightSide = IS_NOT_UNIFORM;  };
                        };
                    }
                else
                    {
                    $rightSide = IS_NOT_UNIFORM;
                    };
                };

            # We'll remove vertical lines later if they're uniform throughout the entire comment.
            };

        $index++;
        };


    if ($leftSide == IS_UNIFORM_IF_AT_END)
        {  $leftSide = IS_UNIFORM;  };
    if ($rightSide == IS_UNIFORM_IF_AT_END)
        {  $rightSide = IS_UNIFORM;  };


    $index = 0;
    my $inCodeSection = 0;

    while ($index < scalar @$commentLines)
        {
        # Clear horizontal lines only if we're not in a code section.
        if ($commentLines->[$index] =~ /^ *([^a-zA-Z0-9 ])\1{3,}$/ ||
            ( length $commentLines->[$index] < 256 &&
              $commentLines->[$index] =~ /^ *([^a-zA-Z0-9 ])\1*([^a-zA-Z0-9 ])\2{3,}([^a-zA-Z0-9 ])\3*$/ ) )
        	{
        	if (!$inCodeSection)
        		{  $commentLines->[$index] = '';  }
        	}

        else
        	{
	        # Clear vertical lines.

	        if ($leftSide == IS_UNIFORM)
	            {
	            # This works because every line should either start this way, be blank, or be the first line that doesn't start with a
	            # symbol.
	            $commentLines->[$index] =~ s/^ *([^a-zA-Z0-9 ])\1*//;
	            };

	        if ($rightSide == IS_UNIFORM)
	            {
	            $commentLines->[$index] =~ s/ *([^a-zA-Z0-9 ])\1*$//;
	            };


	        # Clear horizontal lines again if there were vertical lines.  This catches lines that were separated from the verticals by
	        # whitespace.

	        if (($leftSide == IS_UNIFORM || $rightSide == IS_UNIFORM) && !$inCodeSection)
	            {
	            $commentLines->[$index] =~ s/^ *([^a-zA-Z0-9 ])\1{3,}$//;
	            $commentLines->[$index] =~ s/^ *([^a-zA-Z0-9 ])\1*([^a-zA-Z0-9 ])\2{3,}([^a-zA-Z0-9 ])\3*$//;
	            };


	        # Check for the start and end of code sections.  Note that this doesn't affect vertical line removal.

	        if (!$inCodeSection &&
	        	$commentLines->[$index] =~ /^ *\( *(?:(?:start|begin)? +)?(?:table|code|example|diagram) *\)$/i )
	        	{
	        	$inCodeSection = 1;
	        	}
	        elsif ($inCodeSection &&
	        	    $commentLines->[$index] =~ /^ *\( *(?:end|finish|done)(?: +(?:table|code|example|diagram))? *\)$/i)
	        	 {
	        	 $inCodeSection = 0;
	        	 }
	        }


        $index++;
        };

    };



###############################################################################
# Group: Processing Functions


#
#   Function: RepairPackages
#
#   Recalculates the packages for all comment topics using the auto-topics and the scope record.  Call this *before* calling
#   <MergeAutoTopics()>.
#
#   Parameters:
#
#       autoTopics - A reference to the list of automatically generated <NaturalDocs::Parser::ParsedTopics>.
#       scopeRecord - A reference to an array of <NaturalDocs::Languages::Advanced::ScopeChanges>.
#
sub RepairPackages #(autoTopics, scopeRecord)
    {
    my ($self, $autoTopics, $scopeRecord) = @_;

    my $topicIndex = 0;
    my $autoTopicIndex = 0;
    my $scopeIndex = 0;

    my $topic = $parsedFile[0];
    my $autoTopic = $autoTopics->[0];
    my $scopeChange = $scopeRecord->[0];

    my $currentPackage;
    my $inFakePackage;

    while (defined $topic)
        {
        # First update the scope via the record if its defined and has the lowest line number.
        if (defined $scopeChange &&
            $scopeChange->LineNumber() <= $topic->LineNumber() &&
            (!defined $autoTopic || $scopeChange->LineNumber() <= $autoTopic->LineNumber()) )
            {
            $currentPackage = $scopeChange->Scope();
            $scopeIndex++;
            $scopeChange = $scopeRecord->[$scopeIndex];  # Will be undef when past end.
            $inFakePackage = undef;
            }

        # Next try to end a fake scope with an auto topic if its defined and has the lowest line number.
        elsif (defined $autoTopic &&
                $autoTopic->LineNumber() <= $topic->LineNumber())
            {
            if ($inFakePackage)
                {
                $currentPackage = $autoTopic->Package();
                $inFakePackage = undef;
                };

            $autoTopicIndex++;
            $autoTopic = $autoTopics->[$autoTopicIndex];  # Will be undef when past end.
            }


        # Finally try to handle the topic, since it has the lowest line number.  Check for Type() because headerless topics won't have
        # one.
        else
            {
            my $scope;
            if ($topic->Type())
                {  $scope = NaturalDocs::Topics->TypeInfo($topic->Type())->Scope();  }
            else
                {  $scope = ::SCOPE_NORMAL();  };

            if ($scope == ::SCOPE_START() || $scope == ::SCOPE_END())
                {
                # They should already have the correct class and scope.
                $currentPackage = $topic->Package();
                $inFakePackage = 1;
                }
           else
                {
                # Fix the package of everything else.

                # Note that the first function or variable topic to appear in a fake package will assume that package even if it turns out
                # to be incorrect in the actual code, since the topic will come before the auto-topic.  This will be corrected in
                # MergeAutoTopics().

                $topic->SetPackage($currentPackage);
                };

            $topicIndex++;
            $topic = $parsedFile[$topicIndex];  # Will be undef when past end.
            };
        };

    };


#
#   Function: MergeAutoTopics
#
#   Merges the automatically generated topics into the file.  If an auto-topic matches an existing topic, it will have it's prototype
#   and package transferred.  If it doesn't, the auto-topic will be inserted into the list unless
#   <NaturalDocs::Settings->DocumentedOnly()> is set.  If an existing topic doesn't have a title, it's assumed to be a headerless
#   comment and will be merged with the next auto-topic or discarded.
#
#   Parameters:
#
#       language - The <NaturalDocs::Languages::Base>-derived class for the file.
#       autoTopics - A reference to the list of automatically generated topics.
#
sub MergeAutoTopics #(language, autoTopics)
    {
    my ($self, $language, $autoTopics) = @_;

    my $topicIndex = 0;
    my $autoTopicIndex = 0;

    # Keys are topic types, values are existence hashrefs of titles.
    my %topicsInLists;

    while ($topicIndex < scalar @parsedFile && $autoTopicIndex < scalar @$autoTopics)
        {
        my $topic = $parsedFile[$topicIndex];
        my $autoTopic = $autoTopics->[$autoTopicIndex];

        my $cleanTitle = $topic->Title();
        $cleanTitle =~ s/[\t ]*\([^\(]*$//;

        # Add the auto-topic if it's higher in the file than the current topic.
        if ($autoTopic->LineNumber() < $topic->LineNumber())
            {
            if (exists $topicsInLists{$autoTopic->Type()} &&
                exists $topicsInLists{$autoTopic->Type()}->{$autoTopic->Title()})
                {
                # Remove it from the list so a second one with the same name will be added.
                delete $topicsInLists{$autoTopic->Type()}->{$autoTopic->Title()};
                }
            elsif (!NaturalDocs::Settings->DocumentedOnly())
                {
                splice(@parsedFile, $topicIndex, 0, $autoTopic);
                $topicIndex++;
                };

            $autoTopicIndex++;
            }

        # Remove a headerless topic if there's another topic between it and the next auto-topic.
        elsif (!$topic->Title() && $topicIndex + 1 < scalar @parsedFile &&
                $parsedFile[$topicIndex+1]->LineNumber() < $autoTopic->LineNumber())
            {
            splice(@parsedFile, $topicIndex, 1);
            }

        # Transfer information if we have a match or a headerless topic.
        elsif ( !$topic->Title() ||
        		  $topic->Symbol() eq $autoTopic->Symbol() ||
        		  ( $topic->Type() == $autoTopic->Type() &&
        			( index($autoTopic->Title(), $cleanTitle) != -1 || index($cleanTitle, $autoTopic->Title()) != -1 ) ) )
            {
            $topic->SetType($autoTopic->Type());
            $topic->SetPrototype($autoTopic->Prototype());
            $topic->SetUsing($autoTopic->Using());

            if (!$topic->Title())
                {  $topic->SetTitle($autoTopic->Title());  };

            if (NaturalDocs::Topics->TypeInfo($topic->Type())->Scope() != ::SCOPE_START())
                {  $topic->SetPackage($autoTopic->Package());  }
            elsif ($autoTopic->Package() ne $topic->Package())
                {
                my @autoPackageIdentifiers = NaturalDocs::SymbolString->IdentifiersOf($autoTopic->Package());
                my @packageIdentifiers = NaturalDocs::SymbolString->IdentifiersOf($topic->Package());

                while (scalar @autoPackageIdentifiers && $autoPackageIdentifiers[-1] eq $packageIdentifiers[-1])
                    {
                    pop @autoPackageIdentifiers;
                    pop @packageIdentifiers;
                    };

                if (scalar @autoPackageIdentifiers)
                    {  $topic->SetPackage( NaturalDocs::SymbolString->Join(@autoPackageIdentifiers) );  };
                };

            $topicIndex++;
            $autoTopicIndex++;
            }

        # Extract topics in lists.
        elsif ($topic->IsList())
            {
            if (!exists $topicsInLists{$topic->Type()})
                {  $topicsInLists{$topic->Type()} = { };  };

            my $body = $topic->Body();

            while ($body =~ /<ds>([^<]+)<\/ds>/g)
                {  $topicsInLists{$topic->Type()}->{NaturalDocs::NDMarkup->RestoreAmpChars($1)} = 1;  };

            $topicIndex++;
            }

        # Otherwise there's no match.  Skip the topic.  The auto-topic will be added later.
        else
            {
            $topicIndex++;
            }
        };

    # Add any auto-topics remaining.
    if (!NaturalDocs::Settings->DocumentedOnly())
    	{
	    while ($autoTopicIndex < scalar @$autoTopics)
	        {
	        my $autoTopic = $autoTopics->[$autoTopicIndex];

	        if (exists $topicsInLists{$autoTopic->Type()} &&
	            exists $topicsInLists{$autoTopic->Type()}->{$autoTopic->Title()})
	            {
	            # Remove it from the list so a second one with the same name will be added.
	            delete $topicsInLists{$autoTopic->Type()}->{$autoTopic->Title()};
	            }
	        else
	            {
	            push(@parsedFile, $autoTopic);
	            };

	        $autoTopicIndex++;
	        };
        };
   };


#
#   Function: RemoveRemainingHeaderlessTopics
#
#   After <MergeAutoTopics()> is done, this function removes any remaining headerless topics from the file.  If they don't merge
#   into anything, they're not valid topics.
#
sub RemoveRemainingHeaderlessTopics
    {
    my ($self) = @_;

    my $index = 0;
    while ($index < scalar @parsedFile)
        {
        if ($parsedFile[$index]->Title())
            {  $index++;  }
        else
            {  splice(@parsedFile, $index, 1);  };
        };
    };


#
#   Function: MakeAutoGroups
#
#   Creates group topics for files that do not have them.
#
sub MakeAutoGroups
    {
    my ($self) = @_;

    # No groups only one topic.
    if (scalar @parsedFile < 2)
        {  return;  };

    my $index = 0;
    my $startStretch = 0;

    # Skip the first entry if its the page title.
    if (NaturalDocs::Topics->TypeInfo( $parsedFile[0]->Type() )->PageTitleIfFirst())
        {
        $index = 1;
        $startStretch = 1;
        };

    # Make auto-groups for each stretch between scope-altering topics.
    while ($index < scalar @parsedFile)
        {
        my $scope = NaturalDocs::Topics->TypeInfo($parsedFile[$index]->Type())->Scope();

        if ($scope == ::SCOPE_START() || $scope == ::SCOPE_END())
            {
            if ($index > $startStretch)
                {  $index += $self->MakeAutoGroupsFor($startStretch, $index);  };

            $startStretch = $index + 1;
            };

        $index++;
        };

    if ($index > $startStretch)
        {  $self->MakeAutoGroupsFor($startStretch, $index);  };
    };


#
#   Function: MakeAutoGroupsFor
#
#   Creates group topics for sections of files that do not have them.  A support function for <MakeAutoGroups()>.
#
#   Parameters:
#
#       startIndex - The index to start at.
#       endIndex - The index to end at.  Not inclusive.
#
#   Returns:
#
#       The number of group topics added.
#
sub MakeAutoGroupsFor #(startIndex, endIndex)
    {
    my ($self, $startIndex, $endIndex) = @_;

    # No groups if any are defined already.
    for (my $i = $startIndex; $i < $endIndex; $i++)
        {
        if ($parsedFile[$i]->Type() eq ::TOPIC_GROUP())
            {  return 0;  };
        };


    use constant COUNT => 0;
    use constant TYPE => 1;
    use constant SECOND_TYPE => 2;
    use constant SIZE => 3;

    # This is an array of ( count, type, secondType ) triples.  Count and Type will always be filled in; count is the number of
    # consecutive topics of type.  On the second pass, if small groups are combined secondType will be filled in.  There will not be
    # more than two types per group.
    my @groups;
    my $groupIndex = 0;


    # First pass: Determine all the groups.

    my $i = $startIndex;
    my $currentType;

    while ($i < $endIndex)
        {
        if (!defined $currentType || ($parsedFile[$i]->Type() ne $currentType && $parsedFile[$i]->Type() ne ::TOPIC_GENERIC()) )
            {
            if (defined $currentType)
                {  $groupIndex += SIZE;  };

            $currentType = $parsedFile[$i]->Type();

            $groups[$groupIndex + COUNT] = 1;
            $groups[$groupIndex + TYPE] = $currentType;
            }
        else
            {  $groups[$groupIndex + COUNT]++;  };

        $i++;
        };


    # Second pass: Combine groups based on "noise".  Noise means types go from A to B to A at least once, and there are at least
    # two groups in a row with three or less, and at least one of those groups is two or less.  So 3, 3, 3 doesn't count as noise, but
    # 3, 2, 3 does.

    $groupIndex = 0;

    # While there are at least three groups left...
    while ($groupIndex < scalar @groups - (2 * SIZE))
        {
        # If the group two places in front of this one has the same type...
        if ($groups[$groupIndex + (2 * SIZE) + TYPE] eq $groups[$groupIndex + TYPE])
            {
            # It means we went from A to B to A, which partially qualifies as noise.

            my $firstType = $groups[$groupIndex + TYPE];
            my $secondType = $groups[$groupIndex + SIZE + TYPE];

            if (NaturalDocs::Topics->TypeInfo($firstType)->CanGroupWith($secondType) ||
                NaturalDocs::Topics->TypeInfo($secondType)->CanGroupWith($firstType))
                {
                my $hasNoise;

                my $hasThrees;
                my $hasTwosOrOnes;

                my $endIndex = $groupIndex;

                while ($endIndex < scalar @groups &&
                         ($groups[$endIndex + TYPE] eq $firstType || $groups[$endIndex + TYPE] eq $secondType))
                    {
                    if ($groups[$endIndex + COUNT] > 3)
                        {
                        # They must be consecutive to count.
                        $hasThrees = 0;
                        $hasTwosOrOnes = 0;
                        }
                    elsif ($groups[$endIndex + COUNT] == 3)
                        {
                        $hasThrees = 1;

                        if ($hasTwosOrOnes)
                            {  $hasNoise = 1;  };
                        }
                    else # < 3
                        {
                        if ($hasThrees || $hasTwosOrOnes)
                            {  $hasNoise = 1;  };

                        $hasTwosOrOnes = 1;
                        };

                    $endIndex += SIZE;
                    };

                if (!$hasNoise)
                    {
                    $groupIndex = $endIndex - SIZE;
                    }
                else # hasNoise
                    {
                    $groups[$groupIndex + SECOND_TYPE] = $secondType;

                    for (my $noiseIndex = $groupIndex + SIZE; $noiseIndex < $endIndex; $noiseIndex += SIZE)
                        {
                        $groups[$groupIndex + COUNT] += $groups[$noiseIndex + COUNT];
                        };

                    splice(@groups, $groupIndex + SIZE, $endIndex - $groupIndex - SIZE);

                    $groupIndex += SIZE;
                    };
                }

            else # They can't group together
                {
                $groupIndex += SIZE;
                };
            }

        else
            {  $groupIndex += SIZE;  };
        };


    # Finally, create group topics for the parsed file.

    $groupIndex = 0;
    $i = $startIndex;

    while ($groupIndex < scalar @groups)
        {
        if ($groups[$groupIndex + TYPE] ne ::TOPIC_GENERIC())
            {
            my $topic = $parsedFile[$i];
            my $title = NaturalDocs::Topics->NameOfType($groups[$groupIndex + TYPE], 1);

            if (defined $groups[$groupIndex + SECOND_TYPE])
                {  $title .= ' and ' . NaturalDocs::Topics->NameOfType($groups[$groupIndex + SECOND_TYPE], 1);  };

            splice(@parsedFile, $i, 0, NaturalDocs::Parser::ParsedTopic->New(::TOPIC_GROUP(),
                                                                                                            $title,
                                                                                                            $topic->Package(), $topic->Using(),
                                                                                                            undef, undef, undef,
                                                                                                            $topic->LineNumber()) );
            $i++;
            };

        $i += $groups[$groupIndex + COUNT];
        $groupIndex += SIZE;
        };

    return (scalar @groups / SIZE);
    };


#
#   Function: AddToClassHierarchy
#
#   Adds any class topics to the class hierarchy, since they may not have been called with <OnClass()> if they didn't match up to
#   an auto-topic.
#
sub AddToClassHierarchy
    {
    my ($self) = @_;

    foreach my $topic (@parsedFile)
        {
        if ($topic->Type() && NaturalDocs::Topics->TypeInfo( $topic->Type() )->ClassHierarchy())
            {
            if ($topic->IsList())
                {
                my $body = $topic->Body();

                while ($body =~ /<ds>([^<]+)<\/ds>/g)
                    {
                    $self->OnClass( NaturalDocs::SymbolString->FromText( NaturalDocs::NDMarkup->RestoreAmpChars($1) ) );
                    };
                }
            else
                {
                $self->OnClass($topic->Package());
                };
            };
        };
    };


#
#   Function: AddPackageDelineators
#
#   Adds section and class topics to make sure the package is correctly represented in the documentation.  Should be called last in
#   this process.
#
sub AddPackageDelineators
    {
    my ($self) = @_;

    my $index = 0;
    my $currentPackage;

    # Values are the arrayref [ title, type ];
    my %usedPackages;

    while ($index < scalar @parsedFile)
        {
        my $topic = $parsedFile[$index];

        if ($topic->Package() ne $currentPackage)
            {
            $currentPackage = $topic->Package();
            my $scopeType = NaturalDocs::Topics->TypeInfo($topic->Type())->Scope();

            if ($scopeType == ::SCOPE_START())
                {
                $usedPackages{$currentPackage} = [ $topic->Title(), $topic->Type() ];
                }
            elsif ($scopeType == ::SCOPE_END())
                {
                my $newTopic;

                if (!defined $currentPackage)
                    {
                    $newTopic = NaturalDocs::Parser::ParsedTopic->New(::TOPIC_SECTION(), 'Global',
                                                                                                   undef, undef,
                                                                                                   undef, undef, undef,
                                                                                                   $topic->LineNumber(), undef);
                    }
                else
                    {
                    my ($title, $body, $summary, $type);
                    my @packageIdentifiers = NaturalDocs::SymbolString->IdentifiersOf($currentPackage);

                    if (exists $usedPackages{$currentPackage})
                        {
                        $title = $usedPackages{$currentPackage}->[0];
                        $type = $usedPackages{$currentPackage}->[1];
                        $body = '<p>(continued)</p>';
                        $summary = '(continued)';
                        }
                    else
                        {
                        $title = join($language->PackageSeparator(), @packageIdentifiers);
                        $type = ::TOPIC_CLASS();

                        # Body and summary stay undef.

                        $usedPackages{$currentPackage} = $title;
                        };

                    my @titleIdentifiers = NaturalDocs::SymbolString->IdentifiersOf( NaturalDocs::SymbolString->FromText($title) );
                    for (my $i = 0; $i < scalar @titleIdentifiers; $i++)
                        {  pop @packageIdentifiers;  };

                    $newTopic = NaturalDocs::Parser::ParsedTopic->New($type, $title,
                                                                                                   NaturalDocs::SymbolString->Join(@packageIdentifiers), undef,
                                                                                                   undef, $summary, $body,
                                                                                                   $topic->LineNumber(), undef);
                    }

                splice(@parsedFile, $index, 0, $newTopic);
                $index++;
                }
            };

        $index++;
        };
    };


#
#   Function: BreakLists
#
#   Breaks list topics into individual topics.
#
sub BreakLists
    {
    my $self = shift;

    my $index = 0;

    while ($index < scalar @parsedFile)
        {
        my $topic = $parsedFile[$index];

        if ($topic->IsList() && NaturalDocs::Topics->TypeInfo( $topic->Type() )->BreakLists())
            {
            my $body = $topic->Body();

            my @newTopics;
            my $newBody;

            my $bodyIndex = 0;

            for (;;)
                {
                my $startList = index($body, '<dl>', $bodyIndex);

                if ($startList == -1)
                    {  last;  };

                $newBody .= substr($body, $bodyIndex, $startList - $bodyIndex);

                my $endList = index($body, '</dl>', $startList);
                my $listBody = substr($body, $startList, $endList - $startList);

                while ($listBody =~ /<ds>([^<]+)<\/ds><dd>(.*?)<\/dd>/g)
                    {
                    my ($symbol, $description) = ($1, $2);

                    push @newTopics, NaturalDocs::Parser::ParsedTopic->New( $topic->Type(), $symbol, $topic->Package(),
                                                                                                            $topic->Using(), undef,
                                                                                                            $self->GetSummaryFromDescriptionList($description),
                                                                                                            '<p>' . $description .  '</p>', $topic->LineNumber(),
                                                                                                            undef );
                    };

                $bodyIndex = $endList + 5;
                };

            $newBody .= substr($body, $bodyIndex);

            # Remove trailing headings.
            $newBody =~ s/(?:<h>[^<]+<\/h>)+$//;

            # Remove empty headings.
            $newBody =~ s/(?:<h>[^<]+<\/h>)+(<h>[^<]+<\/h>)/$1/g;

            if ($newBody)
                {
                unshift @newTopics, NaturalDocs::Parser::ParsedTopic->New( ::TOPIC_GROUP(), $topic->Title(), $topic->Package(),
                                                                                                          $topic->Using(), undef,
                                                                                                          $self->GetSummaryFromBody($newBody), $newBody,
                                                                                                          $topic->LineNumber(), undef );
                };

            splice(@parsedFile, $index, 1, @newTopics);

            $index += scalar @newTopics;
            }

        else # not a list
            {  $index++;  };
        };
    };


#
#   Function: GetSummaryFromBody
#
#   Returns the summary text from the topic body.
#
#   Parameters:
#
#       body - The complete topic body, in <NDMarkup>.
#
#   Returns:
#
#       The topic summary, or undef if none.
#
sub GetSummaryFromBody #(body)
    {
    my ($self, $body) = @_;

    my $summary;

    # Extract the first sentence from the leading paragraph, if any.  We'll tolerate a single header beforehand, but nothing else.

    if ($body =~ /^(?:<h>[^<]*<\/h>)?<p>(.*?)(<\/p>|[\.\!\?](?:[\)\}\'\ ]|&quot;|&gt;))/x)
        {
        $summary = $1;

        if ($2 ne '</p>')
            {  $summary .= $2;  };
        };

    return $summary;
    };


#
#   Function: GetSummaryFromDescriptionList
#
#   Returns the summary text from a description list entry.
#
#   Parameters:
#
#       description - The description in <NDMarkup>.  Should be the content between the <dd></dd> tags only.
#
#   Returns:
#
#       The description summary, or undef if none.
#
sub GetSummaryFromDescriptionList #(description)
    {
    my ($self, $description) = @_;

    my $summary;

    if ($description =~ /^(.*?)($|[\.\!\?](?:[\)\}\'\ ]|&quot;|&gt;))/)
        {  $summary = $1 . $2;  };

    return $summary;
    };


1;
