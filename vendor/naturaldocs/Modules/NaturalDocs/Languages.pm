###############################################################################
#
#   Package: NaturalDocs::Languages
#
###############################################################################
#
#   A package to manage all the programming languages Natural Docs supports.
#
#   Usage and Dependencies:
#
#       - Prior to use, <NaturalDocs::Settings> must be initialized and <Load()> must be called.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use Text::Wrap();

use NaturalDocs::Languages::Prototype;

use NaturalDocs::Languages::Base;
use NaturalDocs::Languages::Simple;
use NaturalDocs::Languages::Advanced;

use NaturalDocs::Languages::Perl;
use NaturalDocs::Languages::CSharp;
use NaturalDocs::Languages::ActionScript;

use NaturalDocs::Languages::Ada;
use NaturalDocs::Languages::PLSQL;
use NaturalDocs::Languages::Pascal;
use NaturalDocs::Languages::Tcl;

use strict;
use integer;

package NaturalDocs::Languages;


###############################################################################
# Group: Variables


#
#   handle: FH_LANGUAGES
#
#   The file handle used for writing to <Languages.txt>.
#


#
#   hash: languages
#
#   A hash of all the defined languages.  The keys are the all-lowercase language names, and the values are
#   <NaturalDocs::Languages::Base>-derived objects.
#
my %languages;

#
#   hash: extensions
#
#   A hash of all the defined languages' extensions.  The keys are the all-lowercase extensions, and the values are the
#   all-lowercase names of the languages that defined them.
#
my %extensions;

#
#   hash: shebangStrings
#
#   A hash of all the defined languages' strings to search for in the shebang (#!) line.  The keys are the all-lowercase strings, and
#   the values are the all-lowercase names of the languages that defined them.
#
my %shebangStrings;

#
#   hash: shebangFiles
#
#   A hash of all the defined languages for files where it needs to be found via shebang strings.  The keys are the file names,
#   and the values are language names, or undef if the file isn't supported.  These values should be filled in the first time
#   each file is parsed for a shebang string so that it doesn't have to be done multiple times.
#
my %shebangFiles;

#
#   array: mainLanguageNames
#
#   An array of the language names that are defined in the main <Languages.txt>.
#
my @mainLanguageNames;



###############################################################################
# Group: Files


#
#   File: Languages.txt
#
#   The configuration file that defines or overrides the language definitions for Natural Docs.  One version sits in Natural Docs'
#   configuration directory, and another can be in a project directory to add to or override them.
#
#   > # [comments]
#
#   Everything after a # symbol is ignored.  However, for this particular file, comments can only appear on their own lines.
#   They cannot appear after content on the same line.
#
#   > Format: [version]
#
#   Specifies the file format version of the file.
#
#
#   Sections:
#
#       > Ignore[d] Extension[s]: [extension] [extension] ...
#
#       Causes the listed file extensions to be ignored, even if they were previously defined to be part of a language.  The list is
#       space-separated.  ex. "Ignore Extensions: cvs txt"
#
#
#       > Language: [name]
#
#       Creates a new language.  Everything underneath applies to this language until the next one.  Names can use any
#       characters.
#
#       The languages "Text File" and "Shebang Script" have special meanings.  Text files are considered all comment and don't
#       have comment symbols.  Shebang scripts have their language determined by the shebang string and automatically
#       include files with no extension in addition to the extensions defined.
#
#       If "Text File" doesn't define ignored prefixes, a package separator, or enum value behavior, those settings will be copied
#       from the language with the most files in the source tree.
#
#
#       > Alter Language: [name]
#
#       Alters an existing language.  Everything underneath it overrides the previous settings until the next one.  Note that if a
#       property has an [Add/Replace] form and that property has already been defined, you have to specify whether you're adding
#       to or replacing the defined list.
#
#
#   Language Properties:
#
#       > Extension[s]: [extension] [extension] ...
#       > [Add/Replace] Extension[s]: ...
#
#       Defines file extensions for the language's source files.  The list is space-separated.  ex. "Extensions: c cpp".  You can use
#       extensions that were previously used by another language to redefine them.
#
#
#       > Shebang String[s]: [string] [string] ...
#       > [Add/Replace] Shebang String[s]: ...
#
#       Defines a list of strings that can appear in the shebang (#!) line to designate that it's part of this language.  They can
#       appear anywhere in the line, so "php" will work for "#!/user/bin/php4".  You can use strings that were previously used by
#       another language to redefine them.
#
#
#       > Ignore[d] Prefix[es] in Index: [prefix] [prefix] ...
#       > Ignore[d] [Topic Type] Prefix[es] in Index: [prefix] [prefix] ...
#       > [Add/Replace] Ignore[d] Prefix[es] in Index: ...
#       > [Add/Replace] Ignore[d] [Topic Type] Prefix[es] in Index: ...
#
#       Specifies prefixes that should be ignored when sorting symbols for an index.  Can be specified in general or for a specific
#       <TopicType>.  The prefixes will still appear, the symbols will just be sorted as if they're not there.  For example, specifying
#       "ADO_" for functions will mean that "ADO_DoSomething" will appear under D instead of A.
#
#
#   Basic Language Support Properties:
#
#       These attributes are only available for languages with basic language support.
#
#
#       > Line Comment[s]: [symbol] [symbol] ...
#
#       Defines a space-separated list of symbols that are used for line comments, if any.  ex. "Line Comment: //".
#
#
#       > Block Comment[s]: [opening symbol] [closing symbol] [opening symbol] [closing symbol] ...
#
#       Defines a space-separated list of symbol pairs that are used for block comments, if any.  ex. "Block Comment: /* */".
#
#
#       > Package Separator: [symbol]
#
#       Defines the default package separator symbol, such as . or ::.  This is for presentation only and will not affect how
#       Natural Docs links are parsed.  The default is a dot.
#
#
#       > [Topic Type] Prototype Ender[s]: [symbol] [symbol] ...
#
#       When defined, Natural Docs will attempt to collect prototypes from the code following the specified <TopicType>.  It grabs
#       code until the first ender symbol or the next Natural Docs comment, and if it contains the topic name, it serves as its
#       prototype.  Use \n to specify a line break.  ex. "Function Prototype Enders: { ;", "Variable Prototype Enders: = ;".
#
#
#       > Line Extender: [symbol]
#
#       Defines the symbol that allows a prototype to span multiple lines if normally a line break would end it.
#
#
#       > Enum Values: [global|under type|under parent]
#
#       Defines how enum values are referenced.  The default is global.
#
#       global - Values are always global, referenced as 'value'.
#       under type - Values are under the enum type, referenced as 'package.enum.value'.
#       under parent - Values are under the enum's parent, referenced as 'package.value'.
#
#
#       > Perl Package: [perl package]
#
#       Specifies the Perl package used to fine-tune the language behavior in ways too complex to do in this file.
#
#
#   Full Language Support Properties:
#
#       These attributes are only available for languages with full language support.
#
#
#       > Full Language Support: [perl package]
#
#       Specifies the Perl package that has the parsing routines necessary for full language support.
#
#
#   Revisions:
#
#       1.32:
#
#           - Package Separator is now a basic language support only property.
#           - Added Enum Values setting.
#
#       1.3:
#
#           - The file was introduced.


