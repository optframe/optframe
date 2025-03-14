#ifndef OPM_PROBLEMINSTANCE_HPP_
#define OPM_PROBLEMINSTANCE_HPP_

#include <iostream>

//
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace optframe;

namespace POLAD {

class OPMProblemInstance
{
public:
   OPMProblemInstance(Scanner& scanner)
   {
      numFrentesMin = 0;
      numFrentesEst = 0;

      for (int i = 0; i < 10; i++)
         std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " ";
      pl_est = *scanner.nextDouble();
      pr_est = *scanner.nextDouble();
      pu_est = *scanner.nextDouble();
      std::cout << pl_est << " " << pr_est << " " << pu_est << std::endl;

      std::cout << scanner.next() << " ";
      pl_min = *scanner.nextDouble();
      pr_min = *scanner.nextDouble();
      pu_min = *scanner.nextDouble();
      std::cout << pl_min << " " << pr_min << " " << pu_min << std::endl;

      for (int i = 0; i < 8; i++)
         std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // param
      std::cout << scanner.next() << " "; // wnp
      std::cout << scanner.next() << " "; // :=
      std::cout << scanner.next() << " "; // esteril

      wnp_est = *scanner.nextDouble();
      std::cout << wnp_est << std::endl;

      std::cout << scanner.next() << " "; // minerio
      wnp_min = *scanner.nextDouble();
      std::cout << wnp_min << std::endl;

      std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // param
      std::cout << scanner.next() << " "; // wpp
      std::cout << scanner.next() << " "; // :=
      std::cout << scanner.next() << " "; // esteril

      wpp_est = *scanner.nextDouble();
      std::cout << wpp_est << std::endl;

      std::cout << scanner.next() << " "; // minerio
      wpp_min = *scanner.nextDouble();
      std::cout << wpp_min << std::endl;

      for (int i = 0; i < 4; i++)
         std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // set
      std::cout << scanner.next() << " "; // Parametros
      std::cout << scanner.next() << " "; // :=

      numParams = 0;
      string param = scanner.next();

      while (param != ";") {
         numParams++;
         std::cout << param << " ";
         param = scanner.next();
      }

      std::cout << std::endl
           << scanner.nextLine() << std::endl;

      for (int p = 0; p < numParams; p++) {
         std::cout << scanner.next() << " ";
         double _tu = *scanner.nextDouble();
         double _tr = *scanner.nextDouble();
         double _tl = *scanner.nextDouble();

         tu.push_back(_tu);
         tr.push_back(_tr);
         tl.push_back(_tl);

         std::cout << _tu << " " << _tr << " " << _tl << std::endl;
      }
      std::cout << "tu: " << tu << std::endl;
      std::cout << "tr: " << tr << std::endl;
      std::cout << "tl: " << tl << std::endl;

      for (int i = 0; i < 4; i++)
         std::cout << scanner.nextLine() << std::endl;

      for (int p = 0; p < numParams; p++) {
         std::cout << scanner.next() << " ";
         double _wnm = *scanner.nextDouble();
         double _wpm = *scanner.nextDouble();

         wnm.push_back(_wnm);
         wpm.push_back(_wpm);

         std::cout << _wnm << " " << _wpm << std::endl;
      }
      std::cout << "wnm: " << wnm << std::endl;
      std::cout << "wpm: " << wpm << std::endl;

      for (int i = 0; i < 5; i++)
         std::cout << scanner.nextLine() << std::endl;

      numFrentes = 0;
      string frente = scanner.next();

      while (frente != ";") {
         numFrentes++;
         std::cout << frente << std::endl;
         frente = scanner.next();
      }

      std::cout << std::endl
           << scanner.nextLine() << std::endl;
      std::cout << scanner.nextLine() << std::endl;

      for (int p = 0; p < numFrentes; p++) {
         std::cout << scanner.next() << " ";
         double _qu = *scanner.nextDouble();
         bool _isMin = *scanner.nextInt();

         qu.push_back(_qu);
         isMin.push_back(_isMin);

         _isMin ? numFrentesMin++ : numFrentesEst++; // adicionei isso

         std::cout << _qu << " " << _isMin << std::endl;
      }
      std::cout << "qu: " << qu << std::endl;
      std::cout << "isMin: " << isMin << std::endl;

      for (int i = 0; i < 4; i++)
         std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // set
      std::cout << scanner.next() << " "; // Carregadeiras
      std::cout << scanner.next() << " "; // :=

      numCarregs = 0;
      string carreg = scanner.next();

      while (carreg != ";") {
         numCarregs++;
         std::cout << carreg << " ";
         carreg = scanner.next();
      }

      std::cout << std::endl
           << scanner.nextLine() << std::endl;
      std::cout << scanner.nextLine() << std::endl;

      for (int g = 0; g < numCarregs; g++) {
         std::cout << scanner.next() << " ";
         double _cMin = *scanner.nextDouble();
         double _cMax = *scanner.nextDouble();

         cMin.push_back(_cMin);
         cMax.push_back(_cMax);

         std::cout << _cMin << " " << _cMax << std::endl;
      }
      std::cout << "cMin: " << cMin << std::endl;
      std::cout << "cMax: " << cMax << std::endl;

      for (int i = 0; i < 4; i++)
         std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // set
      std::cout << scanner.next() << " "; // Caminhoes
      std::cout << scanner.next() << " "; // :=

      numCaminhoes = 0;
      string caminhao = scanner.next();

      while (caminhao != ";") {
         numCaminhoes++;
         std::cout << caminhao << std::endl;
         caminhao = scanner.next();
      }

      std::cout << scanner.nextLine() << std::endl;

      std::cout << scanner.next() << " "; // param
      std::cout << scanner.next() << " "; // txUtilCam
      std::cout << scanner.next() << " "; // :=
      txUtilCam = *scanner.nextDouble();
      std::cout << txUtilCam << std::endl;

      std::cout << scanner.nextLine() << std::endl;
      std::cout << scanner.nextLine() << std::endl;

      for (int c = 0; c < numCaminhoes; c++) {
         std::cout << scanner.next() << " ";
         double _l = *scanner.nextDouble();

         l.push_back(_l);

         std::cout << _l << std::endl;
      }

      std::cout << "l: " << l << std::endl;

      // =======================
      //       Matrizes
      // =======================

      for (int i = 0; i < 5; i++)
         std::cout << scanner.nextLine() << std::endl;

      teor = new Matrix<double>(numFrentes, numParams);

      for (int f = 0; f < numFrentes; f++) {
         std::cout << scanner.next() << " ";

         for (int p = 0; p < numParams; p++) {
            double _teor = *scanner.nextDouble();
            std::cout << _teor << " ";

            (*teor)(f, p) = _teor;
         }

         std::cout << std::endl;
      }

      std::cout << "teor: " << (*teor);

      std::cout << scanner.nextLine() << std::endl;
      std::cout << scanner.nextLine() << std::endl;

      comp = new Matrix<bool>(numCaminhoes, numCarregs);

      for (int c = 0; c < numCaminhoes; c++) {
         std::cout << scanner.next() << " ";

         for (int g = 0; g < numCarregs; g++) {
            bool _comp = *scanner.nextInt();
            std::cout << _comp << " ";

            (*comp)(c, g) = _comp;
         }

         std::cout << std::endl;
      }

      std::cout << "comp: " << (*comp);

      std::cout << scanner.nextLine() << std::endl;
      std::cout << scanner.nextLine() << std::endl;

      for (int f = 0; f < numFrentes; f++) {
         std::cout << scanner.next() << " ";
         double _tempo = *scanner.nextDouble();

         tempo.push_back(_tempo);

         std::cout << _tempo << std::endl;
      }

      std::cout << "tempo: " << tempo << std::endl;

      std::cout << std::endl
           << "Fim leitura." << std::endl;
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
      //cout << "limits ["<<comp->getRows()<<","<<comp->getCols()<<"]" << std::endl;

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
