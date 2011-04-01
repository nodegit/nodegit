###############################################################################
#
#   Package: NaturalDocs::Parser::ParsedTopic
#
###############################################################################
#
#   A class for parsed topics of source files.  Also encompasses some of the <TopicType>-specific behavior.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Parser::ParsedTopic;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The object is a blessed arrayref with the following indexes.
#
#       TYPE           - The <TopicType>.
#       TITLE          - The title of the topic.
#       PACKAGE    - The package <SymbolString> the topic appears in, or undef if none.
#       USING         - An arrayref of additional package <SymbolStrings> available to the topic via "using" statements, or undef if
#                           none.
#       PROTOTYPE - The prototype, if it exists and is applicable.
#       SUMMARY    - The summary, if it exists.
#       BODY          - The body of the topic, formatted in <NDMarkup>.  Some topics may not have bodies, and if not, this
#                           will be undef.
#       LINE_NUMBER  - The line number the topic appears at in the file.
#       IS_LIST - Whether the topic is a list.
#
use NaturalDocs::DefineMembers 'TYPE', 'TITLE', 'PACKAGE', 'USING', 'PROTOTYPE', 'SUMMARY', 'BODY',
                                                 'LINE_NUMBER', 'IS_LIST';
# DEPENDENCY: New() depends on the order of these constants, and that this class is not inheriting any members.


#
#   Architecture: Title, Package, and Symbol Behavior
#
#   Title, package, and symbol behavior is a little awkward so it deserves some explanation.  Basically you set them according to
#   certain rules, but you get computed values that try to hide all the different scoping situations.
#
#   Normal Topics:
#
#       Set them to the title and package as they appear.  "Function" and "PkgA.PkgB" will return "Function" for the title,
#       "PkgA.PkgB" for the package, and "PkgA.PkgB.Function" for the symbol.
#
#       In the rare case that a title has a separator symbol it's treated as inadvertant, so "A vs. B" in "PkgA.PkgB" still returns just
#       "PkgA.PkgB" for the package even though if you got it from the symbol it can be seen as "PkgA.PkgB.A vs".
#
#   Scope Topics:
#
#       Set the title normally and leave the package undef.  So "PkgA.PkgB" and undef will return "PkgA.PkgB" for the title as well
#       as for the package and symbol.
#
#       The only time you should set the package is when you have full language support and they only documented the class with
#       a partial title.  So if you documented "PkgA.PkgB" with just "PkgB", you want to set the package to "PkgA".  This
#       will return "PkgB" as the title for presentation and will return "PkgA.PkgB" for the package and symbol, which is correct.
#
#   Always Global Topics:
#
#       Set the title and package normally, do not set the package to undef.  So "Global" and "PkgA.PkgB" will return "Global" as
#       the title, "PkgA.PkgB" as the package, and "Global" as the symbol.
#
#   Um, yeah...:
#
#       So does this suck?  Yes, yes it does.  But the suckiness is centralized here instead of having to be handled everywhere these
#       issues come into play.  Just realize there are a certain set of rules to follow when you *set* these variables, and the results
#       you see when you *get* them are computed rather than literal.
#


###############################################################################
# Group: Functions

#
#   Function: New
#
#   Creates a new object.
#
#   Parameters:
#
#       type          - The <TopicType>.
#       title           - The title of the topic.
#       package    - The package <SymbolString> the topic appears in, or undef if none.
#       using         - An arrayref of additional package <SymbolStrings> available to the topic via "using" statements, or undef if
#                          none.
#       prototype   - The prototype, if it exists and is applicable.  Otherwise set to undef.
#       summary   - The summary of the topic, if any.
#       body          - The body of the topic, formatted in <NDMarkup>.  May be undef, as some topics may not have bodies.
#       lineNumber - The line number the topic appears at in the file.
#       isList          - Whether the topic is a list topic or not.
#
#   Returns:
#
#       The new object.
#
sub New #(type, title, package, using, prototype, summary, body, lineNumber, isList)
    {
    # DEPENDENCY: This depends on the order of the parameter list being the same as the constants, and that there are no
    # members inherited from a base class.

    my $package = shift;

    my $object = [ @_ ];
    bless $object, $package;

    if (defined $object->[USING])
        {  $object->[USING] = [ @{$object->[USING]} ];  };

    return $object;
    };


