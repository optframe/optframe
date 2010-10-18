#ifndef TESTSOLUTION_HPP_
#define TESTSOLUTION_HPP_

#include "../Solution.h"

#include <cstdlib>
#include <iostream>

using namespace std;

template<class R>
class TestSolution : public Solution<R>
{
private:
	R* representacao;

	// Contagem de objetos na memoria - Anti-leak
	static int objetos_solucao;

	// Contagem de objetos na memoria - Sem retorno de numero (toString)
	static int objetos_solucao_nocancel;

	// Aviso de Numero de objetos alocados na memoria - Anti-leak
	static int obj_solucao_aviso;

	// Numero maximo de objetos permitidos na memoria
	static const int MAX_OBJETOS = 250;

	// Numero da solucao
	int num_solucao;

public:

	TestSolution(R* representacao);	
	~TestSolution();

	//
	//Solution interface
	//

	Solution<R>* clone();	
	void print();

	R* getRepresentation();
	void setRepresentation(R*);

}; 


// Inicializacao da contagem de objetos da classe Solucao<R>
template<class R>
int TestSolution<R>::objetos_solucao = 0;

// Contagem de objetos na memoria - Sem retorno de numero (toString)
template<class R>
int TestSolution<R>::objetos_solucao_nocancel = 0;

// Inicializacao do aviso de contagem de objetos
template<class R>
int TestSolution<R>::obj_solucao_aviso = 150;

template<class R>
TestSolution<R>::TestSolution(R* representacao)
{
	// =========================
	// Objeto alocado na memoria
	// =========================
	objetos_solucao++;
	objetos_solucao_nocancel++;
	// =========================

	// =============================
	// Teste de vazamento de memoria
	// =============================
	if(objetos_solucao >= obj_solucao_aviso)
	{
		cout << "AVISO: " << TestSolution<R>::objetos_solucao << " objetos TestSolution alocados na memoria!" << endl;
		TestSolution<R>::obj_solucao_aviso++;
	}

	if(objetos_solucao >= MAX_OBJETOS)
	{
		cout << "ERRO: " << objetos_solucao << " objetos TestSolution alocados na memoria! MAX = " << MAX_OBJETOS << endl;
		exit(1);
	}
	// =============================

	this->representacao = representacao;
	this->num_solucao = objetos_solucao_nocancel;
}

template<class R>
TestSolution<R>::~TestSolution<R>()
{
	if(representacao) // if not null
		delete representacao;

	// ===========================
	// Objeto removido da memoria
	// ===========================
	objetos_solucao--;
	// ===========================

	//cout << objetos_solucao << " Objetos Alocados" << endl;
}

template<class R>
Solution<R>* TestSolution<R>::clone()
{
	R* nova = new R(*representacao);

	return new TestSolution<R>(nova);
}

template<class R>
void TestSolution<R>::print()
{
	cout << "Solucao (#"<<num_solucao<<" de "<<objetos_solucao<<"): ";
	cout << *(representacao) << endl;
}

template<class R>
R* TestSolution<R>::getRepresentation()
{
	return this->representacao;
}

template<class R>
void TestSolution<R>::setRepresentation(R* r)
{
	this->representacao = r;
}

/*
template<class R>
bool TestSolution<R>::operator== (Solution<R>* s) const
{

	return false;
}
 */

#endif /*TESTSOLUTION_HPP_*/
