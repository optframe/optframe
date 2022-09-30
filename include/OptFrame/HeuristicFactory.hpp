// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef HEURISTICFACTORY_HPP_
#define HEURISTICFACTORY_HPP_

#include <iostream>
#include <string>

#include "MultiESolution.hpp"
#include "MultiSolution.hpp"
#include "Solution.hpp"

#include "./Scanner++/Scanner.hpp"

#include "OptFrameList.hpp"

#include "RandGen.hpp"

#include "Component.hpp"
#include "ComponentHelper.hpp"

#include "Action.hpp"

#include "LocalSearch.hpp"
#include "MultiObjSearch.hpp"
#include "SingleObjSearch.hpp"

#include "Heuristics/Empty.hpp"

//Heuristics
#include "Heuristics/LocalSearches/BestImprovement.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/FirstImprovement.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"
//#include "Heuristics/LocalSearches/RVND.hpp"

//Metaheuristics
#include "Heuristics/EmptySingleObjSearch.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
//TODO ERROR on IntensifiedIteratedLocalSearchLevels
//#include "Heuristics/ILS/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristics/GRASP/BasicGRASP.hpp"
#include "Heuristics/ILS/Intensification.hpp"
#include "Heuristics/TS/BasicTabuSearch.hpp"
//TODO ERROR on BasicGeneticAlgorithm
//#include "Heuristics/EvolutionaryAlgorithms/BasicGeneticAlgorithm.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"

#include "Heuristics/EmptyMultiObjSearch.hpp"
//#include "Heuristics/VNS/MOVNS.hpp"

#include "ComponentBuilder.h"

#include "Solutions/CopySolution.hpp"

#include "Component.hpp"

//using namespace std;
//using namespace optframe; (?????????????????) Don't use namespace declarations in headers

// design pattern: Factory

