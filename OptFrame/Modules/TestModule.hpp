#ifndef TESTMODULE_HPP_
#define TESTODULE_HPP_

#include "../OptFrameTestModule.hpp"

template<class R, class M>
class TestModule : public OptFrameTestModule<R,M>
{
public:
	string id()
	{
		return "test";
	}
	string usage()
	{
		string u = "test N T TF BF ISG EVAL METHOD OUTPUTFILE\n WHERE:\n";
		u+="N is the number of tests to be executed;\n";
		u+="T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
		u+="TF is the target evaluation function value;\n";
		u+="BF is the best known evaluation function value;\n";
		u+="ISG is the initial solution generator; (e.g. initsol 0)\n";
		u+="EVAL is the main evaluator; (e.g. ev 0)\n";
		u+="METHOD is the method to be tested with its own parameters;\n";
		u+="OUTPUTFILE is the output file.\n";

		return u;
	}

	void run(vector<OptFrameTestModule<R,M>*> all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		int n = scanner.nextInt();
		int t = scanner.nextDouble();
		double tf = scanner.nextDouble();
		double bf = scanner.nextDouble();
		InitialSolution<R>* initsol = factory->read_initsol(&scanner);
		Evaluator<R,M>* eval = factory->read_ev(&scanner);
		pair<Heuristic<R,M>*,string> method = factory->createHeuristic(scanner.rest());

		Heuristic<R,M>* h = method.first;

		string filename = method.second;

		if(t==0) // if no timelimit
			t=1000000000;

		long timelimit = t;

		if(bf==0) // if best is zero
			bf=0.00001;


		FILE* file = fopen(filename.c_str(),"a");
		if(!file)
		{
			cout << "Error creating file '"<<filename<<"'" << endl;
			return;
		}

		double s_fo_ini = 0; double s_t_ini = 0;
		double s_fo_end = 0; double s_t_end = 0;
		double min_fo = 1000000000; double min_t = 1000000000;
		double max_fo = -1000000000; double max_t = -1000000000;

		double t_now = 0;
		double fo_now = 0;

		for(int i=0;i<n;i++)
		{
			long seed = time(NULL) + i;
			fprintf(file,"%ld\t", seed);

			cout << "Test "<<i<<" {seed="<<seed<<"}... Running";
			Timer t(false);

			Solution<R>* s = initsol->generateSolution();
			t_now = t.now();
			fo_now = eval->evaluate(s)->evaluation();
			fprintf(file,"%.3f\t%.3f\t", fo_now, t_now);
			s_fo_ini += fo_now;
			s_t_ini  += t_now;

			Solution<R>* s2 = h->searchFT(tf,timelimit,s);
			t_now = t.now();
			fo_now = eval->evaluate(s2)->evaluation();
			fprintf(file,"%.3f\t%.3f\t", fo_now, t_now);
			s_fo_end += fo_now;
			s_t_end  += t_now;

			if(fo_now < min_fo)
				min_fo = fo_now;
			if(t_now < min_t)
				min_t = t_now;
			if(fo_now > max_fo)
				max_fo = fo_now;
			if(t_now > max_t)
				max_t = t_now;

			cout << "... Finished! ("<<t.now()<<"secs.)"<<endl;

			delete s;
			delete s2;

			fprintf(file,"\n");
		}

		s_fo_ini /= n;
		s_t_ini /= n;
		s_fo_end /= n;
		s_t_end /= n;

		fprintf(file,"AVERAGE:\t%f\t%f\t%f\t%f\n", s_fo_ini, s_t_ini, s_fo_end, s_t_end);
		fprintf(file,"MIN:\t-\t-\t%f\t%f\n", min_fo, min_t);
		fprintf(file,"MAX:\t-\t-\t%f\t%f\n", max_fo, max_t);
		if(min_fo==0)
			min_fo=0.00001;
		fprintf(file,"VARIABILITY:\t-\t-\t%f\t-\n", (s_fo_end - min_fo)/min_fo);
		fprintf(file,"GAP_FROM_BEST:\t%f\t-\t%f\t-\n", (s_fo_ini - bf)/bf, (s_fo_end - bf)/bf);
		fprintf(file,"GAP_FROM_AVG:\t%f\t-\t%f\t-\n", (s_fo_ini - bf)/s_fo_ini, (s_fo_end - bf)/s_fo_end);
		fprintf(file,"IMPROVEMENT:\t-\t-\t%f\t-\n", (bf - min_fo)/bf);
		fprintf(file,"BEST(LIT):\t%f\n", bf);
		fprintf(file,"PARAMETERS:%s\n", input.c_str());

		fclose(file);
	}

};

#endif /* TESTMODULE_HPP_ */
