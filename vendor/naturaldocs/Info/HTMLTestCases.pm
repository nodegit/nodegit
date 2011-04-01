###############################################################################
#
#   File: Browser Testing
#
###############################################################################
#
#   This file tests Natural Docs' generated output.  Particularly useful when testing various browsers.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


#
#   About: Browsers
#
#   The specific browser versions tested are below.  Everything is tested on Windows Vista unless otherwise noted.
#
#   Firefox 2.0.0.10 - 2.0 released October 2006.
#   Firefox 1.5.0.8 - 1.5 released Novemer 2005.
#   Firefox 1.0.8 - 1.0 released November 2004.  Not critical to support.
#
#   IE 7.0 - 7.0 released October 2006.
#   IE 6.0 - 6.0 released August 2001.  Tested on Windows XP SP2 via Virtual PC.
#
#	Safari 3.0.4 - 3.0 released June 2007.  Tested Windows version.
#	Safari 2.0.4 - 2.0 released April 2005.  Tested on Mac OS X 10.4 Tiger.
#
#   Opera 9.02 - 9.0 released June 2006.
#   Opera 8.54 - 8.5 released September 2005.
#   Opera 8.02 - 8.0 released April 2005.
#   Opera 7.51 - 7.5 released around August 2004 I think.  Not critical to support.
#   Opera 7.02 - 7.0 released January 2003.  Not critical to support.
#
#   Konqueror 3.5.5 - Tested on openSUSE 10.2 via VMware Player.
#


###############################################################################
# Group: Search

#
#   Topic: Unframed HTML Search
#
#   Tests:
#
#       - Make sure the search box appears and disappears correctly on hover.
#       - Type to bring up results.  Type further to narrow them.  Narrow until there's no results.
#       - Backspace to bring the results back.  Backspacing to empty closes the results.
#       - Type to bring up results with a different first letter.  (Tests iframe content switch.)
#       - Type *Z* to bring up empty page when there's nothing with that first letter.  (Tests generic no results page.)
#       - Type *Name* in Everything search to test expanding and collapsing, especially between two that differ only by case.
#       - Change filter to *Functions* to test changing filter while results are open.  Change to *Types* to switch to one with
#         no results.
#       - Test Close button on results.  Should deactivate panel as well.
#       - Clicking away should deactivate panel if the box is empty, not have an effect if there are results open.
#       - Text should always change back to "Search" when deactivating.
#
#   Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - Functionally OK.  Search panel doesn't activate on hover.  Works fine when clicked.
#
#		Safari 3.0  - OK
#		Safari 2.0  - *Broken.*  Results panel doesn't show up.  Border around deactivated search box.
#
#       Opera 9.0  - OK
#       Opera 8.5  - OK
#       Opera 8.0  - OK
#       Opera 7.5  - Functionally OK.  Search panel has sunken border when deactivated, minor pixel shifting.
#       Opera 7.0  - *Broken.*  Completely.
#
#       Konqueror 3.5  - *Broken.*  Results panel doesn't show up.  Seems to fail on "resultsFrame = window.frames.MSearchResults;"
#

#
#   Topic: Framed HTML Search
#
#   Tests:
#
#       - Make sure the search box appears and disappears correctly on hover.
#       - Type to bring up results on right.  Type further to narrow them.  Narrow until there's no results.
#       - Backspace to bring the results back.
#       - Type to bring up results with a different first letter.  (Tests frame content switch.)
#       - Type *Z* to bring up empty page when there's nothing with that first letter.  (Tests generic no results page.)
#       - Type *Name* in Everything search to see that there's no collapsing in this mode.
#       - Change filter to *Functions* to test changing filter while results are open.  Change to *Types* to switch to one with
#         no results.
#       - Clicking away should deactivate panel.
#       - Clicking a result should deactivate panel and show up in correct frame.
#       - Text should always change back to "Search" when deactivating.
#
#   Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - Functionally OK.  Search panel doesn't activate on hover, is a little wide.  Works fine when clicked.
#
#		Safari 3.0  - OK
#		Safari 2.0  - Functionally OK.  Has a sunken border around the deactivated seach field.
#
#       Opera 9.0  - OK
#       Opera 8.5  - OK
#       Opera 8.0  - OK
#       Opera 7.5  - Functionally OK.  Search panel has sunken border when deactivated, minor pixel shifting.
#       Opera 7.0  - *Broken.*
#
#       Konqueror 3.5  - Functionally OK.  Panel doesn't reset and deactivate when clicking a result link.
#


