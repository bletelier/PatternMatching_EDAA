#!/bin/bash

./build/query sources.csv 64 5 250 5 ../../../dataset/sources sources
./build/query dna.csv 64 5 250 5 ../../../dataset/dna dna
./build/query proteins.csv 64 5 250 5 ../../../dataset/proteins proteins
./build/query genome.csv 64 5 250 5 ../../../dataset/GCF_000001405.40_GRCh38.p14_genomic.fna genome
