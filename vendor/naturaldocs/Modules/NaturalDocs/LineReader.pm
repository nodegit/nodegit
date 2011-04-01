###############################################################################
#
#   Class: NaturalDocs::LineReader
#
###############################################################################
#
#   An object to handle reading text files line by line in a cross platform manner.  Using this class instead of the standard
#	angle brackets approach has the following benefits:
#
#	- It strips all three types of line breaks automatically: CR/LF (Windows) LF (Unix) and CR (Classic Mac).  You do not need to
#	  call chomp().  Perl's chomp() fails when parsing Windows-format line breaks on a Unix platform anyway.  It leaves the /r on,
#	  which screws everything up.
#	- It reads Classic Mac files line by line correctly, whereas the Perl version returns it all as one line.
#	- It abstracts away ignoring the Unicode BOM on the first line, if present.
#
###############################################################################

# This file is part of Natural Docs, which is Copyright © 2003-2010 Greg Valure
# Natural Docs is licensed under version 3 of the GNU Affero General Public License (AGPL)
# Refer to License.txt for the complete details

use strict;
use integer;


package NaturalDocs::LineReader;

#
#	Constants: Members
#
#	LINEREADER_FILEHANDLE - The file handle being used to read the file.  Has the LINEREADER_ prefix to make sure it doesn't
#											 conflict with any actual filehandles named FILEHANDLE in the program.
#	CACHED_LINES - An arrayref of lines already read into memory.
#
use NaturalDocs::DefineMembers 'LINEREADER_FILEHANDLE',
                                                 'CACHED_LINES';

#
#   Function: New
#
#   Creates and returns a new object.
#
#   Parameters:
#
#       filehandle - The file handle being used to read the file.
#
sub New #(filehandle)
    {
    my ($selfPackage, $filehandle) = @_;

    my $object = [ ];

    $object->[LINEREADER_FILEHANDLE] = $filehandle;
    $object->[CACHED_LINES] = [ ];

    binmode($filehandle, ':raw');

    my $possibleBOM = undef;
    read($filehandle, $possibleBOM, 2);

    if ($possibleBOM eq "\xEF\xBB")
        {
        read($filehandle, $possibleBOM, 1);
        if ($possibleBOM eq "\xBF")
            {
            seek($filehandle, 3, 0);
            binmode($filehandle, ':crlf:encoding(UTF-8)');  # Strict UTF-8, not Perl's lax version.
            }
        else
            {
            seek($filehandle, 0, 0);
            binmode($filehandle, ':crlf');
            }
        }
    elsif ($possibleBOM eq "\xFE\xFF")
        {
        seek($filehandle, 2, 0);
        binmode($filehandle, ':crlf:encoding(UTF-16BE)');
        }
    elsif ($possibleBOM eq "\xFF\xFE")
        {
        seek($filehandle, 2, 0);
        binmode($filehandle, ':crlf:encoding(UTF-16LE)');
        }
    else
        {
        seek($filehandle, 0, 0);
        binmode($filehandle, ':crlf');
        }

    bless $object, $selfPackage;
    return $object;
    };


#
#   Function: Chomp
#
#   Removes any line breaks from the end of a value.  It does not remove any that are in the middle of it.
#
#   Parameters:
#
#       lineRef - A *reference* to the line to chomp.
#
sub Chomp #(lineRef)
    {
    my ($self, $lineRef) = @_;
    $$lineRef =~ s/(?:\r\n|\r|\n)$//;
    };


#
#	Function: Get
#
#	Returns the next line of text from the file, or undef if there are no more.  The line break will be removed automatically.  If
#	the first line contains a Unicode BOM, that will also be removed automatically.
#
sub Get
	{
	my $self = shift;
	my $line = undef;

	if (scalar @{$self->[CACHED_LINES]} == 0)
		{
		my $filehandle = $self->[LINEREADER_FILEHANDLE];
		my $rawLine = <$filehandle>;

		if (!defined $rawLine)
			{  return undef;  }

		$self->Chomp(\$rawLine);

        if ($rawLine =~ /\r/)
        	{
	  		push @{$self->[CACHED_LINES]}, split(/\r/, $rawLine);  # Split for Classic Mac
			$line = shift @{$self->[CACHED_LINES]};
          	}
        else
        	{  $line = $rawLine;  }
		}
	else
		{  $line = shift @{$self->[CACHED_LINES]};  }

	return $line;
	}


#
#	Function: GetAll
#
#	Returns an array of all the lines from the file.  The line breaks will be removed automatically.  If the first line contains a
#	Unicode BOM, that will also be removed automatically.
#
sub GetAll
	{
	my $self = shift;

	my $filehandle = $self->[LINEREADER_FILEHANDLE];
	my $rawContent;

    read($filehandle, $rawContent, -s $filehandle);

    return split(/\r\n|\n|\r/, $rawContent);
	}

1;