###############################################################################
# Group: File Functions


#
#   Function: Load
#
#   Loads both the master and the project version of <Languages.txt>.
#
sub Load
    {
    my $self = shift;

    # Hashrefs where the keys are all-lowercase extensions/shebang strings, and the values are arrayrefs of the languages
    # that defined them, earliest first, all lowercase.
    my %tempExtensions;
    my %tempShebangStrings;

    $self->LoadFile(1, \%tempExtensions, \%tempShebangStrings);  # Main

    if (!exists $languages{'shebang script'})
        {  NaturalDocs::ConfigFile->AddError('You must define "Shebang Script" in the main languages file.');  };
    if (!exists $languages{'text file'})
        {  NaturalDocs::ConfigFile->AddError('You must define "Text File" in the main languages file.');  };

    my $errorCount = NaturalDocs::ConfigFile->ErrorCount();

    if ($errorCount)
        {
        NaturalDocs::ConfigFile->PrintErrorsAndAnnotateFile();
        NaturalDocs::Error->SoftDeath('There ' . ($errorCount == 1 ? 'is an error' : 'are ' . $errorCount . ' errors')
                                                    . ' in ' . NaturalDocs::Project->MainConfigFile('Languages.txt'));
        }


    $self->LoadFile(0, \%tempExtensions, \%tempShebangStrings);  # User

    $errorCount = NaturalDocs::ConfigFile->ErrorCount();

    if ($errorCount)
        {
        NaturalDocs::ConfigFile->PrintErrorsAndAnnotateFile();
        NaturalDocs::Error->SoftDeath('There ' . ($errorCount == 1 ? 'is an error' : 'are ' . $errorCount . ' errors')
                                                    . ' in ' . NaturalDocs::Project->UserConfigFile('Languages.txt'));
        };


    # Convert the temp hashes into the real ones.

    while (my ($extension, $languages) = each %tempExtensions)
        {
        $extensions{$extension} = $languages->[-1];
        };
    while (my ($shebangString, $languages) = each %tempShebangStrings)
        {
        $shebangStrings{$shebangString} = $languages->[-1];
        };
    };


#
#   Function: LoadFile
#
#   Loads a particular version of <Languages.txt>.
#
#   Parameters:
#
#       isMain - Whether the file is the main file or not.
#       tempExtensions - A hashref where the keys are all-lowercase extensions, and the values are arrayrefs of the all-lowercase
#                                 names of the languages that defined them, earliest first.  It will be changed by this function.
#       tempShebangStrings - A hashref where the keys are all-lowercase shebang strings, and the values are arrayrefs of the
#                                        all-lowercase names of the languages that defined them, earliest first.  It will be changed by this
#                                        function.
#
sub LoadFile #(isMain, tempExtensions, tempShebangStrings)
    {
    my ($self, $isMain, $tempExtensions, $tempShebangStrings) = @_;

    my ($file, $status);

    if ($isMain)
        {
        $file = NaturalDocs::Project->MainConfigFile('Languages.txt');
        $status = NaturalDocs::Project->MainConfigFileStatus('Languages.txt');
        }
    else
        {
        $file = NaturalDocs::Project->UserConfigFile('Languages.txt');
        $status = NaturalDocs::Project->UserConfigFileStatus('Languages.txt');
        };


    my $version;

    # An array of properties for the current language.  Each entry is the three consecutive values ( lineNumber, keyword, value ).
    my @properties;

    if ($version = NaturalDocs::ConfigFile->Open($file))
        {
        # The format hasn't changed significantly since the file was introduced.

        if ($status == ::FILE_CHANGED())
            {
            NaturalDocs::Project->ReparseEverything();
            NaturalDocs::SymbolTable->RebuildAllIndexes();  # Because the ignored prefixes could change.
            };

        my ($keyword, $value, $comment);

        while (($keyword, $value, $comment) = NaturalDocs::ConfigFile->GetLine())
            {
            $value .= $comment;
            $value =~ s/^ //;

            # Process previous properties.
            if (($keyword eq 'language' || $keyword eq 'alter language') && scalar @properties)
                {
                if ($isMain && $properties[1] eq 'language')
                    {  push @mainLanguageNames, $properties[2];  };

                $self->ProcessProperties(\@properties, $version, $tempExtensions, $tempShebangStrings);
                @properties = ( );
                };

            if ($keyword =~ /^ignored? extensions?$/)
                {
                $value =~ tr/.*//d;
                my @extensions = split(/ /, lc($value));

                foreach my $extension (@extensions)
                    {  delete $tempExtensions->{$extension};  };
                }
            else
                {
                push @properties, NaturalDocs::ConfigFile->LineNumber(), $keyword, $value;
                };
            };

        if (scalar @properties)
            {
            if ($isMain && $properties[1] eq 'language')
                {  push @mainLanguageNames, $properties[2];  };

            $self->ProcessProperties(\@properties, $version, $tempExtensions, $tempShebangStrings);
            };
        }

    else # couldn't open file
        {
        if ($isMain)
            {  die "Couldn't open languages file " . $file . "\n";  };
        };
    };


