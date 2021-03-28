#include <gsl/gsl> // not_null (local copy)
#include <memory>  // shared_ptr

// Not Null Shared Pointer
// Authors: Igor M. Coelho & Fellipe Pessanha
// MIT License (2021)

namespace nn {
//
template<typename T>
class shared
{
   using shared_type = T;

public:
   gsl::not_null<std::shared_ptr<T>> data_;

   // this constructor can be used to "move" into new shared_ptr versions
   // this requires a copy constructor over the type X (must be concrete)
   template<class X,
            typename = typename std::enable_if<
              std::is_convertible<X*, T*>::value>::type,
            typename = typename std::enable_if<
              std::is_copy_constructible<X>::value>::type>
   shared(X&& other)
     : data_{ std::shared_ptr<T>{ new X(other) } }
   {
   }

   // this is for existing references (must have copy constructor)
   template<class X,
            typename = typename std::enable_if<
              std::is_convertible<X*, T*>::value>::type,
            typename = typename std::enable_if<
              std::is_copy_constructible<X>::value>::type>
   shared(const X& other)
     : data_{ std::shared_ptr<T>{ new X(other) } }
   {
   }

   shared(const shared<T>& other)
     : data_{ other.data_ }
   {
   }

   shared(const shared<T>&& corpse)
     : data_{ corpse.data_ }
   {
   }

   shared(std::shared_ptr<T>& data)
     : data_{ data }
   {
   }

   shared(T* data)
     : data_{ std::shared_ptr<T>{ data } }
   {
   }

   // disallow explicit nullptr
   shared(std::nullptr_t data) = delete;

   T* operator->()
   {
      return data_.get().get();
   }

   T& operator*()
   {
      // return dereferenced shared object
      return *data_;
   }

   T& get()
   {
      // return dereferenced shared object
      return *data_;
   }

   operator T&()
   {
      // return dereferenced shared object
      return *data_;
   }

   template<class Y,
            typename = typename std::enable_if<
              std::is_convertible<T*, Y*>::value>::type>
   operator shared<Y>() // explicit? not necessary... (until now)
   {
      std::shared_ptr<Y> py = data_.get(); // remove encapsulation from not_null
      return py;
   }
};

} // namespace nn