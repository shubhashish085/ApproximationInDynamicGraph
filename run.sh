#! /bin/bash
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_thinkd_fd_1000_5d.txt -a 3 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_triest_fd_1000_5d.txt -a 2 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt -o ./amazon_mascot_fd_1000_5d.txt -a 1 -m 65536 -s 0.1

./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_thinkd_fd_1000_5d.txt -a 3 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_triest_fd_1000_5d.txt -a 2 -m 65536 -s 0.1
./adg.out -d /home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt -o ./dblp_mascot_fd_1000_5d.txt -a 1 -m 65536 -s 0.1
