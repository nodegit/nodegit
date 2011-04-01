###############################################################################
#
#   Class: NaturalDocs::Languages::CSharp
#
###############################################################################
#
#   A subclass to handle the language variations of C#.
#
#
#   Topic: Language Support
#
#       Supported:
#
#       - Classes
#       - Namespaces (no topic generated)
#       - Functions
#       - Constructors and Destructors
#       - Properties
#       - Indexers
#       - Operators
#       - Delegates
#       - Variables
#       - Constants
#       - Events
#       - Enums
#
#       Not supported yet:
#
#       - Autodocumenting enum members
#       - Using alias
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::CSharp;

use base 'NaturalDocs::Languages::Advanced';


###############################################################################
# Group: Package Variables

#
#   hash: classKeywords
#   An existence hash of all the acceptable class keywords.  The keys are in all lowercase.
#
my %classKeywords = ( 'class' => 1,
                                    'struct' => 1,
                                    'interface' => 1 );

#
#   hash: classModifiers
#   An existence hash of all the acceptable class modifiers.  The keys are in all lowercase.
#
my %classModifiers = ( 'new' => 1,
                                   'public' => 1,
                                   'protected' => 1,
                                   'internal' => 1,
                                   'private' => 1,
                                   'abstract' => 1,
                                   'sealed' => 1,
                                   'unsafe' => 1,
                                   'static' => 1,
                                   'partial' => 1 );

#
#   hash: functionModifiers
#   An existence hash of all the acceptable function modifiers.  Also applies to properties.  Also encompasses those for operators
#   and indexers, but have more than are valid for them.  The keys are in all lowercase.
#
my %functionModifiers = ( 'new' => 1,
                                       'public' => 1,
                                       'protected' => 1,
                                       'internal' => 1,
                                       'private' => 1,
                                       'static' => 1,
                                       'virtual' => 1,
                                       'sealed' => 1,
                                       'override' => 1,
                                       'abstract' => 1,
                                       'extern' => 1,
                                       'unsafe' => 1 );

#
#   hash: variableModifiers
#   An existence hash of all the acceptable variable modifiers.  The keys are in all lowercase.
#
my %variableModifiers = ( 'new' => 1,
                                       'public' => 1,
                                       'protected' => 1,
                                       'internal' => 1,
                                       'private' => 1,
                                       'static' => 1,
                                       'readonly' => 1,
                                       'volatile' => 1,
                                       'unsafe' => 1 );

#
#   hash: enumTypes
#   An existence hash of all the possible enum types.  The keys are in all lowercase.
#
my %enumTypes = ( 'sbyte' => 1,
                             'byte' => 1,
                             'short' => 1,
                             'ushort' => 1,
                             'int' => 1,
                             'uint' => 1,
                             'long' => 1,
                             'ulong' => 1 );

#
#   hash: impossibleTypeWords
#   An existence hash of all the reserved words that cannot be in a type.  This includes 'enum' and all modifiers.  The keys are in
#   all lowercase.
#
my %impossibleTypeWords = ( 'abstract' => 1, 'as' => 1, 'base' => 1, 'break' => 1, 'case' => 1, 'catch' => 1,
                                              'checked' => 1, 'class' => 1, 'const' => 1, 'continue' => 1, 'default' => 1, 'delegate' => 1,
                                              'do' => 1, 'else' => 1, 'enum' => 1, 'event' => 1, 'explicit' => 1, 'extern' => 1,
                                              'false' => 1, 'finally' => 1, 'fixed' => 1, 'for' => 1, 'foreach' => 1, 'goto' => 1, 'if' => 1,
                                              'implicit' => 1, 'in' => 1, 'interface' => 1, 'internal' => 1, 'is' => 1, 'lock' => 1,
                                              'namespace' => 1, 'new' => 1, 'null' => 1, 'operator' => 1, 'out' => 1, 'override' => 1,
                                              'params' => 1, 'private' => 1, 'protected' => 1, 'public' => 1, 'readonly' => 1, 'ref' => 1,
                                              'return' => 1, 'sealed' => 1, 'sizeof' => 1, 'stackalloc' => 1, 'static' => 1,
                                              'struct' => 1, 'switch' => 1, 'this' => 1, 'throw' => 1, 'true' => 1, 'try' => 1, 'typeof' => 1,
                                              'unchecked' => 1, 'unsafe' => 1, 'using' => 1, 'virtual' => 1, 'volatile' => 1, 'while' => 1 );
# Deleted from the list: object, string, bool, decimal, sbyte, byte, short, ushort, int, uint, long, ulong, char, float, double, void



###############################################################################
# Group: Interface Functions


#
#   Function: PackageSeparator
#   Returns the package separator symbol.
#
sub PackageSeparator
    {  return '.';  };


