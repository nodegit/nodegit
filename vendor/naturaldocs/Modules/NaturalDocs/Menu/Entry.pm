###############################################################################
#
#   Package: NaturalDocs::Menu::Entry
#
###############################################################################
#
#   A class representing an entry in the menu.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Menu::Entry;


###############################################################################
# Group: Implementation

#
#   Constants: Members
#
#   The object is implemented as a blessed arrayref with the indexes below.
#
#       TYPE      - The <MenuEntryType>
#       TITLE     - The title of the entry.
#       TARGET  - The target of the entry.  If the type is <MENU_FILE>, it will be the source <FileName>.  If the type is
#                       <MENU_LINK>, it will be the URL.  If the type is <MENU_GROUP>, it will be an arrayref of
#                       <NaturalDocs::Menu::Entry> objects representing the group's content.  If the type is <MENU_INDEX>, it will be
#                       a <TopicType>.
#       FLAGS    - Any <Menu Entry Flags> that apply.
#
use constant TYPE => 0;
use constant TITLE => 1;
use constant TARGET => 2;
use constant FLAGS => 3;
# DEPENDENCY: New() depends on the order of these constants.


###############################################################################
# Group: Functions

#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       type     - The <MenuEntryType>.
#       title      - The title of the entry.
#       target   - The target of the entry, if applicable.  If the type is <MENU_FILE>, use the source <FileName>.  If the type is
#                     <MENU_LINK>, use the URL.  If the type is <MENU_INDEX>, use the <TopicType>.  Otherwise set it to undef.
#       flags     - Any <Menu Entry Flags> that apply.
#
sub New #(type, title, target, flags)
    {
    # DEPENDENCY: This gode depends on the order of the constants.

    my $package = shift;

    my $object = [ @_ ];
    bless $object, $package;

    if ($object->[TYPE] == ::MENU_GROUP())
        {  $object->[TARGET] = [ ];  };
    if (!defined $object->[FLAGS])
        {  $object->[FLAGS] = 0;  };

    return $object;
    };


#   Function: Type
#   Returns the <MenuEntryType>.
sub Type
    {  return $_[0]->[TYPE];  };

#   Function: Title
#   Returns the title of the entry.
sub Title
    {  return $_[0]->[TITLE];  };

# Function: SetTitle
# Replaces the entry's title.
sub SetTitle #(title)
    {  $_[0]->[TITLE] = $_[1];  };

#
#   Function: Target
#
#   Returns the target of the entry, if applicable.  If the type is <MENU_FILE>, it returns the source <FileName>.  If the type is
#   <MENU_LINK>, it returns the URL.  If the type is <MENU_INDEX>, it returns the <TopicType>.  Otherwise it returns undef.
#
sub Target
    {
    my $self = shift;

    # Group entries are the only time when target won't be undef when it should be.
    if ($self->Type() == ::MENU_GROUP())
        {  return undef;  }
    else
        {  return $self->[TARGET];  };
    };

# Function: SetTarget
# Replaces the entry's target.
sub SetTarget #(target)
    {  $_[0]->[TARGET] = $_[1];  };

#   Function: Flags
#   Returns the <Menu Entry Flags>.
sub Flags
    {  return $_[0]->[FLAGS];  };

# Function: SetFlags
# Replaces the <Menu Entry Flags>.
sub SetFlags #(flags)
    {  $_[0]->[FLAGS] = $_[1];  };



###############################################################################
# Group: Group Functions
#
#   All of these functions assume the type is <MENU_GROUP>.  Do *not* call any of these without checking <Type()> first.


#
#   Function: GroupContent
#
#   Returns an arrayref of <NaturalDocs::Menu::Entry> objects representing the contents of the
#   group, or undef otherwise.  This arrayref will always exist for <MENU_GROUP>'s and can be changed.
#
sub GroupContent
    {
    return $_[0]->[TARGET];
    };


#
#   Function: GroupIsEmpty
#
#   If the type is <MENU_GROUP>, returns whether the group is empty.
#
sub GroupIsEmpty
    {
    my $self = shift;
    return (scalar @{$self->GroupContent()} > 0);
    };


#
#   Function: PushToGroup
#
#   Pushes the entry to the end of the group content.
#
sub PushToGroup #(entry)
    {
    my ($self, $entry) = @_;
    push @{$self->GroupContent()}, $entry;
    };


#
#   Function: DeleteFromGroup
#
#   Deletes an entry from the group content by index.
#
sub DeleteFromGroup #(index)
    {
    my ($self, $index) = @_;

    my $groupContent = $self->GroupContent();

    splice( @$groupContent, $index, 1 );
    };


#
#   Function: MarkEndOfOriginal
#
#   If the group doesn't already have one, adds a <MENU_ENDOFORIGINAL> entry to the end and sets the
#   <MENU_GROUP_HASENDOFORIGINAL> flag.
#
sub MarkEndOfOriginal
    {
    my $self = shift;

    if (($self->Flags() & ::MENU_GROUP_HASENDOFORIGINAL()) == 0)
        {
        $self->PushToGroup( NaturalDocs::Menu::Entry->New(::MENU_ENDOFORIGINAL(), undef, undef, undef) );
        $self->SetFlags( $self->Flags() | ::MENU_GROUP_HASENDOFORIGINAL() );
        };
    };


1;
