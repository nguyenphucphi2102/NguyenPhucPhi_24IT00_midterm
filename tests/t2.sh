#!/bin/sh
# Sorting tests: myls vs system ls.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out
D=$HOME/t2
rm -rf $O $D; mkdir -p $O $D
cd $D || exit 1
echo aaaa > a; echo bb > b; echo cccccc > c; echo d > d; echo e > e
touch -t 202601010000 a
touch -t 202602010000 b
touch -t 202603010000 c
touch -t 202603010000 d
touch -t 202501010000 e
touch -a -t 202412010000 a
touch -a -t 202412050000 b
touch -a -t 202412030000 c
touch -a -t 202412020000 d
touch -a -t 202412040000 e

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

t "-t"
t "-tr"
t "-S"
t "-Sr"
t "-u"
t "-tu"
t "-tc"
t "-tuc"
t "-tcu"
t "-tS"
t "-St"
t "-f"
