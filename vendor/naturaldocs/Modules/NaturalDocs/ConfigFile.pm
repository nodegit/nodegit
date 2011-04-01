###############################################################################
#
#   Package: NaturalDocs::ConfigFile
#
###############################################################################
#
#   A package to manage Natural Docs' configuration files.
#
#   Usage:
#
#       - Only one configuration file can be managed with this package at a time.  You must close the file before opening another
#         one.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;

package NaturalDocs::ConfigFile;



#
#   Topic: Format
#
#   All configuration files are text files.
#
#   > # [comment]
#
#   Comments start with the # character.
#
#   > Format: [version]
#
#   All configuration files *must* have a format line as its first line containing content.  Whitespace and comments are permitted
#   ahead of it.
#
#   > [keyword]: [value]
#
#   Keywords can only contain <CFChars>.  Keywords are not case sensitive.  Values can be anything and run until the end of
#   the line or a comment.
#
#   > [value]
#
#   Lines that don't start with a valid keyword format are considered to be all value.
#
#   > [line] { [line] } [line]
#
#   Files supporting brace groups (specified in <Open()>) may also have braces that can appear anywhere.  It allows more than
#   one thing to appear per line, which isn't supported otherwise.  Consequently, values may not have braces.
#


#
#   Type: CFChars
#
#   The characters that can appear in configuration file keywords and user-defined element names: letters, numbers, spaces,
#   dashes, slashes, apostrophes, and periods.
#
#   Although the list above is exhaustive, it should be noted that you especially can *not* use colons (messes up keyword: value
#   sequences) commas (messes up item, item, item list sequences) and hashes (messes up comment detection.)
#
#   You can search the source code for [CFChars] to find all the instances where this definition is used.
#


###############################################################################
# Group: Variables

#
#   handle: CONFIG_FILEHANDLE
#
#   The file handle used for the configuration file.
#


#
#   string: file
#
#   The <FileName> for the current configuration file being parsed.
#
my $file;


#
#	var: lineReader
#
#	The <LineReader> used to read the configuration file.
#
my $lineReader;


#
#   array: errors
#
#   An array of errors added by <AddError()>.  Every odd entry is the line number, and every even entry following is the
#   error message.
#
my @errors;


#
#   var: lineNumber
#
#   The current line number for the configuration file.
#
my $lineNumber;


#
#   bool: hasBraceGroups
#
#   Whether the file has brace groups or not.
#
my $hasBraceGroups;


#
#   array: virtualLines
#
#   An array of virtual lines if a line from the file contained more than one.
#
#   Files with brace groups may have more than one virtual line per actual file line, such as "Group: A { Group: B".  When that
#   happens, any extra virtual lines are put into here so they can be returned on the next call.
#
my @virtualLines;



###############################################################################
# Group: Functions


#
#   Function: Open
#
#   Opens a configuration file for parsing and returns the format <VersionInt>.
#
#   Parameters:
#
#       file - The <FileName> to parse.
#       hasBraceGroups - Whether the file supports brace groups or not.  If so, lines with braces will be split apart behind the
#                                  scenes.
#
#   Returns:
#
#       The <VersionInt> of the file, or undef if the file doesn't exist.
#
sub Open #(file, hasBraceGroups)
    {
    my $self;
    ($self, $file, $hasBraceGroups) = @_;

    @errors = ( );

    # It will be incremented to one when the first line is read from the file.
    $lineNumber = 0;

    open(CONFIG_FILEHANDLE, '<' . $file) or return undef;
    $lineReader = NaturalDocs::LineReader->New(\*CONFIG_FILEHANDLE);


    # Get the format line.

    my ($keyword, $value, $comment) = $self->GetLine();

    if ($keyword eq 'format')
        {  return NaturalDocs::Version->FromString($value);  }
    else
        {  die "The first content line in " . $file . " must be the Format: line.\n";  };
    };


#
#   Function: Close
#
#   Closes the current configuration file.
#
sub Close
    {
    my $self = shift;
    close(CONFIG_FILEHANDLE);
    };


