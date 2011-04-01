###############################################################################
#
#   Class: NaturalDocs::Languages::Pascal
#
###############################################################################
#
#   A subclass to handle the language variations of Pascal and Delphi.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Languages::Pascal;

use base 'NaturalDocs::Languages::Simple';


#
#   hash: prototypeDirectives
#
#   An existence hash of all the directives that can appear after a function prototype and will be included.  The keys are the all
#   lowercase keywords.
#
my %prototypeDirectives = ( 'overload' => 1,
                                           'override' => 1,
                                           'virtual' => 1,
                                           'abstract' => 1,
                                           'reintroduce' => 1,
                                           'export' => 1,
                                           'public' => 1,
                                           'interrupt' => 1,
                                           'register' => 1,
                                           'pascal' => 1,
                                           'cdecl' => 1,
                                           'stdcall' => 1,
                                           'popstack' => 1,
                                           'saveregisters' => 1,
                                           'inline' => 1,
                                           'safecall' => 1 );

#
#   hash: longPrototypeDirectives
#
#   An existence hash of all the directives with parameters that can appear after a function prototype and will be included.  The
#   keys are the all lowercase keywords.
#
my %longPrototypeDirectives = ( 'alias' => 1,
                                                 'external' => 1 );

#
#   bool: checkingForDirectives
#
#   Set after the first function semicolon, which means we're in directives mode.
#
my $checkingForDirectives;


#
#   Function: OnCode
#
#   Just overridden to reset <checkingForDirectives>.
#
sub OnCode #(...)
    {
    my ($self, @parameters) = @_;

    $checkingForDirectives = 0;

    return $self->SUPER::OnCode(@parameters);
    };


#
#   Function: OnPrototypeEnd
#
#   Pascal's syntax has directives after the prototype that should be included.
#
#   > function MyFunction ( param1: type ); virtual; abstract;
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

    if ($type eq ::TOPIC_FUNCTION() && $ender eq ';')
        {
        if (!$checkingForDirectives)
            {
            $checkingForDirectives = 1;
            return ::ENDER_ACCEPT_AND_CONTINUE();
            }
        elsif ($$prototypeRef =~ /;[ \t]*([a-z]+)([^;]*)$/i)
            {
            my ($lastDirective, $extra) = (lc($1), $2);

            if (exists $prototypeDirectives{$lastDirective} && $extra =~ /^[ \t]*$/)
                {  return ::ENDER_ACCEPT_AND_CONTINUE();  }
            elsif (exists $longPrototypeDirectives{$lastDirective})
                {  return ::ENDER_ACCEPT_AND_CONTINUE();  }
            else
                {  return ::ENDER_REVERT_TO_ACCEPTED();  };
            }
        else
            {  return ::ENDER_REVERT_TO_ACCEPTED();  };
        }
    else
        {  return ::ENDER_ACCEPT();  };
    };


sub ParseParameterLine #(...)
    {
    my ($self, @params) = @_;
    return $self->SUPER::ParsePascalParameterLine(@params);
    };

sub TypeBeforeParameter
    {
    return 0;
    };


1;
