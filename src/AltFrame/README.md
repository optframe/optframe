## AltFrame

Alternative version of OptFrame.

Compatible with OptFCore, aiming a more efficient 4.2 version.


### tests and benchmarks

```
cd tests
make bench_alt
./build/AltFrame_bench --benchmark_filter=TSP_AltFrame
```

```
-----------------------------------------------------------------------------------
Benchmark                                         Time             CPU   Iterations
-----------------------------------------------------------------------------------
TSP_AltFrame_1_CPP_baseline/10/0                575 ns          576 ns      1216629
TSP_AltFrame_1_CPP_baseline/20/0                947 ns          947 ns       743656
TSP_AltFrame_1_CPP_baseline/30/0               1594 ns         1595 ns       440222
TSP_AltFrame_NSSeqTestPtr_raw/10/0              598 ns          597 ns      1177473
TSP_AltFrame_NSSeqTestPtr_raw/20/0             1074 ns         1073 ns       654463
TSP_AltFrame_NSSeqTestPtr_raw/30/0             1925 ns         1924 ns       357477
TSP_AltFrame_NSSeqTestPtr_raw/100/0           17391 ns        17352 ns        40802
TSP_AltFrame_NSSeqTestPtr_raw/200/0           67605 ns        67505 ns        10392
TSP_AltFrame_NSSeqTestPtr_raw_iter/10/0         691 ns          691 ns      1022702
TSP_AltFrame_NSSeqTestPtr_raw_iter/20/0        1508 ns         1508 ns       463666
TSP_AltFrame_NSSeqTestPtr_raw_iter/30/0        2788 ns         2789 ns       251790
TSP_AltFrame_NSSeqTestPtr_raw_iter/100/0      26206 ns        26176 ns        26927
TSP_AltFrame_NSSeqTestPtr_raw_iter/200/0     102944 ns       102853 ns         6832
TSP_AltFrame_NSSeqTestPtr_iter_ptr/10/0         640 ns          640 ns      1099738
TSP_AltFrame_NSSeqTestPtr_iter_ptr/20/0        1332 ns         1331 ns       528893
TSP_AltFrame_NSSeqTestPtr_iter_ptr/30/0        2461 ns         2462 ns       273667
TSP_AltFrame_NSSeqTestPtr_iter_ptr/100/0      22772 ns        22736 ns        31241
TSP_AltFrame_NSSeqTestPtr_iter_ptr/200/0      88800 ns        88702 ns         7964
```

AltFrame demonstrated that it's not feasible to have a 1-to-1 NSSeq-to-NSIterator structure.
NSSeq should be the iterator class itself.

Coroutine overhead seems high (compared to raw baseline):

```
TSP_AltFrame_NSSeqTestPtr_raw_coro/10/0         831 ns          830 ns       847887
TSP_AltFrame_NSSeqTestPtr_raw_coro/20/0        1945 ns         1945 ns       362478
TSP_AltFrame_NSSeqTestPtr_raw_coro/30/0        4787 ns         4786 ns       145642
TSP_AltFrame_NSSeqTestPtr_raw_coro/100/0      34998 ns        34959 ns        19591
TSP_AltFrame_NSSeqTestPtr_raw_coro/200/0     138003 ns       137894 ns         5080
```