namespace optframe {

//template<XRepresentation R, class ADS = _ADS, XBaseSolution<S, XEv> S = CopySolution<S, XEv>, XEvaluation XEv = Evaluation<>>
//template<class R, class ADS, XSolution S>
//template<XSolution S, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class HeuristicFactory
{
private:
   // for debug purposes (mainly 'memory debug')
   LogLevel loglevel;
   // default random number generator
   sref<RandGen> rg;

public:
   map<string, vector<sptr<Component>>> components;
   vector<ComponentBuilder<S, XEv, XES, X2ES>*> builders;
   vector<Action<S, XEv, XES, X2ES>*> actions;
   map<string, vector<vector<sptr<Component>>>> componentLists;

   HeuristicFactory(LogLevel _loglevel = LogLevel::Warning)
     : loglevel{ _loglevel }
     , rg{ new RandGen }
   {
   }

   HeuristicFactory(sref<RandGen> _rg, LogLevel _loglevel = LogLevel::Warning)
     : loglevel{ _loglevel }
     , rg{ _rg }
   {
   }

   virtual ~HeuristicFactory()
   {
      clear();

      //delete &rg; // shared reference 'sref'

      for (unsigned i = 0; i < builders.size(); i++)
         delete builders.at(i);
      builders.clear();

      for (unsigned i = 0; i < actions.size(); i++)
         delete actions.at(i);
      actions.clear();
   }

   void setLogLevel(LogLevel ll)
   {
      this->loglevel = ll;
      // must apply loglevel to all builders
      for (unsigned i = 0; i < builders.size(); i++) {
         if (ll == LogLevel::Debug)
            std::cout << "Setting builder LogLevel to Debug: " << builders[i]->id() << std::endl;
         builders[i]->setMessageLevel(ll);
      }
   }

   ComponentBuilder<S, XEv, XES, X2ES>* getBuilder(string id)
   {
      for (unsigned i = 0; i < builders.size(); i++)
         if (builders[i]->id() == id)
            return builders[i];
      return nullptr;
   }

   bool inComponents(sptr<Component> c)
   {
      map<std::string, vector<sptr<Component>>>::iterator iter;
      for (iter = components.begin(); iter != components.end(); iter++) {
         vector<sptr<Component>> v = iter->second;

         for (unsigned int i = 0; i < v.size(); i++)
            if (v[i] == c)
               return true;
      }

      return false;
   }

   /// compName is an optional reference return value
   /// compNumber is an optional reference return value
   sptr<Component> getNextComponent(Scanner& scanner, string* compName = nullptr, int* compNumber = nullptr)
   {
      if (!scanner.hasNext())
         return nullptr;

      std::string id = scanner.next();

      if (!scanner.hasNextInt())
         return nullptr;

      int inumber = *scanner.nextInt();

      if (inumber < 0)
         return nullptr;

      unsigned number = inumber;

      sptr<Component> component = nullptr;

      if (id[0] == ':') {
         // COMPONENT SHORTCUT!
         // look for pattern
         map<std::string, vector<std::shared_ptr<Component>>>::iterator iter;
         for (iter = components.begin(); iter != components.end(); iter++) {
            string name = iter->first;

            int p = name.find(id, 0);
            if ((p > 0) && (p + id.length() == name.length())) // exact match after position 'p'
            {
               component = iter->second.at(number);
               id = name;
               break;
            }
         }
         if (!component) {
            if (loglevel >= LogLevel::Warning)
               std::cout << "HeuristicFactory warning: pattern of component '" << id << " " << number << "' not found" << std::endl;
         }
      } else {
         // look for exact
         if (components.count(id) > 0) {
            vector<sptr<Component>> v = components[id];

            if (number < v.size())
               component = v[number];
         } else {
            if (loglevel >= LogLevel::Warning)
               std::cout << "HeuristicFactory warning: component '" << id << " " << number << "' not found!" << std::endl;
         }
      }

      if (compName && compNumber) {
         (*compName) = id;
         (*compNumber) = number;
      }

      return component;
   }

   template<class T>
   void assign(std::shared_ptr<T>& component, unsigned number, string id)
   {
      // NOTE THAT component is likely to be NULL!!
      if (loglevel >= LogLevel::Debug) {
         std::cout << "Debug: hf will try to assign component '" << (component ? component->id() : "nullptr");
         std::cout << "' with id = '" << id << " #" << number << std::endl;
      }
      // check prefix "OptFrame:"
      if (id[0] != 'O') {
         string id2 = id;
         id = "OptFrame:";
         id.append(id2);
      }

      if (!ComponentHelper::compareBase(T::idComponent(), id)) {
         if (loglevel >= LogLevel::Warning)
            std::cout << "HeuristicFactory: incompatible assign '" << T::idComponent() << "' <- '" << id << "'" << std::endl;

         component = nullptr;
         return;
      }

      if (components.count(id) > 0) {
         vector<sptr<Component>>& v = components[id];
         if (number < v.size()) {
            //component = std::shared_ptr<T>((T*)v[number].get()); // need to cast to type T...
            component = std::shared_ptr<T>((std::shared_ptr<T>&)v[number]); // need to cast to type T...
            return;
         }
      } else {
         if (loglevel >= LogLevel::Warning)
            std::cout << "'" << id << "' not found!" << std::endl;
      }

      // not found!
      component = nullptr;
   }

   template<class T>
   void assignList(vector<std::shared_ptr<T>>& cList, unsigned number, string _listId)
   {
      // type checking for safety!
      string noList = ComponentHelper::typeOfList(_listId);
      string listId = noList;
      listId += "[]";

      if (!ComponentHelper::compareBase(T::idComponent(), noList)) {

         if (loglevel >= LogLevel::Warning)
            std::cout << "HeuristicFactory: incompatible list assign '[" << T::idComponent() << "]' <- '[" << noList << "]'" << std::endl;

         return;
      }

      if (componentLists.count(listId) > 0) {
         vector<vector<sptr<Component>>>& vv = componentLists[listId];
         if (number < vv.size())
            for (unsigned i = 0; i < vv[number].size(); i++)
               cList.push_back(sptr<T>((std::shared_ptr<T>&)vv[number][i]));
      } else {
         if (loglevel >= LogLevel::Warning)
            std::cout << "'" << listId << " " << number << "' not found!" << std::endl;
      }
   }

   int addComponent(sref<Component> component, string id)
   {
      // NO NEED FOR DOUBLE POINTER WARNING ANYMORE... LONG LIVE SHARED_PTR!!!
      /*
      if (inComponents(component.sptr())) {
         if (loglevel >= LogLevel::Warning)
            std::cout << "WARNING: HeuristicFactory addComponent: component '" << component->id() << "' already registered!" << std::endl;
         //return -1;
      }
      */

      if (!component->compatible(id)) {

         if (loglevel >= LogLevel::Warning) {
            std::cout << "HeuristicFactory addComponent: incompatible components '";
            std::cout << component->id() << "' and '" << id << "'! " << std::endl;
            std::cout << "Rejecting component '" << component->toString() << "'." << std::endl;
         }

         return -1;
      }

      vector<std::shared_ptr<Component>>& v = components[id];
      std::shared_ptr<Component> scomp = component.sptr();
      v.push_back(scomp);

      int idx = components[id].size() - 1;

      //cout << "HeuristicFactory: added component '" << id << " " << idx << "'" << endl;

      return idx;
   }

   int addComponentRef(Component& component)
   {
      return addComponent(component, component.id());
   }

   template<class T>
   void readComponent(T*& component, Scanner& scanner)
   {
      std::string tmp = scanner.next();

      if (tmp != T::idComponent()) {

         if (loglevel >= LogLevel::Error)
            std::cout << "Error: expected '" << T::idComponent() << "' and found '" << tmp << "'." << std::endl;

         component = nullptr;

         return;
      }

      unsigned int number = *scanner.nextInt();

      component = nullptr;

      assign(component, number, tmp);
   }

   int addComponentListRef(vsref<Component>& cList, string _listId)
   {
      vector<sptr<Component>> sptrList;
      for (unsigned i = 0; i < cList.size(); i++)
         sptrList.push_back(cList[i].sptr());
      return addComponentList(sptrList, _listId);
   }

   int addComponentList(vector<sptr<Component>>& cList, string _listId)
   {
      // type checking for safety!
      string noList = ComponentHelper::typeOfList(_listId);
      string listId = noList;
      listId += "[]";

      for (unsigned i = 0; i < cList.size(); i++)
         if ((cList[i] == nullptr) || (!cList[i]->compatible(noList))) {

            if (loglevel >= LogLevel::Warning) {
               std::cout << "Warning: incompatible components '";
               std::cout << cList[i]->id() << "' and '" << ComponentHelper::typeOfList(listId) << "'!" << std::endl;
            }

            return -1;
         }

      vector<vector<sptr<Component>>>& v = componentLists[listId];
      v.push_back(cList);

      if (loglevel >= LogLevel::Info)
         std::cout << "HeuristicFactory: adding to list id '" << listId << "'" << std::endl;

      int idx = componentLists[listId].size() - 1;

      //cout << "HeuristicFactory: added component list '" << listId << " " << idx << "'" << endl;

      return idx;
   }

   int addComponentList(vector<Component*>& cList)
   {
      if ((cList.size() > 0) && (cList[0] != nullptr)) {
         string listId = cList[0]->id();
         listId += "[]";

         return addComponentList(cList, listId);
      } else
         return -1;
   }

   //! \english listComponents lists all available components that match a given pattern. \endenglish \portuguese listComponents lista todos componentes disponiveis que coincidem com um padrao dado. \endportuguese
   /*!
		 \sa listComponents(string)
	 */
   vector<string> listComponents(string pattern)
   {
      vector<string> list;

      map<std::string, vector<std::shared_ptr<Component>>>::iterator iter;

      for (iter = components.begin(); iter != components.end(); iter++) {
         //
         std::vector<std::shared_ptr<Component>> v = iter->second;
         //vector<Component*> v = iter->second;

         for (unsigned int i = 0; i < v.size(); i++)
            if (ComponentHelper::compareBase(pattern, v[i]->id())) {
               stringstream ss;
               ss << iter->first << " " << i;
               list.push_back(ss.str());
            }
      }

      return list;
   }

   //! \english listAllComponents lists all available OptFrame components. \endenglish \portuguese listAllComponents lista todos os componentes do OptFrame disponiveis. \endportuguese
   /*!
		 \sa listAllComponents()
	 */

   vector<string> listAllComponents()
   {
      return listComponents("OptFrame:");
   }

   //! \english listComponents lists all available components that match a given pattern. \endenglish \portuguese listComponents lista todos componentes disponiveis que coincidem com um padrao dado. \endportuguese
   /*!
		 \sa listComponents(string)
	 */
   vector<string> listComponentLists(string pattern)
   {
      if (loglevel >= LogLevel::Debug)
         std::cout << "listing component lists for pattern = '" << pattern << "'" << std::endl;
      vector<string> list;

      map<std::string, vector<vector<sptr<Component>>>>::iterator iter;

      for (iter = componentLists.begin(); iter != componentLists.end(); iter++) {
         vector<vector<std::shared_ptr<Component>>>& vl = iter->second;

         for (unsigned int i = 0; i < vl.size(); i++)
            if (ComponentHelper::compareBase(pattern, iter->first)) {
               stringstream ss;
               ss << iter->first << " " << i;
               list.push_back(ss.str());
            }
      }

      return list;
   }

   //! \english listBuilders lists all component builders that match a given pattern, with their respective parameters. \endenglish \portuguese listBuilders lista todos component builders, com seus respectivos parametros, que coincidem com um padrao dado. \endportuguese
   /*!
		 \sa listComponents(string)
	 */

   vector<pair<string, vector<pair<string, string>>>> listBuilders(string pattern)
   {
      vector<pair<string, vector<pair<string, string>>>> list;

      for (unsigned i = 0; i < builders.size(); i++)
         if (ComponentHelper::compareBase(pattern, builders[i]->id()))
            list.push_back(make_pair(builders[i]->id(), builders[i]->parameters()));

      return list;
   }

   // ================================================================
   // ================================================================

   bool drop(string type, int id)
   {
      if (components.count(type) > 0) {
         vector<sptr<Component>> v = components[type];

         if (id < ((int)v.size())) // else return false?
         {
            if (v[id] != nullptr) // else return false?
            {
               //delete v[id];
               v[id] = nullptr;
               components[type] = v;

               return true;
            }
         }
      }

      return false;
   }

   // EMPTY ALL LISTS!
   // Components, Builders and Actions
   void clear()
   {
      map<std::string, vector<std::shared_ptr<Component>>>::iterator iter;

      for (iter = components.begin(); iter != components.end(); iter++) {
         vector<std::shared_ptr<Component>>& v = iter->second;

         for (unsigned int i = 0; i < v.size(); i++) {
            if (loglevel >= LogLevel::Debug)
               std::cout << "HF: will clear v.toString()=" << v[i]->toString() << std::endl;
            //delete v[i];
            v[i] = nullptr;
         }

         if (loglevel >= LogLevel::Debug)
            std::cout << "    => HF: CLEARING COMPONENT: '" << iter->first << "'" << std::endl;
         //
         // TODO: MUST KEEP LINE BELOW!
         //iter->second.clear();
         v.clear();
      }

      map<std::string, vector<vector<sptr<Component>>>>::iterator iter2;

      for (iter2 = componentLists.begin(); iter2 != componentLists.end(); iter2++) {

         vector<vector<std::shared_ptr<Component>>>& v = iter2->second;

         for (unsigned int i = 0; i < v.size(); i++)
            //delete v[i];
            v[i].clear();

         // Should not delete the components inside lists. They're already deleted!
         iter2->second.clear();
      }
   }

   sref<RandGen> getRandGen()
   {
      return rg;
   }

   pair<sptr<LocalSearch<XES, XEv>>, std::string> createLocalSearch(std::string str)
   {
      Scanner scanner(str);

      // No heuristic!
      if (!scanner.hasNext())
         return pair<sptr<LocalSearch<XES, XEv>>, std::string>(nullptr, "");

      string h = scanner.next();

      if ((h == LocalSearch<XES, XEv>::idComponent()) || (h == "LocalSearch")) {
         unsigned int id = *scanner.nextInt();

         sptr<LocalSearch<XES, XEv>> mtd = nullptr;

         assign(mtd, id, LocalSearch<XES, XEv>::idComponent());

         if (!mtd)
            return pair<sptr<LocalSearch<XES, XEv>>, std::string>(new EmptyLocalSearch<XES, XEv>, scanner.rest());

         return make_pair(mtd, scanner.rest());
      }

      if (h == EmptyLocalSearch<XES, XEv>::idComponent())
         return pair<sptr<LocalSearch<XES, XEv>>, std::string>(new EmptyLocalSearch<XES, XEv>, scanner.rest());

      for (unsigned i = 0; i < builders.size(); i++) {
         // build local search directly by builder name
         if (builders[i]->id() == h) {
            LocalSearch<XES, XEv>* ls = ((LocalSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
            return pair<sptr<LocalSearch<XES, XEv>>, std::string>(ls, scanner.rest());
         }

         // locate builder by local search name
         if (builders[i]->canBuild(h)) {
            LocalSearch<XES, XEv>* ls = ((LocalSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
            return pair<sptr<LocalSearch<XES, XEv>>, std::string>(ls, scanner.rest());
         }
      }

      if (loglevel >= LogLevel::Warning)
         std::cout << "HeuristicFactory::createLocalSearch warning: no LocalSearch '" << h << "' found! ignoring..." << std::endl;

      return pair<sptr<LocalSearch<XES, XEv>>, std::string>(nullptr, scanner.rest());
   }

   pair<sptr<SingleObjSearch<XES>>, std::string> createSingleObjSearch(std::string str)
   {
      Scanner scanner(str);

      // No heuristic!
      if (!scanner.hasNext())
         return pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr, "");

      string h = scanner.next();

      if (h == SingleObjSearch<XES>::idComponent()) {
         unsigned int id = *scanner.nextInt();

         sptr<SingleObjSearch<XES>> mtd = nullptr;

         assign(mtd, id, SingleObjSearch<XES>::idComponent());

         if (!mtd)
            return pair<sptr<SingleObjSearch<XES>>, std::string>(new EmptySingleObjSearch<XES, XEv>, scanner.rest());

         return pair<sptr<SingleObjSearch<XES>>, std::string>(mtd, scanner.rest());
      }

      if (h == EmptySingleObjSearch<XES, XEv>::idComponent())
         return pair<sptr<SingleObjSearch<XES>>, std::string>(new EmptySingleObjSearch<XES, XEv>, scanner.rest());

      for (unsigned i = 0; i < builders.size(); i++) {
         // build local search directly by builder name
         if (builders[i]->id() == h) {
            SingleObjSearch<XES>* sios = ((SingleObjSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
            return pair<sptr<SingleObjSearch<XES>>, std::string>(sios, scanner.rest());
         }

         // locate builder by local search name
         if (builders[i]->canBuild(h)) {
            SingleObjSearch<XES>* sios = ((SingleObjSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
            return pair<sptr<SingleObjSearch<XES>>, std::string>(sios, scanner.rest());
         }
      }

      if (loglevel >= LogLevel::Warning)
         std::cout << "HeuristicFactory::createSingleObjSearch warning: no SingleObjSearch '" << h << "' found! ignoring..." << std::endl;

      return pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr, scanner.rest());
   }

   pair<MultiObjSearch<XES>*, std::string> createMultiObjSearch(std::string str)
   {
      Scanner scanner(str);

      // No heuristic!
      if (!scanner.hasNext())
         return pair<MultiObjSearch<XES>*, std::string>(nullptr, "");

      string h = scanner.next();

      if (h == MultiObjSearch<XES>::idComponent()) {
         unsigned int id = *scanner.nextInt();

         MultiObjSearch<XES>* mtd = nullptr;

         assign(mtd, id, MultiObjSearch<XES>::idComponent());

         if (!mtd)
            return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

         return make_pair(mtd, scanner.rest());
      }

      if (h == EmptyMultiObjSearch<XES>::idComponent())
         return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

      if (loglevel >= LogLevel::Warning)
         std::cout << "HeuristicFactory::createMultiObjSearch warning: no MultiObjSearch '" << h << "' found! ignoring..." << std::endl;

      return pair<MultiObjSearch<XES>*, std::string>(nullptr, scanner.rest());
   }
};

} // namespace optframe

#endif /* HEURISTICFACTORY_HPP_ */