#
#   Function: EnumValues
#   Returns the <EnumValuesType> that describes how the language handles enums.
#
sub EnumValues
    {  return ::ENUM_UNDER_TYPE();  };


#
#   Function: ParseFile
#
#   Parses the passed source file, sending comments acceptable for documentation to <NaturalDocs::Parser->OnComment()>.
#
#   Parameters:
#
#       sourceFile - The <FileName> to parse.
#       topicList - A reference to the list of <NaturalDocs::Parser::ParsedTopics> being built by the file.
#
#   Returns:
#
#       The array ( autoTopics, scopeRecord ).
#
#       autoTopics - An arrayref of automatically generated topics from the file, or undef if none.
#       scopeRecord - An arrayref of <NaturalDocs::Languages::Advanced::ScopeChanges>, or undef if none.
#
sub ParseFile #(sourceFile, topicsList)
    {
    my ($self, $sourceFile, $topicsList) = @_;

    $self->ParseForCommentsAndTokens($sourceFile, [ '//' ], [ '/*', '*/' ], [ '///' ], [ '/**', '*/' ] );

    my $tokens = $self->Tokens();
    my $index = 0;
    my $lineNumber = 1;

    while ($index < scalar @$tokens)
        {
        if ($self->TryToSkipWhitespace(\$index, \$lineNumber) ||
            $self->TryToGetNamespace(\$index, \$lineNumber) ||
            $self->TryToGetUsing(\$index, \$lineNumber) ||
            $self->TryToGetClass(\$index, \$lineNumber) ||
            $self->TryToGetFunction(\$index, \$lineNumber) ||
            $self->TryToGetOverloadedOperator(\$index, \$lineNumber) ||
            $self->TryToGetVariable(\$index, \$lineNumber) ||
            $self->TryToGetEnum(\$index, \$lineNumber) )
            {
            # The functions above will handle everything.
            }

        elsif ($tokens->[$index] eq '{')
            {
            $self->StartScope('}', $lineNumber, undef, undef, undef);
            $index++;
            }

        elsif ($tokens->[$index] eq '}')
            {
            if ($self->ClosingScopeSymbol() eq '}')
                {  $self->EndScope($lineNumber);  };

            $index++;
            }

        else
            {
            $self->SkipRestOfStatement(\$index, \$lineNumber);
            };
        };


    # Don't need to keep these around.
    $self->ClearTokens();


    my $autoTopics = $self->AutoTopics();

    my $scopeRecord = $self->ScopeRecord();
    if (defined $scopeRecord && !scalar @$scopeRecord)
        {  $scopeRecord = undef;  };

    return ( $autoTopics, $scopeRecord );
    };



###############################################################################
# Group: Statement Parsing Functions
# All functions here assume that the current position is at the beginning of a statement.
#
# Note for developers: I am well aware that the code in these functions do not check if we're past the end of the tokens as
# often as it should.  We're making use of the fact that Perl will always return undef in these cases to keep the code simpler.


