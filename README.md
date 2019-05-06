
Switching Practice: Routing Lookups in Hardware at Memory Access Speeds 

Programmed by Álvaro Carrascosa de Miguel. Telematic Engineering bachelor student in Universidad Carlos III de Madrid.

In this practice we implement a route lookup algorithm based on multi-bit binary trie, in two levels.
According Pankaj Gupta, Steven Lin, and Nick McKeown from Stanford University, most of prefixes are /24 of below so we can achieve a route lookup operation in just one memory access, which is the principal bottleneck in such a time critical operation.
Therefore we will create a two layer binary trie, one for /24 and below and another for /24-/32, we will have at most two memory accesses.
# route_lookup #uc3m #switching #conmutacion
