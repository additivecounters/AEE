### Repository for: 

## "Faster and More Accurate Measurement through Additive-Error Counters"


#### To compile the code, run Make in the terminal.


#### This code includes tests and implementation of several Sketch and Cache-based measurement algorithms with regular counters as well as with AEE estimators. Among them are the implementation of:


1. A single AEE estimator.

2. Count Min Sketch (Baseline, AEE MaxACCURACY, AEE MaxSpeed).

3. Conservative Update Sketch (Baseline, AEE MaxACCURACY).

4. Space Saving (Baseline Heap, Baseline, AEE MaxACCURACY).

5. RAP (Baseline using a heap, Baseline, AEE MaxACCURACY, AEE MaxACCURACY using a heap).

6. Dway-RAP (Baseline, AEE MaxACCURACY).


#### The Space Saving and RAP heap implementations (Baseline and AEE) are based on the code by Graham Cormode (see Methods for finding frequent items in data streams, VLDB J, 2010; http://hadjieleftheriou.com/frequent-items/).

#### To run the tests, there is a need for a packet trace as follows:

1. Unweighted tests: a binary sequence of 13 bytes (packet 5-tuples). 

2. Weighted tests: a binary sequence of 15 bytes (packet 5-tuples + 2 bytes specifying the packet length). 


#### Each test produces text files with reported speed and ACCURACY results. The arguments that the  main function receives are as follows:

    "argv[1]: int N" - the length of the trace.
    
    "argv[2]: int Seed" - a seed for the random number generators.
    
    "argv[3]: int Alg" - the algorithm(s) to run (see main function).
    
    "argv[4]: bool weighted_experiment" - is the experiment is weighted or not (should be aligned with the trace).
    
    "argv[5]: String Trace" - a path to the used packet trace.
    
