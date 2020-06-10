
#include<vector>

#include "NSSeqTest.hpp"


int main()
{

   int nTSP = 6;
   NSSeqTest nsseq{nTSP};
   std::vector<int> v{nTSP, 0};
   for(nsseq.first(v); nsseq.next();)
   {
      auto mv = nsseq.current();
      mv->print();
      mv->fApplyDo(v);
   }



   return 0;
}