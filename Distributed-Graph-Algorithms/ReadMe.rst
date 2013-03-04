Distributed Graph Algorithms in DistAlgo
========================================

This repository contains the implementations of some *distributed* graph algorithms as well as two token-based mutex algorithms in *DistAlgo*. DistAlgo is a superset of Python that adds special constructs to Python for specifying distributed algorithms in a high-level and succint manner. DistAlgo programs get compiled to portable Python 3 code. DistAlgo was developed at the `Department of Computer Science <http://www.cs.sunysb.edu/>`_ in Stony Brook University. It is used in the instruction of some graduate courses such as `CSE 535 Asynchronous Systems <http://www.cs.stonybrook.edu/~liu/cse535/>`_ by `Annie Liu <http://www.cs.sunysb.edu/~liu/>`_ and `CSE 594 Distributed Systems <http://www.cs.sunysb.edu/~stoller/cse594/>`_ by `Scott Stoller <http://www.cs.sunysb.edu/~stoller/>`_.

The Distributed Graph Algorithms
--------------------------------
The following distributed graph algorithms that have implemented using **DistAlgo-0.2**:

1. `Minimum Spanning Tree`_
2. `Maximal Independent Set`_
3. `Breadth First Search`_
4. `Shortest Path`_

.. _Minimum Spanning Tree: https://github.com/arjungmenon/Old-Projects/tree/master/Distributed-Graph-Algorithms/Minimum-Spanning-Tree
.. _Maximal Independent Set: https://github.com/arjungmenon/Old-Projects/tree/master/Distributed-Graph-Algorithms/Maximal-Independent-Set
.. _Breadth First Search: https://github.com/arjungmenon/Old-Projects/tree/master/Distributed-Graph-Algorithms/Breadth-First-Search
.. _Shortest Path: https://github.com/arjungmenon/Old-Projects/tree/master/Distributed-Graph-Algorithms/ShortestPath

The subdirectory of each algorithm contains a relevant **ReadMe** file that describes the algorithm in detail with specific information about its implementation, limitations, etc. The ReadMe should be viewed in GitHub or with a Markdown converter as it contains images, styling, sections, etc.

Note about References
~~~~~~~~~~~~~~~~~~~~~
All references have been hyperlinked **in-place** using `Markdown's link syntax <http://daringfireball.net/projects/markdown/syntax#link>`_.

NetworkX
~~~~~~~~
A graph library known as `NetworkX <http://networkx.lanl.gov/>`_ was used in this project. It is a well-renown pure Python library that provides several useful features pertaining to graphs. Granted, it was a bit unnecessary for this project, but I was anticipating more use for it than was needed.

Other
-----
`Ricart-Agrawala's and Suzuki-Kasami's token-based distributed mutex algorithms <https://github.com/arjungmenon/Distributed-Graph-Algorithms/tree/master/DistributedMutex>`_ have also been been implemented in DistAlgo-0.2. In addition, `Lamport's fast mutual exclusion and bakery algorithms <https://github.com/arjungmenon/Distributed-Graph-Algorithms/tree/master/ConcurrentMutex>`_ have been implemented using the built-in Python library ``threading``.
