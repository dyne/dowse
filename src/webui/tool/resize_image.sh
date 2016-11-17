F=$1 ;
for R in 16x16 32x32 64x64 ; do
 convert $F -resize $R ${R}/$F-${R}.png
done
