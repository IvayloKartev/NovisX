use strict;
use warnings;

my $width  = 300;
my $height = 300;

# Initialize map with grass (0)
my @map = map { [('0') x $width] } (1..$height);

sub set_tile {
    my ($x, $y, $val) = @_;
    return if $x < 0 || $x >= $width || $y < 0 || $y >= $height;
    $map[$y][$x] = $val;
}

# Draw a 2-tile wide path with borders
sub draw_path {
    my ($x, $y, $dir, $length) = @_;
    my ($dx, $dy) = (0, 0);

    if    ($dir eq 'right') { $dx = 1 }
    elsif ($dir eq 'left')  { $dx = -1 }
    elsif ($dir eq 'down')  { $dy = 1 }
    elsif ($dir eq 'up')    { $dy = -1 }

    for (1..$length) {
        if ($dir eq 'right' or $dir eq 'left') {
            set_tile($x, $y, '2');       # center
            set_tile($x + 1, $y, '2');   # center
            set_tile($x, $y - 1, '4');   # top
            set_tile($x + 1, $y - 1, '4');
            set_tile($x, $y + 1, '5');   # bottom
            set_tile($x + 1, $y + 1, '5');
            set_tile($x - 1, $y, '1');   # left edge
            set_tile($x + 2, $y, '3');   # right edge
        } else {
            set_tile($x, $y, '2');       # center
            set_tile($x, $y + 1, '2');   # center
            set_tile($x - 1, $y, '1');   # left
            set_tile($x - 1, $y + 1, '1');
            set_tile($x + 1, $y, '3');   # right
            set_tile($x + 1, $y + 1, '3');
            set_tile($x, $y - 1, '4');   # top
            set_tile($x, $y + 2, '5');   # bottom
        }

        $x += $dx;
        $y += $dy;
    }

    return ($x, $y, $dir);
}

# Draw corner and return updated position and direction
sub draw_corner {
    my ($x, $y, $from_dir, $to_dir) = @_;

    my %corner = (
        'right-down' => [6, 1, 1],
        'down-right' => [9, 1, 1],
        'down-left'  => [7, -1, 1],
        'left-down'  => [8, -1, 1],
        'left-up'    => [9, -1, -1],
        'up-left'    => [6, -1, -1],
        'up-right'   => [8, 1, -1],
        'right-up'   => [7, 1, -1],
    );

    my $key = "$from_dir-$to_dir";
    if (exists $corner{$key}) {
        my ($tile, $dx, $dy) = @{$corner{$key}};
        set_tile($x, $y, $tile);
        $x += $dx;
        $y += $dy;
    }

    return ($x, $y, $to_dir);
}

# Complex path
my ($x, $y, $dir) = (10, 10, 'right');
($x, $y, $dir) = draw_path($x, $y, $dir, 80);  ($x, $y, $dir) = draw_corner($x, $y, 'right', 'down');
($x, $y, $dir) = draw_path($x, $y, $dir, 50);  ($x, $y, $dir) = draw_corner($x, $y, 'down', 'left');
($x, $y, $dir) = draw_path($x, $y, $dir, 30);  ($x, $y, $dir) = draw_corner($x, $y, 'left', 'down');
($x, $y, $dir) = draw_path($x, $y, $dir, 40);  ($x, $y, $dir) = draw_corner($x, $y, 'down', 'right');
($x, $y, $dir) = draw_path($x, $y, $dir, 50);  ($x, $y, $dir) = draw_corner($x, $y, 'right', 'up');
($x, $y, $dir) = draw_path($x, $y, $dir, 20);  ($x, $y, $dir) = draw_corner($x, $y, 'up', 'right');
($x, $y, $dir) = draw_path($x, $y, $dir, 40);  ($x, $y, $dir) = draw_corner($x, $y, 'right', 'down');
($x, $y, $dir) = draw_path($x, $y, $dir, 30);  

# Write terrain file as one line
open my $fh, '>', 'terrain.txt';
for my $row (@map) {
    print $fh join('', @$row);  # No newline
}
close $fh;

print "Fixed and flattened map written to terrain.txt\n";

