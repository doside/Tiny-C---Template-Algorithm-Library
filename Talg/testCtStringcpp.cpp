	

//已弃坑

#include "ctstring.h"
#include "exString.h"
#include "type_traits.h"
#include "test_suits.h"
#include "fold.h"
#include "functional.h" //for logic_and


namespace {
	using namespace Talg;
	void test() {
		constexpr auto str=ctString<4>{"abc"}+ctString<4>{"abc"};
		constexpr ctString<7> str2{ "abcabc" };
		//constexpr auto str3 = str2.append(str2);
		TestConstexpr<
			str2==str2
			,(str2.size()>0) //防止把>当作模板的尖括号
			,str[0]=='a'
			,str==str
			,str2==str
			,str2[6]==0
		>{};
		constexpr ctArray<char, 5> val{{ 'a','b','c','d','e' }};
		constexpr ctArray<char, 5> val2{{ 'a','b','c' }};
		constexpr ctArray<char, 5> val3{{ 'a','b','c','\0','\0' }};
		constexpr ctArray<char, 6> val4{{}};
		constexpr ctArray<char, 6> val5{};
		static_assert(val != val2, "");
		static_assert(val2 == val3, "");
		static_assert(val[0] == val2[0], "");
		static_assert(val[1] == val2[1], "");
		static_assert(val[2] == val2[2], "");
		static_assert(val[3] != val2[3], "");
		static_assert(val[4] != val2[4], "");

		static_assert(val3[0] == val2[0], "");
		static_assert(val3[1] == val2[1], "");
		static_assert(val3[2] == val2[2], "");
		static_assert(val3[3] == val2[3], "");
		static_assert(val3[4] == val2[4], "");

		static_assert(val[0] == val3[0], "");
		static_assert(val[1] == val3[1], "");
		static_assert(val[2] == val3[2], "");
		static_assert(val[3] != val3[3], "");
		static_assert(val[4] != val3[4], "");

		constexpr auto val6 = val + val2;
		static_assert(val6==ctArray<char,10>{{
			'a','b','c','d','e',
			'a','b','c','\0','\0'
			}}, "");
		static_assert(val6[5] == 'a', "");
		static_assert(val6 == val6, "");
		
	}
}


#if 0
//旧的已经废弃的ctArray实现
template<class T,size_t N>
struct ctArray :ctArray<T, N - 1> {
	using Base = ctArray<T, N - 1>;
	static constexpr size_t length = N;
	T val;
	constexpr ctArray():Base(),val(){}
	constexpr ctArray(const ctArray& rhs) = default;

	template<size_t...Ids>
	constexpr ctArray(const ctArray<T, N-1>& lhs,const T& c, IdSeq<Ids...>)
		:Base(lhs[Ids+1]...,c), val(lhs.val){}

	constexpr Base to_base()const {
		return *this;
	}

	template<class U,class...Us>
	constexpr ctArray(const LimitTo<T,U>& first,const LimitTo<T,Us>&... args)
		:Base(forward_m(args)...),val(first){}

	constexpr T at(size_t id)const {
		return id == N-1 ? val : Base::at(id);
	}
	constexpr T operator[](size_t id)const {
		return id < N ? at(N - 1 - id) : throw std::runtime_error("index over range");
	}
	template<class U,size_t M>
	constexpr bool operator==(const ctArray<U,M>& rhs)const{
		return N!=M ?
			false:
			val == rhs.val && Base::operator==(static_cast<const ctArray<U, M - 1>&>(rhs));
	}
	template<class U,size_t M>
	constexpr bool operator!=(const ctArray<U,M>& rhs)const{
		return !operator==(rhs);
	}

};
template<class T>
struct ctArray<T, 1> {
	T val={};
	constexpr ctArray():val(){}
	constexpr ctArray(const T& v):val(v){}
	constexpr ctArray(T&& v):val(std::move(v)){}
	constexpr T at(size_t )const{
		return val;
	}
	constexpr T operator[](size_t )const{
		return val;
	}
	template<class U,size_t M>
	constexpr bool operator==(const ctArray<U,M>& rhs)const{
		return M != 1 ? false : val == rhs.val;
	}
	template<class U,size_t M>
	constexpr bool operator!=(const ctArray<U,M>& rhs)const{
		return M != 1 ? true : val != rhs.val;
	}
};


template<size_t N>
struct AppendCt {
	template<size_t M,class T>
	static constexpr auto call(const ctArray<T, M>& lhs, const ctArray<T, N>& rhs){
		return AppendCt<N-1>::call(
			ctArray<T, M + 1>(lhs, rhs.val,std::make_index_sequence<M-1>()),
			rhs.to_base()
		);
	}
};

template<>
struct AppendCt<1> {
	template<size_t M,class T>
	static constexpr auto call(const ctArray<T, M>& lhs, const ctArray<T, 1>& rhs){
		return ctArray<T, M + 1>(lhs, rhs.val,std::make_index_sequence<M-1>());
	}
};



		//constexpr ctArray<char, 6> val4(val3,'d',std::make_index_sequence<4>());
		//static_assert(val4[5] == 'd', "");
		//constexpr auto val5 = val2.to_base();
		//static_assert(val5 == val5, "");
		//constexpr auto val6 = AppendCt<5>::call(val, val2);
		//static_assert(val6==AppendCt<5>::call(val, val3), "");
		//static_assert(val6==ctArray<char,10>{
		//	'a','b','c','d','e',
		//	'a','b','c','\0','\0'
		//}, "");
		//static_assert(val6[5] == 'a', "");
		//static_assert(val6 == val6, "");

#endif
