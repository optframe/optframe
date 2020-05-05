#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"

namespace POLAD
{

void OPMSolCheck(OPMProblemInstance& opm, Solution<RepOPM>& s)
{
	cout << "Solution Checking" << endl;
	RepOPM& rep = s.getR();

	vector<bool> carregs(opm.numCarregs, false);
	int numCarregs = 0;

	int nFrentes = rep.second.getNumRows();
	int nCams = rep.second.getNumCols();

	double prodTotalMinerio = 0;
	double prodTotalEsteril = 0;

	vector<double> vx(opm.numFrentes, 0);

	vector<double> paramTon(opm.numParams, 0);

	for (int i = 0; i < nFrentes; i++)
	{
		cout << "Frente " << i << ":\t";

		double x = 0;
		int carreg = rep.first[i];

		if (carreg >= 0)
			for (int j = 0; j < nCams; j++)
				if (rep.second(i, j) > 0)
				{
					if (opm.isCompatible(j, carreg))
						x += rep.second(i, j) * opm.l[j];
					else
						cout << "Erro: viagem sem compatibilidade!" << endl;
				}

		if (carreg < 0)
			for (int j = 0; j < nCams; j++)
				if (rep.second(i, j) > 0)
					cout << "Erro: producao sem carregadeira!" << endl;

		cout << x << " " << ((x <= opm.qu[i]) ? " Ok" : " Erro");

		if (carreg >= 0)
		{
			numCarregs++;
			if (carregs[carreg])
			{
				cout << "Erro! Carregadeira repetida. ";
			}
			carregs[carreg] = true;

			cout << "\tCar" << carreg << " ";
			if ((x >= opm.cMin[carreg]) && (x <= opm.cMax[carreg]))
				cout << "Ok" << endl;
			else
			{
				if (x == 0)
					cout << "Inativa!" << endl;
				else
					cout << "Erro! (" << opm.cMin[carreg] << ";" << opm.cMax[carreg] << ")" << endl;
			}
		}
		else
			cout << "\tSem Carregadeira" << endl;

		vx[i] = x;

		if (opm.isMin[i])
		{
			prodTotalMinerio += x;
			for (int par = 0; par < opm.numParams; par++)
				paramTon[par] += (*opm.teor)(i, par) * x;
		}
		else
			prodTotalEsteril += x;
	}
	cout << endl;
	cout << "ProdMinerio = " << prodTotalMinerio << (((prodTotalMinerio >= opm.pl_min) && (prodTotalMinerio <= opm.pu_min)) ? " Ok" : " Erro") << endl;
	cout << "ProdEsteril = " << prodTotalEsteril << (((prodTotalEsteril >= opm.pl_est) && (prodTotalEsteril <= opm.pu_est)) ? " Ok" : " Erro") << endl;
	cout << "|Carregs| = " << numCarregs << ((numCarregs <= opm.numCarregs) ? " Ok" : " Erro") << endl;

	cout << "parametros: " << endl;
	for (unsigned par = 0; par < paramTon.size(); par++)
	{
		double parPerc = paramTon[par] / prodTotalMinerio;
		//cout << "("<<opm.tl[par]<<"-"<<parPerc<<"/"<<opm.tr[par]<<"-"<<opm.tu[par]<<") ";
		//cout << ( (parPerc<opm.tl[par])||(parPerc>opm.tu[par])?'*':' ');
		cout << "Par" << par << ": " << ((parPerc >= opm.tl[par]) && (parPerc <= opm.tu[par]) ? "Ok" : "Erro") << endl;

	}
	cout << endl;

	cout << "MST File" << endl;
	cout << endl << endl;
	cout << "<?xml version = \"1.0\" standalone=\"yes\"?>" << endl;
	cout << "<?xml-stylesheet" << endl;
	cout << "href=\"https://www.ilog.com/products/cplex/xmlv1.0/solution.xsl\"" << endl;
	cout << "type=\"text/xsl\"?>" << endl;
	cout << "<CPLEXSolution version=\"1.0\">" << endl;
	//cout << "<header"<<endl;
	//cout << "problemName=\""<<nome<<"\""<<endl;
	//objectiveValue="-122.5"
	//solutionTypeValue="3"
	//solutionTypeString="primal"
	//solutionStatusValue="101"
	//solutionStatusString="integer optimal solution"
	//MIPNodes="0"
	//MIPIterations="3"/>
	//cout << "/>"<<endl;

	cout << "<variables>" << endl;

	for (int i = 0; i < opm.numFrentes; i++)
		for (int g = 0; g < opm.numCarregs; g++)
			if (vx[i] > 0)
				if (rep.first[i] == g) // *
					cout << "<variable name=\"opera(Frente" << i << ",Car" << g << ")\" value=\"1\"/>" << endl;

	for (int i = 0; i < opm.numFrentes; i++)
		for (int c = 0; c < opm.numCaminhoes; c++)
			//if(rep.second(i,c)>0)
			cout << "<variable name=\"viagens(Frente" << i << ",Cam" << c << ")\" value=\"" << rep.second(i, c) << "\"/>" << endl;

	cout << "</variables>" << endl;
	cout << "</CPLEXSolution>" << endl;
}
}
