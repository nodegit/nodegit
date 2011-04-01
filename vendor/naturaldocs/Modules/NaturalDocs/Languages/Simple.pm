###############################################################################
#
#   Class: NaturalDocs::Languages::Simple
#
###############################################################################
#
#   A class containing the characteristics of a particular programming language for basic support within Natural Docs.
#   Also serves as a base class for languages that break from general conventions, such as not having parameter lists use
#   parenthesis and commas.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Simple;

use base 'NaturalDocs::Languages::Base';
use base 'Exporter';

our @EXPORT = ( 'ENDER_ACCEPT', 'ENDER_IGNORE', 'ENDER_ACCEPT_AND_CONTINUE', 'ENDER_REVERT_TO_ACCEPTED' );


use NaturalDocs::DefineMembers 'LINE_COMMENT_SYMBOLS', 'LineCommentSymbols()', 'SetLineCommentSymbols() duparrayref',
                                                 'BLOCK_COMMENT_SYMBOLS', 'BlockCommentSymbols()',
                                                                                              'SetBlockCommentSymbols() duparrayref',
                                                 'PROTOTYPE_ENDERS',
                                                 'LINE_EXTENDER', 'LineExtender()', 'SetLineExtender()',
                                                 'PACKAGE_SEPARATOR', 'PackageSeparator()',
                                                 'PACKAGE_SEPARATOR_WAS_SET', 'PackageSeparatorWasSet()',
                                                 'ENUM_VALUES', 'EnumValues()',
                                                 'ENUM_VALUES_WAS_SET', 'EnumValuesWasSet()';

#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       name - The name of the language.
#
sub New #(name)
    {
    my ($selfPackage, $name) = @_;

    my $object = $selfPackage->SUPER::New($name);

    $object->[ENUM_VALUES] = ::ENUM_GLOBAL();
    $object->[PACKAGE_SEPARATOR] = '.';

    return $object;
    };


#
#   Functions: Members
#
#   LineCommentSymbols - Returns an arrayref of symbols that start a line comment, or undef if none.
#   SetLineCommentSymbols - Replaces the arrayref of symbols that start a line comment.
#   BlockCommentSymbols - Returns an arrayref of start/end symbol pairs that specify a block comment, or undef if none.  Pairs
#                                        are specified with two consecutive array entries.
#   SetBlockCommentSymbols - Replaces the arrayref of start/end symbol pairs that specify a block comment.  Pairs are
#                                             specified with two consecutive array entries.
#   LineExtender - Returns the symbol to ignore a line break in languages where line breaks are significant.
#   SetLineExtender - Replaces the symbol to ignore a line break in languages where line breaks are significant.
#   PackageSeparator - Returns the package separator symbol.
#   PackageSeparatorWasSet - Returns whether the package separator symbol was ever changed from the default.
#

#
#   Function: SetPackageSeparator
#   Replaces the language's package separator string.
#
sub SetPackageSeparator #(separator)
    {
    my ($self, $separator) = @_;
    $self->[PACKAGE_SEPARATOR] = $separator;
    $self->[PACKAGE_SEPARATOR_WAS_SET] = 1;
    };


#
#   Functions: Members
#
#   EnumValues - Returns the <EnumValuesType> that describes how the language handles enums.
#   EnumValuesWasSet - Returns whether <EnumValues> was ever changed from the default.


#
#   Function: SetEnumValues
#   Replaces the <EnumValuesType> that describes how the language handles enums.
#
sub SetEnumValues #(EnumValuesType newBehavior)
    {
    my ($self, $behavior) = @_;
    $self->[ENUM_VALUES] = $behavior;
    $self->[ENUM_VALUES_WAS_SET] = 1;
    };


#
#   Function: PrototypeEndersFor
#
#   Returns an arrayref of prototype ender symbols for the passed <TopicType>, or undef if none.
#
sub PrototypeEndersFor #(type)
    {
    my ($self, $type) = @_;

    if (defined $self->[PROTOTYPE_ENDERS])
        {  return $self->[PROTOTYPE_ENDERS]->{$type};  }
    else
        {  return undef;  };
    };


