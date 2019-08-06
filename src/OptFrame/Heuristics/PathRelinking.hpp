// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_PATHRELINKING_HPP_
#define OPTFRAME_PATHRELINKING_HPP_

#include "../Component.hpp"
#include "../LocalSearch.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PathRelinking: public Component//LocalSearch<R, ADS, DS>
{
	typedef vector<Evaluation<DS>*> FitnessValues;
	typedef const vector<const Evaluation<DS>*> ConstFitnessValues;

protected:
	Evaluator<R, ADS, DS>& evaluator;
	unsigned int k;
	bool forward;
	LocalSearch<R, ADS, DS>& localSearch;

public:

	//using HTrajectory<R, ADS, DS>::exec; // prevents name hiding

	PathRelinking(LocalSearch<R, ADS, DS>& localSearch, Evaluator<R, ADS, DS>& evaluator, int k = 1, bool forward = false) :
		localSearch(localSearch), evaluator(evaluator), k(k), forward(forward)
	{
	}

	virtual ~PathRelinking()
	{
	}

	virtual vector<pair<Move<R, ADS, DS>*, double> >& symmetric_difference(Solution<R, ADS>& x, Evaluation<DS>& e_x, const Solution<R, ADS>& xt,
			const Evaluation<DS>& e_xt) = 0;

	virtual void update_delta(vector<pair<Move<R, ADS, DS>*, double> >& delta, int index_best, Solution<R, ADS>& x, Evaluation<DS>& e_x, const Solution<R, ADS>& xt,
			const Evaluation<DS>& e_xt)
	{
		delta.erase(delta.begin() + index_best);
	}

	// path-relinking from starting solution 'xs' to target solution 'xt'
	virtual pair<Solution<R, ADS>&, Evaluation<DS>&>& path_relinking(const Solution<R, ADS>& xs, const Evaluation<DS>& e_xs, const Solution<R, ADS>& xt,
			const Evaluation<DS>& e_xt, double timelimit, double target_f)
	{
		cout << "path_relinking " << (forward ? "forward" : "backward") << endl;
		cout << "from: ";
		e_xs.print();
		cout << "to: ";
		e_xt.print();

		Solution<R, ADS>& x = xs.clone();
		Evaluation<DS>& e_x = e_xs.clone();

		// compute the symmetric difference 'delta' between xs and xt
		vector<pair<Move<R, ADS, DS>*, double> >& delta = symmetric_difference(x, e_x, xt, e_xt);

		// compute f*
		Solution<R, ADS>* s_star;
		Evaluation<DS>* e_star;

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
			/*
			 cout << "\nNOVA ITERACAO = |delta| = " << delta.size() << endl;
			 cout << "waiting...";
			 getchar();
			 */

			//1. find best move
			unsigned index_best = 0;

			for (unsigned i = 1; i < delta.size(); i++)
				if (evaluator.betterThan(delta[i].second, delta[index_best].second))
					index_best = i;

			Move<R, ADS, DS>* m_star = delta[index_best].first;
			double f_m_star = delta[index_best].second;

			//2. update 'x' and 'e_x'
			m_star->apply(e_x, x);
			//m_star->print();
			//cout << "FO ATUAL ANTES BUSCA= " << evaluator.evaluate(x).evaluation() << endl;
			localSearch.exec(x, e_x, timelimit, target_f);
			//cout << "FO ATUAL= " << evaluator.evaluate(x).evaluation() << endl;

			evaluator.evaluate(e_x, x);

			//3. update 'delta'
			update_delta(delta, index_best, x, e_x, xt, e_xt);
			//delta.erase(delta.begin() + index_best);

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

		pair<Solution<R, ADS>&, Evaluation<DS>&>& r = *new pair<Solution<R, ADS>&, Evaluation<DS>&> (*s_star, *e_star);
		cout << "best path_relinking: ";
		e_star->print();
		//getchar();
		return r;
	}

	// rewritting search method for efficiency purposes!
	// safe use of const_cast
	Population<R, ADS>& search(const Population<R, ADS>& p, double timelimit = 100000000, double target_f = 0)
	{
		int p_size = p.size();

		Population<R, ADS>* v = new Population<R, ADS> ;

		for (unsigned i = 0; i < p.size(); i++)
			v->push_back(const_cast<Solution<R, ADS>&> (p.at(i)));

		exec(*v, timelimit, target_f);

		for (int i = 0; i < p_size; i++)
			v->remove(0);

		return *v;
	}

	void exec(Population<R, ADS>& p, double timelimit, double target_f)
	{
		vector<Evaluation<DS>*>& ev = *new vector<Evaluation<DS>*> ;
		for (int i = 0; i < p.size(); i++)
			ev.push_back(&evaluator.evaluate(p.at(i)));

		exec(p, ev, timelimit, target_f);

		for (int i = 0; i < ev.size(); i++)
			delete ev[i];
		delete &ev;
	}

	// rewritting search method for efficiency purposes!
	// safe use of const_cast
	pair<Population<R, ADS>&, FitnessValues&>& search(const Population<R, ADS>& p, ConstFitnessValues& ev, double timelimit = 100000000, double target_f = 0)
	{
		Population<R, ADS>* p2 = new Population<R, ADS> ;
		for (unsigned i = 0; i < p.size(); i++)
			p2->push_back(const_cast<Solution<R, ADS>&> (p.at(i)));

		vector<Evaluation<DS>*>* ev2 = new vector<Evaluation<DS>*> ;
		for (unsigned i = 0; i < p.size(); i++)
			ev2->push_back(const_cast<Evaluation<DS>*> (ev[i]));

		exec(*p2, *ev2, timelimit, target_f);

		// update and return correct values

		return *new pair<Population<R, ADS>&, FitnessValues&> (*p2, *ev2);
	}

	void exec(Population<R, ADS>& p, vector<Evaluation<DS>*>& ev, double timelimit, double target_f)
	{
		if (p.size() <= 1)
		{
			cout << "Path Relinking exec(p&,e&) warning: empty or only one solution in the pool!" << endl;
			return;
		}

		long tini = time(nullptr);

		cout << "Path Relinking starts!" << endl;

		vector<Solution<R, ADS>*> new_s;
		vector<Evaluation<DS>*> new_e;

		int iter = 0;

		long tnow = time(nullptr);

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

			pair<Solution<R, ADS> &, Evaluation<DS>&>& ret_path = path_relinking(p.at(x1), *ev[x1], p.at(x2), *ev[x2], timelimit, target_f);

			new_s.push_back(&ret_path.first);
			new_e.push_back(&ret_path.second);

			delete &ret_path;

			tnow = time(nullptr);
			iter++;
		}

		for(int i=0;i<new_s.size();i++)
			p.push_back(*new_s[i]);

		new_s.clear();
		ev.insert(ev.end(), new_e.begin(), new_e.end());
		new_e.clear();
	}

};

}

#endif /*OPTFRAME_PATHRELINKING_HPP_*/
