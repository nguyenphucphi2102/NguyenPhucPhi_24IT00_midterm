#!/bin/sh
# Long-format tests: myls vs system ls.
M=$HOME/NguyenPhucPhi_24IT00_midterm/myls
O=$HOME/out; mkdir -p $O
D=$HOME/t4; rm -rf $D; mkdir -p $D; cd $D || exit 1
now=$(date +%s)
mk() { : > $1; touch -t "$(date -r $(( now - $2 )) +%Y%m%d%H%M.%S)" $1; }
mk recent 3600; mk in182 15721200; mk out182 15728400
mk d400 34560000; mk fut10 -864000
mk fut_in -15721200; mk fut_out -15728400
for n in s1 s2 s3 s4 t1 t2 z0; do : > $n; done
chmod 4755 s1; chmod 4644 s2; chmod 2755 s3; chmod 2644 s4
chmod 1755 t1; chmod 1644 t2; chmod 000 z0
echo hello > f1; mkdir sub emptyd; touch sub/x
ln -s f1 l1; ln -s nothing broken; ln -s sub ls1; mkfifo ff
for i in 1 2 3 4 5 6 7 8 9 10 11; do ln f1 hl$i; done
dd if=/dev/null of=big9 bs=1 seek=123456789 2>/dev/null

t() {
    $M $1 > $O/m.txt 2>&1; a=$?
    ls -1 $1 > $O/l.txt 2>&1; c=$?
    if cmp -s $O/m.txt $O/l.txt && [ $a -eq $c ]; then echo "OK   [$1]"
    else echo "FAIL [$1] exit: myls=$a ls=$c"; diff $O/m.txt $O/l.txt | head -12
    fi
}
for o in -l -ln -la -lA -lh -lk -lF -ls -lsi -lsih -lt -lS -lu -lc -ltu \
    -ltc -lr -lf -lq -lw; do t "$o"; done
t "-ld sub l1 broken ls1"
t "-l l1 f1 sub"
t "-l ls1"
t "-lF ls1"
t "-l emptyd"
t "-l emptyd sub"
t "-l /dev/null /dev/zero /etc/passwd /bin/ls"
t "-ln /dev/null /dev/zero"
t "-lh /bin/ls /etc/passwd"
t "-ld /tmp /etc"
BLOCKSIZE=1k; export BLOCKSIZE; t "-l"; t "-ls"; unset BLOCKSIZE
