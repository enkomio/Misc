#!/usr/bin/perl

use strict;
use warnings;

#########################################################
# Modify this array to set the chars to use		#
#########################################################
my @chars = ('a','b','c','d','e','f');			#
#########################################################

destroyer("",@chars); # destrooooy!!!

#########################################################
# Modify this function to set the action that will be 	#
# taken with the generated word				#
#########################################################
sub myFunc {						#
	my $string = $_[0]; # <- Don't Touch !!!!	#
	# --- Start Edit ---				#
	print $string."\n";				#
	# --- End Edit ---				#
}							#
#########################################################

### Start program functions
sub destroyer {
	my ($currStr,@charsArray) = @_;

	if(length($currStr) > $#charsArray) {
		return;
	}

	for(my $i=0; $i<=$#charsArray; $i++) {
		
		myFunc($currStr.$charsArray[$i]); # callback

		destroyer($currStr.$charsArray[$i],@charsArray);
	}
}

__END__
