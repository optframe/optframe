// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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


void PCAPSolCheck(PCAPProblemInstance& pPCAP, Solution<RepPCAP>& s) {
	vector<int> utilizacao;
	for (unsigned int i = 0; i < s.getR().first.size(); i++)
		utilizacao.push_back(0);

	for (unsigned int i = 0; i < s.getR().second.size(); i++) {
		int med = s.getR().second[i];

		int d = pPCAP.vecCidades[i].demanda;
		utilizacao[med] += d;
		cout<<"Cidade i= "<<i<<" Associada Facilidade: "<<s.getR().second[i]<<endl;
	}

	for (unsigned int i = 0; i < s.getR().first.size(); i++) {
		cout<<"Facilidade i: "<<i<<" eh CIDADE: "<<s.getR().first[i]<<endl;
	}



	for (unsigned int i = 0; i < s.getR().first.size(); i++) {
				if (utilizacao[i] > pPCAP.vecCidades[i].capacidade)
			cout << "Capacidade Estourada da mediana: " << s.getR().first[i]
					<< " Passaram: " << utilizacao[i]
					- pPCAP.vecCidades[i].capacidade << endl;

				//cout<<"Utilizacao = "<<utilizacao[i]<<" Capacidade= "<<pPCAP.vecCidades[i].capacidade<<endl;
	}


}