#
#   Function: SetPrototypeEndersFor
#
#   Replaces the arrayref of prototype ender symbols for the passed <TopicType>.
#
sub SetPrototypeEndersFor #(type, enders)
    {
    my ($self, $type, $enders) = @_;

    if (!defined $self->[PROTOTYPE_ENDERS])
        {  $self->[PROTOTYPE_ENDERS] = { };  };

    if (!defined $enders)
        {  delete $self->[PROTOTYPE_ENDERS]->{$type};  }
    else
        {
        $self->[PROTOTYPE_ENDERS]->{$type} = [ @$enders ];
        };
    };




###############################################################################
# Group: Parsing Functions


#
#   Function: ParseFile
#
#   Parses the passed source file, sending comments acceptable for documentation to <NaturalDocs::Parser->OnComment()>
#   and all other sections to <OnCode()>.
#
#   Parameters:
#
#       sourceFile - The <FileName> of the source file to parse.
#       topicList - A reference to the list of <NaturalDocs::Parser::ParsedTopics> being built by the file.
#
#   Returns:
#
#       Since this class cannot automatically document the code or generate a scope record, it always returns ( undef, undef ).
#
sub ParseFile #(sourceFile, topicsList)
    {
    my ($self, $sourceFile, $topicsList) = @_;

    open(SOURCEFILEHANDLE, '<' . $sourceFile)
        or die "Couldn't open input file " . $sourceFile . "\n";

    my $lineReader = NaturalDocs::LineReader->New(\*SOURCEFILEHANDLE);

    my @commentLines;
    my @codeLines;
    my $lastCommentTopicCount = 0;

    if ($self->Name() eq 'Text File')
        {
        @commentLines = $lineReader->GetAll();
        NaturalDocs::Parser->OnComment(\@commentLines, 1);
        }

    else
        {
        my $line = $lineReader->Get();
        my $lineNumber = 1;

        while (defined $line)
            {
            my $originalLine = $line;


            # Retrieve multiline comments.  This leaves $line at the next line.
            # We check for multiline comments before single line comments because in Lua the symbols are --[[ and --.

            if (my $closingSymbol = $self->StripOpeningBlockSymbols(\$line, $self->BlockCommentSymbols()))
                {
                # Note that it is possible for a multiline comment to start correctly but not end so.  We want those comments to stay in
                # the code.  For example, look at this prototype with this splint annotation:
                #
                # int get_array(integer_t id,
                #                    /*@out@*/ array_t array);
                #
                # The annotation starts correctly but doesn't end so because it is followed by code on the same line.

                my $lineRemainder;

                for (;;)
                    {
                    $lineRemainder = $self->StripClosingSymbol(\$line, $closingSymbol);

                    push @commentLines, $line;

                    #  If we found an end comment symbol...
                    if (defined $lineRemainder)
                        {  last;  };

                    $line = $lineReader->Get();

                    if (!defined $line)
                        {  last;  };
                    };

                if ($lineRemainder !~ /^[ \t]*$/)
                    {
                    # If there was something past the closing symbol this wasn't an acceptable comment, so move the lines to code.
                    push @codeLines, @commentLines;
                    @commentLines = ( );
                    };

                $line = $lineReader->Get();
                }


            # Retrieve single line comments.  This leaves $line at the next line.

            elsif ($self->StripOpeningSymbols(\$line, $self->LineCommentSymbols()))
                {
                do
                    {
                    push @commentLines, $line;
                    $line = $lineReader->Get();

                    if (!defined $line)
                        {  goto EndDo;  };
                    }
                while ($self->StripOpeningSymbols(\$line, $self->LineCommentSymbols()));

                EndDo:  # I hate Perl sometimes.
                }


            # Otherwise just add it to the code.

            else
                {
                push @codeLines, $line;
                $line = $lineReader->Get();
                };


            # If there were comments, send them to Parser->OnComment().

            if (scalar @commentLines)
                {
                # First process any code lines before the comment.
                if (scalar @codeLines)
                    {
                    $self->OnCode(\@codeLines, $lineNumber, $topicsList, $lastCommentTopicCount);
                    $lineNumber += scalar @codeLines;
                    @codeLines = ( );
                    };

                $lastCommentTopicCount = NaturalDocs::Parser->OnComment(\@commentLines, $lineNumber);
                $lineNumber += scalar @commentLines;
                @commentLines = ( );
                };

            };  # while (defined $line)


        # Clean up any remaining code.
        if (scalar @codeLines)
            {
            $self->OnCode(\@codeLines, $lineNumber, $topicsList, $lastCommentTopicCount);
            @codeLines = ( );
            };

        };

    close(SOURCEFILEHANDLE);

    return ( undef, undef );
    };