#
#   Function: ProcessProperties
#
#   Processes an array of language properties from <Languages.txt>.
#
#   Parameters:
#
#       properties - An arrayref of properties where each entry is the three consecutive values ( lineNumber, keyword, value ).
#                         It must start with the Language or Alter Language property.
#       version - The <VersionInt> of the file.
#       tempExtensions - A hashref where the keys are all-lowercase extensions, and the values are arrayrefs of the all-lowercase
#                                 names of the languages that defined them, earliest first.  It will be changed by this function.
#       tempShebangStrings - A hashref where the keys are all-lowercase shebang strings, and the values are arrayrefs of the
#                                        all-lowercase names of the languages that defined them, earliest first.  It will be changed by this
#                                        function.
#
sub ProcessProperties #(properties, version, tempExtensions, tempShebangStrings)
    {
    my ($self, $properties, $version, $tempExtensions, $tempShebangStrings) = @_;


    # First validate the name and check whether the language has full support.

    my $language;
    my $fullLanguageSupport;
    my ($lineNumber, $languageKeyword, $languageName) = @$properties[0..2];
    my $lcLanguageName = lc($languageName);
    my ($keyword, $value);

    if ($languageKeyword eq 'alter language')
        {
        $language = $languages{$lcLanguageName};

        if (!defined $language)
            {
            NaturalDocs::ConfigFile->AddError('The language ' . $languageName . ' is not defined.', $lineNumber);
            return;
            }
        else
            {
            $fullLanguageSupport = (!$language->isa('NaturalDocs::Languages::Simple'));
            };
        }

    elsif ($languageKeyword eq 'language')
        {
        if (exists $languages{$lcLanguageName})
            {
            NaturalDocs::ConfigFile->AddError('The language ' . $value . ' is already defined.  Use "Alter Language" if you want '
                                                             . 'to override its settings.', $lineNumber);
            return;
            };

        # Case is important with these two.
        if ($lcLanguageName eq 'shebang script')
            {  $languageName = 'Shebang Script';  }
        elsif ($lcLanguageName eq 'text file')
            {  $languageName = 'Text File';  };


        # Go through the properties looking for whether the language has basic or full support and which package to use to create
        # it.

        for (my $i = 3; $i < scalar @$properties; $i += 3)
            {
            ($lineNumber, $keyword, $value) = @$properties[$i..$i+2];

            if ($keyword eq 'full language support')
                {
                $fullLanguageSupport = 1;

                eval
                    {
                    $language = $value->New($languageName);
                    };
                if ($::EVAL_ERROR)
                    {
                    NaturalDocs::ConfigFile->AddError('Could not create ' . $value . ' object.', $lineNumber);
                    return;
                    };

                last;
                }

            elsif ($keyword eq 'perl package')
                {
                eval
                    {
                    $language = $value->New($languageName);
                    };
                if ($::EVAL_ERROR)
                    {
                    NaturalDocs::ConfigFile->AddError('Could not create ' . $value . ' object.', $lineNumber);
                    return;
                    };
                };
            };

        # If $language was not created by now, it's a generic basic support language.
        if (!defined $language)
            {  $language = NaturalDocs::Languages::Simple->New($languageName);  };

        $languages{$lcLanguageName} = $language;
        }

    else # not language or alter language
        {
        NaturalDocs::ConfigFile->AddError('You must start this line with "Language", "Alter Language", or "Ignore Extensions".',
                                                           $lineNumber);
        return;
        };


    # Decode the properties.

    for (my $i = 3; $i < scalar @$properties; $i += 3)
        {
        ($lineNumber, $keyword, $value) = @$properties[$i..$i+2];

        if ($keyword =~ /^(?:(add|replace) )?extensions?$/)
            {
            my $command = $1;


            # Remove old extensions.

            if (defined $language->Extensions() && $command eq 'replace')
                {
                foreach my $extension (@{$language->Extensions()})
                    {
                    if (exists $tempExtensions->{$extension})
                        {
                        my $languages = $tempExtensions->{$extension};
                        my $i = 0;

                        while ($i < scalar @$languages)
                            {
                            if ($languages->[$i] eq $lcLanguageName)
                                {  splice(@$languages, $i, 1);  }
                            else
                                {  $i++;  };
                            };

                        if (!scalar @$languages)
                            {  delete $tempExtensions->{$extension};  };
                        };
                    };
                };


            # Add new extensions.

            # Ignore stars and dots so people could use .ext or *.ext.
            $value =~ s/\*\.|\.//g;

            my @extensions = split(/ /, lc($value));

            foreach my $extension (@extensions)
                {
                if (!exists $tempExtensions->{$extension})
                    {  $tempExtensions->{$extension} = [ ];  };

                push @{$tempExtensions->{$extension}}, $lcLanguageName;
                };


            # Set the extensions for the language object.

            if (defined $language->Extensions())
                {
                if ($command eq 'add')
                    {  push @extensions, @{$language->Extensions()};  }
                elsif (!$command)
                    {
                    NaturalDocs::ConfigFile->AddError('You need to specify whether you are adding to or replacing the list of extensions.',
                                                                       $lineNumber);
                    };
                };

            $language->SetExtensions(\@extensions);
            }

        elsif ($keyword =~ /^(?:(add|replace) )?shebang strings?$/)
            {
            my $command = $1;


            # Remove old strings.

            if (defined $language->ShebangStrings() && $command eq 'replace')
                {
                foreach my $shebangString (@{$language->ShebangStrings()})
                    {
                    if (exists $tempShebangStrings->{$shebangString})
                        {
                        my $languages = $tempShebangStrings->{$shebangString};
                        my $i = 0;

                        while ($i < scalar @$languages)
                            {
                            if ($languages->[$i] eq $lcLanguageName)
                                {  splice(@$languages, $i, 1);  }
                            else
                                {  $i++;  };
                            };

                        if (!scalar @$languages)
                            {  delete $tempShebangStrings->{$shebangString};  };
                        };
                    };
                };


            # Add new strings.

            my @shebangStrings = split(/ /, lc($value));

            foreach my $shebangString (@shebangStrings)
                {
                if (!exists $tempShebangStrings->{$shebangString})
                    {  $tempShebangStrings->{$shebangString} = [ ];  };

                push @{$tempShebangStrings->{$shebangString}}, $lcLanguageName;
                };


            # Set the strings for the language object.

            if (defined $language->ShebangStrings())
                {
                if ($command eq 'add')
                    {  push @shebangStrings, @{$language->ShebangStrings()};  }
                elsif (!$command)
                    {
                    NaturalDocs::ConfigFile->AddError('You need to specify whether you are adding to or replacing the list of shebang '
                                                                     . 'strings.', $lineNumber);
                    };
                };

            $language->SetShebangStrings(\@shebangStrings);
            }

        elsif ($keyword eq 'package separator')
            {
            if ($fullLanguageSupport)
                {
                # Prior to 1.32, package separator was used with full language support too.  Accept it without complaining, even though
                # we ignore it.
                if ($version >= NaturalDocs::Version->FromString('1.32'))
                    {
                    NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                    };
                }
            else
                {  $language->SetPackageSeparator($value);  };
            }

        elsif ($keyword =~ /^(?:(add|replace) )?ignored? (?:(.+) )?prefix(?:es)? in index$/)
            {
            my ($command, $topicName) = ($1, $2);
            my $topicType;

            if ($topicName)
                {
                if (!( ($topicType, undef) = NaturalDocs::Topics->NameInfo($topicName) ))
                    {
                    NaturalDocs::ConfigFile->AddError($topicName . ' is not a defined topic type.', $lineNumber);
                    };
                }
            else
                {  $topicType = ::TOPIC_GENERAL();  };

            if ($topicType)
                {
                my @prefixes;

                if (defined $language->IgnoredPrefixesFor($topicType))
                    {
                    if ($command eq 'add')
                        {  @prefixes = @{$language->IgnoredPrefixesFor($topicType)};  }
                    elsif (!$command)
                        {
                        NaturalDocs::ConfigFile->AddError('You need to specify whether you are adding to or replacing the list of '
                                                                         . 'ignored prefixes.', $lineNumber);
                        };
                    };

                push @prefixes, split(/ /, $value);
                $language->SetIgnoredPrefixesFor($topicType, \@prefixes);
                };
            }

        elsif ($keyword eq 'full language support' || $keyword eq 'perl package')
            {
            if ($languageKeyword eq 'alter language')
                {
                NaturalDocs::ConfigFile->AddError('You cannot use ' . $keyword . ' with Alter Language.', $lineNumber);
                };
            # else ignore it.
            }

        elsif ($keyword =~ /^line comments?$/)
            {
            if ($fullLanguageSupport)
                {
                NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                }
            else
                {
                my @symbols = split(/ /, $value);
                $language->SetLineCommentSymbols(\@symbols);
                };
            }

        elsif ($keyword =~ /^block comments?$/)
            {
            if ($fullLanguageSupport)
                {
                NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                }
            else
                {
                my @symbols = split(/ /, $value);

                if ((scalar @symbols) % 2 == 0)
                    {  $language->SetBlockCommentSymbols(\@symbols);  }
                else
                    {  NaturalDocs::ConfigFile->AddError('Block comment symbols must appear in pairs.', $lineNumber);  };
                };
            }

        elsif ($keyword =~ /^(?:(.+) )?prototype enders?$/)
            {
            if ($fullLanguageSupport)
                {
                NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                }
            else
                {
                my $topicName = $1;
                my $topicType;

                if ($topicName)
                    {
                    if (!( ($topicType, undef) = NaturalDocs::Topics->NameInfo($topicName) ))
                        {
                        NaturalDocs::ConfigFile->AddError($topicName . ' is not a defined topic type.', $lineNumber);
                        };
                    }
                else
                    {  $topicType = ::TOPIC_GENERAL();  };

                if ($topicType)
                    {
                    $value =~ s/\\n/\n/g;
                    my @symbols = split(/ /, $value);
                    $language->SetPrototypeEndersFor($topicType, \@symbols);
                    };
                };
            }

        elsif ($keyword eq 'line extender')
            {
            if ($fullLanguageSupport)
                {
                NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                }
            else
                {
                $language->SetLineExtender($value);
                };
            }

        elsif ($keyword eq 'enum values')
            {
            if ($fullLanguageSupport)
                {
                NaturalDocs::ConfigFile->AddError('You cannot define this property when using full language support.', $lineNumber);
                }
            else
                {
                $value = lc($value);
                my $constant;

                if ($value eq 'global')
                    {  $constant = ::ENUM_GLOBAL();  }
                elsif ($value eq 'under type')
                    {  $constant = ::ENUM_UNDER_TYPE();  }
                elsif ($value eq 'under parent')
                    {  $constant = ::ENUM_UNDER_PARENT();  };

                if (defined $constant)
                    {  $language->SetEnumValues($constant);  }
                else
                    {
                    NaturalDocs::ConfigFile->AddError('Enum Values must be "Global", "Under Type", or "Under Parent".', $lineNumber);
                    };
                };
            }

        else
            {
            NaturalDocs::ConfigFile->AddError($keyword . ' is not a valid keyword.', $lineNumber);
            };
        };
    };


