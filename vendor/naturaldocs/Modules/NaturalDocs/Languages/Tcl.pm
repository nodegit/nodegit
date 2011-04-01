###############################################################################
#
#   Class: NaturalDocs::Languages::Tcl
#
###############################################################################
#
#   A subclass to handle the language variations of Tcl.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Tcl;

use base 'NaturalDocs::Languages::Simple';


#
#   bool: pastFirstBrace
#
#   Whether we've past the first brace in a function prototype or not.
#
my $pastFirstBrace;


#
#   Function: OnCode
#
#   This is just overridden to reset <pastFirstBrace>.
#
sub OnCode #(...)
    {
    my ($self, @params) = @_;

    $pastFirstBrace = 0;

    return $self->SUPER::OnCode(@params);
    };


#
#   Function: OnPrototypeEnd
#
#   Tcl's function syntax is shown below.
#
#   > proc [name] { [params] } { [code] }
#
#   The opening brace is one of the prototype enders.  We need to allow the first opening brace because it contains the
#   parameters.
#
#   Also, the parameters may have braces within them.  I've seen one that used { seconds 20 } as a parameter.
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

    if ($type eq ::TOPIC_FUNCTION() && $ender eq '{' && !$pastFirstBrace)
        {
        $pastFirstBrace = 1;
        return ::ENDER_IGNORE();
        }
    else
        {  return ::ENDER_ACCEPT();  };
    };


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

    if ($type ne ::TOPIC_FUNCTION())
        {
        my $object = NaturalDocs::Languages::Prototype->New($prototype);
        return $object;
        };


    # Parse the parameters out of the prototype.

    my @tokens = $prototype =~ /([^\{\}\ ]+|.)/g;

    my $parameter;
    my @parameterLines;

    my $braceLevel = 0;

    my ($beforeParameters, $afterParameters, $finishedParameters);

    foreach my $token (@tokens)
        {
        if ($finishedParameters)
            {  $afterParameters .= $token;  }

        elsif ($token eq '{')
            {
            if ($braceLevel == 0)
                {  $beforeParameters .= $token;  }

            else # braceLevel > 0
                {  $parameter .= $token;   };

            $braceLevel++;
            }

        elsif ($token eq '}')
            {
            if ($braceLevel == 1)
                {
                if ($parameter && $parameter ne ' ')
                    {  push @parameterLines, $parameter;  };

                $finishedParameters = 1;
                $afterParameters .= $token;

                $braceLevel--;
                }
            elsif ($braceLevel > 1)
                {
                $parameter .= $token;
                $braceLevel--;
                };
            }

        elsif ($token eq ' ')
            {
            if ($braceLevel == 1)
                {
                if ($parameter)
                    {  push @parameterLines, $parameter;  };

                $parameter = undef;
                }
            elsif ($braceLevel > 1)
                {
                $parameter .= $token;
                }
            else
                {
                $beforeParameters .= $token;
                };
            }

        else
            {
            if ($braceLevel > 0)
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
sub ParseParameterLine #(line)
    {
    my ($self, $line) = @_;
    return NaturalDocs::Languages::Prototype::Parameter->New(undef, undef, $line, undef, undef, undef);
    };


1;
