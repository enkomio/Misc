#!/usr/bin/perl
#
#  Copyright (C) 2008 by Antonio "s4tan" Parata - s4tan@ictsc.it
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#

use strict;
use warnings;

#########################################################
# Modify this array to set the chars to use		
#########################################################
my @alphabet = ('a','b','c','d','e','1','2','3','\'');			
#########################################################


#########################################################
# Modify this function to set the action that will be 	
# taken with the generated word				
#########################################################
sub myFunc {						
	my $string = $_[0]; # <- Don't Touch !!!!	
	# --- Start Edit ---				
	print $string."\n";				
	# --- End Edit ---				
}							
#########################################################


if ($#ARGV < 0) {
	usage();
}

# ottengo la lunghezza della password
my $passLen = shift @ARGV;

generate('');

sub generate {
	my ($curChar) = @_;
	
	if (length($curChar) eq $passLen) {
		myFunc($curChar);
	}
	else {
		for(my $i=0; $i<=$#alphabet; $i++) {
			generate($curChar.$alphabet[$i]);
		}
	}	
}

sub usage {
	print "\n usage: $0  <passlen>\n\n";
	exit 1;
}