#
#   Function: TryToGetNamespace
#
#   Determines whether the position is at a namespace declaration statement, and if so, adjusts the scope, skips it, and returns
#   true.
#
#   Why no topic?:
#
#       The main reason we don't create a Natural Docs topic for a namespace is because in order to declare class A.B.C in C#,
#       you must do this:
#
#       > namespace A.B
#       >    {
#       >    class C
#       >        { ... }
#       >    }
#
#       That would result in a namespace topic whose only purpose is really to qualify C.  It would take the default page title, and
#       thus the default menu title.  So if you have files for A.B.X, A.B.Y, and A.B.Z, they all will appear as A.B on the menu.
#
#       If something actually appears in the namespace besides a class, it will be handled by
#       <NaturalDocs::Parser->AddPackageDelineators()>.  That function will add a package topic to correct the scope.
#
#       If the user actually documented it, it will still appear because of the manual topic.
#
sub TryToGetNamespace #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    if (lc($tokens->[$$indexRef]) ne 'namespace')
        {  return undef;  };

    my $index = $$indexRef + 1;
    my $lineNumber = $$lineNumberRef;

    if (!$self->TryToSkipWhitespace(\$index, \$lineNumber))
        {  return undef;  };

    my $name;

    while ($tokens->[$index] =~ /^[a-z_\.\@]/i)
        {
        $name .= $tokens->[$index];
        $index++;
        };

    if (!defined $name)
        {  return undef;  };

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    if ($tokens->[$index] ne '{')
        {  return undef;  };

    $index++;


    # We found a valid one if we made it this far.

    my $autoTopic = NaturalDocs::Parser::ParsedTopic->New(::TOPIC_CLASS(), $name,
                                                                                         $self->CurrentScope(), $self->CurrentUsing(),
                                                                                         undef,
                                                                                         undef, undef, $$lineNumberRef);

    # We don't add an auto-topic for namespaces.  See the function documentation above.

    NaturalDocs::Parser->OnClass($autoTopic->Package());

    $self->StartScope('}', $lineNumber, $autoTopic->Package());

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetClass
#
#   Determines whether the position is at a class declaration statement, and if so, generates a topic for it, skips it, and
#   returns true.
#
#   Supported Syntaxes:
#
#       - Classes
#       - Structs
#       - Interfaces
#
sub TryToGetClass #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    my $startIndex = $index;
    my $startLine = $lineNumber;
    my $needsPrototype = 0;

    if ($self->TryToSkipAttributes(\$index, \$lineNumber))
        {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }

    my @modifiers;

    while ($tokens->[$index] =~ /^[a-z]/i &&
              !exists $classKeywords{lc($tokens->[$index])} &&
              exists $classModifiers{lc($tokens->[$index])} )
        {
        push @modifiers, lc($tokens->[$index]);
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };

    if (!exists $classKeywords{lc($tokens->[$index])})
        {  return undef;  };

    my $lcClassKeyword = lc($tokens->[$index]);

    $index++;

    if (!$self->TryToSkipWhitespace(\$index, \$lineNumber))
        {  return undef;  };

    my $name;

    while ($tokens->[$index] =~ /^[a-z_\@]/i)
        {
        $name .= $tokens->[$index];
        $index++;
        };

    if (!defined $name)
        {  return undef;  };

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

	if ($self->TryToSkipTemplateSpec(\$index, \$lineNumber))
		{
		$needsPrototype = 1;
		$self->TryToSkipWhitespace(\$index, \$lineNumber);

		if ($self->TryToSkipWhereClauses(\$index, \$lineNumber))
			{  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }
		}

    my @parents;

    if ($tokens->[$index] eq ':')
        {
        my $inheritsTemplates;

        do
            {
            $index++;

            $self->TryToSkipWhitespace(\$index, \$lineNumber);

            my $parentName;

            while ($tokens->[$index] =~ /^[a-z_\.\@]/i)
                {
                $parentName .= $tokens->[$index];
                $index++;
                };

            if (!defined $parentName)
                {  return undef;  };

            push @parents, NaturalDocs::SymbolString->FromText($parentName);

            $self->TryToSkipWhitespace(\$index, \$lineNumber);

            if ($self->TryToSkipTemplateSpec(\$index, \$lineNumber))
            	{
            	$inheritsTemplates = 1;
            	$needsPrototype = 1;
            	$self->TryToSkipWhitespace(\$index, \$lineNumber);
            	}
            }
        while ($tokens->[$index] eq ',');

        if ($inheritsTemplates)
        	{
        	if ($self->TryToSkipWhereClauses(\$index, \$lineNumber))
        		{  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }
        	}
        };

    if ($tokens->[$index] ne '{')
        {  return undef;  };


    # If we made it this far, we have a valid class declaration.

    my @scopeIdentifiers = NaturalDocs::SymbolString->IdentifiersOf($self->CurrentScope());
    $name = join('.', @scopeIdentifiers, $name);

    my $topicType;

    if ($lcClassKeyword eq 'interface')
        {  $topicType = ::TOPIC_INTERFACE();  }
    else
        {  $topicType = ::TOPIC_CLASS();  };

    my $prototype;

    if ($needsPrototype)
            {
            $prototype = $self->CreateString($startIndex, $index);
            }

    my $autoTopic = NaturalDocs::Parser::ParsedTopic->New($topicType, $name,
                                                                                         undef, $self->CurrentUsing(),
                                                                                         $prototype,
                                                                                         undef, undef, $$lineNumberRef);

    $self->AddAutoTopic($autoTopic);
    NaturalDocs::Parser->OnClass($autoTopic->Package());

    foreach my $parent (@parents)
        {
        NaturalDocs::Parser->OnClassParent($autoTopic->Package(), $parent, $self->CurrentScope(), undef,
                                                               ::RESOLVE_RELATIVE());
        };

    $self->StartScope('}', $lineNumber, $autoTopic->Package());

    $index++;

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetUsing
#
#   Determines whether the position is at a using statement, and if so, adds it to the current scope, skips it, and returns
#        true.
#
#        Supported:
#
#       - Using
#
#        Unsupported:
#
#                - Using with alias
#
sub TryToGetUsing #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    if (lc($tokens->[$index]) ne 'using')
        {  return undef;  };

    $index++;
    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    my $name;

    while ($tokens->[$index] =~ /^[a-z_\@\.]/i)
        {
        $name .= $tokens->[$index];
        $index++;
        };

    if ($tokens->[$index] ne ';' ||
                !defined $name)
        {  return undef;  };

    $index++;


    $self->AddUsing( NaturalDocs::SymbolString->FromText($name) );

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };



