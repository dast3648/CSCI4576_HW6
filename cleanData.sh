#!/bin/bash

sed -i 's/Average latency to send //g' $1
sed -i 's/bytes over //g' $1
sed -i 's/iterations: //g' $1
sed -i 's/microseconds //g' $1
sed -i 's/+\/- //g' $1

echo "$(tail -n +2 $1)" > $1
