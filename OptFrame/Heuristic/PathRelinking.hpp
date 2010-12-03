#ifndef OPTFRAME_PATHRELINKING_HPP_
#define OPTFRAME_PATHRELINKING_HPP_

#include "../Heuristic.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class PathRelinking: public Heuristic<R, M>
{
protected:
	Evaluator<R, M>& evaluator;
	unsigned int k;
	bool forward;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	PathRelinking(Evaluator<R, M>& evaluator, int k = 1, bool forward = false) :
		evaluator(evaluator), k(k), forward(forward)
	{
	}

	virtual ~PathRelinking()
	{
	}

	virtual vector<pair<Move<R, M>*, double> >& symmetric_difference(Solution<R>& x, Evaluation<M>& e_x, const Solution<R>& xt, const Evaluation<M>& e_xt) = 0;

	// path-relinking from starting solution 'xs' to target solution 'xt'
	pair<Solution<R>&, Evaluation<M>&>& path_relinking(const Solution<R>& xs, const Evaluation<M>& e_xs, const Solution<R>& xt, const Evaluation<M>& e_xt, double timelimit, double target_f)
	{
		cout << "path_relinking" << endl;
		cout << "from: ";
		e_xs.print();
		cout << "to: ";
		e_xt.print();

		Solution<R>& x = xs.clone();
		Evaluation<M>& e_x = e_xs.clone();

		// compute the symmetric difference 'delta' between xs and xt
		vector<pair<Move<R, M>*, double> >& delta = symmetric_difference(x, e_x, xt, e_xt);

		// compute f*
		Solution<R>* s_star;
		Evaluation<M>* e_star;

		if (evaluator.betterThan(e_xs, e_xt))
		{
			e_star = &e_xs.clone();
			s_star = &xs.clone();
		}
		else
		{
			e_star = &e_xt.clone();
			s_star = &xt.clone();
		}

		// while 'delta' > 0, i.e., 'xs' are 'xt' are different
		while (delta.size() > 0)
		{
			//1. find best move
			unsigned index_best = 0;

			for (unsigned i = 1; i < delta.size(); i++)
				if (evaluator.betterThan(delta[i].second, delta[index_best].second))
					index_best = i;

			Move<R, M>* m_star = delta[index_best].first;
			double f_m_star = delta[index_best].second;

			//2. update 'delta'
			delta.erase(delta.begin() + index_best);

			//3. update 'x' and 'e_x'
			m_star->apply(e_x, x);
			evaluator.evaluate(e_x, x);

			//4. compare with 'x*'
			if (evaluator.betterThan(e_x, *e_star))
			{
				delete e_star;
				delete s_star;

				e_star = &e_x.clone();
				s_star = &x.clone();
			}
		}

		delete &x;
		delete &e_x;

		pair<Solution<R>&, Evaluation<M>&>& r = *new pair<Solution<R>&, Evaluation<M>&> (*s_star, *e_star);
		cout << "best path_relinking: ";
		e_star->print();
		//getchar();
		return r;
	}

	// rewritting search method for efficiency purposes!
	// safe use of const_cast
	vector<Solution<R>*>& search(vector<const Solution<R>*>& p, double timelimit = 100000000, double target_f = 0)
	{
		int p_size = p.size();

		vector<Solution<R>*>* v = new vector<Solution<R>*> ;
		for (unsigned i = 0; i < p.size(); i++)
			v->push_back(const_cast<Solution<R>*> (p[i]));

		exec(*v, timelimit, target_f);

		v->erase(v->begin(), v->begin() + p_size);

		return *v;
	}

	void exec(vector<Solution<R>*>& p, double timelimit, double target_f)
	{
		vector<Evaluation<M>*>& ev = *new vector<Evaluation<M>*> ;
		for (int i = 0; i < p.size(); i++)
			ev.push_back(&evaluator.evaluate(p[i]->getR()));

		exec(p, ev, timelimit, target_f);

		for (int i = 0; i < ev.size(); i++)
			delete ev[i];
		delete &ev;
	}

	// rewritting search method for efficiency purposes!
	// safe use of const_cast
	pair<vector<Solution<R>*>&, vector<Evaluation<M>*>&>& search(vector<const Solution<R>*>& p, vector<const Evaluation<M>*>& ev, double timelimit = 100000000, double target_f = 0)
	{
		vector<Solution<R>*>* p2 = new vector<Solution<R>*> ;
		for (unsigned i = 0; i < p.size(); i++)
			p2->push_back(p[i]);

		vector<Evaluation<M>*>* ev2 = new vector<Evaluation<M>*> ;
		for (unsigned i = 0; i < p.size(); i++)
			ev2->push_back(ev[i]);

		exec(*p2, *ev2, timelimit, target_f);

		// update and return correct values

		return *new pair<vector<Solution<R>*>&, vector<Evaluation<M>*>&> (*p2, *ev2);
	}

	void exec(vector<Solution<R>*>& p, vector<Evaluation<M>*>& ev, double timelimit, double target_f)
	{
		if (p.size() <= 1)
		{
			cout << "Path Relinking exec(p&,e&) warning: empty or only one solution in the pool!" << endl;
			return;
		}

		long tini = time(NULL);

		vector<Solution<R>*> new_s;
		vector<Evaluation<M>*> new_e;

		int iter = 0;

		long tnow = time(NULL);

		while (iter < k && ((tnow - tini) < timelimit))
		{
			int x1 = rand() % p.size();
			// different values of x, for efficiency purposes
			int x2 = x1;
			while (x2 == x1)
				x2 = rand() % p.size();

			if (forward && evaluator.betterThan(ev[x1]->evaluation(), ev[x2]->evaluation()))
			{
				int aux = x1;
				x1 = x2;
				x2 = aux;
			}

			if ((!forward) && evaluator.betterThan(ev[x2]->evaluation(), ev[x1]->evaluation()))
			{
				int aux = x1;
				x1 = x2;
				x2 = aux;
			}

			pair<Solution<R> &, Evaluation<M>&>& ret_path = path_relinking(*p[x1], *ev[x1], *p[x2], *ev[x2], timelimit, target_f);

			new_s.push_back(&ret_path.first);
			new_e.push_back(&ret_path.second);

			delete &ret_path;

			tnow = time(NULL);
			iter++;
		}

		p.insert(p.end(), new_s.begin(), new_s.end());
		new_s.clear();
		ev.insert(ev.end(), new_e.begin(), new_e.end());
		new_e.clear();
	}
};

#endif /*OPTFRAME_PATHRELINKING_HPP_*/