#
#   Function: TryToGetFunction
#
#   Determines if the position is on a function declaration, and if so, generates a topic for it, skips it, and returns true.
#
#   Supported Syntaxes:
#
#       - Functions
#       - Constructors
#       - Destructors
#       - Properties
#       - Indexers
#       - Delegates
#       - Events
#
sub TryToGetFunction #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    if ($self->TryToSkipAttributes(\$index, \$lineNumber))
        {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  };

    my $startIndex = $index;
    my $startLine = $lineNumber;

    my @modifiers;

    while ($tokens->[$index] =~ /^[a-z]/i &&
              exists $functionModifiers{lc($tokens->[$index])} )
        {
        push @modifiers, lc($tokens->[$index]);
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };

    my $isDelegate;
    my $isEvent;

    if (lc($tokens->[$index]) eq 'delegate')
        {
        $isDelegate = 1;
        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        }
    elsif (lc($tokens->[$index]) eq 'event')
        {
        $isEvent = 1;
        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };

    my $returnType = $self->TryToGetType(\$index, \$lineNumber);

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    my $name;
    my $lastNameWord;
    my $hasTemplates;

    while ($tokens->[$index] =~ /^[a-z\_\@\.\~]/i)
        {
        $name .= $tokens->[$index];
        $lastNameWord = $tokens->[$index];
        $index++;

        # For explicit generic interface definitions, such as
        # IDObjectType System.Collections.Generic.IEnumerator<IDObjectType>.Current
        # or templated functions.

        if ($self->TryToSkipTemplateSpec(\$index, \$lineNumber))
        	{
        	$hasTemplates = 1;
        	$self->TryToSkipWhitespace(\$index, \$lineNumber);
        	}
        };

    if (!defined $name)
        {
        # Constructors and destructors don't have return types.  It's possible their names were mistaken for the return type.
        if (defined $returnType)
            {
            $name = $returnType;
            $returnType = undef;

            $name =~ /([a-z0-9_]+)$/i;
            $lastNameWord = $1;
            }
        else
            {  return undef;  };
        };

    # If there's no return type, make sure it's a constructor or destructor.
    if (!defined $returnType)
        {
        my @identifiers = NaturalDocs::SymbolString->IdentifiersOf( $self->CurrentScope() );

        if ($lastNameWord ne $identifiers[-1])
            {  return undef;  };
        };

    $self->TryToSkipWhitespace(\$index, \$lineNumber);


    # Skip the brackets on indexers.
    if ($tokens->[$index] eq '[' && lc($lastNameWord) eq 'this')
        {
        # This should jump the brackets completely.
        $self->GenericSkip(\$index, \$lineNumber);
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        $name .= '[]';
        };


    # Properties, indexers, events with braces

    if ($tokens->[$index] eq '{')
        {
        my $prototype = $self->CreateString($startIndex, $index);

        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        my ($aWord, $bWord, $hasA, $hasB);

        if ($isEvent)
            {
            $aWord = 'add';
            $bWord = 'remove';
            }
        else
            {
            $aWord = 'get';
            $bWord = 'set';
            };

        while ($index < scalar @$tokens)
            {
            if ($self->TryToSkipAttributes(\$index, \$lineNumber))
                {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  };

            if (lc($tokens->[$index]) eq $aWord)
                {  $hasA = 1;  }
            elsif (lc($tokens->[$index]) eq $bWord)
                {  $hasB = 1;  }
            elsif ($tokens->[$index] eq '}')
                {
                $index++;
                last;
                };

            $self->SkipRestOfStatement(\$index, \$lineNumber);
            $self->TryToSkipWhitespace(\$index, \$lineNumber);
            };

        if ($hasA && $hasB)
            {  $prototype .= ' { ' . $aWord . ', ' . $bWord . ' }';  }
        elsif ($hasA)
            {  $prototype .= ' { ' . $aWord . ' }';  }
        elsif ($hasB)
            {  $prototype .= ' { ' . $bWord . ' }';  };

        $prototype = $self->NormalizePrototype($prototype);

        my $topicType = ( $isEvent ? ::TOPIC_EVENT() : ::TOPIC_PROPERTY() );

        $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New($topicType, $name,
                                                                                                  $self->CurrentScope(), $self->CurrentUsing(),
                                                                                                  $prototype,
                                                                                                  undef, undef, $startLine));
        }


    # Functions, constructors, destructors, delegates.

    elsif ($tokens->[$index] eq '(')
        {
        # This should jump the parenthesis completely.
        $self->GenericSkip(\$index, \$lineNumber);
		$self->TryToSkipWhitespace(\$index, \$lineNumber);

        if ($hasTemplates && $self->TryToSkipWhereClauses(\$index, \$lineNumber))
        	{  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }

        my $topicType = ( $isDelegate ? ::TOPIC_DELEGATE() : ::TOPIC_FUNCTION() );
        my $prototype = $self->NormalizePrototype( $self->CreateString($startIndex, $index) );

        $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New($topicType, $name,
                                                                                                  $self->CurrentScope(), $self->CurrentUsing(),
                                                                                                  $prototype,
                                                                                                  undef, undef, $startLine));

        $self->SkipRestOfStatement(\$index, \$lineNumber);
        }


    # Events without braces

    elsif ($isEvent && $tokens->[$index] eq ';')
        {
        my $prototype = $self->NormalizePrototype( $self->CreateString($startIndex, $index) );

        $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New(::TOPIC_EVENT(), $name,
                                                                                                  $self->CurrentScope(), $self->CurrentUsing(),
                                                                                                  $prototype,
                                                                                                  undef, undef, $startLine));
        $index++;
        }

    else
        {  return undef;  };


    # We succeeded if we got this far.

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetOverloadedOperator
#
#   Determines if the position is on an operator overload declaration, and if so, generates a topic for it, skips it, and returns true.
#
sub TryToGetOverloadedOperator #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    if ($self->TryToSkipAttributes(\$index, \$lineNumber))
        {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  };

    my $startIndex = $index;
    my $startLine = $lineNumber;

    my @modifiers;

    while ($tokens->[$index] =~ /^[a-z]/i &&
              exists $functionModifiers{lc($tokens->[$index])} )
        {
        push @modifiers, lc($tokens->[$index]);
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };


    my $name;


    # Casting operators.

    if (lc($tokens->[$index]) eq 'implicit' || lc($tokens->[$index]) eq 'explicit')
        {
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if (lc($tokens->[$index]) ne 'operator')
            {  return undef;  };

        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        $name = $self->TryToGetType(\$index, \$lineNumber);

        if (!defined $name)
            {  return undef;  };
        }


    # Symbol operators.

    else
        {
        if (!$self->TryToGetType(\$index, \$lineNumber))
            {  return undef;  };

        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if (lc($tokens->[$index]) ne 'operator')
            {  return undef;  };

        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if (lc($tokens->[$index]) eq 'true' || lc($tokens->[$index]) eq 'false')
            {
            $name = $tokens->[$index];
            $index++;
            }
        else
            {
            while ($tokens->[$index] =~ /^[\+\-\!\~\*\/\%\&\|\^\<\>\=]$/)
                {
                $name .= $tokens->[$index];
                $index++;
                };
            };
        };

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    if ($tokens->[$index] ne '(')
        {  return undef;  };

    # This should skip the parenthesis completely.
    $self->GenericSkip(\$index, \$lineNumber);

    my $prototype = $self->NormalizePrototype( $self->CreateString($startIndex, $index) );

    $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New(::TOPIC_FUNCTION(), 'operator ' . $name,
                                                                                              $self->CurrentScope(), $self->CurrentUsing(),
                                                                                              $prototype,
                                                                                              undef, undef, $startLine));

    $self->SkipRestOfStatement(\$index, \$lineNumber);


    # We succeeded if we got this far.

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetVariable
#
#   Determines if the position is on a variable declaration statement, and if so, generates a topic for each variable, skips the
#   statement, and returns true.
#
#   Supported Syntaxes:
#
#       - Variables
#       - Constants
#
sub TryToGetVariable #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    if ($self->TryToSkipAttributes(\$index, \$lineNumber))
        {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  };

    my $startIndex = $index;
    my $startLine = $lineNumber;

    my @modifiers;

    while ($tokens->[$index] =~ /^[a-z]/i &&
              exists $variableModifiers{lc($tokens->[$index])} )
        {
        push @modifiers, lc($tokens->[$index]);
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };

    my $type;
    if (lc($tokens->[$index]) eq 'const')
        {
        $type = ::TOPIC_CONSTANT();
        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        }
    else
        {
        $type = ::TOPIC_VARIABLE();
        };

    if (!$self->TryToGetType(\$index, \$lineNumber))
        {  return undef;  };

    my $endTypeIndex = $index;

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    my @names;

    for (;;)
        {
        my $name;

        while ($tokens->[$index] =~ /^[a-z\@\_]/i)
            {
            $name .= $tokens->[$index];
            $index++;
            };

        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if ($tokens->[$index] eq '=')
            {
            do
                {
                $self->GenericSkip(\$index, \$lineNumber);
                }
            while ($tokens->[$index] ne ',' && $tokens->[$index] ne ';');
            };

        push @names, $name;

        if ($tokens->[$index] eq ';')
            {
            $index++;
            last;
            }
        elsif ($tokens->[$index] eq ',')
            {
            $index++;
            $self->TryToSkipWhitespace(\$index, \$lineNumber);
            }
        else
            {  return undef;  };
        };


    # We succeeded if we got this far.

    my $prototypePrefix = $self->CreateString($startIndex, $endTypeIndex);

    foreach my $name (@names)
        {
        my $prototype = $self->NormalizePrototype( $prototypePrefix . ' ' . $name );

        $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New($type, $name,
                                                                                                  $self->CurrentScope(), $self->CurrentUsing(),
                                                                                                  $prototype,
                                                                                                  undef, undef, $startLine));
        };

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetEnum
#
#   Determines if the position is on an enum declaration statement, and if so, generates a topic for it.
#
#   Supported Syntaxes:
#
#       - Enums
#       - Enums with declared types
#
#   Unsupported:
#
#       - Documenting the members automatically
#
sub TryToGetEnum #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    if ($self->TryToSkipAttributes(\$index, \$lineNumber))
        {  $self->TryToSkipWhitespace(\$index, \$lineNumber);  };

    my $startIndex = $index;
    my $startLine = $lineNumber;

    my @modifiers;

    while ($tokens->[$index] =~ /^[a-z]/i &&
              exists $variableModifiers{lc($tokens->[$index])} )
        {
        push @modifiers, lc($tokens->[$index]);
        $index++;

        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        };

    if (lc($tokens->[$index]) ne 'enum')
        {  return undef;  }

    $index++;
    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    my $name;

    while ($tokens->[$index] =~ /^[a-z\@\_]/i)
        {
        $name .= $tokens->[$index];
        $index++;
        };

    $self->TryToSkipWhitespace(\$index, \$lineNumber);

    if ($tokens->[$index] eq ':')
        {
        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if (!exists $enumTypes{ lc($tokens->[$index]) })
            {  return undef;  }

        $index++;
        $self->TryToSkipWhitespace(\$index, \$lineNumber);
        }

    if ($tokens->[$index] ne '{')
        {  return undef;  }

    # We succeeded if we got this far.

    my $prototype = $self->CreateString($startIndex, $index);
    $prototype = $self->NormalizePrototype( $prototype );

    $self->SkipRestOfStatement(\$index, \$lineNumber);

    $self->AddAutoTopic(NaturalDocs::Parser::ParsedTopic->New(::TOPIC_ENUMERATION(), $name,
                                                                                              $self->CurrentScope(), $self->CurrentUsing(),
                                                                                              $prototype,
                                                                                              undef, undef, $startLine));

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
    };


