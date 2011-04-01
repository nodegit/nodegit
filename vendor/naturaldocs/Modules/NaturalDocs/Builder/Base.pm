###############################################################################
#
#   Class: NaturalDocs::Builder::Base
#
###############################################################################
#
#   A base class for all Builder output formats.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::Builder::Base;


###############################################################################
# Group: Notes


#
#   Topic: Implementation
#
#   Builder packages are implemented as blessed arrayrefs, not hashrefs.  This is done for all objects in Natural Docs for
#   efficiency reasons.  You create members by defining constants via <NaturalDocs::DefineMembers> and using them as
#   indexes into the array.
#

#
#   Topic: Function Order
#
#   The functions in the build process will always be called in the following order.
#
#   - <BeginBuild()> will always be called.
#   - <PurgeFiles()> will be called next only if there's files that need to be purged.
#   - <PurgeIndexes()> will be called next only if there's indexes that need to be purged.
#   - <PurgeImages()> will e called next only if there's images that need to be purged.
#   - <BuildFile()> will be called once for each file that needs to be built, if any.
#   - <BuildIndex()> will be called once for each index that changed and is part of the menu, if any.
#   - <UpdateImage()> will be called once for each image that needs to be updated, if any.
#   - <UpdateMenu()> will be called next only if the menu changed.
#   - <EndBuild()> will always be called.
#

#
#   Topic: How to Approach
#
#   Here's an idea of how to approach making packages for different output types.
#
#
#   Multiple Output Files, Embedded Menu:
#
#       This example is for when you want to build one output file per source file, each with its own copy of the menu within it.
#       This is how <NaturalDocs::Builder::HTML> works.
#
#       Make sure you create a function that generates just the menu for a particular source file.  We'll need to generate menus for
#       both building a file from scratch and for updating the menu on an existing output file, so it's better to give it its own function.
#       You may want to surround it with something that can be easily detected in the output file to make replacing easier.
#
#       <BeginBuild()> isn't important.  You don't need to implement it.
#
#       Implement <PurgeFiles()> to delete the output files associated with the purged files.
#
#       Implement <PurgeIndexes()> to delete the output files associated with the purged indexes.
#
#       Implement <BuildFile()> to create an output file for the parsed source file.  Use the menu function described earlier.
#
#       Implement <BuildIndex()> to create an output file for each index.  Use the menu function described earlier for each page.
#
#       Implement <UpdateMenu()> to go through the list of unbuilt files and update their menus.  You can get the list from
#       <NaturalDocs::Project->UnbuiltFilesWithContent()>.  You need to open their output files, replace the menu, and save it back
#       to disk.  Yes, it would be simpler from a programmer's point of view to just rebuild the file completely, but that would be
#       _very_ inefficient since there could potentially be a _lot_ of files in this group.
#
#       Also make sure <UpdateMenu()> goes through the unchanged indexes and updates them as well.
#
#       <EndBuild()> isn't important.  You don't need to implement it.
#
#
#   Multiple Output Files, Menu in File:
#
#       This example is for when you want to build one output file per source file, but keep the menu in its own separate file.  This
#       is how <NaturalDocs::Builder::FramedHTML> works.
#
#       <BeginBuild()> isn't important.  You don't need to implement it.
#
#       Implement <PurgeFiles()> to delete the output files associated with the purged files.
#
#       Implement <PurgeIndexes()> to delete the output files associated with the purged indexes.
#
#       Implement <BuildFile()> to generate an output file from the parsed source file.
#
#       Implement <BuildIndex()> to generate an output file for each index.
#
#       Implement <UpdateMenu()> to rebuild the menu file.
#
#       <EndBuild()> isn't important.  You don't need to implement it.
#
#
#   Single Output File using Intermediate Files:
#
#       This example is for when you want to build one output file, such as a PDF file, but use intermediate files to handle differential
#       building.  This would be much like how a compiler compiles each source file into a object file, and then a linker stitches them
#       all together into the final executable file.
#
#       <BeginBuild()> isn't important.  You don't need to implement it.
#
#       Implement <PurgeFiles()> to delete the intermediate files associated with the purged files.
#
#       Implement <PurgeIndexes()> to delete the intermediate files associated with the purged indexes.
#
#       Implement <BuildFile()> to generate an intermediate file from the parsed source file.
#
#       Implement <BuildIndex()> to generate an intermediate file for the specified index.
#
#       Implement <UpdateMenu()> to generate the intermediate file for the menu.
#
#       Implement <EndBuild()> so that if the project changed, it stitches the intermediate files together into the final
#       output file.  Make sure you check the parameter because the function will be called when nothing changes too.
#
#
#   Single Output File using Direct Changes:
#
#       This example is for when you want to build one output file, such as a PDF file, but engineering it in such a way that you don't
#       need to use intermediate files.  In other words, you're able to add, delete, and modify entries directly in the output file.
#
#       Implement <BeginBuild()> so that if the project changed, it opens the output file and does anything it needs to do
#       to get ready for editing.
#
#       Implement <PurgeFiles()> to remove the entries associated with the purged files.
#
#       Implement <PurgeIndexes()> to remove the entries associated with the purged indexes.
#
#       Implement <BuildFile()> to add or replace a section of the output file with a new one generated from the parsed file.
#
#       Implement <BuildIndex()> to add or replace an index in the output file with a new one generated from the specified index.
#
#       Implement <EndBuild()> so that if the project changed, it saves the output file to disk.
#
#       How you handle the menu depends on how the output file references other sections of itself.  If it can do so by name, then
#       you can implement <UpdateMenu()> to update the menu section of the file and you're done.  If it has to reference itself
#       by address or offset, it gets trickier.  You should skip <UpdateMenu()> and instead rebuild the menu in <EndBuild()> if
#       the parameter is true.  This lets you do it whenever anything changes in a file, rather than just when the menu
#       visibly changes.  How you keep track of the locations and how they change is your problem.
#


