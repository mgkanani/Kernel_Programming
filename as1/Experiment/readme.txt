After compiling module, started scripts named testing_* and loaded module at same time.
After complition of all 3 scripts, module unloaded.
log generated and copied into log.txt.

log_1.txt -> output of testing_1.sh
log_2.txt -> output of testing_2.sh
log_3.txt -> output of testing_3.sh
inode.txt -> output of ls -li scripts/ [for inode to file mappings]
table.txt -> used for generating plot.
plot.gnu  -> gnu script for generating plot from table.txt
page_hits.eps -> graph generated from this experiment.

dd commands with bs(blocksize) of 16,8 and 4 bytes are used for reading and writting files.[see testing.sh files for more details]

Mappings:-
filename[size]			inode	cnt
scripts/rfile_1[81MB]		12254	19532
scripts/rfile_2[40MB]		12252	9766
scripts/rfile_3[20MB]		12251	4883
scripts/wfile_1[77MB]		12268	4980468
scripts/wfile_2[39MB]		12267	4990234
scripts/wfile_3[20MB]		12266	4995177

Specifications:-
1GB RAM, 2cpus , virtual m/c, none process were executing except these scripts.