# Function: Type
# Returns the <TopicType>.
sub Type
    {  return $_[0]->[TYPE];  };

# Function: SetType
# Replaces the <TopicType>.
sub SetType #(type)
    {  $_[0]->[TYPE] = $_[1];  };

# Function: IsList
# Returns whether the topic is a list.
sub IsList
    {  return $_[0]->[IS_LIST];  };

# Function: SetIsList
# Sets whether the topic is a list.
sub SetIsList
    {  $_[0]->[IS_LIST] = $_[1];  };

# Function: Title
# Returns the title of the topic.
sub Title
    {  return $_[0]->[TITLE];  };

# Function: SetTitle
# Replaces the topic title.
sub SetTitle #(title)
    {  $_[0]->[TITLE] = $_[1];  };

#
#   Function: Symbol
#
#   Returns the <SymbolString> defined by the topic.  It is fully resolved and does _not_ need to be joined with <Package()>.
#
#   Type-Specific Behavior:
#
#       - If the <TopicType> is always global, the symbol will be generated from the title only.
#       - Everything else's symbols will be generated from the title and the package passed to <New()>.
#
sub Symbol
    {
    my ($self) = @_;

    my $titleSymbol = NaturalDocs::SymbolString->FromText($self->[TITLE]);

    if (NaturalDocs::Topics->TypeInfo($self->Type())->Scope() == ::SCOPE_ALWAYS_GLOBAL())
        {  return $titleSymbol;  }
    else
        {
        return NaturalDocs::SymbolString->Join( $self->[PACKAGE], $titleSymbol );
        };
    };


#
#   Function: Package
#
#   Returns the package <SymbolString> that the topic appears in.
#
#   Type-Specific Behavior:
#
#       - If the <TopicType> has scope, the package will be generated from both the title and the package passed to <New()>, not
#         just the package.
#       - If the <TopicType> is always global, the package will be the one passed to <New()>, even though it isn't part of it's
#         <Symbol()>.
#       - Everything else's package will be what was passed to <New()>, even if the title has separator symbols in it.
#
sub Package
    {
    my ($self) = @_;

    # Headerless topics may not have a type yet.
    if ($self->Type() && NaturalDocs::Topics->TypeInfo($self->Type())->Scope() == ::SCOPE_START())
        {  return $self->Symbol();  }
    else
        {  return $self->[PACKAGE];  };
    };


# Function: SetPackage
# Replaces the package the topic appears in.  This will behave the same way as the package parameter in <New()>.  Later calls
# to <Package()> will still be generated according to its type-specific behavior.
sub SetPackage #(package)
    {  $_[0]->[PACKAGE] = $_[1];  };

# Function: Using
# Returns an arrayref of additional scope <SymbolStrings> available to the topic via "using" statements, or undef if none.
sub Using
    {  return $_[0]->[USING];  };

# Function: SetUsing
# Replaces the using arrayref of sope <SymbolStrings>.
sub SetUsing #(using)
    {  $_[0]->[USING] = $_[1];  };

# Function: Prototype
# Returns the prototype if one is defined.  Will be undef otherwise.
sub Prototype
    {  return $_[0]->[PROTOTYPE];  };

# Function: SetPrototype
# Replaces the function or variable prototype.
sub SetPrototype #(prototype)
    {  $_[0]->[PROTOTYPE] = $_[1];  };

# Function: Summary
# Returns the topic summary, if it exists, formatted in <NDMarkup>.
sub Summary
    {  return $_[0]->[SUMMARY];  };

# Function: Body
# Returns the topic's body, formatted in <NDMarkup>.  May be undef.
sub Body
    {  return $_[0]->[BODY];  };

# Function: SetBody
# Replaces the topic's body, formatted in <NDMarkup>.  May be undef.
sub SetBody #(body)
    {
    my ($self, $body) = @_;
    $self->[BODY] = $body;
    };

# Function: LineNumber
# Returns the line the topic appears at in the file.
sub LineNumber
    {  return $_[0]->[LINE_NUMBER];  };


1;
