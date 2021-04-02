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

   shared<T> operator=(const shared<T>& other) {
      if(this == &other)
         return *this; // self-check
      this->get() = other.get();
      return *this;
   }

   T* operator->()
   {
      return data_.get().get();
   }

   const T* operator->() const
   {
      return data_.get().get();
   }

   T& operator*()
   {
      // return dereferenced shared object
      return *data_;
   }

   const T& operator*() const
   {
      // return dereferenced shared object
      return *data_;
   }

   T& get()
   {
      // return dereferenced shared object
      return *data_;
   }

   const T& get() const
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

   // get copy as a std::shared_ptr
   std::shared_ptr<T> sptr() const
   {
      std::shared_ptr<T> py = data_.get(); // remove encapsulation from not_null
      return py;
   }
};

} // namespace nn