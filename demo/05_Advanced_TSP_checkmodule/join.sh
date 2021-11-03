#!/bin/bash

DIR03=../03_QuickstartTSP_VNS_BRKGA/

cat ${DIR03}/TSP-fcore-part1.hpp ${DIR03}/TSP-fcore-part2.hpp ${DIR03}/TSP-fcore-part3.hpp ${DIR03}/TSP-fcore-part4.hpp TSP-fcore-part5-applyupdate.hpp ${DIR03}/TSP-fcore-part6.hpp ${DIR03}/TSP-fcore-part7.hpp > TSP-fcore-applyupdate.hpp 
cat ${DIR03}/TSP-fcore-part1.hpp ${DIR03}/TSP-fcore-part2.hpp ${DIR03}/TSP-fcore-part3.hpp ${DIR03}/TSP-fcore-part4.hpp TSP-fcore-part5-cost.hpp        ${DIR03}/TSP-fcore-part6.hpp ${DIR03}/TSP-fcore-part7.hpp > TSP-fcore-cost.hpp 
