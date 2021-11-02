//
class MoveBitFlip : public Move<ESolutionKP>
{
public:
   int k; // MoveBitFlip (moveData = 'int' (k))

   MoveBitFlip(int _k)
     : k{ _k }
   {
   }

   uptr<Move<ESolutionKP>> apply(ESolutionKP& se) override
   {
      se.first[k] = !se.first[k];                           // reverts element 'k'
      return uptr<Move<ESolutionKP>>(new MoveBitFlip{ k }); // returns reverse move
   }
};

uptr<Move<ESolutionKP>>
makeMoveBitFlip(int k)
{
   return uptr<Move<ESolutionKP>>(new MoveBitFlip{ k });
}