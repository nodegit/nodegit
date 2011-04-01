###############################################################################
#
#   Class: NaturalDocs::Languages::Base
#
###############################################################################
#
#   A base class for all programming language parsers.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Base;

use NaturalDocs::DefineMembers 'NAME', 'Name()',
                                                 'EXTENSIONS', 'Extensions()', 'SetExtensions() duparrayref',
                                                 'SHEBANG_STRINGS', 'ShebangStrings()', 'SetShebangStrings() duparrayref',
                                                 'IGNORED_PREFIXES',
                                                 'ENUM_VALUES';

use base 'Exporter';
our @EXPORT = ('ENUM_GLOBAL', 'ENUM_UNDER_TYPE', 'ENUM_UNDER_PARENT');


#
#   Constants: EnumValuesType
#
#   How enum values are handled in the language.
#
#   ENUM_GLOBAL - Values are always global and thus 'value'.
#   ENUM_UNDER_TYPE - Values are under the type in the hierarchy, and thus 'package.enum.value'.
#   ENUM_UNDER_PARENT - Values are under the parent in the hierarchy, putting them on the same level as the enum itself.  Thus
#                                       'package.value'.
#
use constant ENUM_GLOBAL => 1;
use constant ENUM_UNDER_TYPE => 2;
use constant ENUM_UNDER_PARENT => 3;


#
#   Handle: SOURCEFILEHANDLE
#
#   The handle of the source file currently being parsed.
#


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

    my $object = [ ];

    $object->[NAME] = $name;

    bless $object, $selfPackage;
    return $object;
    };


#
#   Functions: Members
#
#   Name - Returns the language's name.
#   Extensions - Returns an arrayref of the language's file extensions, or undef if none.
#   SetExtensions - Replaces the arrayref of the language's file extensions.
#   ShebangStrings - Returns an arrayref of the language's shebang strings, or undef if none.
#   SetShebangStrings - Replaces the arrayref of the language's shebang strings.
#

#
#   Function: PackageSeparator
#   Returns the language's package separator string.
#
sub PackageSeparator
    {  return '.';  };

#
#   Function: PackageSeparatorWasSet
#   Returns whether the language's package separator string was ever changed from the default.
#
sub PackageSeparatorWasSet
    {  return 0;  };


#
#   Function: EnumValues
#   Returns the <EnumValuesType> that describes how the language handles enums.
#
sub EnumValues
    {  return ENUM_GLOBAL;  };


#
#   Function: IgnoredPrefixesFor
#
#   Returns an arrayref of ignored prefixes for the passed <TopicType>, or undef if none.  The array is sorted so that the longest
#   prefixes are first.
#
sub IgnoredPrefixesFor #(type)
    {
    my ($self, $type) = @_;

    if (defined $self->[IGNORED_PREFIXES])
        {  return $self->[IGNORED_PREFIXES]->{$type};  }
    else
        {  return undef;  };
    };


#
#   Function: SetIgnoredPrefixesFor
#
#   Replaces the arrayref of ignored prefixes for the passed <TopicType>.
#
sub SetIgnoredPrefixesFor #(type, prefixes)
    {
    my ($self, $type, $prefixesRef) = @_;

    if (!defined $self->[IGNORED_PREFIXES])
        {  $self->[IGNORED_PREFIXES] = { };  };

    if (!defined $prefixesRef)
        {  delete $self->[IGNORED_PREFIXES]->{$type};  }
    else
        {
        my $prefixes = [ @$prefixesRef ];

        # Sort prefixes to be longest to shortest.
        @$prefixes = sort { length $b <=> length $a } @$prefixes;

        $self->[IGNORED_PREFIXES]->{$type} = $prefixes;
        };
    };


#
#   Function: HasIgnoredPrefixes
#
#   Returns whether the language has any ignored prefixes at all.
#
sub HasIgnoredPrefixes
    {  return defined $_[0]->[IGNORED_PREFIXES];  };


#
#   Function: CopyIgnoredPrefixesOf
#
#   Copies all the ignored prefix settings of the passed <NaturalDocs::Languages::Base> object.
#
sub CopyIgnoredPrefixesOf #(language)
    {
    my ($self, $language) = @_;

    if ($language->HasIgnoredPrefixes())
        {
        $self->[IGNORED_PREFIXES] = { };

        while (my ($topicType, $prefixes) = each %{$language->[IGNORED_PREFIXES]})
            {
            $self->[IGNORED_PREFIXES]->{$topicType} = [ @$prefixes ];
            };
        };
    };



###############################################################################
# Group: Parsing Functions


