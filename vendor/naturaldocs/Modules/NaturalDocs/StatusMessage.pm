###############################################################################
#
#   Package: NaturalDocs::StatusMessage
#
###############################################################################
#
#   A package to handle status message updates.  Automatically handles <NaturalDocs::Settings->IsQuiet()>.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::StatusMessage;


#
#   var: message
#   The message to display.
#
my $message;

#
#   var: total
#   The number of items to work through.
#
my $total;

#
#   var: completed
#   The number of items completed.
#
my $completed;

#
#   var: lastMessageTime
#   The time the last message was posted.
#
my $lastMessageTime;


#
#   constant: TIME_BETWEEN_UPDATES
#   The number of seconds that should occur between updates.
#
use constant TIME_BETWEEN_UPDATES => 10;



#
#   Function: Start
#
#   Starts the status message.
#
#   Parameters:
#
#       message - The message to post.
#       total - The number of items that are going to be worked through.
#
sub Start #(message, total)
    {
    my $self = shift;

    if (!NaturalDocs::Settings->IsQuiet())
        {
        ($message, $total) = @_;
        $completed = 0;

        print $message . "\n";

        $lastMessageTime = time();
        };
    };


#
#   Function: CompletedItem
#
#   Should be called every time an item is completed.
#
sub CompletedItem
    {
    my $self = shift;

    if (!NaturalDocs::Settings->IsQuiet())
        {
        # We scale completed by 100 since we need to anyway to get the percentage.

        $completed += 100;

        if (time() >= $lastMessageTime + TIME_BETWEEN_UPDATES && $completed != $total * 100)
            {
            print $message . ' (' . ($completed / $total) . '%)' . "\n";
            $lastMessageTime = time();
            };
        };
    };

1;
