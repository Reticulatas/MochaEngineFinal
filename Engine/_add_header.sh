#!/bin/bash
for f in *.h *.cpp *.c;
do
	echo "Processing $f file..."
	echo '/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */

'	| cat - $f > temp && mv temp $f
done	
