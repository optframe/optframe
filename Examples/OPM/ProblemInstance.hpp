#ifndef OPM_PROBLEMINSTANCE_HPP_
#define OPM_PROBLEMINSTANCE_HPP_

#include <iostream>
#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Util/Matrix.hpp"
#include "../../OptFrame/Util/printable.h"

using namespace std;

namespace POLAD
{

class OPMProblemInstance
{
public:
	OPMProblemInstance(Scanner& scanner)
	{
		numFrentesMin = 0;
		numFrentesEst = 0;

		for (int i = 0; i < 10; i++)
			cout << scanner.nextLine() << endl;

		cout << scanner.next() << " ";
		pl_est = scanner.nextDouble();
		pr_est = scanner.nextDouble();
		pu_est = scanner.nextDouble();
		cout << pl_est << " " << pr_est << " " << pu_est << endl;

		cout << scanner.next() << " ";
		pl_min = scanner.nextDouble();
		pr_min = scanner.nextDouble();
		pu_min = scanner.nextDouble();
		cout << pl_min << " " << pr_min << " " << pu_min << endl;

		for (int i = 0; i < 8; i++)
			cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // param
		cout << scanner.next() << " "; // wnp
		cout << scanner.next() << " "; // :=
		cout << scanner.next() << " "; // esteril

		wnp_est = scanner.nextDouble();
		cout << wnp_est << endl;

		cout << scanner.next() << " "; // minerio
		wnp_min = scanner.nextDouble();
		cout << wnp_min << endl;

		cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // param
		cout << scanner.next() << " "; // wpp
		cout << scanner.next() << " "; // :=
		cout << scanner.next() << " "; // esteril

		wpp_est = scanner.nextDouble();
		cout << wpp_est << endl;

		cout << scanner.next() << " "; // minerio
		wpp_min = scanner.nextDouble();
		cout << wpp_min << endl;

		for (int i = 0; i < 4; i++)
			cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // set
		cout << scanner.next() << " "; // Parametros
		cout << scanner.next() << " "; // :=

		numParams = 0;
		string param = scanner.next();

		while (param != ";")
		{
			numParams++;
			cout << param << " ";
			param = scanner.next();
		}

		cout << endl << scanner.nextLine() << endl;

		for (int p = 0; p < numParams; p++)
		{
			cout << scanner.next() << " ";
			double _tu = scanner.nextDouble();
			double _tr = scanner.nextDouble();
			double _tl = scanner.nextDouble();

			tu.push_back(_tu);
			tr.push_back(_tr);
			tl.push_back(_tl);

			cout << _tu << " " << _tr << " " << _tl << endl;
		}
		cout << "tu: " << tu << endl;
		cout << "tr: " << tr << endl;
		cout << "tl: " << tl << endl;

		for (int i = 0; i < 4; i++)
			cout << scanner.nextLine() << endl;

		for (int p = 0; p < numParams; p++)
		{
			cout << scanner.next() << " ";
			double _wnm = scanner.nextDouble();
			double _wpm = scanner.nextDouble();

			wnm.push_back(_wnm);
			wpm.push_back(_wpm);

			cout << _wnm << " " << _wpm << endl;
		}
		cout << "wnm: " << wnm << endl;
		cout << "wpm: " << wpm << endl;

		for (int i = 0; i < 5; i++)
			cout << scanner.nextLine() << endl;

		numFrentes = 0;
		string frente = scanner.next();

		while (frente != ";")
		{
			numFrentes++;
			cout << frente << endl;
			frente = scanner.next();
		}

		cout << endl << scanner.nextLine() << endl;
		cout << scanner.nextLine() << endl;

		for (int p = 0; p < numFrentes; p++)
		{
			cout << scanner.next() << " ";
			double _qu = scanner.nextDouble();
			bool _isMin = scanner.nextInt();

			qu.push_back(_qu);
			isMin.push_back(_isMin);

			_isMin ? numFrentesMin++ : numFrentesEst++; // adicionei isso

			cout << _qu << " " << _isMin << endl;
		}
		cout << "qu: " << qu << endl;
		cout << "isMin: " << isMin << endl;

		for (int i = 0; i < 4; i++)
			cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // set
		cout << scanner.next() << " "; // Carregadeiras
		cout << scanner.next() << " "; // :=

		numCarregs = 0;
		string carreg = scanner.next();

		while (carreg != ";")
		{
			numCarregs++;
			cout << carreg << " ";
			carreg = scanner.next();
		}

		cout << endl << scanner.nextLine() << endl;
		cout << scanner.nextLine() << endl;

		for (int g = 0; g < numCarregs; g++)
		{
			cout << scanner.next() << " ";
			double _cMin = scanner.nextDouble();
			double _cMax = scanner.nextDouble();

			cMin.push_back(_cMin);
			cMax.push_back(_cMax);

			cout << _cMin << " " << _cMax << endl;
		}
		cout << "cMin: " << cMin << endl;
		cout << "cMax: " << cMax << endl;

		for (int i = 0; i < 4; i++)
			cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // set
		cout << scanner.next() << " "; // Caminhoes
		cout << scanner.next() << " "; // :=

		numCaminhoes = 0;
		string caminhao = scanner.next();

		while (caminhao != ";")
		{
			numCaminhoes++;
			cout << caminhao << endl;
			caminhao = scanner.next();
		}

		cout << scanner.nextLine() << endl;

		cout << scanner.next() << " "; // param
		cout << scanner.next() << " "; // txUtilCam
		cout << scanner.next() << " "; // :=
		txUtilCam = scanner.nextDouble();
		cout << txUtilCam << endl;

		cout << scanner.nextLine() << endl;
		cout << scanner.nextLine() << endl;

		for (int c = 0; c < numCaminhoes; c++)
		{
			cout << scanner.next() << " ";
			double _l = scanner.nextDouble();

			l.push_back(_l);

			cout << _l << endl;
		}

		cout << "l: " << l << endl;

		// =======================
		//       Matrizes
		// =======================

		for (int i = 0; i < 5; i++)
			cout << scanner.nextLine() << endl;

		teor = new Matrix<double> (numFrentes, numParams);

		for (int f = 0; f < numFrentes; f++)
		{
			cout << scanner.next() << " ";

			for (int p = 0; p < numParams; p++)
			{
				double _teor = scanner.nextDouble();
				cout << _teor << " ";

				(*teor)(f, p) = _teor;
			}

			cout << endl;
		}

		cout << "teor: " << (*teor);

		cout << scanner.nextLine() << endl;
		cout << scanner.nextLine() << endl;

		comp = new Matrix<bool> (numCaminhoes, numCarregs);

		for (int c = 0; c < numCaminhoes; c++)
		{
			cout << scanner.next() << " ";

			for (int g = 0; g < numCarregs; g++)
			{
				bool _comp = scanner.nextInt();
				cout << _comp << " ";

				(*comp)(c, g) = _comp;
			}

			cout << endl;
		}

		cout << "comp: " << (*comp);

		cout << scanner.nextLine() << endl;
		cout << scanner.nextLine() << endl;

		for (int f = 0; f < numFrentes; f++)
		{
			cout << scanner.next() << " ";
			double _tempo = scanner.nextDouble();

			tempo.push_back(_tempo);

			cout << _tempo << endl;
		}

		cout << "tempo: " << tempo << endl;

		cout << endl << "Fim leitura." << endl;
	}

