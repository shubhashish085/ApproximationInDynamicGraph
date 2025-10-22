#! /bin/bash
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_thinkd_inc_1000.txt -a 3 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_triest_inc_1000.txt -a 2 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_mascot_inc_1000.txt -a 1 -m 65536 -s 0.1

./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_thinkd_inc_1000.txt -a 3 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_triest_inc_1000.txt -a 2 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_mascot_inc_1000.txt -a 1 -m 65536 -s 0.1
