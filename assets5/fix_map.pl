use strict;
use warnings;

my $width  = 300;
my $height = 300;

# Read terrain.txt
open my $in, '<', 'terrain.txt' or die "Can't open terrain.txt: $!";
my $line = <$in>;
close $in;

die "Invalid terrain size" unless length($line) == $width * $height;

my @map;
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        $map[$y][$x] = substr($line, $y * $width + $x, 1);
    }
}

# Check if tile is non-grass
sub is_path {
    my ($x, $y) = @_;
    return 0 if $x < 0 || $x >= $width || $y < 0 || $y >= $height;
    return $map[$y][$x] ne '0';
}

# Fix the map
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        next if $map[$y][$x] eq '0';  # skip grass

        my $top    = is_path($x, $y - 1);
        my $bottom = is_path($x, $y + 1);
        my $left   = is_path($x - 1, $y);
        my $right  = is_path($x + 1, $y);

        # Fully surrounded
        if ($top && $bottom && $left && $right) {
            $map[$y][$x] = '2';
        }
        # Corners
        elsif ($left && $bottom && !$top && !$right) {
            $map[$y][$x] = '6';  # top-right corner
        }
        elsif ($right && $bottom && !$top && !$left) {
            $map[$y][$x] = '9';  # top-left corner
        }
        elsif ($left && $top && !$bottom && !$right) {
            $map[$y][$x] = '7';  # bottom-right corner
        }
        elsif ($right && $top && !$bottom && !$left) {
            $map[$y][$x] = '8';  # bottom-left corner
        }
    }
}

# Flatten and save to file
open my $out, '>', 'terrain.txt' or die "Can't write to terrain.txt: $!";
for my $y (0 .. $height - 1) {
    for my $x (0 .. $width - 1) {
        print $out $map[$y][$x];
    }
}
close $out;

print "Map fixed and written back to terrain.txt\n";

