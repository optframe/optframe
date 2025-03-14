#pragma once

// C
#include <assert.h>
// C++
#include <functional>
#include <string>
#include <utility>
// OptFrame C++
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>
// OptFrameLib C base
#include <OptFrameLib/LibCTypes.h>

class FCoreLibSolution {
 public:
  // 'solution_ptr' is internal representation/solution pointer
  // note that the pointer can be a view (is_view=1) or an owned reference that
  // needs decref (is_view=0)
  FakePythonObjPtr solution_ptr;
  // is view: when is_view=1, no decref is needed
  bool is_view;
  // copy constructor function
  std::function<FakePythonObjPtr(FakePythonObjPtr)> f_sol_deepcopy;
  // print/to string function
  std::function<size_t(FakePythonObjPtr, char*, size_t)> f_sol_tostring;
  // utils function
  std::function<bool(FakePythonObjPtr)> f_utils_decref;

  FCoreLibSolution(const FCoreLibSolution& s) {
    // std::cout << "FCoreLibSolution(COPY)" << std::endl;
    assert(s.solution_ptr);
    assert(!s.is_view);  // because of deepcopy anyway... COULD we copy a view
                         // here? I don't think so..
    // std::cout << "\tFCoreLibSolution(COPY)-> will copy functions" <<
    // std::endl;
    //  copy functions
    this->f_sol_deepcopy = s.f_sol_deepcopy;
    this->f_sol_tostring = s.f_sol_tostring;
    this->f_utils_decref = s.f_utils_decref;
    // copy flags
    this->is_view = s.is_view;
    // std::cout << "\tFCoreLibSolution(COPY)-> will deepcopy" << std::endl;
    //  perform deepcopy and IncRef
    this->solution_ptr = this->f_sol_deepcopy(s.solution_ptr);
    // std::cout << "\tFCoreLibSolution(COPY; ptr1_origin=" << s.solution_ptr <<
    // "; ptr2_dest=" << this->solution_ptr << ")" << std::endl;
  }

  FCoreLibSolution& operator=(const FCoreLibSolution& other) {
    if (this == &other) return *this;

    assert(other.solution_ptr);
    assert(!other.is_view);  // because of deepcopy anyway... COULD we copy a
                             // view here? I don't think so..
    // copy functions
    this->f_sol_deepcopy = other.f_sol_deepcopy;
    this->f_sol_tostring = other.f_sol_tostring;
    this->f_utils_decref = other.f_utils_decref;
    // copy flags
    this->is_view = other.is_view;

    // will decref current copy of solution
    if (!this->is_view) {
      // must decref solution_ptr and discard it
      int x = f_utils_decref(solution_ptr);
      if (x > 1) {
        // std::cout << "operator=(FCoreLibSolution) ptr_count = " << x <<
        // std::endl;
      }
    }
    solution_ptr = nullptr;

    // perform deepcopy and IncRef
    this->solution_ptr = this->f_sol_deepcopy(other.solution_ptr);
    return *this;
  }

  FCoreLibSolution& operator=(FCoreLibSolution&& other) {
    // std::cout << "ERROR! Could not find a use-case for move solution... is
    // there one?" << std::endl; assert(false);
    if (this == &other) return *this;

    assert(other.solution_ptr);
    // copy functions
    this->f_sol_deepcopy = other.f_sol_deepcopy;
    this->f_sol_tostring = other.f_sol_tostring;
    this->f_utils_decref = other.f_utils_decref;
    // copy flags
    this->is_view = other.is_view;

    // steal pointer from corpse
    this->solution_ptr = other.solution_ptr;
    // kill it
    other.solution_ptr = 0;
    other.is_view = 1;
    return *this;
  }

