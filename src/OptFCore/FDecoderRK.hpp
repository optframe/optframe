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

#ifndef OPTFRAME_FCORE_FDECODER_RK_HPP_
#define OPTFRAME_FCORE_FDECODER_RK_HPP_

#include <functional>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/DecoderRandomKeys.hpp>

namespace optframe {

template<
  XSolution S,
  XEvaluation XEv,
  optframe::comparability KeyType,
  MinOrMax Minimizing>
class FDecoderRK final : public DecoderRandomKeys<S, XEv, KeyType>
{
   using RSK = std::vector<KeyType>;

public:
   pair<XEv, S> (*fDecode)(const RSK& rk); // decode function

   FDecoderRK(
     pair<XEv, S> (*_fDecode)(const RSK& rk)
   )
     : fDecode{ _fDecode }
   {
   }

   virtual ~FDecoderRK()
   {
   }

   virtual pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override
   {
      auto p = fDecode(rk);
      return make_pair(p.first, make_optional(p.second));
   }

   virtual bool isMinimization() const override
   {
      return Minimizing == MinOrMax::MINIMIZE;
   }
};

} // namespace optframe

#endif /*OPTFRAME_FCORE_FDECODER_RK_HPP_*/
