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
TSP_AltFrame_1_CPP_baseline/10/0                       598 ns          596 ns      1172584
TSP_AltFrame_1_CPP_baseline/20/0                       959 ns          957 ns       732916
TSP_AltFrame_1_CPP_baseline/30/0                      1604 ns         1600 ns       437491
TSP_AltFrame_1_CPP_baseline/100/0                    13732 ns        13704 ns        51239
TSP_AltFrame_1_CPP_baseline/200/0                    53623 ns        53530 ns        13101
TSP_AltFrame_1_CPP_baseline/1000/0                 1303942 ns      1303388 ns          534
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/10/0                618 ns          616 ns      1128480
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/20/0               1104 ns         1102 ns       635929
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/30/0               1964 ns         1960 ns       357043
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/100/0             17195 ns        17175 ns        40822
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/200/0             66637 ns        66560 ns        10503
TSP_AltFrame_NSSeqTestPtr_MoveFPtr/1000/0          1651396 ns      1650873 ns          425
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/10/0          659 ns          659 ns      1052014
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/20/0         1448 ns         1447 ns       451864
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/30/0         2728 ns         2728 ns       256476
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/100/0       24888 ns        24873 ns        27997
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/200/0       96595 ns        96511 ns         7268
TSP_AltFrame_NSSeqTestPtr_MoveFPtr_byref/1000/0    2387924 ns      2387275 ns          293
TSP_AltFrame_NSSeqTestPtr_MoveFRef/10/0                667 ns          665 ns      1054671
TSP_AltFrame_NSSeqTestPtr_MoveFRef/20/0               1451 ns         1450 ns       481877
TSP_AltFrame_NSSeqTestPtr_MoveFRef/30/0               2737 ns         2733 ns       259408
TSP_AltFrame_NSSeqTestPtr_MoveFRef/100/0             25282 ns        25272 ns        27735
TSP_AltFrame_NSSeqTestPtr_MoveFRef/200/0             99927 ns        99845 ns         7073
TSP_AltFrame_NSSeqTestPtr_MoveFRef/1000/0          2518843 ns      2518248 ns          283
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/10/0               967 ns          966 ns       727040
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/20/0              2660 ns         2659 ns       263142
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/30/0              5655 ns         5652 ns       125670
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/100/0            59697 ns        59680 ns        11554
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/200/0           239226 ns       239146 ns         2926
TSP_AltFrame_NSSeqTestPtr_MoveFCopy/1000/0         6027872 ns      6027307 ns          116
TSP_AltFrame_NSSeqTestPtr_MoveInternal/10/0            619 ns          617 ns      1136411
TSP_AltFrame_NSSeqTestPtr_MoveInternal/20/0           1103 ns         1101 ns       636778
TSP_AltFrame_NSSeqTestPtr_MoveInternal/30/0           1959 ns         1955 ns       358874
TSP_AltFrame_NSSeqTestPtr_MoveInternal/100/0         17212 ns        17194 ns        40798
TSP_AltFrame_NSSeqTestPtr_MoveInternal/200/0         66804 ns        66726 ns        10443
TSP_AltFrame_NSSeqTestPtr_MoveInternal/1000/0      1662851 ns      1662185 ns          424
TSP_AltFrame_NSSeqTestPtr_iter_heap/10/0               702 ns          701 ns      1001367
TSP_AltFrame_NSSeqTestPtr_iter_heap/20/0              1505 ns         1504 ns       454881
TSP_AltFrame_NSSeqTestPtr_iter_heap/30/0              2794 ns         2789 ns       251269
TSP_AltFrame_NSSeqTestPtr_iter_heap/100/0            25970 ns        25958 ns        26876
TSP_AltFrame_NSSeqTestPtr_iter_heap/200/0           101815 ns       101748 ns         6795
TSP_AltFrame_NSSeqTestPtr_iter_heap/1000/0         2543111 ns      2542584 ns          273
TSP_AltFrame_NSSeqTestPtr_iter_stack/10/0              648 ns          646 ns      1086395
TSP_AltFrame_NSSeqTestPtr_iter_stack/20/0             1363 ns         1362 ns       514199
TSP_AltFrame_NSSeqTestPtr_iter_stack/30/0             2544 ns         2541 ns       275847
TSP_AltFrame_NSSeqTestPtr_iter_stack/100/0           23281 ns        23257 ns        29042
TSP_AltFrame_NSSeqTestPtr_iter_stack/200/0           88795 ns        88723 ns         7645
TSP_AltFrame_NSSeqTestPtr_iter_stack/1000/0        2205448 ns      2204876 ns          308
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