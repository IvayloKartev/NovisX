use strict;
use warnings;
use List::Util qw(shuffle);

my $width  = 300;
my $height = 300;
my $terrain_file = 'terrain.txt';

# Read terrain from terrain.txt
open my $in, '<', $terrain_file or die "Cannot open terrain.txt: $!";
my $terrain = <$in>;
close $in;
die "Invalid terrain length!" unless length($terrain) == $width * $height;

# Convert to 2D array
my @map;
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        $map[$y][$x] = substr($terrain, $y * $width + $x, 1);
    }
}

# Gather all grass tile coordinates (value == '0')
my @grass_tiles;
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        push @grass_tiles, [$x, $y] if $map[$y][$x] eq '0';
    }
}

# Shuffle for randomness
@grass_tiles = shuffle(@grass_tiles);

# Calculate total counts based on percentages
my $total = scalar @grass_tiles;
my %counts = (
    '1' => int($total * 0.25),  # long grass
    '2' => int($total * 0.25),  # short grass
    '5' => int($total * 0.20),  # bush
    '6' => int($total * 0.20),  # tree
    '3' => int($total * 0.04),  # big stone
    '4' => int($total * 0.04),  # small stone
    '7' => int($total * 0.02),  # fireplace
);

# Assign objects to grass tiles
for my $symbol (keys %counts) {
    for (1 .. $counts{$symbol}) {
        my $tile = shift @grass_tiles or last;
        my ($x, $y) = @$tile;
        $map[$y][$x] = $symbol;
    }
}

# Write modified map back as a single line
open my $out, '>', 'objects_map.txt' or die "Cannot write to objects_map.txt: $!";
for my $row (@map) {
    print $out join('', @$row);
}
close $out;

print "Object-filled map written to objects_map.txt\n";