###############################################################################
#
#   Group: Required Interface Functions
#
#   All Builder classes *must* define these functions.
#


#
#   Function: INIT
#
#   Define this function to call <NaturalDocs::Builder->Add()> so that <NaturalDocs::Builder> knows about this package.
#   Packages are defined this way so that new ones can be added without messing around in other code.
#


#
#   Function: CommandLineOption
#
#   Define this function to return the text that should be put in the command line after -o to use this package.  It cannot have
#   spaces and is not case sensitive.
#
#   For example, <NaturalDocs::Builder::HTML> returns 'html' so someone could use -o html [directory] to use that package.
#
sub CommandLineOption
    {
    NaturalDocs::Error->SoftDeath($_[0] . " didn't define CommandLineOption().");
    };


#
#   Function: BuildFile
#
#   Define this function to convert a parsed file to this package's output format.  This function will be called once for every source
#   file that needs to be rebuilt.  However, if a file hasn't changed since the last time Natural Docs was run, it will not be sent to
#   this function.  All packages must support differential build.
#
#   Parameters:
#
#       sourceFile  - The name of the source file.
#       parsedFile  - The parsed source file, as an arrayref of <NaturalDocs::Parser::ParsedTopic> objects.
#
sub BuildFile #(sourceFile, parsedFile)
    {
    NaturalDocs::Error->SoftDeath($_[0] . " didn't define BuildFile().");
    };


###############################################################################
#
#   Group: Optional Interface Functions
#
#   These functions can be implemented but packages are not required to do so.
#


#
#   Function: New
#
#   Creates and returns a new object.
#
#   Note that this is the only function where the first parameter will be the package name, not the object itself.
#
sub New
    {
    my $package = shift;

    my $object = [ ];
    bless $object, $package;

    return $object;
    };


#
#   Function: BeginBuild
#
#   Define this function if the package needs to do anything at the beginning of the build process.  This function will be called
#   every time Natural Docs is run, even if the project hasn't changed.  This allows you to manage dependencies specific
#   to the output format that may change independently from the source tree and menu.  For example,
#   <NaturalDocs::Builder::HTML> needs to keep the CSS files in sync regardless of whether the source tree changed or not.
#
#   Parameters:
#
#       hasChanged - Whether the project has changed, such as source files or the menu file.  If false, nothing else is going to be
#                            called except <EndBuild()>.
#
sub BeginBuild #(hasChanged)
    {
    };


#
#   Function: EndBuild
#
#   Define this function if the package needs to do anything at the end of the build process.  This function will be called every time
#   Natural Docs is run, even if the project hasn't changed.  This allows you to manage dependencies specific to the output
#   format that may change independently from the source tree.  For example, <NaturalDocs::Builder::HTML> needs to keep the
#   CSS files in sync regardless of whether the source tree changed or not.
#
#   Parameters:
#
#       hasChanged - Whether the project has changed, such as source files or the menu file.  If false, the only other function that
#                            was called was <BeginBuild()>.
#
sub EndBuild #(hasChanged)
    {
    };


#
#   Function: BuildIndex
#
#   Define this function to create an index for the passed topic.  You can get the index from
#   <NaturalDocs::SymbolTable->Index()>.
#
#   The reason it's not passed directly to this function is because indexes may be time-consuming to create.  As such, they're
#   generated on demand because some output packages may choose not to implement them.
#
#   Parameters:
#
#       topic  - The <TopicType> to limit the index by.
#
sub BuildIndex #(topic)
    {
    };


#
#   Function: UpdateImage
#
#   Define this function to add or update the passed image in the output.
#
#   Parameters:
#
#       file - The image <FileName>
#
sub UpdateImage #(file)
    {
    };


#
#   Function: PurgeFiles
#
#   Define this function to make the package remove all output related to the passed files.  These files no longer have Natural Docs
#   content.
#
#   Parameters:
#
#       files - An existence hashref of the files to purge.
#
sub PurgeFiles #(files)
    {
    };


#
#   Function: PurgeIndexes
#
#   Define this function to make the package remove all output related to the passed indexes.  These indexes are no longer part
#   of the menu.
#
#   Parameters:
#
#       indexes  - An existence hashref of the <TopicTypes> of the indexes to purge.
#
sub PurgeIndexes #(indexes)
    {
    };


#
#   Function: PurgeImages
#
#   Define this function to make the package remove all output related to the passed image files.  These files are no longer used
#   by the documentation.
#
#   Parameters:
#
#       files - An existence hashref of the image <FileNames> to purge.
#
sub PurgeImages #(files)
    {
    };


#
#   Function: UpdateMenu
#
#   Define this function to make the package update the menu.  It will only be called if the menu changed.
#
sub UpdateMenu
    {
    };


1;
