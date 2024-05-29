//
ILSLPerturbationLPlus2<ESolutionTSP> pert(demo.eval, demo.nsSwap, rg2);

IteratedLocalSearchLevels<ESolutionTSP> ils(demo.eval, nnptr::copy(initRand),
                                            nnptr::copy(VND), nnptr::copy(pert),
                                            10, 5);
// ils.setVerbose();

std::cout << "will start ILS for 10 seconds" << std::endl;

optframe::Timer t;
ils.setVerbose();
auto status = ils.search(
    StopCriteria<ESolutionTSP::second_type>{10.0});  // 3.0 seconds max
ESolutionTSP best = *status.best;
std::cout << "spent time: " << t.now() << "s" << std::endl;
// best solution value
best.second.print();
std::cout << "solution: " << best.first << std::endl;

std::cout << "FINISHED" << std::endl;
return 0;
}