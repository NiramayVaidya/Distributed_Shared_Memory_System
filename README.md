**Note-** My partner's implementation (Aman's git repository) has been added as 
a git submodule in my repository, so in order to get his code in my repo,
execute this after doing 'git clone'-

```
git submodule update --init --recursive
```

For the sequential consistency and dist sorting dsm based implementation 
(without malloc), refer to Aman's repo (works more reliably than mine).
</br>
For the dsm based implementation specific to malloc (just for testing the basic 
test that has been added), word count and k-means, refer to my repo.

### Summary

#### Distributed mutual locks-

The mutex folder is for the mutual exclusion portion of the implementation.
</br>
The single_system implementation is for running multiple nodes with different
ports on a single system, as opposed to running multiple nodes on different
systems with the same port on all these systems, which is contained in the
multiple_systems implementation
</br>
In the application based mutual exclusion implementation, according to the
updated version which supports dealing with multiple locks, psu_init_lock for 
all locknos shared between all the nodes must be done at each node at the start
of the application for distributed locking to work properly i.e. be functionally
correct. This means that the application needs to have knowledge of how many 
locks it would need throughout its execution beforehand.

#### Distributed shared memory system-

The two test application codes are in the seq_consistency and dist_sorting
folders. Within the dist_sorting folder, the dist_sorting_generated_files
contains the actual output file and the terminal output (because of DEBUG mode
on) obtained for all the running hosts in case of a successful execution.
</br>
The dsm malloc implementation is in a separate folder called 
seq_consistency_with_malloc, containing another subfolder having a file which
has the terminal output of a successful execution (similar to what has been
explained above).

#### Map reduce framework-

The implementation is in the word_count folder. Within this folder, the
word_count_generated_files contains the correct intermediate and final output, 
along with the terminal output obtained for a successful execution.

#### K-means on map reduce-

The code is in the k_means folder. Within this folder, the 
k_means_generated_files contains the correct intermediate output, along with the
terminal output obtained for a successful execution.

### Build

To build in all implementations, run-

```
make
```

This is all that is required.

**Note-** In implementation folders which have cmake related files (build.sh, the
cmake and cmake/build folder, and CMakeLists.txt) kindly ignore all these files.

### Execute

**Note-** Keep DEBUG flag in the header files set to 1 for verbose output.

#### Distributed mutex locks-

In all hostnames present in the node_list except the last one, to run a node on
each of these hosts-

```
./app
```

#### Distributed shared memory system-

**Sequential consistency**

Run in this order only (for now)-

```
./directory
./p3
./p2
./p1
```

**Note-** For the seq consistency test using malloc, follow the same execution 
order as above.

**Merge sort**

Run in either orders only (for now)-

```
./directory
./app1 0 2
./app1 1 2
```

OR 

```
./directory
./app1 1 2
./app1 0 2
```

**Note-** If more than two processes are used, follow either increasing or
decreasing order of execution as above (this has not been tested yet though).

#### Map reduce framework-

**Note-** Only the first test case has been tried yet. Files for the test case 
to be mentioned/edited within app1.cc.

Run in either orders only (for now)-

```
./directory
./app1 0 3
./app1 1 3
./app1 2 3
```

OR

```
./directory
./app1 2 3
./app1 1 3
./app1 0 3
```

**Note-** More than three processes have not been tried as of now.

#### K-means on map reduce- 

**Note-** Only the first test case has been tried yet. Files for the test case 
to be mentioned/edited within app1.cc.

Run in either orders only (for now)-

```
./directory
./app1 0 4
./app1 1 4
./app1 2 4
./app1 3 4
```

OR

```
./directory
./app1 3 4
./app1 2 4
./app1 1 4
./app1 0 4
```

**Note-** Only 4 processes are required in this case.
