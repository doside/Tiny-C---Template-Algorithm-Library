#pragma once
#include "core.h"
#include "find_val.h"

namespace Talg {

	struct default_t{};

	template<class T,class...Ts>
	struct AsImp {
		//利用断言而不是特化来排除非法情况从而有利于编译时间.
		static_assert(std::is_same<Seq<Ts...>, Seq<Seq<>>>::value, "");
		using type = T;
	};



	/*
		Alias并不是别名,其实是模板,此处刻意误用这个说法.
		举例而言,下述的map便是这样的“别名”.
		template<class...Ts>
		struct map {
			using entity=std::map<Ts...>; 
		};
	*/
	template<template<class...>class Alias,class...Us,class...Ts>
	struct AsImp<Alias<Us...>, Seq<default_t,Ts...>> {
		static constexpr size_t index = sizeof...(Us);

		using default_param = typename Transform<Alias,		//先利用别名模板参数的前面非default的部分生成
			Before<index, Alias<Us...>>						//真正的模板类实体从而获知默认参数的真实类型.
		>::entity;

		static_assert( staticCheck<NotValue<std::is_same<default_param, FailTrans>>,Us...>(),
			"this argument could not be default_t, note: arguments before it are Us... "
			"see instantation information about WithVal<Us...> for more details.");

		using type= typename AsImp<
			Transform<Alias, Before<index+1,default_param>>,//再把得到的真实类型提取出来又重新转移到别名模板上,然后依此类推.
			Seq<Ts...>
		>::type;
	};

	template<class T,template<class...>class Alias,class...Us,class...Ts>
	struct AsImp<Alias<Us...>, Seq<T,Ts...>> {
		using type= typename AsImp<
			Alias<Us...,T>,
			Seq<Ts...>
		>::type;
	};

	template<class Alias>
	using AsAlias = typename AsImp<Transform_t<Alias,Seq<>>, Seqfy<Alias>>::type::entity;

	template<class T>
	struct DefaultAlias {
		template<class...Ts>
		struct type {
			using entity = Transform_t<T, Seq<Ts...>>;
		};
	};

	//例:As<std::map<int,int,default_t,Alloc>>相当于std::map<int,int,std::less<int>,Alloc>
	template<class T>
	using As = AsAlias<  Transform<DefaultAlias<T>::template type,T>  >;


}//namespace Talg
