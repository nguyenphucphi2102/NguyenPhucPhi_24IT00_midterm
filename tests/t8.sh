#!/bin/sh
# Edge cases: myls vs system ls (stdout + exit status), plus crash check.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out; mkdir -p $O
D=$HOME/t8; chmod -R u+rwx $D 2>/dev/null; rm -rf $D
mkdir -p $D; cd $D || exit 1
: > "with space"; : > "$(printf 'tab\tin')"; : > "$(printf 'new\nline')"
: > "-dash"; : > "--"; : > "a'quote"; : > 'dq"x'
: > "$(printf 'ctl\001x')"; : > "$(printf 'hi\351x')"
: > .hid; mkdir -p sub/.hid2 nop; : > sub/z; chmod 000 nop
mkdir many; i=0; while [ $i -lt 2000 ]; do : > many/f$i; i=$((i+1)); done
p=x; i=0; while [ $i -lt 40 ]; do p=$p/deepdir; i=$((i+1)); done
mkdir -p $p; : > $p/leaf
long=$(printf 'n%.0s' $(seq 1 250)); : > "$long"

t() {
    $M "$@" > $O/m.txt 2>/dev/null; a=$?
    ls -1 "$@" > $O/l.txt 2>/dev/null; c=$?
    if [ $a -ge 128 ]; then echo "CRASH [$*] exit=$a"; return; fi
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $c ]; then echo "OK   [$*]"
    else echo "FAIL [$*] exit: myls=$a ls=$c"; diff $O/m.txt $O/l.txt | head -8
    fi
}
t
t -a; t -A; t -q; t -w; t -F; t -l; t -ln; t -lh; t -s; t -si
t -R sub; t -Ra sub; t -d sub .; t -d .
t -- -dash; t -- --; t -l -- -dash
t ""; t -d ""; t sub ""; t nop; t -R nop; t -l nop
t many; t -R many; t -ltr many; t -S many; t -f many
t -R x; t -l x; t -d x/deepdir
t "$long"; t -l "$long"; t -d "with space" "$(printf 'tab\tin')"
t /dev; t -l /dev; t -R /etc; t -l /etc /usr/bin
t /nonexistent /also/missing; t sub /nonexistent
t -lR /usr/include
t -z; t -l -z
for o in -lt -ltr -lS -lSr -lu -lc -ltu -ltc -lh -lk -ls -lsh -lf -lfr \
    -dl -dF -dlF -Fi -sS -sSr -sR -AF -aF -lAF -ldh; do t $o; t $o sub x; done