  FCoreLibSolution(FCoreLibSolution&& s) {
    // std::cout << "FCoreLibSolution(MOVE) BEGIN" << std::endl;
    assert(s.solution_ptr);
    // std::cout << "FCoreLibSolution(MOVE).other.toString() -> '" <<
    // s.toString() << "'" << std::endl;

    // copy flags
    this->is_view = s.is_view;
    if (!s.is_view) {
      // std::cout << "\tNOT_VIEW! FCoreLibSolution(MOVE)-> will move functions"
      // << std::endl;
      //  copy functions OR MOVE???? TODO....
      this->f_sol_deepcopy = std::move(s.f_sol_deepcopy);
      this->f_sol_tostring = std::move(s.f_sol_tostring);
      this->f_utils_decref = std::move(s.f_utils_decref);
    }
    // std::cout << "\tFCoreLibSolution(MOVE)-> will steal pointer" <<
    // std::endl;
    this->solution_ptr = s.solution_ptr;
    // prepare corpse
    s.solution_ptr = 0;
    s.is_view = true;
    //
    // std::cout << "\tFCoreLibSolution(MOVE).toString() -> '" << toString() <<
    // "'" << std::endl; std::cout << "\tFCoreLibSolution(MOVE finished; ptr="
    // << solution_ptr << ")" << std::endl; std::cout << "FCoreLibSolution(MOVE)
    // ENDS" << std::endl;
  }

  virtual ~FCoreLibSolution() {
    // std::cout << "~FCoreLibSolution is_view = " << this->is_view << " ptr: "
    // << solution_ptr << std::endl;
    //
    if (!this->is_view) {
      assert(solution_ptr);
      // must decref solution_ptr and discard it
      int x = f_utils_decref(solution_ptr);
      // std::cout << "~FCoreLibSolution ptr_count = " << x << std::endl;
      if (x > 1) {
        // std::cout << "~FCoreLibSolution ptr_count = " << x << std::endl;
      }
    }
    solution_ptr = nullptr;
    // std::cout << "~FCoreLibSolution finished" << std::endl;
  }

  // FCoreLibSolution(FakePythonObjPtr solution_ptr,
  // std::function<FakePythonObjPtr(FakePythonObjPtr)> copy_solution)
  FCoreLibSolution(
      FakePythonObjPtr solution_ptr,
      std::function<FakePythonObjPtr(FakePythonObjPtr)> f_sol_deepcopy,
      std::function<size_t(FakePythonObjPtr, char*, size_t)> f_sol_tostring,
      std::function<int(FakePythonObjPtr)> f_utils_decref)
      : solution_ptr{solution_ptr},
        is_view{false},
        f_sol_deepcopy{f_sol_deepcopy},
        f_sol_tostring{f_sol_tostring},
        f_utils_decref{f_utils_decref}  //, copy_solution{ copy_solution }
  {
    // printf("FCoreLibSolution3(%p, func, func, func) is_view=%d\n",
    // solution_ptr, is_view); std::string str = toString(); std::cout <<
    // "\tFCoreLibSolution3->C++ str: '" << str << "'" << std::endl;
  }

  // temporary construction (no copy_solution required)

  FCoreLibSolution(FakePythonObjPtr solution_ptr_view)
      : solution_ptr{solution_ptr_view}, is_view{true} {
    // printf("FCoreLibSolution1(%p) is_view=%d\n", solution_ptr, is_view);
  }

  FakePythonObjPtr releasePtr() {
    // pointer must exist
    assert(this->solution_ptr);
    // cannot take this from view
    assert(!this->is_view);
    FakePythonObjPtr sol = this->solution_ptr;
    // "move" from this container
    this->solution_ptr = nullptr;
    this->is_view = true;
    // TODO: do we need to remove the functions too?
    return sol;
  }

  std::string toString() const {
    // std::cout << "WILL PRINT! is_view=" << is_view << " ptr=" << solution_ptr
    // << std::endl;
    constexpr int max_buffer = 10'000;  // TODO: MAKE THIS FLEXIBLE!!!
    //
    std::string str_buffer(max_buffer, '\0');
    // std::cout << "size = " << str_buffer.size() << std::endl;
    // std::cout << "str_buffer => '" << str_buffer << "'" << std::endl;
    //
    char* s_ptr = &str_buffer[0];
    int sz = f_sol_tostring(solution_ptr, s_ptr, str_buffer.size());
    assert(sz < max_buffer);
    // std::cout << "toString spent sz=" << sz << " from max=" << max_buffer <<
    // std::endl;
    std::string str_ret(s_ptr, s_ptr + sz);
    // assert(str_ret.size() == sz);
    // std::cout << "finished print!" << std::endl;
    return str_ret;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const FCoreLibSolution& me) {
    os << me.toString();
    return os;
  }
};

using FCoreLibESolution =
    std::pair<FCoreLibSolution, optframe::Evaluation<double>>;

using FCoreLibEMSolution =
    std::pair<FCoreLibSolution, optframe::MultiEvaluation<double>>;
