use strict;
use warnings;
use List::Util qw(shuffle);

my $width  = 300;
my $height = 300;
my $terrain_file = 'terrain.txt';

# Read terrain
open my $in, '<', $terrain_file or die "Cannot open terrain.txt: $!";
my $terrain = <$in>;
close $in;
die "Invalid terrain length!" unless length($terrain) == $width * $height;

# Convert to 2D map
my @map;
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        $map[$y][$x] = substr($terrain, $y * $width + $x, 1);
    }
}

# Gather all grass (0) tiles
my @grass_tiles;
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        push @grass_tiles, [$x, $y] if $map[$y][$x] eq '0';
    }
}

# Reduce to 50% of available grass tiles
@grass_tiles = shuffle(@grass_tiles);
my $max_coverage = int(@grass_tiles * 0.5);
@grass_tiles = @grass_tiles[0 .. $max_coverage - 1];

# Calculate object counts
my %percent = (
    '1' => 0.25,  # long grass
    '2' => 0.25,  # short grass
    '5' => 0.20,  # bush
    '6' => 0.20,  # tree
    '3' => 0.04,  # big stone
    '4' => 0.04,  # small stone
    '7' => 0.02,  # fireplace
);

my %counts;
my $assigned = 0;
for my $symbol (sort { $percent{$b} <=> $percent{$a} } keys %percent) {
    $counts{$symbol} = int($max_coverage * $percent{$symbol});
    $assigned += $counts{$symbol};
}

$counts{'1'} += $max_coverage - $assigned;

for my $symbol (keys %counts) {
    for (1 .. $counts{$symbol}) {
        my $tile = shift @grass_tiles or last;
        my ($x, $y) = @$tile;
        $map[$y][$x] = $symbol;
    }
}

open my $out, '>', 'objects_map.txt' or die "Cannot write to objects_map.txt: $!";
for my $row (@map) {
    print $out join('', @$row); 
}
close $out;

print "Objects placed on 50% of grass tiles. Output written to objects_map.txt\n";