#
#   Function: GetLine
#
#   Returns the next line containing content, or an empty array if none.
#
#   Returns:
#
#       Returns the array ( keyword, value, comment ), or an empty array if none.  All tabs will be converted to spaces, and all
#       whitespace will be condensed into a single space.
#
#       keyword - The keyword part of the line, if any.  Is converted to lowercase and doesn't include the colon.  If the file supports
#                       brace groups, opening and closing braces will be returned as keywords.
#       value - The value part of the line, minus any whitespace.  Keeps its original case.
#       comment - The comment following the line, if any.  This includes the # symbol and a leading space if there was
#                       any whitespace, since it may be significant.  Otherwise undef.  Used for lines where the # character needs to be
#                       accepted as part of the value.
#
sub GetLine
    {
    my $self = shift;

    my ($line, $comment);


    # Get the next line with content.

    do
        {
        # Get the next line.

        my $isFileLine;

        if (scalar @virtualLines)
            {
            $line = shift @virtualLines;
            $isFileLine = 0;
            }
        else
            {
            $line = $lineReader->Get();
            $lineNumber++;

            if (!defined $line)
                {  return ( );  };

            # Condense spaces and tabs into a single space.
            $line =~ tr/\t /  /s;
            $isFileLine = 1;
            };


        # Split off the comment.

        if ($line =~ /^(.*?)( ?#.*)$/)
            {  ($line, $comment) = ($1, $2);  }
        else
            {  $comment = undef;  };


        # Split any brace groups.

        if ($isFileLine && $hasBraceGroups && $line =~ /[\{\}]/)
            {
            ($line, @virtualLines) = split(/([\{\}])/, $line);

            $virtualLines[-1] .= $comment;
            $comment = undef;
            };


        # Remove whitespace.

        $line =~ s/^ //;
        $line =~ s/ $//;
        $comment =~ s/ $//;
        # We want to keep the leading space on a comment.
        }
    while (!$line);


    # Process the line.

    if ($hasBraceGroups && ($line eq '{' || $line eq '}'))
        {
        return ($line, undef, undef);
        };


    if ($line =~ /^([a-z0-9\ \'\/\.\-]+?) ?: ?(.*)$/i) # [CFChars]
        {
        my ($keyword, $value) = ($1, $2);
        return (lc($keyword), $value, $comment);
        }

    else
        {
        return (undef, $line, $comment);
        };
    };


#
#   Function: LineNumber
#
#   Returns the line number for the line last returned by <GetLine()>.
#
sub LineNumber
    {  return $lineNumber;  };



###############################################################################
# Group: Error Functions


#
#   Function: AddError
#
#   Stores an error for the current configuration file.  Will be attached to the last line read by <GetLine()>.
#
#   Parameters:
#
#       message - The error message.
#       lineNumber - The line number to use.  If not specified, it will use the line number from the last call to <GetLine()>.
#
sub AddError #(message, lineNumber)
    {
    my ($self, $message, $messageLineNumber) = @_;

    if (!defined $messageLineNumber)
        {  $messageLineNumber = $lineNumber;  };

    push @errors, $messageLineNumber, $message;
    };


#
#   Function: ErrorCount
#
#   Returns how many errors the configuration file has.
#
sub ErrorCount
    {
    return (scalar @errors) / 2;
    };


#
#   Function: PrintErrorsAndAnnotateFile
#
#   Prints the errors to STDERR in the standard GNU format and annotates the configuration file with them.  It does *not* end
#   execution.  <Close()> *must* be called before this function.
#
sub PrintErrorsAndAnnotateFile
    {
    my ($self) = @_;

    if (scalar @errors)
        {
        open(CONFIG_FILEHANDLE, '<' . $file);

        my $lineReader = NaturalDocs::LineReader->New(\*CONFIG_FILEHANDLE);
        my @lines = $lineReader->GetAll();

        close(CONFIG_FILEHANDLE);

        # We need to keep track of both the real and the original line numbers.  The original line numbers are for matching errors in
        # the errors array, and don't include any comment lines added or deleted.  Line number is the current line number including
        # those comment lines for sending to the display.
        my $lineNumber = 1;
        my $originalLineNumber = 1;

        open(CONFIG_FILEHANDLE, '>' . $file);

        # We don't want to keep the old error header, if present.
        if ($lines[0] =~ /^\# There (?:is an error|are \d+ errors) in this file\./)
            {
            shift @lines;
            $originalLineNumber++;

            # We want to drop the blank line after it as well.
            if ($lines[0] eq "\n")
                {
                shift @lines;
                $originalLineNumber++;
                };
            };

        if ($self->ErrorCount() == 1)
            {
            print CONFIG_FILEHANDLE
            "# There is an error in this file.  Search for ERROR to find it.\n\n";
            }
        else
            {
            print CONFIG_FILEHANDLE
            "# There are " . $self->ErrorCount() . " errors in this file.  Search for ERROR to find them.\n\n";
            };

        $lineNumber += 2;


        foreach my $line (@lines)
            {
            while (scalar @errors && $originalLineNumber == $errors[0])
                {
                my $errorLine = shift @errors;
                my $errorMessage = shift @errors;

                print CONFIG_FILEHANDLE "# ERROR: " . $errorMessage . "\n";

                # Use the GNU error format, which should make it easier to handle errors when Natural Docs is part of a build process.
                # See http://www.gnu.org/prep/standards_15.html

                $errorMessage = lcfirst($errorMessage);
                $errorMessage =~ s/\.$//;

                print STDERR 'NaturalDocs:' . $file . ':' . $lineNumber . ': ' . $errorMessage . "\n";

                $lineNumber++;
                };

            # We want to remove error lines from previous runs.
            if (substr($line, 0, 9) ne '# ERROR: ')
                {
                print CONFIG_FILEHANDLE $line;
                $lineNumber++;
                };

            $originalLineNumber++;
            };

        # Clean up any remaining errors.
        while (scalar @errors)
            {
            my $errorLine = shift @errors;
            my $errorMessage = shift @errors;

            print CONFIG_FILEHANDLE "# ERROR: " . $errorMessage . "\n";

            # Use the GNU error format, which should make it easier to handle errors when Natural Docs is part of a build process.
            # See http://www.gnu.org/prep/standards_15.html

            $errorMessage = lcfirst($errorMessage);
            $errorMessage =~ s/\.$//;

            print STDERR 'NaturalDocs:' . $file . ':' . $lineNumber . ': ' . $errorMessage . "\n";
            };

        close(CONFIG_FILEHANDLE);
        };
    };



###############################################################################
# Group: Misc Functions


#
#   Function: HasOnlyCFChars
#
#   Returns whether the passed string contains only <CFChars>.
#
sub HasOnlyCFChars #(string)
    {
    my ($self, $string) = @_;
    return ($string =~ /^[a-z0-9\ \.\-\/\']*$/i);  # [CFChars]
    };


#
#   Function: CFCharNames
#
#   Returns a plain-english list of <CFChars> which can be embedded in a sentence.  For example, "You can only use
#   [CFCharsList()] in the name.
#
sub CFCharNames
    {
    # [CFChars]
    return 'letters, numbers, spaces, periods, dashes, slashes, and apostrophes';
    };


#
#   Function: Obscure
#
#   Obscures the passed text so that it is not user editable and returns it.  The encoding method is not secure; it is just designed
#   to be fast and to discourage user editing.
#
sub Obscure #(text)
    {
    my ($self, $text) = @_;

    # ` is specifically chosen to encode to space because of its rarity.  We don't want a trailing one to get cut off before decoding.
    $text =~ tr{a-zA-Z0-9\ \\\/\.\:\_\-\`}
                    {pY9fGc\`R8lAoE\\uIdH6tN\/7sQjKx0B5mW\.vZ41PyFg\:CrLaO\_eUi2DhT\-nSqJkXb3MwVz\ };

    return $text;
    };


#
#   Function: Unobscure
#
#   Restores text encoded with <Obscure()> and returns it.
#
sub Unobscure #(text)
    {
    my ($self, $text) = @_;

    $text =~ tr{pY9fGc\`R8lAoE\\uIdH6tN\/7sQjKx0B5mW\.vZ41PyFg\:CrLaO\_eUi2DhT\-nSqJkXb3MwVz\ }
                    {a-zA-Z0-9\ \\\/\.\:\_\-\`};

    return $text;
    };



1;
