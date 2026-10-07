#!/bin/sh
# Printing tests (-s -k -h -i -F -q -w): myls vs system ls -1.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out
mkdir -p $O
cd $HOME/t3 || exit 1
[ -e dlink ] || ln -s dir dlink
[ -d emptydir ] || mkdir emptydir
touch "$(printf 'ctl\001x')"
if [ ! -d hs ]; then
    mkdir hs
    for n in 0 1 9 10 999 1000 1023 1024 1025 1536 10239 10240 10241 \
        102400 1023999 1024000 1048576 1500000 10485760 1073741824; do
        dd if=/dev/null of=hs/f$n bs=1 seek=$n 2>/dev/null
    done
fi

t() {
    $M $1 > $O/m.txt 2>&1; a=$?
    ls -1 $1 > $O/l.txt 2>&1; b=$?
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $b ]; then
        echo "OK   [$1]"
    else
        echo "FAIL [$1] exit: myls=$a ls=$b"
        diff $O/m.txt $O/l.txt | head -10
    fi
}

for o in -s -sk -sh -i -F -siF -sik -sihF -sa -sA -sr -sS -sSr -q -w; do
    t "$o"
done
for b in 512 1k 2k 4k 1m; do
    BLOCKSIZE=$b; export BLOCKSIZE
    t "-s"
    echo "   ^ BLOCKSIZE=$b"
done
unset BLOCKSIZE
t "-s . dir"
t "-s dir emptydir"
t "-F dlink"
t "dlink"
t "-d dlink"
t "-Fd dlink"
t "-F dir"
t "-F slink"
t "-d dir slink"
( cd hs && t "-sh" && t "-s" && t "-skh" )
