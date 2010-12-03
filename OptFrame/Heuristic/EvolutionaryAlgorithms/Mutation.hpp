#ifndef MUTATION_HPP_
#define MUTATION_HPP_

template<class R, class M>
class Mutation
{
protected:

   typedef Solution<R> chromossome;
   //typedef vector<chromossome*> Population;
   //typedef vector<Evaluation<M>*> FitnessValues;

public:

   virtual void mutate(chromossome& individual) = 0;

};

#endif /* MUTATION_HPP_ */
