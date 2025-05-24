for f in *.png; do convert "$f" "${f%.png}.bmp"; done