#
#   Function: OnCode
#
#   Called whenever a section of code is encountered by the parser.  Is used to find the prototype of the last topic created.
#
#   Parameters:
#
#       codeLines - The source code as an arrayref of lines.
#       codeLineNumber - The line number of the first line of code.
#       topicList - A reference to the list of <NaturalDocs::Parser::ParsedTopics> being built by the file.
#       lastCommentTopicCount - The number of Natural Docs topics that were created by the last comment.
#
sub OnCode #(codeLines, codeLineNumber, topicList, lastCommentTopicCount)
    {
    my ($self, $codeLines, $codeLineNumber, $topicList, $lastCommentTopicCount) = @_;

    if ($lastCommentTopicCount && defined $self->PrototypeEndersFor($topicList->[-1]->Type()))
        {
        my $lineIndex = 0;
        my $prototype;

        # Skip all blank lines before a prototype.
        while ($lineIndex < scalar @$codeLines && $codeLines->[$lineIndex] =~ /^[ \t]*$/)
            {  $lineIndex++;  };

        my @tokens;
        my $tokenIndex = 0;

        my @brackets;
        my $enders = $self->PrototypeEndersFor($topicList->[-1]->Type());

        # Add prototype lines until we reach the end of the prototype or the end of the code lines.
        while ($lineIndex < scalar @$codeLines)
            {
            my $line = $self->RemoveLineExtender($codeLines->[$lineIndex] . "\n");

            push @tokens, $line =~ /([^\(\)\[\]\{\}\<\>]+|.)/g;

            while ($tokenIndex < scalar @tokens)
                {
                # If we're not inside brackets, check for ender symbols.
                if (!scalar @brackets)
                    {
                    my $startingIndex = 0;
                    my $testPrototype;

                    for (;;)
                        {
                        my ($enderIndex, $ender) = ::FindFirstSymbol($tokens[$tokenIndex], $enders, $startingIndex);

                        if ($enderIndex == -1)
                            {  last;  }
                        else
                            {
                            # We do this here so we don't duplicate prototype for every single token.  Just the first time an ender symbol
                            # is found in one.
                            if (!defined $testPrototype)
                                {  $testPrototype = $prototype;  };

                            $testPrototype .= substr($tokens[$tokenIndex], $startingIndex, $enderIndex - $startingIndex);

                            my $enderResult;

                            # If the ender is all text and the character preceding or following it is as well, ignore it.
                            if ($ender =~ /^[a-z0-9]+$/i &&
                                ( ($enderIndex > 0 && substr($tokens[$tokenIndex], $enderIndex - 1, 1) =~ /^[a-z0-9_]$/i) ||
                                   substr($tokens[$tokenIndex], $enderIndex + length($ender), 1) =~ /^[a-z0-9_]$/i ) )
                                {  $enderResult = ENDER_IGNORE();  }
                            else
                                {  $enderResult = $self->OnPrototypeEnd($topicList->[-1]->Type(), \$testPrototype, $ender);  }

                            if ($enderResult == ENDER_IGNORE())
                                {
                                $testPrototype .= $ender;
                                $startingIndex = $enderIndex + length($ender);
                                }
                            elsif ($enderResult == ENDER_REVERT_TO_ACCEPTED())
                                {
                                return;
                                }
                            else # ENDER_ACCEPT || ENDER_ACCEPT_AND_CONTINUE
                                {
                                my $titleInPrototype = $topicList->[-1]->Title();

                                # Strip parenthesis so Function(2) and Function(int, int) will still match Function(anything).
                                $titleInPrototype =~ s/[\t ]*\([^\(]*$//;

                                if (index($testPrototype, $titleInPrototype) != -1)
                                    {
                                    $topicList->[-1]->SetPrototype( $self->NormalizePrototype($testPrototype) );
                                    };

                                if ($enderResult == ENDER_ACCEPT())
                                    {  return;  }
                                else # ENDER_ACCEPT_AND_CONTINUE
                                    {
                                    $testPrototype .= $ender;
                                    $startingIndex = $enderIndex + length($ender);
                                    };
                                };
                            };
                        };
                    }

                # If we are inside brackets, check for closing symbols.
                elsif ( ($tokens[$tokenIndex] eq ')' && $brackets[-1] eq '(') ||
                         ($tokens[$tokenIndex] eq ']' && $brackets[-1] eq '[') ||
                         ($tokens[$tokenIndex] eq '}' && $brackets[-1] eq '{') ||
                         ($tokens[$tokenIndex] eq '>' && $brackets[-1] eq '<') )
                    {
                    pop @brackets;
                    };

                # Check for opening brackets.
				if ($tokens[$tokenIndex] =~ /^[\(\[\{]$/ ||
				    ($tokens[$tokenIndex] eq "<" && $tokens[$tokenIndex-1] !~ /operator[ \t]*$/) )
	                {
                    push @brackets, $tokens[$tokenIndex];
                    };

                $prototype .= $tokens[$tokenIndex];
                $tokenIndex++;
                };

            $lineIndex++;
            };

        # If we got out of that while loop by running out of lines, there was no prototype.
        };
    };


use constant ENDER_ACCEPT => 1;
use constant ENDER_IGNORE => 2;
use constant ENDER_ACCEPT_AND_CONTINUE => 3;
use constant ENDER_REVERT_TO_ACCEPTED => 4;

#
#   Function: OnPrototypeEnd
#
#   Called whenever the end of a prototype is found so that there's a chance for derived classes to mark false positives.
#
#   Parameters:
#
#       type - The <TopicType> of the prototype.
#       prototypeRef - A reference to the prototype so far, minus the ender in dispute.
#       ender - The ender symbol.
#
#   Returns:
#
#       ENDER_ACCEPT - The ender is accepted and the prototype is finished.
#       ENDER_IGNORE - The ender is rejected and parsing should continue.  Note that the prototype will be rejected as a whole
#                                  if all enders are ignored before reaching the end of the code.
#       ENDER_ACCEPT_AND_CONTINUE - The ender is accepted so the prototype may stand as is.  However, the prototype might
#                                                          also continue on so continue parsing.  If there is no accepted ender between here and
#                                                          the end of the code this version will be accepted instead.
#       ENDER_REVERT_TO_ACCEPTED - The expedition from ENDER_ACCEPT_AND_CONTINUE failed.  Use the last accepted
#                                                        version and end parsing.
#
sub OnPrototypeEnd #(type, prototypeRef, ender)
    {
    return ENDER_ACCEPT();
    };


#
#   Function: RemoveLineExtender
#
#   If the passed line has a line extender, returns it without the extender or the line break that follows.  If it doesn't, or there are
#   no line extenders defined, returns the passed line unchanged.
#
sub RemoveLineExtender #(line)
    {
    my ($self, $line) = @_;

    if (defined $self->LineExtender())
        {
        my $lineExtenderIndex = rindex($line, $self->LineExtender());

        if ($lineExtenderIndex != -1 &&
            substr($line, $lineExtenderIndex + length($self->LineExtender())) =~ /^[ \t]*\n$/)
            {
            $line = substr($line, 0, $lineExtenderIndex) . ' ';
            };
        };

    return $line;
    };


1;
