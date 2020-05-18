
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

/*
template<
  class X
  //,
  //X& (*fInit)(X*) // cache pointer is only updated if necessary
  >
class lazy_ref final
{
private:
   X* val_cache{ nullptr }; // just used if 'val' reused
   X && (*fInit)(X*);       // cache pointer is only updated if necessary

public:
   lazy_ref(X && (*_fInit)(X*))
     : fInit{ _fInit }
   {
   }

public:
   X& get()
   {
      return this->val_cache ? *this->val_cache : fInit(this->val_cache);
   }
};

class MoveX
{
public:
   virtual lazy_ref<MoveX> apply(std::vector<int>& data) = 0;
   virtual void print() = 0;
};

class MyMoveX : public MoveX
{
public:
   int x, y;
   MyMoveX(int _x, int _y)
     : x(_x)
     , y(_y)
   {
   }

   lazy_ref<MoveX> apply(std::vector<int>& data) override
   {
      return lazy_ref<MoveX>{
         [](MoveX* cache_ptr) -> MoveX&& {
            //MyMoveX& mex = (MyMoveX&) *pthis;
            return std::move(MyMoveX(5, 10)); // "reverse" move
         }
      };
   }

   void print() override
   {
      std::cout << "MyMoveX(x=" << x << ";y=" << y << ")" << std::endl;
   }
};
*/
// the implementation above has issues with "this" pointer... using std::function below

//============

// this implementation has issues... cannot return && for usage... and cannot return copy of abstract Move.
// trying to escape from "heap" here. No "new" is allowed. How to do this?

/*

template<  class X  >
class lazy_ref final
{
private:
   X* val_cache{ nullptr }; // just used if 'val' reused
   //X && (*fInit)(X*);       // cache pointer is only updated if necessary
   std::function<X&&(X*)> fInit;       // cache pointer is only updated if necessary
   std::function<void(X*)> fPrint;       // cache pointer is only updated if necessary

public:
   lazy_ref(std::function<X&&(X*)>&& _fInit, std::function<void(X*)>&& _fPrint)
     : fInit{ _fInit }, fPrint(_fPrint)
   {
   }

public:
   X& get()
   {
      return this->val_cache ? *this->val_cache : fInit(this->val_cache);
   }

   // BAD IDEA
   X&& rget()
   {
      std::cout << "rget pval_cache=" << this->val_cache << std::endl;
      return this->val_cache ? std::move(*this->val_cache) : std::move(fInit(this->val_cache));
   }


};

class MoveX
{
public:
   virtual lazy_ref<MoveX> apply(std::vector<int>& data) = 0;
   virtual void print() = 0;
};

class MyMoveX : public MoveX
{
public:
   int x, y;
   MyMoveX(int _x, int _y)
     : x(_x)
     , y(_y)
   {
   }

   lazy_ref<MoveX> apply(std::vector<int>& data) override
   {
      return lazy_ref<MoveX>{
         [this](MoveX* cache_ptr) -> MoveX&& {
            //MyMoveX& mex = (MyMoveX&) *pthis;
            return std::move(MyMoveX(this->y+1, this->x+1)); // "reverse" move
         }
      };
   }

   void print() override
   {
      std::cout << "MyMoveX(x=" << x << ";y=" << y << ")" << std::endl;
   }
};

class GenMyMoveX
{
public:
   lazy_ref<MoveX> genMove()
   {    
      return lazy_ref<MoveX>{
         [this](MoveX* cache_ptr) -> MoveX&& {
            std::cout << "getting move!" << std::endl;
            return std::move(MyMoveX(10, 5)); // "reverse" move
         }
      };
   }
};
*/

// trying a radical idea: abstract (pure virtual) final base class
// since it's pure virtual (abstract), and also final, it would be unusable..

template<class X>
using op = std::optional<X>;

template<class XES = int>
class MoveXF
{
protected:
   using Self = MoveXF<XES>;
   //
   std::function<op<Self>(XES&)> fApply;
   std::function<void()> fPrint;

public:
   // slice copying
   MoveXF(const Self& m)
     : fApply(m.fApply)
     , fPrint(m.fPrint)
   {
   }

   // "empty" constructor
   MoveXF(std::function<void()> _fPrint, std::function<op<Self>(XES&)> _fApply)
     : fPrint(_fPrint)
     , fApply(_fApply)
   {
   }

protected:
   // do not keep this if "pure virtual"
   MoveXF() = delete;

public:
   // why do we need this?
   virtual ~MoveXF()
   {
   }

   virtual op<Self> apply(XES& se) final
   {
      return fApply(se);
   }

   virtual void print() final
   {
      fPrint();
   }
};

class MyMoveX final : public MoveXF<std::vector<int>>
{
   using XES = std::vector<int>;

public:
   MyMoveX(int x, int y)
     : MoveXF<XES>{ super(x, y) }
   {
   }

   static MoveXF<XES> super(int x, int y)
   {
      return MoveXF<XES>{
         //[x, y]() { std::cout << "x=" << x << ",y=" << y << std::endl; },
         [x, y]() { stPrint(x, y); }, // print x, y
         [x, y](XES& v) -> op<MoveXF<XES>> {
            // swap
            int aux = v[x];
            v[x] = v[y];
            v[y] = aux;
            return op<MoveXF<XES>>{
               MoveXF<XES>{
                 [x, y]() { stPrint(y, x); }, // print y, x
                 [x, y](XES& v) -> op<MoveXF<XES>> {
                    // swap
                    int aux = v[y];
                    v[y] = v[x];
                    v[x] = aux;
                    return nullopt; // no return
                 } }
            }; // end return
         }     // end apply
      };       // end 'super' constructor
   }           // end 'super' method

   static void stPrint(int x, int y)
   {
      std::cout << "x=" << x << ",y=" << y << std::endl;
   }
};

template<class XES>
class GenMyMoveX
{
public:
   MoveXF<XES> genMove()
   {
      return MyMoveX{ 5, 10 };
   }
};

int
main()
{
   MyMoveX mv{ 5, 10 };
   mv.print();
   MoveXF<std::vector<int>> sup{ MyMoveX::super(5, 10) };
   sup.print();

   return 0;
}


// ideas:
// lazy unique ref 
// deferred initialized ref/ptr
// lazy unique ptr
//
// none worked, because we actually needed to defeat the Slicing.