#
#   Function: Save
#
#   Saves the main and user versions of <Languages.txt>.
#
sub Save
    {
    my $self = shift;

    $self->SaveFile(1); # Main
    $self->SaveFile(0); # User
    };


#
#   Function: SaveFile
#
#   Saves a particular version of <Topics.txt>.
#
#   Parameters:
#
#       isMain - Whether the file is the main file or not.
#
sub SaveFile #(isMain)
    {
    my ($self, $isMain) = @_;

    my $file;

    if ($isMain)
        {
        if (NaturalDocs::Project->MainConfigFileStatus('Languages.txt') == ::FILE_SAME())
            {  return;  };
        $file = NaturalDocs::Project->MainConfigFile('Languages.txt');
        }
    else
        {
        # Have to check the main too because this file lists the languages defined there.
        if (NaturalDocs::Project->UserConfigFileStatus('Languages.txt') == ::FILE_SAME() &&
            NaturalDocs::Project->MainConfigFileStatus('Languages.txt') == ::FILE_SAME())
            {  return;  };
        $file = NaturalDocs::Project->UserConfigFile('Languages.txt');
        };


    # Array of segments, with each being groups of three consecutive entries.  The first is the keyword ('language' or
    # 'alter language'), the second is the value, and the third is a hashref of all the properties.
    # - For properties that can accept a topic type, the property values are hashrefs mapping topic types to the values.
    # - For properties that can accept 'add' or 'replace', there is an additional property ending in 'command' that stores it.
    # - For properties that can accept both, the 'command' thing is applied to the topic types rather than the properties.
    my @segments;

    my @ignoredExtensions;

    my $currentProperties;
    my $version;

    if ($version = NaturalDocs::ConfigFile->Open($file))
        {
        # We can assume the file is valid.

        while (my ($keyword, $value, $comment) = NaturalDocs::ConfigFile->GetLine())
            {
            $value .= $comment;
            $value =~ s/^ //;

            if ($keyword eq 'language')
                {
                $currentProperties = { };

                # Case is important with these two.
                if (lc($value) eq 'shebang script')
                    {  $value = 'Shebang Script';  }
                elsif (lc($value) eq 'text file')
                    {  $value = 'Text File';  };

                push @segments, 'language', $value, $currentProperties;
                }

            elsif ($keyword eq 'alter language')
                {
                $currentProperties = { };
                push @segments, 'alter language', $languages{lc($value)}->Name(), $currentProperties;
                }

            elsif ($keyword =~ /^ignored? extensions?$/)
                {
                $value =~ tr/*.//d;
                push @ignoredExtensions, split(/ /, $value);
                }

            elsif ($keyword eq 'package separator' || $keyword eq 'full language support' || $keyword eq 'perl package' ||
                    $keyword eq 'line extender' || $keyword eq 'enum values')
                {
                $currentProperties->{$keyword} = $value;
                }

            elsif ($keyword =~ /^line comments?$/)
                {
                $currentProperties->{'line comments'} = $value;
                }
            elsif ($keyword =~ /^block comments?$/)
                {
                $currentProperties->{'block comments'} = $value;
                }

            elsif ($keyword =~ /^(?:(add|replace) )?extensions?$/)
                {
                my $command = $1;

                if ($command eq 'add' && exists $currentProperties->{'extensions'})
                    {  $currentProperties->{'extensions'} .= ' ' . $value;  }
                else
                    {
                    $currentProperties->{'extensions'} = $value;
                    $currentProperties->{'extensions command'} = $command;
                    };
                }

            elsif ($keyword =~ /^(?:(add|replace) )?shebang strings?$/)
                {
                my $command = $1;

                if ($command eq 'add' && exists $currentProperties->{'shebang strings'})
                    {  $currentProperties->{'shebang strings'} .= ' ' . $value;  }
                else
                    {
                    $currentProperties->{'shebang strings'} = $value;
                    $currentProperties->{'shebang strings command'} = $command;
                    };
                }

            elsif ($keyword =~ /^(?:(.+) )?prototype enders?$/)
                {
                my $topicName = $1;
                my $topicType;

                if ($topicName)
                    {  ($topicType, undef) = NaturalDocs::Topics->NameInfo($topicName);  }
                else
                    {  $topicType = ::TOPIC_GENERAL();  };

                my $currentTypeProperties = $currentProperties->{'prototype enders'};

                if (!defined $currentTypeProperties)
                    {
                    $currentTypeProperties = { };
                    $currentProperties->{'prototype enders'} = $currentTypeProperties;
                    };

                $currentTypeProperties->{$topicType} = $value;
                }

            elsif ($keyword =~ /^(?:(add|replace) )?ignored? (?:(.+) )?prefix(?:es)? in index$/)
                {
                my ($command, $topicName) = ($1, $2);
                my $topicType;

                if ($topicName)
                    {  ($topicType, undef) = NaturalDocs::Topics->NameInfo($topicName);  }
                else
                    {  $topicType = ::TOPIC_GENERAL();  };

                my $currentTypeProperties = $currentProperties->{'ignored prefixes in index'};

                if (!defined $currentTypeProperties)
                    {
                    $currentTypeProperties = { };
                    $currentProperties->{'ignored prefixes in index'} = $currentTypeProperties;
                    };

                if ($command eq 'add' && exists $currentTypeProperties->{$topicType})
                    {  $currentTypeProperties->{$topicType} .= ' ' . $value;  }
                else
                    {
                    $currentTypeProperties->{$topicType} = $value;
                    $currentTypeProperties->{$topicType . ' command'} = $command;
                    };
                };
            };

        NaturalDocs::ConfigFile->Close();
        };


    if (!open(FH_LANGUAGES, '>' . $file))
        {
        # The main file may be on a shared volume or some other place the user doesn't have write access to.  Since this is only to
        # reformat the file, we can ignore the failure.
        if ($isMain)
            {  return;  }
        else
            {  die "Couldn't save " . $file;  };
        };

    print FH_LANGUAGES 'Format: ' . NaturalDocs::Settings->TextAppVersion() . "\n\n";

    # Remember the 80 character limit.

    if ($isMain)
        {
        print FH_LANGUAGES
        "# This is the main Natural Docs languages file.  If you change anything here,\n"
        . "# it will apply to EVERY PROJECT you use Natural Docs on.  If you'd like to\n"
        . "# change something for just one project, edit the Languages.txt in its project\n"
        . "# directory instead.\n";
        }
    else
        {
        print FH_LANGUAGES
        "# This is the Natural Docs languages file for this project.  If you change\n"
        . "# anything here, it will apply to THIS PROJECT ONLY.  If you'd like to change\n"
        . "# something for all your projects, edit the Languages.txt in Natural Docs'\n"
        . "# Config directory instead.\n\n\n";

        if (scalar @ignoredExtensions == 1)
            {
            print FH_LANGUAGES
            'Ignore Extension: ' . $ignoredExtensions[0] . "\n";
            }
        elsif (scalar @ignoredExtensions)
            {
            print FH_LANGUAGES
            'Ignore Extensions: ' . join(' ', @ignoredExtensions) . "\n";
            }
        else
            {
            print FH_LANGUAGES
            "# You can prevent certain file extensions from being scanned like this:\n"
            . "# Ignore Extensions: [extension] [extension] ...\n"
            };
        };

    print FH_LANGUAGES
    "\n\n"
    . "#-------------------------------------------------------------------------------\n"
    . "# SYNTAX:\n"
    . "#\n"
    . "# Unlike other Natural Docs configuration files, in this file all comments\n"
    . "# MUST be alone on a line.  Some languages deal with the # character, so you\n"
    . "# cannot put comments on the same line as content.\n"
    . "#\n"
    . "# Also, all lists are separated with spaces, not commas, again because some\n"
    . "# languages may need to use them.\n"
    . "#\n";

    if ($isMain)
        {
        print FH_LANGUAGES
        "# Language: [name]\n"
        . "#    Defines a new language.  Its name can use any characters.\n"
        . "#\n";
        }
    else
        {
        print FH_LANGUAGES
        "# Language: [name]\n"
        . "# Alter Language: [name]\n"
        . "#    Defines a new language or alters an existing one.  Its name can use any\n"
        . "#    characters.  If any of the properties below have an add/replace form, you\n"
        . "#    must use that when using Alter Language.\n"
        . "#\n";
        };

    print FH_LANGUAGES
    "#    The language Shebang Script is special.  It's entry is only used for\n"
    . "#    extensions, and files with those extensions have their shebang (#!) lines\n"
    . "#    read to determine the real language of the file.  Extensionless files are\n"
    . "#    always treated this way.\n"
    . "#\n"
    . "#    The language Text File is also special.  It's treated as one big comment\n"
    . "#    so you can put Natural Docs content in them without special symbols.  Also,\n"
    . "#    if you don't specify a package separator, ignored prefixes, or enum value\n"
    . "#    behavior, it will copy those settings from the language that is used most\n"
    . "#    in the source tree.\n"
    . "#\n"
    . "# Extensions: [extension] [extension] ...\n";

    if ($isMain)
        {
        print FH_LANGUAGES
        "#    Defines the file extensions of the language's source files.  You can use *\n"
        . "#    to mean any undefined extension.\n"
        . "#\n"
        . "# Shebang Strings: [string] [string] ...\n"
        . "#    Defines a list of strings that can appear in the shebang (#!) line to\n"
        . "#    designate that it's part of the language.\n"
        . "#\n";
        }
    else
        {
        print FH_LANGUAGES
        "# [Add/Replace] Extensions: [extension] [extension] ...\n"
        . "#    Defines the file extensions of the language's source files.  You can\n"
        . "#    redefine extensions found in the main languages file.  You can use * to\n"
        . "#    mean any undefined extension.\n"
        . "#\n"
        . "# Shebang Strings: [string] [string] ...\n"
        . "# [Add/Replace] Shebang Strings: [string] [string] ...\n"
        . "#    Defines a list of strings that can appear in the shebang (#!) line to\n"
        . "#    designate that it's part of the language.  You can redefine strings found\n"
        . "#    in the main languages file.\n"
        . "#\n";
        };

    print FH_LANGUAGES
    "# Ignore Prefixes in Index: [prefix] [prefix] ...\n"
    . (!$isMain ? "# [Add/Replace] Ignored Prefixes in Index: [prefix] [prefix] ...\n#\n" : '')
    . "# Ignore [Topic Type] Prefixes in Index: [prefix] [prefix] ...\n"
    . (!$isMain ? "# [Add/Replace] Ignored [Topic Type] Prefixes in Index: [prefix] [prefix] ...\n" : '')
    . "#    Specifies prefixes that should be ignored when sorting symbols in an\n"
    . "#    index.  Can be specified in general or for a specific topic type.\n"
    . "#\n"
    . "#------------------------------------------------------------------------------\n"
    . "# For basic language support only:\n"
    . "#\n"
    . "# Line Comments: [symbol] [symbol] ...\n"
    . "#    Defines a space-separated list of symbols that are used for line comments,\n"
    . "#    if any.\n"
    . "#\n"
    . "# Block Comments: [opening sym] [closing sym] [opening sym] [closing sym] ...\n"
    . "#    Defines a space-separated list of symbol pairs that are used for block\n"
    . "#    comments, if any.\n"
    . "#\n"
    . "# Package Separator: [symbol]\n"
    . "#    Defines the default package separator symbol.  The default is a dot.\n"
    . "#\n"
    . "# [Topic Type] Prototype Enders: [symbol] [symbol] ...\n"
    . "#    When defined, Natural Docs will attempt to get a prototype from the code\n"
    . "#    immediately following the topic type.  It stops when it reaches one of\n"
    . "#    these symbols.  Use \\n for line breaks.\n"
    . "#\n"
    . "# Line Extender: [symbol]\n"
    . "#    Defines the symbol that allows a prototype to span multiple lines if\n"
    . "#    normally a line break would end it.\n"
    . "#\n"
    . "# Enum Values: [global|under type|under parent]\n"
    . "#    Defines how enum values are referenced.  The default is global.\n"
    . "#    global       - Values are always global, referenced as 'value'.\n"
    . "#    under type   - Values are under the enum type, referenced as\n"
    . "#               'package.enum.value'.\n"
    . "#    under parent - Values are under the enum's parent, referenced as\n"
    . "#               'package.value'.\n"
    . "#\n"
    . "# Perl Package: [perl package]\n"
    . "#    Specifies the Perl package used to fine-tune the language behavior in ways\n"
    . "#    too complex to do in this file.\n"
    . "#\n"
    . "#------------------------------------------------------------------------------\n"
    . "# For full language support only:\n"
    . "#\n"
    . "# Full Language Support: [perl package]\n"
    . "#    Specifies the Perl package that has the parsing routines necessary for full\n"
    . "#    language support.\n"
    . "#\n"
    . "#-------------------------------------------------------------------------------\n\n";

    if ($isMain)
        {
        print FH_LANGUAGES
        "# The following languages MUST be defined in this file:\n"
        . "#\n"
        . "#    Text File, Shebang Script\n";
        }
    else
        {
        print FH_LANGUAGES
        "# The following languages are defined in the main file, if you'd like to alter\n"
        . "# them:\n"
        . "#\n"
        . Text::Wrap::wrap('#    ', '#    ', join(', ', @mainLanguageNames)) . "\n";
        };

    print FH_LANGUAGES "\n"
    . "# If you add a language that you think would be useful to other developers\n"
    . "# and should be included in Natural Docs by default, please e-mail it to\n"
    . "# languages [at] naturaldocs [dot] org.\n";

    my @topicTypeOrder = ( ::TOPIC_GENERAL(), ::TOPIC_CLASS(), ::TOPIC_FUNCTION(), ::TOPIC_VARIABLE(),
                                         ::TOPIC_PROPERTY(), ::TOPIC_TYPE(), ::TOPIC_CONSTANT() );

    for (my $i = 0; $i < scalar @segments; $i += 3)
        {
        my ($keyword, $name, $properties) = @segments[$i..$i+2];

        print FH_LANGUAGES "\n\n";

        if ($keyword eq 'language')
            {  print FH_LANGUAGES 'Language: ' . $name . "\n\n";  }
        else
            {  print FH_LANGUAGES 'Alter Language: ' . $name . "\n\n";  };

        if (exists $properties->{'extensions'})
            {
            print FH_LANGUAGES '   ';

            if ($properties->{'extensions command'})
                {  print FH_LANGUAGES ucfirst($properties->{'extensions command'}) . ' ';  };

            my @extensions = split(/ /, $properties->{'extensions'}, 2);

            if (scalar @extensions == 1)
                {  print FH_LANGUAGES 'Extension: ';  }
            else
                {  print FH_LANGUAGES 'Extensions: ';  };

            print FH_LANGUAGES lc($properties->{'extensions'}) . "\n";
            };

        if (exists $properties->{'shebang strings'})
            {
            print FH_LANGUAGES '   ';

            if ($properties->{'shebang strings command'})
                {  print FH_LANGUAGES ucfirst($properties->{'shebang strings command'}) . ' ';  };

            my @shebangStrings = split(/ /, $properties->{'shebang strings'}, 2);

            if (scalar @shebangStrings == 1)
                {  print FH_LANGUAGES 'Shebang String: ';  }
            else
                {  print FH_LANGUAGES 'Shebang Strings: ';  };

            print FH_LANGUAGES lc($properties->{'shebang strings'}) . "\n";
            };

        if (exists $properties->{'ignored prefixes in index'})
            {
            my $topicTypePrefixes = $properties->{'ignored prefixes in index'};

            my %usedTopicTypes;
            my @topicTypes = ( @topicTypeOrder, keys %$topicTypePrefixes );

            foreach my $topicType (@topicTypes)
                {
                if ($topicType !~ / command$/ &&
                    exists $topicTypePrefixes->{$topicType} &&
                    !exists $usedTopicTypes{$topicType})
                    {
                    print FH_LANGUAGES '   ';

                    if ($topicTypePrefixes->{$topicType . ' command'})
                        {  print FH_LANGUAGES ucfirst($topicTypePrefixes->{$topicType . ' command'}) . ' Ignored ';  }
                    else
                        {  print FH_LANGUAGES 'Ignore ';  };

                    if ($topicType ne ::TOPIC_GENERAL())
                        {  print FH_LANGUAGES NaturalDocs::Topics->TypeInfo($topicType)->Name() . ' ';  };

                    my @prefixes = split(/ /, $topicTypePrefixes->{$topicType}, 2);

                    if (scalar @prefixes == 1)
                        {  print FH_LANGUAGES 'Prefix in Index: ';  }
                    else
                        {  print FH_LANGUAGES 'Prefixes in Index: ';  };

                    print FH_LANGUAGES $topicTypePrefixes->{$topicType} . "\n";

                    $usedTopicTypes{$topicType} = 1;
                    };
                };
            };

        if (exists $properties->{'line comments'})
            {
            my @comments = split(/ /, $properties->{'line comments'}, 2);

            if (scalar @comments == 1)
                {  print FH_LANGUAGES '   Line Comment: ';  }
            else
                {  print FH_LANGUAGES '   Line Comments: ';  };

            print FH_LANGUAGES $properties->{'line comments'} . "\n";
            };

        if (exists $properties->{'block comments'})
            {
            my @comments = split(/ /, $properties->{'block comments'}, 3);

            if (scalar @comments == 2)
                {  print FH_LANGUAGES '   Block Comment: ';  }
            else
                {  print FH_LANGUAGES '   Block Comments: ';  };

            print FH_LANGUAGES $properties->{'block comments'} . "\n";
            };

        if (exists $properties->{'package separator'})
            {
            # Prior to 1.32, Package Separator was allowed for full language support.  Ignore it when reformatting.
            if ($version >= NaturalDocs::Version->FromString('1.32') || !exists $properties->{'full language support'})
                {  print FH_LANGUAGES '   Package Separator: ' . $properties->{'package separator'} . "\n";  };
            };

        if (exists $properties->{'enum values'})
            {
            print FH_LANGUAGES '   Enum Values: ' . ucfirst(lc($properties->{'enum values'})) . "\n";
            };

        if (exists $properties->{'prototype enders'})
            {
            my $topicTypeEnders = $properties->{'prototype enders'};

            my %usedTopicTypes;
            my @topicTypes = ( @topicTypeOrder, keys %$topicTypeEnders );

            foreach my $topicType (@topicTypes)
                {
                if ($topicType !~ / command$/ &&
                    exists $topicTypeEnders->{$topicType} &&
                    !exists $usedTopicTypes{$topicType})
                    {
                    print FH_LANGUAGES '   ';

                    if ($topicType ne ::TOPIC_GENERAL())
                        {  print FH_LANGUAGES NaturalDocs::Topics->TypeInfo($topicType)->Name() . ' ';  };

                    my @enders = split(/ /, $topicTypeEnders->{$topicType}, 2);

                    if (scalar @enders == 1)
                        {  print FH_LANGUAGES 'Prototype Ender: ';  }
                    else
                        {  print FH_LANGUAGES 'Prototype Enders: ';  };

                    print FH_LANGUAGES $topicTypeEnders->{$topicType} . "\n";

                    $usedTopicTypes{$topicType} = 1;
                    };
                };
            };

        if (exists $properties->{'line extender'})
            {
            print FH_LANGUAGES '   Line Extender: ' . $properties->{'line extender'} . "\n";
            };

        if (exists $properties->{'perl package'})
            {
            print FH_LANGUAGES '   Perl Package: ' . $properties->{'perl package'} . "\n";
            };

        if (exists $properties->{'full language support'})
            {
            print FH_LANGUAGES '   Full Language Support: ' . $properties->{'full language support'} . "\n";
            };
        };

    close(FH_LANGUAGES);
    };



###############################################################################
# Group: Functions


#
#   Function: LanguageOf
#
#   Returns the language of the passed source file.
#
#   Parameters:
#
#       sourceFile - The source <FileName> to get the language of.
#
#   Returns:
#
#       A <NaturalDocs::Languages::Base>-derived object for the passed file, or undef if the file is not a recognized language.
#
sub LanguageOf #(sourceFile)
    {
    my ($self, $sourceFile) = @_;

    my $extension = NaturalDocs::File->ExtensionOf($sourceFile);
    if (defined $extension)
        {  $extension = lc($extension);  };

    my $languageName;

    if (!defined $extension)
        {  $languageName = 'shebang script';  }
    else
        {  $languageName = $extensions{$extension};  };

    if (!defined $languageName)
        {  $languageName = $extensions{'*'};  };

    if (defined $languageName)
        {
        if ($languageName eq 'shebang script')
            {
            if (exists $shebangFiles{$sourceFile})
                {
                if (defined $shebangFiles{$sourceFile})
                    {  return $languages{$shebangFiles{$sourceFile}};  }
                else
                    {  return undef;  };
                }

            else # (!exists $shebangFiles{$sourceFile})
                {
                my $shebangLine;

                if (open(SOURCEFILEHANDLE, '<' . $sourceFile))
                	{
                    my $lineReader = NaturalDocs::LineReader->New(\*SOURCEFILEHANDLE);
                    $shebangLine = $lineReader->Get();

	                if (substr($shebangLine, 0, 2) ne '#!')
	                    {  $shebangLine = undef;  };

	                close (SOURCEFILEHANDLE);
	                }
	            elsif (defined $extension)
	            	{  die 'Could not open ' . $sourceFile;  }
	            # Ignore extensionless files that can't be opened.  They may be system files.

                if (!defined $shebangLine)
                    {
                    $shebangFiles{$sourceFile} = undef;
                    return undef;
                    }
                else
                    {
                    $shebangLine = lc($shebangLine);

                    foreach my $shebangString (keys %shebangStrings)
                        {
                        if (index($shebangLine, $shebangString) != -1)
                            {
                            $shebangFiles{$sourceFile} = $shebangStrings{$shebangString};
                            return $languages{$shebangStrings{$shebangString}};
                            };
                        };

                    $shebangFiles{$sourceFile} = undef;
                    return undef;
                    };
                };
            }

        else # language name ne 'shebang script'
            {  return $languages{$languageName};  };
        }
    else # !defined $language
        {
        return undef;
        };
    };


#
#   Function: OnMostUsedLanguageKnown
#
#   Called when the most used language is known.
#
sub OnMostUsedLanguageKnown
    {
    my $self = shift;

    my $language = $languages{lc( NaturalDocs::Project->MostUsedLanguage() )};

    if ($language)
        {
        if (!$languages{'text file'}->HasIgnoredPrefixes())
            {  $languages{'text file'}->CopyIgnoredPrefixesOf($language);  };
        if (!$languages{'text file'}->PackageSeparatorWasSet())
            {  $languages{'text file'}->SetPackageSeparator($language->PackageSeparator());  };
        if (!$languages{'text file'}->EnumValuesWasSet())
            {  $languages{'text file'}->SetEnumValues($language->EnumValues());  };
        };
    };


1;
