// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_IEVALUATOR_HPP_
#define OPTFRAME_IEVALUATOR_HPP_

#include <OptFrame/Concepts/BaseConcepts.hpp>

namespace optframe {

// Allows Single (XEv) and Multi (XMEv)
template <XESolution XES>
class IEvaluator  // NO INHERITANCE HERE!
{
 private:
  using _S = typename XES::first_type;
  using _XEv = typename XES::second_type;

 public:
  virtual _XEv evaluate(const _S&) = 0;
  // DO NOT PUT 'reevaluate' HERE (IT COMES FROM GeneralEvaluator)

  // DO NOT CREATE id() OR ANY COMPONENT-like FEATURE
  // THIS JUST WORKS TO COORDINATE TWO CLASSES
  // - Evaluator
  // - MultiEvaluator
  // THIS COULD BE REMOVED IN THE FUTURE
};

}  // namespace optframe

#endif  // OPTFRAME_IEVALUATOR_HPP_
