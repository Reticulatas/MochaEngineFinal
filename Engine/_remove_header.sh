#!/bin/bash
for f in *.h *.c *.cpp;
do
	echo "Processing $f file..."
	for i in 'seq 1 6'; do
		tail -n +2 "$f" | cat > $f
	done
done	
