#!/usr/bin/perl

# LTLcounterCarry.pl
#
# Input: n = the number of bits in the counter
# 
# Output: an LTL formula describing an n-bit counter
#
# Purpose: Simplify the formula created by LTLcounter by eliminating the 'until' operators and using a carry bit (one more variable) instead. 
#
# Usage: LTLcounterCarry.pl n
#

# System Description
#
# - 3 variables: 
#                m = marker
#                b = bits
#                c = carries
#
# - the counter represents a sequence of n-bit strings in 3 variables
#   ex: n=4
#       m = 0001 0001 0001 0001 0001 0001 0001 0001 0001 0001 0001 0001 0001 0001
#       b = 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101
#       c = 0000 0001 0000 0011 0000 0001 0000 0111 0000 0001 0000 0011 0000 0001
#       m marks the least-significant bit
#       b is the sequence of bit-strings representing the counter
#       c is the sequence of bit-strings representing the carries from each addition
#         ex: 0011 + 0001 = 0100 with carries 0011
#
# - reverse bit order: in our actual LTL formula, we represent each n-length string 
#     in reverse order (left -> right : least -> most significant bit)
#   - the example above for n=4 becomes:
#       m = 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000
#       b = 0000 1000 0100 1100 0010 1010 0110 1110 0001 1001 0101 1101 0011 1011
#       c = 0000 1000 0000 1100 0000 1000 0000 1110 0000 1000 0000 1100 0000 1000
#
# - the LTL formula produced is true if the values of the three variables over time 
#     represent a proper n-bit counter



#################### Argument Setup ####################

#Check for correct number and type of command line arguments
if ($ARGV[0] =~ /^-v?/) {
    $verbose = 1;
    shift(@ARGV);
} #end if
else {
    $verbose = 0;
} #end else

if (@ARGV != 1) {
    die "Usage: LTLcounterCarry.pl n\n\tproduces a formula describing an n-bit counter\n\tUse flag -v for verbose, human-readable output.\n";
} #end if

$n = $ARGV[0];

#################### Generation of the Formula ####################
$pattern = ""; #make sure the pattern starts empty
$ppattern = ""; #make sure the printable pattern starts empty

#Here are the parts of the formula we know to be true:

#1) The marker consists of a repeated pattern of a 1 followed by n-1 0's.
$mpattern .= "(m) && ( [](m -> (";
for ($i = 1; $i <= $n; $i++) {
    if ($i == $n) { $nest = "m"; }
    else { $nest = "!m"; }
    for ($j = 0; $j < $i; $j++) {
	$nest = "X($nest)";
    } #end for
    if ($i == $n) { $mpattern .= "($nest))))"; }
    else { $mpattern .= "($nest) && ";}
} #end for

$ppattern .= "1. $mpattern\n"; #just for now: add a return for readability
$pattern .= "($mpattern) && ";


#2) The bit is initially n 0's.
$bpattern = "(!b)";
for ($i = 1; $i < $n; $i++) {
    $nest = "!b";
    for ($j = 0; $j < $i; $j++) {
	$nest = "X($nest)";
    } #end for
    $bpattern .= " && ($nest)";
} #end for
$ppattern .= "2. $bpattern\n"; #just for now: add a return for readability
$pattern .= "($bpattern) && ";


#Establish strings for "n steps from now, b is 0 (or 1)"
$nestb0 = "!b";
$nestb1 = "b";
for ($i = 1; $i <= $n; $i++) {
    $nestb0 = "X($nestb0)";
    $nestb1 = "X($nestb1)";
} #end for


### Add m to b ###

#3) If m is 1 and b is 0 then c is 0 and n steps later b is 1.
$ppattern .= "3. [] ( (m && !b) -> (!c && $nestb1) )\n";
$pattern .= "([] ((m && (!b)) -> ((!c) && ($nestb1)))) && ";


#4) If m is 1 and b is 1 then c is 1 and n steps later b is 0.
$ppattern .= "4. [] ( (m && b) -> (c && $nestb0) )\n";
$pattern .= "([] ((m && b) -> (c && ($nestb0)))) && ";


### Add in the carry ###

#5) If there's no carry, then all of the bits stay the same n steps later.
$ppattern .= "5. [] (!c && X(!m)) -> ( X(!c) && (X(b) -> X($nestb1)) && (X(!b) -> X($nestb0)) )\n";
$pattern .= "([] ( ((!c) && X(!m)) -> ( X(!c) && (X(b) -> (X($nestb1))) && (X(!b) -> (X($nestb0)) ) ) )) && ";


#6) If there's a carry, then add one: flip the bits of b and adjust the carry.
$ppattern .= "6. [] (c -> ( ( X(!b) -> ( X(!c) && X($nestb0) ) ) && ( X(c) && X($nestb1) ) ))\n";
$pattern .= "([] ( c -> ( ( X(!b) -> ( X(!c) && (X($nestb1)) ) ) && ( X(b) -> ( X(c) && (X($nestb0)) ) ) )))";


if ($verbose == 1) {
    print "1) The marker consists of a repeated pattern of a 1 followed by n-1 0's.\n";
    print "2) The bit is initially n 0's.\n";
    print "3) If m is 1 and b is 0 then c is 0 and n steps later b is 1.\n";
    print "4) If m is 1 and b is 1 then c is 1 and n steps later b is 0.\n";
#    print "5) If there's no carry, then all of the bits stay the same n steps later.\n";
    print "5) If there is no carry, then the next bit stays the same n steps later.\n";
#    print "6) If there's a carry, then add one: flip the bits of b and adjust the carry.\n";
    print "6) If there is a carry, flip the next bit n steps later and adjust the carry.\n";
    print "\nPattern: \n$ppattern\n";
} #end if

$pattern =~ s/\[\]/G/g;
$pattern =~ s/m/a/g;

if ($verbose == 1) {
    print "\nComputer readable: ";
} #end if
print "($pattern)\n";
