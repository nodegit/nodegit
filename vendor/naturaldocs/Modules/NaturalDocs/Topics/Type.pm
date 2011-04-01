###############################################################################
#
#   Package: NaturalDocs::Topics::Type
#
###############################################################################
#
#   A class storing information about a <TopicType>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::Topics::Type;

use NaturalDocs::DefineMembers 'NAME',                         'Name()',
                                                 'PLURAL_NAME',             'PluralName()',      'SetPluralName()',
                                                 'INDEX',                        'Index()',              'SetIndex()',
                                                 'SCOPE',                       'Scope()',              'SetScope()',
                                                 'PAGE_TITLE_IF_FIRST', 'PageTitleIfFirst()', 'SetPageTitleIfFirst()',
                                                 'BREAK_LISTS',             'BreakLists()',        'SetBreakLists()',
                                                 'CLASS_HIERARCHY',    'ClassHierarchy()',  'SetClassHierarchy()',
                                                 'CAN_GROUP_WITH';

# Dependency: New() depends on the order of these and that there are no parent classes.

use base 'Exporter';
our @EXPORT = ('SCOPE_NORMAL', 'SCOPE_START', 'SCOPE_END', 'SCOPE_ALWAYS_GLOBAL');

#
#   Constants: Members
#
#   The object is implemented as a blessed arrayref, with the following constants as its indexes.
#
#   NAME - The topic's name.
#   PLURAL_NAME - The topic's plural name.
#   INDEX - Whether the topic is indexed.
#   SCOPE - The topic's <ScopeType>.
#   PAGE_TITLE_IF_FIRST - Whether the topic becomes the page title if it's first in a file.
#   BREAK_LISTS - Whether list topics should be broken into individual topics in the output.
#   CLASS_HIERARCHY - Whether the topic is part of the class hierarchy.
#   CAN_GROUP_WITH - The existence hashref of <TopicTypes> the type can be grouped with.
#



###############################################################################
# Group: Types


#
#   Constants: ScopeType
#
#   The possible values for <Scope()>.
#
#   SCOPE_NORMAL - The topic stays in the current scope without affecting it.
#   SCOPE_START - The topic starts a scope.
#   SCOPE_END - The topic ends a scope, returning it to global.
#   SCOPE_ALWAYS_GLOBAL - The topic is always global, but it doesn't affect the current scope.
#
use constant SCOPE_NORMAL => 1;
use constant SCOPE_START => 2;
use constant SCOPE_END => 3;
use constant SCOPE_ALWAYS_GLOBAL => 4;



###############################################################################
# Group: Functions


#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       name - The topic name.
#       pluralName - The topic's plural name.
#       index - Whether the topic is indexed.
#       scope - The topic's <ScopeType>.
#       pageTitleIfFirst - Whether the topic becomes the page title if it's the first one in a file.
#       breakLists - Whether list topics should be broken into individual topics in the output.
#
sub New #(name, pluralName, index, scope, pageTitleIfFirst, breakLists)
    {
    my ($self, @params) = @_;

    # Dependency: Depends on the parameter order matching the member order and that there are no parent classes.

    my $object = [ @params ];
    bless $object, $self;

    return $object;
    };


#
#   Functions: Accessors
#
#   Name - Returns the topic name.
#   PluralName - Returns the topic's plural name.
#   SetPluralName - Replaces the topic's plural name.
#   Index - Whether the topic is indexed.
#   SetIndex - Sets whether the topic is indexed.
#   Scope - Returns the topic's <ScopeType>.
#   SetScope - Replaces the topic's <ScopeType>.
#   PageTitleIfFirst - Returns whether the topic becomes the page title if it's first in the file.
#   SetPageTitleIfFirst - Sets whether the topic becomes the page title if it's first in the file.
#   BreakLists - Returns whether list topics should be broken into individual topics in the output.
#   SetBreakLists - Sets whether list topics should be broken into individual topics in the output.
#   ClassHierarchy - Returns whether the topic is part of the class hierarchy.
#   SetClassHierarchy - Sets whether the topic is part of the class hierarchy.
#


#
#   Function: CanGroupWith
#
#   Returns whether the type can be grouped with the passed <TopicType>.
#
sub CanGroupWith #(TopicType type) -> bool
    {
    my ($self, $type) = @_;
    return ( defined $self->[CAN_GROUP_WITH] && exists $self->[CAN_GROUP_WITH]->{$type} );
    };


#
#   Function: SetCanGroupWith
#
#   Sets the list of <TopicTypes> the type can be grouped with.
#
sub SetCanGroupWith #(TopicType[] types)
    {
    my ($self, $types) = @_;

    $self->[CAN_GROUP_WITH] = { };

    foreach my $type (@$types)
        {  $self->[CAN_GROUP_WITH]->{$type} = 1;  };
    };



1;
