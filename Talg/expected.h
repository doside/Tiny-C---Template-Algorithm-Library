#pragma once
#include <type_traits>
#include <functional>
#include <stdexcept>
#include "basic_marco.h
#include "optional.h"


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
		MaybeError res;
		res.Base::assign([err=forward_m(exception)]{
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
struct EmptyValueError:public std::logic_error{};

//使用公有继承从而能提供直接获取异常的可能性.MaybeError是无法获取异常的.
template<class Res,class ErrorEmiter=MaybeError>
class Maybe:public ErrorEmiter { 
public:
	static_assert(std::is_reference<Res>::value, "Res shall not be rvalue reference.");
	using ret_t = Res; //假定Res为轻量级的类似指针的可空类型
	using Base = ErrorEmiter;
private:
	ret_t handle_;
public:
	Maybe():Base([]{ throw EmptyValueError<Maybe>{}; }) {}
	Maybe(ret_t res):Base(),handle_(res){} //此处不提供对handle_的空值检查因为那偶尔可能是所希望的情况
	Maybe(Base&& error_emiter):Base(std::move(error_emiter)),handle_(){}
	Maybe(const Base& error_emiter):Base(error_emiter),handle_(){}

	res_t& operator()() {
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return handle_;
	}
	res_t operator()()&&{
		if (Base::operator bool()) {
			Base::operator()(); //此处有可能抛出异常或者是其他行为.默认的MaybeError是会在此处抛出异常的
		}
		return std::move(handle_);
	}
	explicit operator bool()const 
	except_when(Base::operator bool()){
		return Base::operator bool();
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
	explicit operator bool()const 
	except_when(static_cast<bool>(handle_)){
		return ptr_;
	}
	~Maybe(){};
};






