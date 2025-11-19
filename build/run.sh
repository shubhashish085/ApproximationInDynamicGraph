#! /bin/bash
./adg.out -d ./com-orkut_stm_5fd_l.ungraph.txt -o ./orkut_thinkd_fd_1000_5fd_l.txt -a 3 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_5fd_l.ungraph.txt -o ./orkut_triest_fd_1000_5fd_l.txt -a 2 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_5fd_l.ungraph.txt -o ./orkut_mascot_fd_1000_5fd_l.txt -a 1 -m 65536 -s 0.1

./adg.out -d ./com-orkut_stm_10fd_l.ungraph.txt -o ./orkut_thinkd_fd_1000_10fd_l.txt -a 3 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_10fd_l.ungraph.txt -o ./orkut_triest_fd_1000_10fd_l.txt -a 2 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_10fd_l.ungraph.txt -o ./orkut_mascot_fd_1000_10fd_l.txt -a 1 -m 65536 -s 0.1

./adg.out -d ./com-orkut_stm_15fd_l.ungraph.txt -o ./orkut_thinkd_fd_1000_15fd_l.txt -a 3 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_15fd_l.ungraph.txt -o ./orkut_triest_fd_1000_15fd_l.txt -a 2 -m 65536 -s 0.1
./adg.out -d ./com-orkut_stm_15fd_l.ungraph.txt -o ./orkut_mascot_fd_1000_15fd_l.txt -a 1 -m 65536 -s 0.1