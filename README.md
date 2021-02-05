
This program compresses and decompresses files of any size. To run program, clone repo from github. In the folder type the commands.

    "make all" - produces both encode and decode
    "make decode" - produces decode
        -uncompresses a file
    "make encode" - produces encode
        -compresses a file
    "make infer" - produces infer
        -checks for memory leaks / other bugs.

After running make type ./ + "filename" to run the program. Each program has the following optional arguements:

    -v:
        which prints file stats.
    -i:
        allocate input file as name and genrates it or through a pipeline.
    -o:
        allocate output file of compression of decompression.


Bugs:

Issue: I used open inside getopt() to parce arguements. It's possible for a resource leakage since you may end up exiting the program early upon receiving an invalid argument.
        
Fix:  save the paths to any specified input and output files during the getopt() loop, then open the files outside the loop.
    