#
#   Function: ParseFile
#
#   Parses the passed source file, sending comments acceptable for documentation to <NaturalDocs::Parser->OnComment()>.
#   This *must* be defined by a subclass.
#
#   Parameters:
#
#       sourceFile - The <FileName> of the source file to parse.
#       topicList - A reference to the list of <NaturalDocs::Parser::ParsedTopics> being built by the file.
#
#   Returns:
#
#       The array ( autoTopics, scopeRecord ).
#
#       autoTopics - An arrayref of automatically generated <NaturalDocs::Parser::ParsedTopics> from the file, or undef if none.
#       scopeRecord - An arrayref of <NaturalDocs::Languages::Advanced::ScopeChanges>, or undef if none.
#


#
#   Function: ParsePrototype
#
#   Parses the prototype and returns it as a <NaturalDocs::Languages::Prototype> object.
#
#   Parameters:
#
#       type - The <TopicType>.
#       prototype - The text prototype.
#
#   Returns:
#
#       A <NaturalDocs::Languages::Prototype> object.
#
sub ParsePrototype #(type, prototype)
    {
    my ($self, $type, $prototype) = @_;

    my $isClass = NaturalDocs::Topics->TypeInfo($type)->ClassHierarchy();

    if ($prototype !~ /\(.*[^ ].*\)/ && (!$isClass || $prototype !~ /\{.*[^ ].*\}/))
        {
        my $object = NaturalDocs::Languages::Prototype->New($prototype);
        return $object;
        };


    # Parse the parameters out of the prototype.

    my @tokens = $prototype =~ /([^\(\)\[\]\{\}\<\>\'\"\,\;]+|.)/g;

    my $parameter;
    my @parameterLines;

    my @symbolStack;
    my $finishedParameters;

    my ($beforeParameters, $afterParameters);

    foreach my $token (@tokens)
        {
        if ($finishedParameters)
            {  $afterParameters .= $token;  }

        elsif ($symbolStack[-1] eq '\'' || $symbolStack[-1] eq '"')
            {
            if ($symbolStack[0] eq '(' || ($isClass && $symbolStack[0] eq '{'))
                {  $parameter .= $token;  }
            else
                {  $beforeParameters .= $token;  };

            if ($token eq $symbolStack[-1])
                {  pop @symbolStack;  };
            }

        elsif ($token =~ /^[\(\[\{\<\'\"]$/)
            {
            if ($symbolStack[0] eq '(' || ($isClass && $symbolStack[0] eq '{'))
                {  $parameter .= $token;   }
            else
                {  $beforeParameters .= $token;  };

            push @symbolStack, $token;
            }

        elsif ( ($token eq ')' && $symbolStack[-1] eq '(') ||
                 ($token eq ']' && $symbolStack[-1] eq '[') ||
                 ($token eq '}' && $symbolStack[-1] eq '{') ||
                 ($token eq '>' && $symbolStack[-1] eq '<') )
            {
            if ($symbolStack[0] eq '(')
                {
                if ($token eq ')' && scalar @symbolStack == 1)
                    {
                    if ($parameter ne ' ')
                        {  push @parameterLines, $parameter;  };

                    $finishedParameters = 1;
                    $afterParameters .= $token;
                    }
                else
                    {  $parameter .= $token;  };
                }
            elsif ($isClass && $symbolStack[0] eq '{')
                {
                if ($token eq '}' && scalar @symbolStack == 1)
                    {
                    if ($parameter ne ' ')
                        {  push @parameterLines, $parameter;  };

                    $finishedParameters = 1;
                    $afterParameters .= $token;
                    }
                else
                    {  $parameter .= $token;  };
                }
            else
                {
                $beforeParameters .= $token;
                };

            pop @symbolStack;
            }

        elsif ($token eq ',' || $token eq ';')
            {
            if ($symbolStack[0] eq '(' || ($isClass && $symbolStack[0] eq '{'))
                {
                if (scalar @symbolStack == 1)
                    {
                    push @parameterLines, $parameter . $token;
                    $parameter = undef;
                    }
                else
                    {
                    $parameter .= $token;
                    };
                }
            else
                {
                $beforeParameters .= $token;
                };
            }

        else
            {
            if ($symbolStack[0] eq '(' || ($isClass && $symbolStack[0] eq '{'))
                {  $parameter .= $token;  }
            else
                {  $beforeParameters .= $token;  };
            };
        };

    foreach my $part (\$beforeParameters, \$afterParameters)
        {
        $$part =~ s/^ //;
        $$part =~ s/ $//;
        };

    my $prototypeObject = NaturalDocs::Languages::Prototype->New($beforeParameters, $afterParameters);


    # Parse the actual parameters.

    foreach my $parameterLine (@parameterLines)
        {
        $prototypeObject->AddParameter( $self->ParseParameterLine($parameterLine) );
        };

    return $prototypeObject;
    };


#
#   Function: ParseParameterLine
#
#   Parses a prototype parameter line and returns it as a <NaturalDocs::Languages::Prototype::Parameter> object.
#
#   This vesion assumes a C++ style line.  If you need a Pascal style line, override this function to forward to
#   <ParsePascalParameterLine()>.
#
#   > Function(parameter, type parameter, type parameter = value);
#
sub ParseParameterLine #(line)
    {
    my ($self, $line) = @_;

    $line =~ s/^ //;
    $line =~ s/ $//;

    my @tokens = $line =~ /([^ \(\)\{\}\[\]\<\>\'\"\=]+|.)/g;

    my @symbolStack;
    my @parameterWords = ( undef );
    my ($defaultValue, $defaultValuePrefix, $inDefaultValue);

    foreach my $token (@tokens)
        {
        if ($inDefaultValue)
            {  $defaultValue .= $token;  }

        elsif ($symbolStack[-1] eq '\'' || $symbolStack[-1] eq '"')
            {
            $parameterWords[-1] .= $token;

            if ($token eq $symbolStack[-1])
                {  pop @symbolStack;  };
            }

        elsif ($token =~ /^[\(\[\{\<\'\"]$/)
            {
            push @symbolStack, $token;
            $parameterWords[-1] .= $token;
            }

        elsif ( ($token eq ')' && $symbolStack[-1] eq '(') ||
                 ($token eq ']' && $symbolStack[-1] eq '[') ||
                 ($token eq '}' && $symbolStack[-1] eq '{') ||
                 ($token eq '>' && $symbolStack[-1] eq '<') )
            {
            pop @symbolStack;
            $parameterWords[-1] .= $token;
            }

        elsif ($token eq ' ')
            {
            if (!scalar @symbolStack)
                {  push @parameterWords, undef;  }
            else
                {  $parameterWords[-1] .= $token;  };
            }

        elsif ($token eq '=')
            {
            if (!scalar @symbolStack)
                {
                $defaultValuePrefix = $token;
                $inDefaultValue = 1;
                }
            else
                {  $parameterWords[-1] .= $token;  };
            }

        else
            {
            $parameterWords[-1] .= $token;
            };
        };

    my ($name, $namePrefix, $type, $typePrefix);

    if (!$parameterWords[-1])
        {  pop @parameterWords;  };

    $name = pop @parameterWords;

    if ($parameterWords[-1]=~ /([\*\&]+)$/)
        {
        $namePrefix = $1;
        $parameterWords[-1] = substr($parameterWords[-1], 0, 0 - length($namePrefix));
        $parameterWords[-1] =~ s/ $//;

        if (!$parameterWords[-1])
            {  pop @parameterWords;  };
        }
    elsif ($name =~ /^([\*\&]+)/)
        {
        $namePrefix = $1;
        $name = substr($name, length($namePrefix));
        $name =~ s/^ //;
        };

    $type = pop @parameterWords;
    $typePrefix = join(' ', @parameterWords);

    if ($typePrefix)
        {  $typePrefix .= ' ';  };

    if ($type =~ /^([a-z0-9_\:\.]+(?:\.|\:\:))[a-z0-9_]/i)
        {
        my $attachedTypePrefix = $1;

        $typePrefix .= $attachedTypePrefix;
        $type = substr($type, length($attachedTypePrefix));
        };

    $defaultValue =~ s/ $//;

    return NaturalDocs::Languages::Prototype::Parameter->New($type, $typePrefix, $name, $namePrefix,
                                                                                             $defaultValue, $defaultValuePrefix);
    };


#
#   Function: ParsePascalParameterLine
#
#   Parses a Pascal-like prototype parameter line and returns it as a <NaturalDocs::Languages::Prototype::Parameter> object.
#   Pascal lines are as follows:
#
#   > Function (name: type; name, name: type := value)
#
#   Also supports ActionScript lines
#
#   > Function (name: type, name, name: type = value)
#
sub ParsePascalParameterLine #(line)
    {
    my ($self, $line) = @_;

    $line =~ s/^ //;
    $line =~ s/ $//;

    my @tokens = $line =~ /([^\(\)\{\}\[\]\<\>\'\"\=\:]+|\:\=|.)/g;
    my ($type, $name, $defaultValue, $defaultValuePrefix, $afterName, $afterDefaultValue);
    my @symbolStack;

    foreach my $token (@tokens)
        {
        if ($afterDefaultValue)
            {  $defaultValue .= $token;  }

        elsif ($symbolStack[-1] eq '\'' || $symbolStack[-1] eq '"')
            {
            if ($afterName)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };

            if ($token eq $symbolStack[-1])
                {  pop @symbolStack;  };
            }

        elsif ($token =~ /^[\(\[\{\<\'\"]$/)
            {
            push @symbolStack, $token;

            if ($afterName)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };
            }

        elsif ( ($token eq ')' && $symbolStack[-1] eq '(') ||
                 ($token eq ']' && $symbolStack[-1] eq '[') ||
                 ($token eq '}' && $symbolStack[-1] eq '{') ||
                 ($token eq '>' && $symbolStack[-1] eq '<') )
            {
            pop @symbolStack;

            if ($afterName)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };
            }

        elsif ($afterName)
            {
            if (($token eq ':=' || $token eq '=') && !scalar @symbolStack)
                {
                $defaultValuePrefix = $token;
                $afterDefaultValue = 1;
                }
            else
                {  $type .= $token;  };
            }

        elsif ($token eq ':' && !scalar @symbolStack)
            {
            $name .= $token;
            $afterName = 1;
            }

        else
            {  $name .= $token;  };
        };

    foreach my $part (\$type, \$name, \$defaultValue)
        {
        $$part =~ s/^ //;
        $$part =~ s/ $//;
        };

    return NaturalDocs::Languages::Prototype::Parameter->New($type, undef, $name, undef, $defaultValue, $defaultValuePrefix);
    };


#
#   Function: TypeBeforeParameter
#
#   Returns whether the type appears before the parameter in prototypes.
#
#   For example, it does in C++
#   > void Function (int a, int b)
#
#   but does not in Pascal
#   > function Function (a: int; b, c: int)
#
sub TypeBeforeParameter
    {
    return 1;
    };



#
#   Function: IgnoredPrefixLength
#
#   Returns the length of the prefix that should be ignored in the index, or zero if none.
#
#   Parameters:
#
#       name - The name of the symbol.
#       type  - The symbol's <TopicType>.
#
#   Returns:
#
#       The length of the prefix to ignore, or zero if none.
#
sub IgnoredPrefixLength #(name, type)
    {
    my ($self, $name, $type) = @_;

    foreach my $prefixes ($self->IgnoredPrefixesFor($type), $self->IgnoredPrefixesFor(::TOPIC_GENERAL()))
        {
        if (defined $prefixes)
            {
            foreach my $prefix (@$prefixes)
                {
                if (substr($name, 0, length($prefix)) eq $prefix)
                    {  return length($prefix);  };
                };
            };
        };

    return 0;
    };



###############################################################################
# Group: Support Functions


#
#   Function: StripOpeningSymbols
#
#   Determines if the line starts with any of the passed symbols, and if so, replaces it with spaces.  This only happens
#   if the only thing before it on the line is whitespace.
#
#   Parameters:
#
#       lineRef - A reference to the line to check.
#       symbols - An arrayref of the symbols to check for.
#
#   Returns:
#
#       If the line starts with any of the passed comment symbols, it will replace it in the line with spaces and return the symbol.
#       If the line doesn't, it will leave the line alone and return undef.
#
sub StripOpeningSymbols #(lineRef, symbols)
    {
    my ($self, $lineRef, $symbols) = @_;

    if (!defined $symbols)
        {  return undef;  };

    my ($index, $symbol) = ::FindFirstSymbol($$lineRef, $symbols);

    if ($index != -1 && substr($$lineRef, 0, $index) =~ /^[ \t]*$/)
        {
        return substr($$lineRef, $index, length($symbol), ' ' x length($symbol));
        };

    return undef;
    };


#
#   Function: StripOpeningJavaDocSymbols
#
#   Determines if the line starts with any of the passed symbols, and if so, replaces it with spaces.  This only happens
#   if the only thing before it on the line is whitespace and the next character after it is whitespace or the end of the line.
#
#   Parameters:
#
#       lineRef - A reference to the line to check.
#       symbols - An arrayref of the symbols to check for.
#
#   Returns:
#
#       If the line starts with any of the passed comment symbols, it will replace it in the line with spaces and return the symbol.
#       If the line doesn't, it will leave the line alone and return undef.
#
sub StripOpeningJavaDocSymbols #(lineRef, symbols)
    {
    my ($self, $lineRef, $symbols) = @_;

    if (!defined $symbols)
        {  return undef;  };

    my ($index, $symbol) = ::FindFirstSymbol($$lineRef, $symbols);

    if ($index != -1 && substr($$lineRef, 0, $index) =~ /^[ \t]*$/ && substr($$lineRef, $index + length($symbol), 1) =~ /^[ \t]?$/)
        {
        return substr($$lineRef, $index, length($symbol), ' ' x length($symbol));
        };

    return undef;
    };


#
#   Function: StripOpeningBlockSymbols
#
#   Determines if the line starts with any of the opening symbols in the passed symbol pairs, and if so, replaces it with spaces.
#   This only happens if the only thing before it on the line is whitespace.
#
#   Parameters:
#
#       lineRef - A reference to the line to check.
#       symbolPairs - An arrayref of the symbol pairs to check for.  Pairs are specified as two consecutive array entries, with the
#                            opening symbol first.
#
#   Returns:
#
#       If the line starts with any of the opening symbols, it will replace it in the line with spaces and return the closing symbol.
#       If the line doesn't, it will leave the line alone and return undef.
#
sub StripOpeningBlockSymbols #(lineRef, symbolPairs)
    {
    my ($self, $lineRef, $symbolPairs) = @_;

    if (!defined $symbolPairs)
        {  return undef;  };

    for (my $i = 0; $i < scalar @$symbolPairs; $i += 2)
        {
        my $index = index($$lineRef, $symbolPairs->[$i]);

        if ($index != -1 && substr($$lineRef, 0, $index) =~ /^[ \t]*$/)
            {
            substr($$lineRef, $index, length($symbolPairs->[$i]), ' ' x length($symbolPairs->[$i]));
            return $symbolPairs->[$i + 1];
            };
        };

    return undef;
    };


#
#   Function: StripOpeningJavaDocBlockSymbols
#
#   Determines if the line starts with any of the opening symbols in the passed symbol pairs, and if so, replaces it with spaces.
#   This only happens if the only thing before it on the line is whitespace and the next character is whitespace or the end of the line.
#
#   Parameters:
#
#       lineRef - A reference to the line to check.
#       symbolPairs - An arrayref of the symbol pairs to check for.  Pairs are specified as two consecutive array entries, with the
#                            opening symbol first.
#
#   Returns:
#
#       If the line starts with any of the opening symbols, it will replace it in the line with spaces and return the closing symbol.
#       If the line doesn't, it will leave the line alone and return undef.
#
sub StripOpeningJavaDocBlockSymbols #(lineRef, symbolPairs)
    {
    my ($self, $lineRef, $symbolPairs) = @_;

    if (!defined $symbolPairs)
        {  return undef;  };

    for (my $i = 0; $i < scalar @$symbolPairs; $i += 2)
        {
        my $index = index($$lineRef, $symbolPairs->[$i]);

        if ($index != -1 && substr($$lineRef, 0, $index) =~ /^[ \t]*$/ &&
            substr($$lineRef, $index + length($symbolPairs->[$i]), 1) =~ /^[ \t]?$/)
            {
            substr($$lineRef, $index, length($symbolPairs->[$i]), ' ' x length($symbolPairs->[$i]));
            return $symbolPairs->[$i + 1];
            };
        };

    return undef;
    };


#
#   Function: StripClosingSymbol
#
#   Determines if the line contains a symbol, and if so, truncates it just before the symbol.
#
#   Parameters:
#
#       lineRef - A reference to the line to check.
#       symbol - The symbol to check for.
#
#   Returns:
#
#       The remainder of the line, or undef if the symbol was not found.
#
sub StripClosingSymbol #(lineRef, symbol)
    {
    my ($self, $lineRef, $symbol) = @_;

    my $index = index($$lineRef, $symbol);

    if ($index != -1)
        {
        my $lineRemainder = substr($$lineRef, $index + length($symbol));
        $$lineRef = substr($$lineRef, 0, $index);

        return $lineRemainder;
        }
    else
        {  return undef;  };
    };


#
#   Function: NormalizePrototype
#
#   Normalizes a prototype.  Specifically, condenses spaces, tabs, and line breaks into single spaces and removes leading and
#   trailing ones.
#
#   Parameters:
#
#       prototype - The original prototype string.
#
#   Returns:
#
#       The normalized prototype.
#
sub NormalizePrototype #(prototype)
    {
    my ($self, $prototype) = @_;

    $prototype =~ tr/ \t\r\n/ /s;
    $prototype =~ s/^ //;
    $prototype =~ s/ $//;

    return $prototype;
    };


1;
