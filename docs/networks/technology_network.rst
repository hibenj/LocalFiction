Technology Network
==================

**Header:** ``fiction/networks/technology_network.hpp``

A ``mockturtle`` logic network type that extends ``mockturtle::klut_network``. It reimplements all node creation functions
to disable structural hashing. Furthermore, create_buf is overwritten such that it creates real buffer nodes that count
towards the size of the network and compute the identity function. In the same way, a buffer with more than one fanout
node is considered a fanout node (or splitter in some contexts) that copies an incoming signal to two or more data paths.

.. doxygenclass:: fiction::technology_network
   :members:
