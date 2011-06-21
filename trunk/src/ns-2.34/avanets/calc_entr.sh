#!/bin/bash

enviadas=$(grep "^s .*$" $1 | wc -l);
recebidas=$(grep "^r .*$" $1 | wc -l);
taxa=$(bc <<< "scale=3; ($recebidas/$enviadas)*100");

echo $enviadas $taxa >> entrega.data

