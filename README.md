# L1-Cache
This project simulates an L1 Cache. 
To compile my program, utilize the Makefile provided.
Then, to run the program, utilize one of the ten command line inputs provided in the "commandLineInputs.txt" file.
The correct outputs for each command line input correspond with the line number in the "commandLineInputs.txt" file. For example, if you enter the command line input: "./cache 512 direct fifo 16 test.txt", which is the first line in that file, the correct output can be found in the "output1.txt" file.
The simulated cache will use each of the read and write instructions in the memory access trace, called "test.txt", to adequately represent the interworkings of an L1 Cache.
Some notes: 
1.) This is a "write through" cache, so in the case of a "write miss," I implemented it so that the block is first read from memory and then is followed by a memory write.
2.) The cache only supports two replacement policies (FIFO and LRU)
3.) This implementation increases the amount of memory reads only when there is a "cache miss," but the memory write count increases with every "W" instruction in the memory access trace 
