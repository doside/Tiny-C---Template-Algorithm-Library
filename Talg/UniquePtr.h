#pragma once
#include <Talg/basic_macro_impl.h>
namespace Talg{
	

	/*!
		\brief	用来辅助unique_ptr的删除器类,
				从而正确地将C风格的资源回收函数用到unique_ptr中,
				充分利用unique_ptr的空基类优化.
		\tparam	\T value type \Dtor dtor type
		\note	遗憾地是没有办法模拟C++17的template<auto* ptr>,
		\example	if we wanted to do nothing when fclose failed,
					std::unique_ptr<FILE,CMemoryDtor<FILE,&fclose>>;
					or UniquePtr<FILE,decltype(&fclose),fclose>
	*/
	template<class T,class Dtor,Dtor* dtor>
	struct CMemoryDtor
	{
	   constexpr CMemoryDtor() noexcept = default;
	   void operator()(T* ptr) const
	   {
		  dtor(ptr);
	   }
	};


	template<class T,class Dtor,Dtor* dtor>
	using UniquePtr=std::unique_ptr<T, CMemoryDtor<T,Dtor,dtor>>;

	//specialized version for void(*dtor)(T*)
	//v means void.
	template<class T,void(*dtor)(T*)>
	using UniquePtr_v = std::unique_ptr<T, CMemoryDtor<T, void(T*),dotr>>;

	//specialized version for void(*dtor)(T*)
	//i means int.
	template<class T,int(*dtor)(T*)>
	using UniquePtr_i = std::unique_ptr<T, CMemoryDtor<T, int(T*),dotr>>;

	//specialized version for void(*dtor)(const T*)
	//k means knost, i.e. const.
	//v means void.
	template<class T,void(*dtor)(const T*)>
	using UniquePtr_vk = std::unique_ptr<T, CMemoryDtor<T, void(const T*),dotr>>;

	//specialized version for int(*dtor)(const T*)
	//k means knost, i.e. const.
	//i means int.
	template<class T,int (*dtor)(const T*)>
	using UniquePtr_ik = std::unique_ptr<T, CMemoryDtor<T, int(const T*),dotr>>;

	
	
	/*
	[[deprecated]]
	因为没有提供各种比较函数,以及各种潜在的设计错误必须被很仔细地考虑,
	该类已经废除.
	template<class T,void(*dtor)(T*)>
	struct UniquePtr: std::unique_ptr<T,CMemoryDtor<T,dtor>>{
		using Base=std::unique_ptr<T,CMemoryDtor<T,dtor>>;
		template<class U>
		using rebind = UniquePtr<U, dtor>;


		template<class U,void(*other_del)(U*)>
		UniquePtr& operator=(UniquePtr<U,other_del>&& )=delete;
		
		template<class U>
		UniquePtr& operator=(U)=delete;
		
		template<class U,class E>
		UniquePtr(std::unique_ptr<U,E>)=delete;
		
		template<class U>
		UniquePtr(U )=delete;	
		

		constexpr UniquePtr()noexcept:Base() { }
		constexpr explicit UniquePtr(T* val): Base(val){}
		explicit UniquePtr(T* val, std::function<void()> error_check) {
			//Shall check the val then could assign it to base,or undefine behaviour will happen.
			if (val == nullptr) {
				error_check();
			}
			this->reset(val);
		}
		UniquePtr(Base&& rval)noexcept:Base(std::move(rval)) {}

		UniquePtr& operator=(UniquePtr&& ptr) {
			Base::operator=(std::move(ptr));
			return *this;
		}
		void swap(UniquePtr& rhs)noexcept{
			Base::swap(rhs);
		}
	};*/



}

#include <Talg/undef_macro.h>
