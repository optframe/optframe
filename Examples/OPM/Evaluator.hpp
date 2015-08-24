#ifndef OPM_EVALUATOR_HPP_
#define OPM_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_OPM 0.0001

namespace POLAD
{
using namespace std;

class OPMEvaluator: public Evaluator<RepOPM>
{
private:
	OPMProblemInstance& polad;

	// Your private vars

public:

	using Evaluator<RepOPM>::evaluate;

	OPMEvaluator(OPMProblemInstance& _pOPM) : // If necessary, add more parameters
			polad(_pOPM)
	{

	}

	EvaluationOPM& evaluate(const RepOPM& rep)
	{
		double fo_q = 0;
		double fo_inv = 0;

		double fo_prod_min_q = 0;
		double fo_prod_est_q = 0;

		double fo_espec_q = 0;
		double fo_usou_q = 0;

		double fo_prod_inv = 0;
		double fo_params_inv = 0;
		double fo_cams_inv = 0;
		double fo_carregs_inv = 0;

		//cout << "Avaliando.." << endl;

		const Matrix<int>* viagens = &(rep.second);
		const vector<int>* alocacoes = &(rep.first);

		// Verificacao de integridade!
		/*
		 vector<bool> vcarregs(polad->numCarregs,false);
		 for(unsigned i=0;i<alocacoes->size();i++)
		 if(alocacoes->at(i)>=0)
		 if(vcarregs[alocacoes->at(i)])
		 {
		 cout << "Erro de carregadeiras repetidas!" << endl;
		 exit(1);
		 }
		 else
		 vcarregs[alocacoes->at(i)] = true;
		 */

		/*
		 for(int i=0;i<viagens->getRows();i++)
		 for(int j=0;j<viagens->getCols();j++)
		 {
		 if((*viagens)(i,j) < 0 )
		 {
		 cout << (*rep) << endl;
		 cout << "Problemas detectados (1)!"<<"["<<i<<","<<j<<"]"<<" abortando..." << endl;
		 int jkl;
		 cin >> jkl;
		 exit(1);
		 }
		 if(((*alocacoes)[i] == -1) && ((*viagens)(i,j) > 0)  )
		 {
		 cout << (*rep) << endl;
		 cout << "Problemas detectados (2)!"<<"["<<i<<","<<j<<"]"<<" abortando..." << endl;
		 int jkl;
		 cin >> jkl;
		 exit(1);
		 }

		 if((*alocacoes)[i] > 0 && ((*viagens)(i,j) > 0) && !polad->isCompatible(j,(*alocacoes)[i]))
		 {
		 cout << "Erro de compatibilidade!"<<endl;
		 exit(1);
		 }

		 }
		 */

		int numFrentes = polad.getNumFrentes();
		int numCarregs = polad.getNumCarregs();
		int numCaminhoes = polad.getNumCaminhoes();
		int numParams = polad.getNumParams();

		// ============================================
		//     Calculo da producao e especificacao
		// ============================================

		vector<double> espec(numParams, 0);

		double min_prod = 0;
		double est_prod = 0;

		vector<double> x(numFrentes, 0);

		for (int f = 0; f < numFrentes; f++)
			for (int c = 0; c < numCaminhoes; c++)
				if (polad.isCompatible(c, alocacoes->at(f)))
				{
					x[f] += (*viagens)(f, c) * polad.getL(c);

					if (polad.getIsMin(f))
					{
						min_prod += (*viagens)(f, c) * polad.getL(c);

						for (int p = 0; p < numParams; p++)
							espec[p] += (polad.getTeor(f, p) / 100.0) * (*viagens)(f, c) * polad.getL(c);
					}
					else
						est_prod += (*viagens)(f, c) * polad.getL(c);
				}

		//cout << "min_prod="<<min_prod << " de " << polad->getPrMin() << endl;
		//cout << "est_prod="<<est_prod << " de " << polad->getPrEst() << endl;
		//cout << "x: " << x << endl;
		//cout << "espec: " <<espec<<endl;

		// ============================================
		//          Desvios de producao
		// ============================================

		double dp_min = min_prod - polad.getPrMin();
		double dp_est = est_prod - polad.getPrEst();

		//cout << "dp_min="<< dp_min << endl;
		//cout << "dp_est="<< dp_est << endl;

		double dnp_min = std::max(0.0, -dp_min);
		double dpp_min = std::max(0.0, dp_min);
		double dnp_est = std::max(0.0, -dp_est);
		double dpp_est = std::max(0.0, dp_est);

		//cout << "dnp_min="<< dnp_min << endl;
		//cout << "dpp_min="<< dpp_min << endl;
		//cout << "dnp_est="<< dnp_est << endl;
		//cout << "dpp_est="<< dpp_est << endl;

		fo_prod_min_q = polad.getWnpMin() * dnp_min + polad.getWppMin() * dpp_min;
		fo_prod_est_q = polad.getWnpEst() * dnp_est + polad.getWppEst() * dpp_est;

		//cout << "fo_prod_q=" << fo_prod_q << endl;

		// ============================================
		//          Desvios de especificacao
		// ============================================

		vector<double> d_espec(numParams, 0);

		for (int p = 0; p < numParams; p++)
			d_espec[p] = espec[p] - (polad.getTrParam(p) * min_prod / 100);

		//cout << "d_espec: " <<d_espec<<endl;

		fo_espec_q = 0;
		for (int p = 0; p < numParams; p++)
			if (d_espec[p] < 0)
				fo_espec_q += -d_espec[p] * 100 * polad.getWnm(p) * 1.0;
			else
				fo_espec_q += d_espec[p] * 100 * polad.getWpm(p) * 1.0;

		//cout << "fo_espec_q="<<fo_espec_q<<endl;

		// ============================================
		//          Uso dos caminhoes
		// ============================================

		vector<bool> usou(numCaminhoes, false);

		for (int f = 0; f < numFrentes; f++)
			for (int c = 0; c < numCaminhoes; c++)
				if (polad.isCompatible(c, alocacoes->at(f)))
					usou[c] = usou[c] || ((*viagens)(f, c) > 0);

		//cout << "usou: " << usou << endl;

		fo_usou_q = 0;
		for (int c = 0; c < numCaminhoes; c++)
			if (usou[c])
				//fo_usou_q+=1.0*polad.l[c];
				fo_usou_q += 1.0;

		//cout << "fo_usou_q="<<fo_usou_q<<endl;

		// ============================================
		//          fo de qualidade
		// ============================================

		fo_q = fo_prod_min_q + fo_prod_est_q + fo_espec_q + fo_usou_q;

		//cout << "fo_q=" << fo_q << endl;

		// ============================================
		//          inviabilidade da producao
		// ============================================

		double theta_m_minus = 1000;
		double theta_m_plus = 1000;
		double theta_e_minus = 1000;
		double theta_e_plus = 1000;

		/*
		 cout << "Inv Prod: " << endl;
		 cout << polad.getPlMin() << " - " << min_prod << "=" << polad.getPlMin() - min_prod << endl;
		 cout << min_prod << " - " << polad.getPuMin() << "=" << min_prod - polad.getPuMin() << endl;
		 cout << polad.getPlEst() << " - " << est_prod << "=" << polad.getPlEst() - est_prod << endl;
		 cout << est_prod << " - " << polad.getPuEst() << "=" << est_prod - polad.getPuEst() << endl;
		 */

		fo_prod_inv = 0;
		fo_prod_inv += theta_m_minus * std::max(0.0, polad.getPlMin() - min_prod);
		fo_prod_inv += theta_m_plus * std::max(0.0, min_prod - polad.getPuMin());
		fo_prod_inv += theta_e_minus * std::max(0.0, polad.getPlEst() - est_prod);
		fo_prod_inv += theta_e_plus * std::max(0.0, est_prod - polad.getPuEst());

		//cout << "fo_prod_inv="<<fo_prod_inv<<endl;

		// ============================================
		//        inviabilidade dos parametros
		// ============================================

		vector<double> fi_minus(numParams, 1000);
		vector<double> fi_plus(numParams, 1000);

		vector<double> delta(numParams, 1);

		for (int par = 0; par < polad.numParams; par++)
			while (delta[par] * polad.getTrParam(par) < 10.0)
				delta[par] *= 10;

		fo_params_inv = 0;
		for (int p = 0; p < numParams; p++)
			fo_params_inv += delta[p] * fi_minus[p] * std::max(0.0, ((polad.getTlParam(p) / 100.0) * min_prod) - espec[p]);

		for (int p = 0; p < numParams; p++)
			fo_params_inv += delta[p] * fi_plus[p] * std::max(0.0, (espec[p] - (polad.getTuParam(p) / 100.0) * min_prod));

		//cout << "fo_params_inv="<<fo_params_inv<<endl;

		// ============================================
		//        inviabilidade dos caminhoes
		// ============================================

		double omega = 1000;

		vector<double> tUso(numCaminhoes, 0.0);

		for (int f = 0; f < numFrentes; f++)
			for (int c = 0; c < numCaminhoes; c++)
				if (polad.isCompatible(c, alocacoes->at(f)))
					tUso[c] += (*viagens)(f, c) * polad.getTempo(f);

		//cout << "tUso: "<<tUso<<endl;

		fo_cams_inv = 0;
		for (int c = 0; c < numCaminhoes; c++)
		{
			//cout << "Cam "<<c <<" worked for "<<tUso[c]<<" ("<<(tUso[c]/60.0)<<")" << ((tUso[c]/60.0)<=polad.getTxUtilCam()?" <= ":" > ")  << polad.getTxUtilCam() << endl;
			fo_cams_inv += omega * polad.getL(c) * std::max(0.0, ((tUso[c] / 60.0) - polad.getTxUtilCam()));
		}
		//cout << "fo_cams_inv="<<fo_cams_inv<<endl;

		// ============================================
		//      inviabilidade das carregadeiras
		// ============================================

		vector<double> theta_c_minus(numCarregs, 1000);
		vector<double> theta_c_plus(numCarregs, 1000);

		fo_carregs_inv = 0;

		for (int f = 0; f < numFrentes; f++)
			if (((*alocacoes)[f] >= 0) && (x[f]) > 0)
			{
				int k = (*alocacoes)[f];
				fo_carregs_inv += theta_c_minus[k] * std::max(0.0, (polad.getCMin(k) - x[f]));
				fo_carregs_inv += theta_c_plus[k] * std::max(0.0, (x[f] - polad.getCMax(k)));
			}

		//cout << "fo_carregs_inv="<<fo_carregs_inv<<endl;

		// ============================================
		//      inviabilidade das massas
		// ============================================

		double fo_qu_inv = 0;
		double gamma = 1000;

		for (int f = 0; f < polad.numFrentes; f++)
			if (x[f] > polad.qu[f])
				fo_qu_inv += (x[f] - polad.qu[f]) * gamma;

		// ============================================
		//          fo de inviabilidade
		// ============================================

		fo_inv = fo_prod_inv + fo_params_inv + fo_cams_inv + fo_carregs_inv + fo_qu_inv;

		/*
		 vector<double>* mem = new vector<double>(11);
		 (*mem)[0] = fo_q;
		 (*mem)[1] = fo_inv;

		 (*mem)[2] = fo_prod_min_q;
		 (*mem)[3] = fo_prod_est_q;
		 (*mem)[4] = fo_espec_q;
		 (*mem)[5] = fo_usou_q;

		 (*mem)[6] = fo_carregs_inv;
		 (*mem)[7] = fo_cams_inv;
		 (*mem)[8] = fo_prod_inv;
		 (*mem)[9] = fo_params_inv;
		 (*mem)[10] = fo_qu_inv;

		 vector<int>* mem1 = new vector<int>(2);
		 (*mem1)[0] = min_prod;
		 (*mem1)[1] = est_prod;

		 return new Evaluation<MemOPM>(fo_q,fo_inv,new pair<vector<int>*,vector<double>* >(mem1,mem));
		 */

		return *new EvaluationOPM(fo_q, fo_inv);
	}

	virtual bool betterThan(double a, double b)
	{
		// MINIMIZATION
		return (a < (b - EPSILON_OPM));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

	static string idComponent()
	{
		return "OptFrame:Evaluator:EFPEvaluator";
	}

	virtual string id() const
	{
		return idComponent();
	}

};
}
#endif /*OPM_EVALUATOR_HPP_*/

