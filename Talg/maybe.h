#pragma once
#include <type_traits>
#include <functional>
#include <stdexcept>
#include <Talg/optional.h>
#include <Talg/basic_macro.h>

struct MaybeError:private std::function<void()>{
	using Base = std::function<void()>;
	using Base::Base;
	using Base::operator();
	using Base::operator bool;
	
	MaybeError() = default;
	MaybeError(MaybeError&&)=default;
	MaybeError(const MaybeError&)=default;

	template<class E>
	static MaybeError from_exception(E&& err) {
		MaybeError res([err=forward_m(err)]{
			throw err;
		});
		return res;
	}
	template<class E>
	static MaybeError from_exception() {
		MaybeError res([] {
			throw E();
		});
		return res;
	}
	template<class F>
	static MaybeError from_function(F&& func) {
		MaybeError res(forward_m(func));
		return res;
	}
};

template<class T>
struct EmptyValueError:public std::logic_error{
	EmptyValueError():logic_error("try to access empty value."){}
};


//使用公有继承从而能提供直接获取异常的可能性.MaybeError是无法获取异常的.
template<class Res,class ErrorEmiter=MaybeError>
class Maybe:public ErrorEmiter { 
public:
	static_assert(std::is_reference<Res>::value==false, "Res shall not be rvalue reference.");
	using Base = ErrorEmiter;
private:
	Res handle_;
public:
	Maybe():Base([]{ throw EmptyValueError<Maybe>{}; }) {}
	Maybe(Res res):Base(),handle_(std::move(res)){} //此处不提供对handle_的空值检查因为那偶尔可能是所希望的情况
	Maybe(Base&& error_emiter):Base(std::move(error_emiter)),handle_(){}
	Maybe(const Base& error_emiter):Base(error_emiter),handle_(){}
	Maybe(Maybe&&) = default;
	Maybe& operator=(Maybe&&) = delete;
	Maybe(std::nullptr_t) = delete;

	Res& operator()()& {
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return handle_;
	}
	Res operator()()&&{
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return std::move(handle_);
	}
	const Res& operator()()const & {
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return handle_;
	}
	operator Res&()&{
		return (*this)();
	}
	operator Res()&&{
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return std::move(handle_);
	}
	explicit operator bool()const 
	except_when_m(Base::operator bool()){
		return !Base::operator bool();
	}
	~Maybe(){};
};

template<class Res,class ErrorEmiter>
class Maybe<Res&,ErrorEmiter>:public ErrorEmiter { 
public:
	using Base = ErrorEmiter;
private:
	Res* ptr_=nullptr;
public:
	Maybe(): Base([]{ throw EmptyValueError<Maybe>{};}){}
	Maybe(Res& res):Base(),ptr_(&res){} 
	Maybe(Base&& error_emiter):Base(std::move(error_emiter)){}
	Maybe(const Base& error_emiter):Base(error_emiter){}
	Maybe(Maybe&&) = default;
	Maybe(const Maybe&) = delete;

	Res& operator()() {
		if (!ptr_) {
			throw EmptyValueError<Maybe>{};
		}
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return *ptr_;
	}
	const Res& operator()()const{
		if (!ptr_) {
			throw EmptyValueError<Maybe>{};
		}
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return *ptr_;
	}
	explicit operator bool()const noexcept{
		return ptr_;
	}
	~Maybe(){};
};



#include "undef_macro.h"