#
#   Function: TryToGetType
#
#   Determines if the position is on a type identifier, and if so, skips it and returns it as a string.  This function does _not_ allow
#   modifiers.  You must take care of those beforehand.
#
sub TryToGetType #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $name;
    my $index = $$indexRef;
    my $lineNumber = $$lineNumberRef;

    while ($tokens->[$index] =~ /^[a-z\@\.\_]/i)
        {
		# Case sensitive since you can declare a class Lock even though lock is a keyword.
        if (exists $impossibleTypeWords{ $tokens->[$index] } && $name !~ /\@$/)
            {  return undef;  };

        $name .= $tokens->[$index];
        $index++;
        };

    if (!defined $name)
        {  return undef;  };

        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if ($tokens->[$index] eq '?')
                {
                $name .= '?';
                $index++;

                $self->TryToSkipWhitespace(\$index, \$lineNumber);
                }

    if ($self->TryToSkipTemplateSpec(\$index, \$lineNumber))
    	{  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }

    while ($tokens->[$index] eq '[')
        {
        $name .= '[';
        $index++;

        while ($tokens->[$index] eq ',')
            {
            $name .= ',';
            $index++;
            };

        if ($tokens->[$index] eq ']')
            {
            $name .= ']';
            $index++;
            }
        else
            {  return undef;  }
        };

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return $name;
    };



