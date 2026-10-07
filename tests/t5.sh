#!/bin/sh
# Recursion tests: myls vs system ls (stdout + exit status only).
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out; mkdir -p $O
D=$HOME/t5; chmod -R u+rwx $D 2>/dev/null; rm -rf $D
mkdir -p $D; cd $D || exit 1
mkdir -p a/b/c a/d e/.hid f emptyd noperm
echo 1 > a/f1; echo 22 > a/b/f2; echo 333 > a/b/c/f3
echo x > e/.hid/h1; echo y > e/.dot; echo z > top
ln -s a la; ln -s nothing broken; ln -s ../a e/up
echo p > noperm/p; chmod 000 noperm

t() {
    $M $1 > $O/m.txt 2>/dev/null; a=$?
    ls -1 $1 > $O/l.txt 2>/dev/null; c=$?
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $c ]; then echo "OK   [$1]"
    else echo "FAIL [$1] exit: myls=$a ls=$c"; diff $O/m.txt $O/l.txt | head -12
    fi
}
for o in -R -Ra -RA -Rr -Rt -Rs -RF -Rl -Rlh -Rsi -Rf -Rd -RS -Rq; do t "$o"; done
t "-R a"
t "-R a e"
t "-R top a"
t "-R top"
t "-R la"
t "-Rl la"
t "-R a/"
t "-R a//b"
t "-R nonexist a"
t "-R noperm"
t "-R . noperm"
t "-d a e"
t "-R emptyd"
t "-Rl emptyd a"
