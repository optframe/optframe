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
TSP_AltFrame_1_CPP_baseline/10/0                   576 ns          576 ns      1216521
TSP_AltFrame_1_CPP_baseline/20/0                   943 ns          941 ns       690559
TSP_AltFrame_1_CPP_baseline/30/0                  1564 ns         1560 ns       449415
TSP_AltFrame_1_CPP_baseline/100/0                13544 ns        13501 ns        52043
TSP_AltFrame_1_CPP_baseline/200/0                52716 ns        52614 ns        13343
TSP_AltFrame_1_CPP_baseline/1000/0             1312776 ns      1312235 ns          537
TSP_AltFrame_NSSeqTestPtr_raw/10/0                 615 ns          615 ns      1141161
TSP_AltFrame_NSSeqTestPtr_raw/20/0                1093 ns         1093 ns       639985
TSP_AltFrame_NSSeqTestPtr_raw/30/0                1996 ns         1994 ns       350343
TSP_AltFrame_NSSeqTestPtr_raw/100/0              17455 ns        17424 ns        40270
TSP_AltFrame_NSSeqTestPtr_raw/200/0              66751 ns        66664 ns        10465
TSP_AltFrame_NSSeqTestPtr_raw/1000/0           1658502 ns      1657931 ns          422
TSP_AltFrame_NSSeqTestPtr_iter_heap/10/0           724 ns          723 ns       974151
TSP_AltFrame_NSSeqTestPtr_iter_heap/20/0          1610 ns         1611 ns       435869
TSP_AltFrame_NSSeqTestPtr_iter_heap/30/0          3019 ns         3014 ns       232509
TSP_AltFrame_NSSeqTestPtr_iter_heap/100/0        28334 ns        28304 ns        24673
TSP_AltFrame_NSSeqTestPtr_iter_heap/200/0       115232 ns       115124 ns         6316
TSP_AltFrame_NSSeqTestPtr_iter_heap/1000/0     2762291 ns      2761694 ns          254
TSP_AltFrame_NSSeqTestPtr_iter_stack/10/0          643 ns          642 ns      1092748
TSP_AltFrame_NSSeqTestPtr_iter_stack/20/0         1349 ns         1349 ns       518284
TSP_AltFrame_NSSeqTestPtr_iter_stack/30/0         2511 ns         2508 ns       279936
TSP_AltFrame_NSSeqTestPtr_iter_stack/100/0       22819 ns        22792 ns        29715
TSP_AltFrame_NSSeqTestPtr_iter_stack/200/0       88579 ns        88487 ns         7947
TSP_AltFrame_NSSeqTestPtr_iter_stack/1000/0    2208612 ns      2208068 ns          319
```

AltFrame demonstrated that it's not feasible to have a 1-to-1 NSSeq-to-NSIterator structure.
NSSeq should be the iterator class itself.

Coroutine overhead seems high (compared to raw baseline):

```
TSP_AltFrame_NSSeqTestPtr_raw_coro/10/0            823 ns          823 ns       854627
TSP_AltFrame_NSSeqTestPtr_raw_coro/20/0           1954 ns         1955 ns       357716
TSP_AltFrame_NSSeqTestPtr_raw_coro/30/0           3749 ns         3744 ns       188380
TSP_AltFrame_NSSeqTestPtr_raw_coro/100/0         35055 ns        35023 ns        19985
TSP_AltFrame_NSSeqTestPtr_raw_coro/200/0        137095 ns       136996 ns         5100
TSP_AltFrame_NSSeqTestPtr_raw_coro/1000/0      3390131 ns      3389511 ns          205
```