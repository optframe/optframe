#ifndef HFMVRP_EVALUATOR_HPP_
#define HFMVRP_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_HFMVRP 0.0001

namespace HFMVRP {

class HFMVRPEvaluator: public Evaluator<RepHFMVRP, AdsHFMVRP> {
private:
	ProblemInstance& pHFMVRP;

	// Your private vars

public:
	HFMVRPEvaluator(ProblemInstance& _pHFMVRP) :
			pHFMVRP(_pHFMVRP) // If necessary, add more parameters

	{
		// Put the rest of your code here
	}

	virtual ~HFMVRPEvaluator() {
	}

	EvaluationHFMVRP& evaluate(const RepHFMVRP& rep) {
		// 'rep' is the representation of the solution

		double fo_q = 0;
		double fo_inv = 0;

		vector<double> routeDist;
		vector<double> routeCap;
		double fo_dist_q = 0; //Calcula o custo total da rota, todavia não inclui na FO
		double fo_fixed_cost_q = 0;
		double fo_dist_cost_q = 0;
		double fo_cap_q = 0; //Calcula o Empytness porém não inclui na FO
		double fo_stores = 0;
		double fo_cap_inv = 0;
		double fo_comp_inv = 0;

		for (int v = 0; v < rep.size(); v++) {
			vector<int> route = rep.at(v); // copia a rota
			routeDist.push_back(0);
			routeCap.push_back(0);

			if (route.size() >= 3) // two depots
					{
				int vehicleType = pHFMVRP.getVehicleType(v);
				if (v >= pHFMVRP.nVehicles) //MULTI-TRIP VEHICLE
					vehicleType = 2;
				int vehicleCap = pHFMVRP.getVehicleTypeCap(vehicleType);

				for (int i = 0; i < int(route.size() - 1); i++) {
					routeCap[v] += pHFMVRP.demands[route[i]];
					routeDist[v] += pHFMVRP.getDist(route[i], route[i + 1]);

					//comp
					if (!pHFMVRP.getComp(vehicleType, route[i])) {
						fo_comp_inv++;
					}

				}

				fo_stores += (route.size() - 2); //All clients, except two depots - (begin,..., end) - v[0,...,0]

				fo_fixed_cost_q += pHFMVRP.getVehicleTypeFixedCost(vehicleType);
				fo_dist_cost_q += pHFMVRP.getVehicleTypeDistCost(vehicleType)
						* routeDist[v];

				//Empytness
				if (routeCap[v] < (vehicleCap - pHFMVRP.theoricalCap))
					fo_cap_q += vehicleCap - pHFMVRP.theoricalCap - routeCap[v];

				//Inviabilidade e Empytness
				if (vehicleCap < routeCap[v]) {
					cout << "BUG! " << v << "\t vehicleCap = " << vehicleCap
							<< "\t routeCap[v] = " << routeCap[v] << endl;
					getchar();
					fo_cap_inv += routeCap[v] - vehicleCap;
				}

				fo_dist_q += routeDist[v];

			}
			//cout<<"routeDist["<<v<<"] = "<<routeDist[v]<<endl;

			/*
			 cout << "Vehicle[" << v << "] = " << pHFMVRP.getVehicleTypeFixedCost(
			 vehicleType) << "\t" << pHFMVRP.getVehicleTypeDistCost(
			 vehicleType) * routeDist[v] << "\t" << routeDist[v] << endl;
			 */
		}

		// ============================================
		//          fo de qualidade
		// ============================================

		if (fo_comp_inv > 0) {
			cout << "BUG fo_comp = " << fo_comp_inv << endl;
			getchar();
		}

		fo_q = fo_fixed_cost_q + fo_dist_cost_q + 8 * fo_stores;
		//fo_q = fo_fixed_cost_q + fo_dist_cost_q + fo_cap_q + fo_dist_q;

		/*
		 cout << "fo_fixed_cost_q = " << fo_fixed_cost_q << "\t fo_dist_cost_q = " << fo_dist_cost_q
		 << "\tfo_stores*8 = " << 8 * fo_stores << "\n fo_dist_q(X) = " << fo_dist_q
		 << "\t fo_cap_q(X) = " << fo_cap_q << "\tfo_cap_inv(XXXX) = " << fo_cap_inv << endl;
		 */
		// ============================================
		//          fo de inviabilidade
		// ============================================
		fo_inv = fo_cap_inv + fo_comp_inv;

		return *new EvaluationHFMVRP(fo_q, fo_inv);
	}