###############################################################################
# Group: Low Level Parsing Functions


#
#   Function: GenericSkip
#
#   Advances the position one place through general code.
#
#   - If the position is on a string, it will skip it completely.
#   - If the position is on an opening symbol, it will skip until the past the closing symbol.
#   - If the position is on whitespace (including comments and preprocessing directives), it will skip it completely.
#   - Otherwise it skips one token.
#
#   Parameters:
#
#       indexRef - A reference to the current index.
#       lineNumberRef - A reference to the current line number.
#
sub GenericSkip #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    # We can ignore the scope stack because we're just skipping everything without parsing, and we need recursion anyway.
    if ($tokens->[$$indexRef] eq '{')
        {
        $$indexRef++;
        $self->GenericSkipUntilAfter($indexRef, $lineNumberRef, '}');
        }
    elsif ($tokens->[$$indexRef] eq '(')
        {
        $$indexRef++;
        $self->GenericSkipUntilAfter($indexRef, $lineNumberRef, ')');
        }
    elsif ($tokens->[$$indexRef] eq '[')
        {
        $$indexRef++;
        $self->GenericSkipUntilAfter($indexRef, $lineNumberRef, ']');
        }

    elsif ($self->TryToSkipWhitespace($indexRef, $lineNumberRef) ||
            $self->TryToSkipString($indexRef, $lineNumberRef))
        {
        }

    else
        {  $$indexRef++;  };
    };


