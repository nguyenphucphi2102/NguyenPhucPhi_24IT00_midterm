#!/bin/sh
# Compare myls with the system ls; outputs go to ~/out, not the test dir.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out
rm -rf $O; mkdir -p $O
cd $HOME/testdir || exit 1
rm -f m*.txt l*.txt

t() {
    $M $1 > $O/m.txt 2>&1; a=$?
    ls -1 $1 > $O/l.txt 2>&1; b=$?
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $b ]; then
        echo "OK   [$1]"
    else
        echo "FAIL [$1] exit: myls=$a ls=$b"
        diff $O/m.txt $O/l.txt
    fi
}

t ""
t "-a"
t "-A"
t "-r"
t "a sub b"
t "-d sub"
t "nonexist"
t "a nonexist sub"