	void exportEvaluation(const RepHFMVRP& rep, string outFile) {
		double fo_q = 0;
		double fo_inv = 0;

		vector<double> routeDist;
		vector<double> routeCap;
		double fo_dist_q = 0; //Calcula o custo total da rota, todavia não inclui na FO
		double fo_fixed_cost_q = 0;
		double fo_dist_cost_q = 0;
		double fo_cap_q = 0; //Calcula o Empytness porém não inclui na FO
		double fo_stores = 0;
		double fo_cap_inv = 0;
		double fo_comp_inv = 0;

		for (int v = 0; v < rep.size(); v++) {
			vector<int> route = rep.at(v); // copia a rota
			routeDist.push_back(0);
			routeCap.push_back(0);

			if (route.size() >= 3) // two depots
					{
				int vehicleType = pHFMVRP.getVehicleType(v);
				if (v >= pHFMVRP.nVehicles) //MULTI-TRIP VEHICLE
					vehicleType = 2;
				int vehicleCap = pHFMVRP.getVehicleTypeCap(vehicleType);

				for (int i = 0; i < int(route.size() - 1); i++) {
					routeCap[v] += pHFMVRP.demands[route[i]];
					routeDist[v] += pHFMVRP.getDist(route[i], route[i + 1]);

					//comp
					if (!pHFMVRP.getComp(vehicleType, route[i])) {
						fo_comp_inv++;
					}

				}

				fo_stores += (route.size() - 2); //All clients, except two depots - (begin,..., end) - v[0,...,0]

				fo_fixed_cost_q += pHFMVRP.getVehicleTypeFixedCost(vehicleType);
				fo_dist_cost_q += pHFMVRP.getVehicleTypeDistCost(vehicleType)
						* routeDist[v];

				//Empytness
				if (routeCap[v] < (vehicleCap - pHFMVRP.theoricalCap))
					fo_cap_q += vehicleCap - pHFMVRP.theoricalCap - routeCap[v];

				//Inviabilidade e Empytness
				if (vehicleCap < routeCap[v]) {
					cout << "BUG! " << v << "\t vehicleCap = " << vehicleCap
							<< "\t routeCap[v] = " << routeCap[v] << endl;
					getchar();
					fo_cap_inv += routeCap[v] - vehicleCap;
				}

				fo_dist_q += routeDist[v];

			}
			//cout<<"routeDist["<<v<<"] = "<<routeDist[v]<<endl;

			/*
			 cout << "Vehicle[" << v << "] = " << pHFMVRP.getVehicleTypeFixedCost(
			 vehicleType) << "\t" << pHFMVRP.getVehicleTypeDistCost(
			 vehicleType) * routeDist[v] << "\t" << routeDist[v] << endl;
			 */
		}

		// ============================================
		//          fo de qualidade
		// ============================================

		if (fo_comp_inv > 0) {
			cout << "BUG fo_comp = " << fo_comp_inv << endl;
			getchar();
		}

		fo_q = fo_fixed_cost_q + fo_dist_cost_q + 8 * fo_stores;
		//fo_q = fo_fixed_cost_q + fo_dist_cost_q + fo_cap_q + fo_dist_q;

		/*
		 cout << "fo_fixed_cost_q = " << fo_fixed_cost_q << "\t fo_dist_cost_q = " << fo_dist_cost_q
		 << "\tfo_stores*8 = " << 8 * fo_stores << "\n fo_dist_q(X) = " << fo_dist_q
		 << "\t fo_cap_q(X) = " << fo_cap_q << "\tfo_cap_inv(XXXX) = " << fo_cap_inv << endl;
		 */
		// ============================================
		//          fo de inviabilidade
		// ============================================
		fo_inv = fo_cap_inv + fo_comp_inv;

		FILE* f = fopen(outFile.c_str(), "w");
		fprintf(f,
				"fo_fixed_cost_q = %.5f \t fo_dist_cost_q = %.5f \t fo_stores*8 = %.5f \n fo_dist_q(X) = %.5f \t fo_cap_q(X) = %.5f \t fo_cap_inv(XXX) = %.5f \n",
				fo_fixed_cost_q, fo_dist_cost_q, 8 * fo_stores, fo_dist_q,
				fo_cap_q, fo_cap_inv);
		fprintf(f, "fo_q = %.5f \t fo_inv = %.5f \n", fo_q, fo_inv);

		fclose(f);
	}
	void exportEXCEL(const RepHFMVRP& rep, string outFile) {
		int nDrives = 0;
		int nStores = 0;
		for (int v = 0; v < rep.size(); v++) {
			vector<int> route = rep.at(v); // copia a rota
			if (route.size() >= 3) {
				nDrives++;
				//nStores++;
				double sumDemands = 0;

				cout << v << "\t" << v << "\t" << pHFMVRP.getStoreID(route[1])
						<< "\t" << pHFMVRP.demands[route[1]] << endl;
				sumDemands += pHFMVRP.demands[route[1]];
				nStores++;
				for (int i = 2; i < (route.size() - 1); i++) {
					nStores++;
					cout << "\t\t" << pHFMVRP.getStoreID(route[i]) << "\t"
							<< pHFMVRP.demands[route[i]] << endl;
					sumDemands += pHFMVRP.demands[route[i]];
				}
				cout << "\t\t\t" << sumDemands << endl;
			}
		}

		cout << "nDrives = " << nDrives << "nStores = " << nStores << endl;

		FILE* f = fopen(outFile.c_str(), "w");
		for (int v = 0; v < rep.size(); v++) {
			vector<int> route = rep.at(v); // copia a rota
			if (route.size() >= 3) {
				//nStores++;
				double sumDemands = 0;
				fprintf(f, "%d\t%d\t%d\t%f\n", v, v,
						pHFMVRP.getStoreID(route[1]),
						pHFMVRP.demands[route[1]]);
				sumDemands += pHFMVRP.demands[route[1]];
				for (int i = 2; i < (route.size() - 1); i++) {
					fprintf(f, "\t\t%d\t%f\n", pHFMVRP.getStoreID(route[i]),
							pHFMVRP.demands[route[i]]);

					sumDemands += pHFMVRP.demands[route[i]];
				}
				fprintf(f, "\t\t\t%f\n", sumDemands);
			}
		}

		fprintf(f, " nDrives = %d \t nStores = %d \n", nDrives, nStores);
		fclose(f);
		//getchar();
	}

	virtual bool betterThan(double a, double b) {
		// MINIMIZATION
		return (a < (b - EPSILON_HFMVRP));
	}

	virtual bool isMinimization() const {
		return true;
	}

	static string idComponent() {
		return "OptFrame:Evaluator:HFMVRPEvaluator";
	}

	virtual string id() const {
		return idComponent();
	}

};

}

#endif /*HFMVRP_EVALUATOR_HPP_*/