#
#   Function: GenericSkipUntilAfter
#
#   Advances the position via <GenericSkip()> until a specific token is reached and passed.
#
sub GenericSkipUntilAfter #(indexRef, lineNumberRef, token)
    {
    my ($self, $indexRef, $lineNumberRef, $token) = @_;
    my $tokens = $self->Tokens();

    while ($$indexRef < scalar @$tokens && $tokens->[$$indexRef] ne $token)
        {  $self->GenericSkip($indexRef, $lineNumberRef);  };

    if ($tokens->[$$indexRef] eq "\n")
        {  $$lineNumberRef++;  };
    $$indexRef++;
    };


#
#   Function: SkipRestOfStatement
#
#   Advances the position via <GenericSkip()> until after the end of the current statement, which is defined as a semicolon or
#   a brace group.  Of course, either of those appearing inside parenthesis, a nested brace group, etc. don't count.
#
sub SkipRestOfStatement #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    while ($$indexRef < scalar @$tokens &&
             $tokens->[$$indexRef] ne ';' &&
             $tokens->[$$indexRef] ne '{')
        {
        $self->GenericSkip($indexRef, $lineNumberRef);
        };

    if ($tokens->[$$indexRef] eq ';')
        {  $$indexRef++;  }
    elsif ($tokens->[$$indexRef] eq '{')
        {  $self->GenericSkip($indexRef, $lineNumberRef);  };
    };


#
#   Function: TryToSkipString
#   If the current position is on a string delimiter, skip past the string and return true.
#
#   Parameters:
#
#       indexRef - A reference to the index of the position to start at.
#       lineNumberRef - A reference to the line number of the position.
#
#   Returns:
#
#       Whether the position was at a string.
#
#   Syntax Support:
#
#       - Supports quotes, apostrophes, and at-quotes.
#
sub TryToSkipString #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    # The three string delimiters.  All three are Perl variables when preceded by a dollar sign.
    if ($self->SUPER::TryToSkipString($indexRef, $lineNumberRef, '\'') ||
        $self->SUPER::TryToSkipString($indexRef, $lineNumberRef, '"') )
        {
        return 1;
        }
    elsif ($tokens->[$$indexRef] eq '@' && $tokens->[$$indexRef+1] eq '"')
        {
        $$indexRef += 2;

        # We need to do at-strings manually because backslash characters are accepted as regular characters, and two consecutive
        # quotes are accepted as well.

        while ($$indexRef < scalar @$tokens && !($tokens->[$$indexRef] eq '"' && $tokens->[$$indexRef+1] ne '"') )
            {
            if ($tokens->[$$indexRef] eq '"')
                {
                # This is safe because the while condition will only let through quote pairs.
                $$indexRef += 2;
                }
            elsif ($tokens->[$$indexRef] eq "\n")
                {
                $$indexRef++;
                $$lineNumberRef++;
                }
            else
                {
                $$indexRef++;
                };
            };

        # Skip the closing quote.
        if ($$indexRef < scalar @$tokens)
            {  $$indexRef++;  };

        return 1;
        }
    else
        {  return undef;  };
    };


#
#   Function: TryToSkipAttributes
#   If the current position is on an attribute section, skip it and return true.  Skips multiple attribute sections if they appear
#   consecutively.
#
sub TryToSkipAttributes #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $success;

    while ($tokens->[$$indexRef] eq '[')
        {
        $success = 1;
        $$indexRef++;
        $self->GenericSkipUntilAfter($indexRef, $lineNumberRef, ']');
        $self->TryToSkipWhitespace($indexRef, $lineNumberRef);
        };

    return $success;
    };


#
#	Function: TryToSkipTemplateSpec
#	If the current position is on a template spec (the part in angle brackets) skip it and return true.  Can handle nested
#	templates.
#
sub TryToSkipTemplateSpec #(indexRef, lineNumberRef)
	{
	my ($self, $indexRef, $lineNumberRef) = @_;
	my $tokens = $self->Tokens();

    if ($tokens->[$$indexRef] eq '<')
        {
        my $nestingLevel = 1;
        $$indexRef++;
        $self->TryToSkipWhitespace($indexRef, $lineNumberRef);

        while ($$indexRef < scalar @$tokens && $nestingLevel > 0)
        	{
        	if ($tokens->[$$indexRef] eq '<')
        		{  $nestingLevel++;  }
        	elsif ($tokens->[$$indexRef] eq '>')
        		{  $nestingLevel--;  }

            $$indexRef++;
        	$self->TryToSkipWhitespace($indexRef, $lineNumberRef);
			}

		return 1;
        }
    else
    	{  return undef;  }
	}