###############################################################################
# Group: Other

#
#   Topic: Images
#
#	Tests:
#
#   - Here is an embedded image on its own line.
#
#   (see images/logo.png)
#
#   - Here is a reference in the middle of a sentence, in the middle of a bullet list: (see images/logo.png)  It should have been
#     converted to a link with the image appearing below the bullet list and the file name used as a caption.  Make sure the
#     caption positions correctly.
#   - Here's a link to a non-existent image, which should appear literally: (see images/doesntexist.jpg)
#   - Here is an embedded image that doesn't exist on it's own line.
#
#   (see images/doesntexist.png)
#
#   - Here is a link using the "(see)" syntax which shouldn't be interpreted as an image link because it doesn't end with an
#     acceptable extension.  Also, links should still resolve because of that.  (see <Framed HTML Search>)
#
#	Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - OK
#
#		Safari 3.0  - OK
#		Safari 2.0  - OK
#
#       Opera 9.0  - OK
#       Opera 8.5  - OK
#       Opera 8.0  - OK
#       Opera 7.5  - OK
#       Opera 7.0  - OK
#
#       Konqueror 3.5  - OK


#
#	Topic: Prototypes and Tooltips
#
#	Hover over <NaturalDocs::Parser::JavaDoc->ParseComment()> and <NaturalDocs::Parser::JavaDoc->IsMine()>
#
#	Tests:
#
#		- A tooltip should appear about a second after you hover over the link above.
#		- It should go away when you move the cursor away.
#		- It shoud be positioned directly underneath with a slight gap.
#		- The prototype should be formatted cleanly with each parameter on its own line and aligned in columns.
#		- The asterisk should be in a separate column.
#		- Test it with the link too close to the edge of the window so the pop-up has to shift left to fit.
#
#	Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - OK
#
#		Safari 3.0  - OK
#		Safari 2.0  - OK
#
#       Opera 9.0  - OK.  Has its own tooltips turned on by default which can cover it up though.
#       Opera 8.5  - OK.  Has its own tooltips turned on by default which can cover it up though.
#       Opera 8.0  - OK.  Has its own tooltips turned on by default which can cover it up though.
#       Opera 7.5  - OK.  Has its own tooltips turned on by default which can cover it up though.
#       Opera 7.0  - *Broken.*  Usually works, if the window is too narrow may collapse completely.
#
#       Konqueror 3.5  - OK
#


#
#	Topic: Long code block scrolling
#
#	Go to <Prototype Parameter Styles>.
#
#	Tests:
#
#		- Shrink the browser window so that a line extends past the end of it.  Only the line should have a scrollbar, not the
#		  entire page.
#		- Expand the browser window.  The scrollbar should disappear.
#
#	Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - OK
#
#		Safari 3.0  - OK
#		Safari 2.0  - OK
#
#       Opera 9.0  - OK
#       Opera 8.5  - OK
#       Opera 8.0  - OK
#       Opera 7.5  - OK
#       Opera 7.0  - OK
#
#       Konqueror 3.5  - OK
#


#
#	Topic: Menu and Class Hierarchies
#
#	Go to <NaturalDocs::Languages::Simple>.
#
#	Tests:
#
#		- Class hierarchy should look okay.
#		- Make sure the menu hierarchy opens up on its own when the page is loaded.
#		- You should be able to click on groups to open and close them.
#
#	Results:
#
#       Firefox 2.0  - OK
#       Firefox 1.5  - OK
#       Firefox 1.0  - OK
#
#       IE 7.0  - OK
#       IE 6.0  - OK
#
#		Safari 3.0  - OK
#		Safari 2.0  - OK
#
#       Opera 9.0  - OK
#       Opera 8.5  - OK
#       Opera 8.0  - OK
#       Opera 7.5  - OK
#       Opera 7.0  - OK
#
#       Konqueror 3.5  - OK
#


1;
