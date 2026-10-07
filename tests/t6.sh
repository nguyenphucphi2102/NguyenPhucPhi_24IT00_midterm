#!/bin/sh
# Narrow-column tests with -h on tiny files.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out; mkdir -p $O
D=$HOME/t6; rm -rf $D; mkdir -p $D; cd $D || exit 1
: > e0; echo 1 > e1; echo 22 > e2
t() {
    $M $1 > $O/m.txt 2>&1; a=$?
    ls -1 $1 > $O/l.txt 2>&1; c=$?
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $c ]; then echo "OK   [$1]"
    else echo "FAIL [$1] exit: myls=$a ls=$c"; diff $O/m.txt $O/l.txt | head -8
    fi
}
for o in -sh -lh -lsh -lhi -ls -l; do t "$o"; done