	int getNumFrentes()
	{
		return numFrentes;
	}

	int getNumFrentesEsteril()
	{
		return numFrentesEst;
	}

	int getNumFrentesMinerio()
	{
		return numFrentesMin;
	}

	bool getIsMin(int f)
	{
		return isMin[f];
	}

	double getPrMin()
	{
		return pr_min;
	}
	double getPlMin()
	{
		return pl_min;
	}
	double getPuMin()
	{
		return pu_min;
	}

	double getPrEst()
	{
		return pr_est;
	}
	double getPlEst()
	{
		return pl_est;
	}
	double getPuEst()
	{
		return pu_est;
	}

	double getWnpMin()
	{
		return wnp_min;
	}
	double getWppMin()
	{
		return wpp_min;
	}
	double getWnpEst()
	{
		return wnp_est;
	}
	double getWppEst()
	{
		return wpp_est;
	}

	double getTempo(int f)
	{
		return tempo[f];
	}

	int getNumCaminhoes()
	{
		return numCaminhoes;
	}
	double getL(int c)
	{
		return l[c];
	}
	double getTxUtilCam()
	{
		return txUtilCam;
	}

	int getNumCarregs()
	{
		return numCarregs;
	}
	double getCMin(int k)
	{
		return cMin[k];
	}
	double getCMax(int k)
	{
		return cMax[k];
	}

	int getNumParams()
	{
		return numParams;
	}
	double getTrParam(int p)
	{
		return tr[p];
	}
	double getTlParam(int p)
	{
		return tl[p];
	}
	double getTuParam(int p)
	{
		return tu[p];
	}

	double getWnm(int p)
	{
		return wnm[p];
	}
	double getWpm(int p)
	{
		return wpm[p];
	}

	bool isCompatible(int c, int g)
	{
		if (c < 0 || g < 0 || c > comp->getNumRows() || g > comp->getNumCols())
			return false;

		//cout << "asking for isCompatible ("<<c<<","<<g<<") ";
		//cout << "limits ["<<comp->getRows()<<","<<comp->getCols()<<"]" << endl;

		return (*comp)(c, g);
	}

	double getTeor(int f, int p)
	{
		return (*teor)(f, p);
	}

	//private:
	// Producao
	double pl_min, pr_min, pu_min;
	double pl_est, pr_est, pu_est;

	double wnp_min, wpp_min;
	double wnp_est, wpp_est;

	// Parametros de controle
	int numParams;
	vector<double> tu, tr, tl;
	vector<double> wnm, wpm;

	// Frentes
	int numFrentes;
	int numFrentesEst;
	int numFrentesMin;
	vector<double> qu;
	vector<bool> isMin;

	// Carregadeiras
	int numCarregs;
	vector<double> cMin, cMax;

	// Caminhoes
	int numCaminhoes;
	double txUtilCam;
	vector<double> l;

	// Matrizes
	Matrix<double>* teor;
	Matrix<bool>* comp;

	vector<double> tempo;
};

}
#endif /*OPM_PROBLEMINSTANCE_HPP_*/

