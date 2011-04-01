###############################################################################
#
#   Class: NaturalDocs::Languages::PLSQL
#
###############################################################################
#
#   A subclass to handle the language variations of PL/SQL.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::PLSQL;

use base 'NaturalDocs::Languages::Simple';


#
#   Function: OnPrototypeEnd
#
#   Microsoft's SQL specifies parameters as shown below.
#
#   > CREATE PROCEDURE Test @as int, @foo int AS ...
#
#   Having a parameter @is or @as is perfectly valid even though those words are also used to end the prototype.  We need to
#   ignore text-based enders preceded by an at sign.  Also note that it does not have parenthesis for parameter lists.  We need to
#   skip all commas if the prototype doesn't have parenthesis but does have @ characters.
#
#	Identifiers such as function names may contain the characters $, #, and _, so if "as" or "is" appears directly after one of them
#	we need to ignore the ender there as well.
#
#	> FUNCTION Something_is_something ...
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
    my ($self, $type, $prototypeRef, $ender) = @_;

    # _ should be handled already.
    if ($ender =~ /^[a-z]+$/i && substr($$prototypeRef, -1) =~ /^[\@\$\#]$/)
        {  return ::ENDER_IGNORE();  }

    elsif ($type eq ::TOPIC_FUNCTION() && $ender eq ',')
        {
        if ($$prototypeRef =~ /^[^\(]*\@/)
            {  return ::ENDER_IGNORE();  }
        else
            {  return ::ENDER_ACCEPT();  };
        }

    else
        {  return ::ENDER_ACCEPT();  };
    };


#
#   Function: ParsePrototype
#
#   Overridden to handle Microsoft's parenthesisless version.  Otherwise just throws to the parent.
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

    my $noParenthesisParameters = ($type eq ::TOPIC_FUNCTION() && $prototype =~ /^[^\(]*\@/);

    if ($prototype !~ /\(.*[^ ].*\)/ && !$noParenthesisParameters)
        {  return $self->SUPER::ParsePrototype($type, $prototype);  };



    my ($beforeParameters, $afterParameters, $isAfterParameters);

    if ($noParenthesisParameters)
        {
        ($beforeParameters, $prototype) = split(/\@/, $prototype, 2);
        $prototype = '@' . $prototype;
        };

    my @tokens = $prototype =~ /([^\(\)\[\]\{\}\<\>\'\"\,]+|.)/g;

    my $parameter;
    my @parameterLines;

    my @symbolStack;

    foreach my $token (@tokens)
        {
        if ($isAfterParameters)
            {  $afterParameters .= $token;  }

        elsif ($symbolStack[-1] eq '\'' || $symbolStack[-1] eq '"')
            {
            if ($noParenthesisParameters || $symbolStack[0] eq '(')
                {  $parameter .= $token;  }
            else
                {  $beforeParameters .= $token;  };

            if ($token eq $symbolStack[-1])
                {  pop @symbolStack;  };
            }

        elsif ($token =~ /^[\(\[\{\<\'\"]$/)
            {
            if ($noParenthesisParameters || $symbolStack[0] eq '(')
                {  $parameter .= $token;  }
            else
                {  $beforeParameters .= $token;  };

            push @symbolStack, $token;
            }

        elsif ( ($token eq ')' && $symbolStack[-1] eq '(') ||
                 ($token eq ']' && $symbolStack[-1] eq '[') ||
                 ($token eq '}' && $symbolStack[-1] eq '{') ||
                 ($token eq '>' && $symbolStack[-1] eq '<') )
            {
            if (!$noParenthesisParameters && $token eq ')' && scalar @symbolStack == 1 && $symbolStack[0] eq '(')
                {
                $afterParameters .= $token;
                $isAfterParameters = 1;
                }
            else
                {  $parameter .= $token;  };

            pop @symbolStack;
            }

        elsif ($token eq ',')
            {
            if (!scalar @symbolStack)
                {
                if ($noParenthesisParameters)
                    {
                    push @parameterLines, $parameter . $token;
                    $parameter = undef;
                    }
                else
                    {
                    $beforeParameters .= $token;
                    };
                }
            else
                {
                if (scalar @symbolStack == 1 && $symbolStack[0] eq '(' && !$noParenthesisParameters)
                    {
                    push @parameterLines, $parameter . $token;
                    $parameter = undef;
                    }
                else
                    {
                    $parameter .= $token;
                    };
                };
            }

        else
            {
            if ($noParenthesisParameters || $symbolStack[0] eq '(')
                {  $parameter .= $token;  }
            else
                {  $beforeParameters .= $token;  };
            };
        };

    push @parameterLines, $parameter;

    foreach my $item (\$beforeParameters, \$afterParameters)
        {
        $$item =~ s/^ //;
        $$item =~ s/ $//;
        }

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
sub ParseParameterLine #(line)
    {
    my ($self, $line) = @_;

    $line =~ s/^ //;
    $line =~ s/ $//;

    my @tokens = $line =~ /([^\(\)\[\]\{\}\<\>\'\"\:\=\ ]+|\:\=|.)/g;

    my ($name, $type, $defaultValue, $defaultValuePrefix, $inType, $inDefaultValue);


    my @symbolStack;

    foreach my $token (@tokens)
        {
        if ($inDefaultValue)
            {  $defaultValue .= $token;  }

        elsif ($symbolStack[-1] eq '\'' || $symbolStack[-1] eq '"')
            {
            if ($inType)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };

            if ($token eq $symbolStack[-1])
                {  pop @symbolStack;  };
            }

        elsif ($token =~ /^[\(\[\{\<\'\"]$/)
            {
            if ($inType)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };

            push @symbolStack, $token;
            }

        elsif ( ($token eq ')' && $symbolStack[-1] eq '(') ||
                 ($token eq ']' && $symbolStack[-1] eq '[') ||
                 ($token eq '}' && $symbolStack[-1] eq '{') ||
                 ($token eq '>' && $symbolStack[-1] eq '<') )
            {
            if ($inType)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };

            pop @symbolStack;
            }

        elsif ($token eq ' ')
            {
            if ($inType)
                {  $type .= $token;  }
            elsif (!scalar @symbolStack)
                {  $inType = 1;  }
            else
                {  $name .= $token;  };
            }

        elsif ($token eq ':=' || $token eq '=')
            {
            if (!scalar @symbolStack)
                {
                $defaultValuePrefix = $token;
                $inDefaultValue = 1;
                }
            elsif ($inType)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };
            }

        else
            {
            if ($inType)
                {  $type .= $token;  }
            else
                {  $name .= $token;  };
            };
        };

    foreach my $part (\$type, \$defaultValue)
        {
        $$part =~ s/ $//;
        };

    return NaturalDocs::Languages::Prototype::Parameter->New($type, undef, $name, undef, $defaultValue, $defaultValuePrefix);
    };


sub TypeBeforeParameter
    {  return 0;  };

1;
