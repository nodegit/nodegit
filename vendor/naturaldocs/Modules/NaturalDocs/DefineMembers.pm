###############################################################################
#
#   Package: NaturalDocs::DefineMembers
#
###############################################################################
#
#   A custom Perl pragma to define member constants and accessors for use in Natural Docs objects while supporting inheritance.
#
#   Each member will be defined as a numeric constant which should be used as that variable's index into the object arrayref.
#   They will be assigned sequentially from zero, and take into account any members defined this way in parent classes.  Note
#   that you can *not* use multiple inheritance with this method.
#
#   If a parameter ends in parenthesis, it will be generated as an accessor for the previous member.  If it also starts with "Set",
#   the accessor will accept a single parameter to replace the value with.  If it's followed with "duparrayref", it will assume the
#   parameter is either an arrayref or undef, and if the former, will duplicate it to set the value.
#
#   Example:
#
#   > package MyPackage;
#   >
#   > use NaturalDocs::DefineMembers 'VAR_A', 'VarA()', 'SetVarA()',
#   >                                'VAR_B', 'VarB()',
#   >                                'VAR_C',
#   >                                'VAR_D', 'VarD()', 'SetVarD() duparrayref';
#   >
#   > sub SetC #(C)
#   >    {
#   >    my ($self, $c) = @_;
#   >    $self->[VAR_C] = $c;
#   >    };
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details


package NaturalDocs::DefineMembers;

sub import #(member, member, ...)
    {
    my ($self, @parameters) = @_;
    my $package = caller();

    no strict 'refs';
    my $parent = ${$package . '::ISA'}[0];
    use strict 'refs';

    my $memberConstant = 0;
    my $lastMemberName;

    if (defined $parent && $parent->can('END_OF_MEMBERS'))
        {  $memberConstant = $parent->END_OF_MEMBERS();  };

    my $code = '{ package ' . $package . ";\n";

    foreach my $parameter (@parameters)
        {
        if ($parameter =~ /^(.+)\(\) *(duparrayref)?$/i)
            {
            my ($functionName, $pragma) = ($1, lc($2));

            if ($functionName =~ /^Set/)
                {
                if ($pragma eq 'duparrayref')
                    {
                    $code .=
                    'sub ' . $functionName . '
                        {
                        if (defined $_[1])
                            {  $_[0]->[' . $lastMemberName . '] = [ @{$_[1]} ];  }
                        else
                            {  $_[0]->[' . $lastMemberName . '] = undef;  };
                        };' . "\n";
                    }
                else
                    {
                    $code .= 'sub ' . $functionName . ' { $_[0]->[' . $lastMemberName . '] = $_[1];  };' . "\n";
                    };
                }
            else
                {
                $code .= 'sub ' . $functionName . ' { return $_[0]->[' . $lastMemberName . '];  };' . "\n";
                };
            }
        else
            {
            $code .= 'use constant ' . $parameter . ' => ' . $memberConstant . ";\n";
            $memberConstant++;
            $lastMemberName = $parameter;
            };
        };

    $code .= 'use constant END_OF_MEMBERS => ' . $memberConstant . ";\n";
    $code .= '};';

    eval $code;
    };

1;
