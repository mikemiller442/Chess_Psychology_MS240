#!/bin/bash

formatPGN() {

	sed -i.bu '/Event/d' $1
	sed -i.bu '/Site/d' $1
	sed -i.bu '/Round/d' $1
	sed -i.bu '/Date/d' $1
	sed -i.bu '/ECO/d' $1
	sed -i.bu '$d' $1
	sed -i.bu '$d' $1

}

for f in ~/Downloads/*.pgn; do	

	formatPGN $f
	dos2unix $f
	echo "Let's continue"
	./a.out $f

done