#
#	Function: TryToSkipWhereClauses
#	If the current position is on a "where" clause, skips it and returns true.  Can handle multiple wheres in a row.
#
sub TryToSkipWhereClauses #(indexRef, lineNumberRef)
	{
	my ($self, $indexRef, $lineNumberRef) = @_;
	my $tokens = $self->Tokens();

    if ($tokens->[$$indexRef] ne 'where')
    	{  return undef;  }

	my $index = $$indexRef;
	my $lineNumber = $$lineNumberRef;

    do
        {
        $index++;  # Skip the where
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        $index++;  # Skip the variable name
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        if ($tokens->[$index] ne ':')
        	{  return undef;  }

        $index++;  # Skip the colon
        $self->TryToSkipWhitespace(\$index, \$lineNumber);

        for (;;)
        	{
        	if ($index >= scalar @$tokens)
        		{  return undef;  }
        	elsif ($tokens->[$index] eq 'new')
        		{
        		$index++;
        		$self->TryToSkipWhitespace(\$index, \$lineNumber);

        		if ($tokens->[$index] ne '(')
        			{  return undef;  }

                $index++;
        		$self->TryToSkipWhitespace(\$index, \$lineNumber);

        		if ($tokens->[$index] ne ')')
        			{  return undef;  }

        		$index++;
        		$self->TryToSkipWhitespace(\$index, \$lineNumber);
        		}
        	else
        		{
        		while ($index < scalar @$tokens && $tokens->[$index] =~ /^[a-z0-9_\.\@]+$/i)
        			{  $index++;  }

        		$self->TryToSkipWhitespace(\$index, \$lineNumber);

        		if ($self->TryToSkipTemplateSpec(\$index, \$lineNumber))
        			{  $self->TryToSkipWhitespace(\$index, \$lineNumber);  }
        		}

        	if ($tokens->[$index] eq ',')
        		{
        		$index++;
        		$self->TryToSkipWhitespace(\$index, \$lineNumber);
        		}
        	else
        		{  last;  }
        	}
        }
    while ($tokens->[$index] eq 'where');

    $$indexRef = $index;
    $$lineNumberRef = $lineNumber;

    return 1;
	}


#
#   Function: TryToSkipWhitespace
#   If the current position is on a whitespace token, a line break token, a comment, or a preprocessing directive, it skips them
#   and returns true.  If there are a number of these in a row, it skips them all.
#
sub TryToSkipWhitespace #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    my $result;

    while ($$indexRef < scalar @$tokens)
        {
        if ($tokens->[$$indexRef] =~ /^[ \t]/)
            {
            $$indexRef++;
            $result = 1;
            }
        elsif ($tokens->[$$indexRef] eq "\n")
            {
            $$indexRef++;
            $$lineNumberRef++;
            $result = 1;
            }
        elsif ($self->TryToSkipComment($indexRef, $lineNumberRef) ||
                $self->TryToSkipPreprocessingDirective($indexRef, $lineNumberRef))
            {
            $result = 1;
            }
        else
            {  last;  };
        };

    return $result;
    };


#
#   Function: TryToSkipComment
#   If the current position is on a comment, skip past it and return true.
#
sub TryToSkipComment #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;

    return ( $self->TryToSkipLineComment($indexRef, $lineNumberRef) ||
                $self->TryToSkipMultilineComment($indexRef, $lineNumberRef) );
    };


#
#   Function: TryToSkipLineComment
#   If the current position is on a line comment symbol, skip past it and return true.
#
sub TryToSkipLineComment #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    if ($tokens->[$$indexRef] eq '/' && $tokens->[$$indexRef+1] eq '/')
        {
        $self->SkipRestOfLine($indexRef, $lineNumberRef);
        return 1;
        }
    else
        {  return undef;  };
    };


#
#   Function: TryToSkipMultilineComment
#   If the current position is on an opening comment symbol, skip past it and return true.
#
sub TryToSkipMultilineComment #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    if ($tokens->[$$indexRef] eq '/' && $tokens->[$$indexRef+1] eq '*')
        {
        $self->SkipUntilAfter($indexRef, $lineNumberRef, '*', '/');
        return 1;
        }
    else
        {  return undef;  };
    };


#
#   Function: TryToSkipPreprocessingDirective
#   If the current position is on a preprocessing directive, skip past it and return true.
#
sub TryToSkipPreprocessingDirective #(indexRef, lineNumberRef)
    {
    my ($self, $indexRef, $lineNumberRef) = @_;
    my $tokens = $self->Tokens();

    if ($tokens->[$$indexRef] eq '#' && $self->IsFirstLineToken($$indexRef))
        {
        $self->SkipRestOfLine($indexRef, $lineNumberRef);
        return 1;
        }
    else
        {  return undef;  };
    };


1;
