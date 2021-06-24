// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#!/bin/bash

clear
echo "Name of project: "
read project
echo "Type of keyA: "
read keyA
echo "Type of A: "
read A
echo "Type of keyB: "
read keyB
echo "Type of B: "
read B
echo "Type of C: "
read C

cp ./.tpl/MRI.tpl ./MRI.cpp

t1="s/\$keyA/$keyA/g"
sed -e $t1 < MRI.cpp > MRI.cpp.2
t2="s/\$A/$A/g"
sed -e $t2 < MRI.cpp.2 > MRI.cpp.3
t3="s/\$keyB/$keyB/g"
sed -e $t3 < MRI.cpp.3 > MRI.cpp.4
t4="s/\$B/$B/g"
sed -e $t4 < MRI.cpp.4 > MRI.cpp.5
t5="s/\$C/$C/g"
sed -e $t5 < MRI.cpp.5 > MRI.cpp.6
t6="s/\$MRI/SeqMR/g"
sed -e $t6 < MRI.cpp.6 > MRI.cpp
rm MRI.cpp.2 MRI.cpp.3 MRI.cpp.4 MRI.cpp.5 MRI.cpp.6
mkdir $project
mv MRI.cpp $project/$project.cpp

echo "Generating file .."
