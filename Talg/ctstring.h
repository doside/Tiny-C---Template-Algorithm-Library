#pragma once
#include "core.h"
#include "index_seq.h"
#include "fold.h"
#include "type_traits.h"
#include "fold.h"
#include "functional.h"
#include "initlist.h"
#include "basic_marco_impl.h"

namespace Talg{

	template<class T,size_t N>
	struct ctArray{
		static constexpr size_t length = N;
		InitList_t<T, N> data;
		constexpr ctArray(const ctArray&)noexcept = default;
		constexpr ctArray()noexcept:data{}{}
		//template<class...Ts,class=
		//	std::enable_if_t<
		//		AndValue<IsDecaySame<T,Ts>...>::value
		//	>
		//>
		//constexpr ctArray(Ts&&...args):data{forward_m(args)...}{}
		constexpr ctArray(const InitList_t<T,N>& list)
			:data(list){}
		template<size_t L,size_t L2,size_t...I,size_t...I2>
		constexpr ctArray(const ctArray<T,L>& lhs,const ctArray<T,L2>& rhs,IdSeq<I...>,IdSeq<I2...>)
			:data{lhs[I]...,rhs[I2]...}{}

		constexpr decltype(auto) operator[](size_t id)const {
			return data[id];
		}
		template<size_t M>
		constexpr bool operator==(const ctArray<T, M>& rhs)const {
			return false;
		}
		constexpr bool operator==(const ctArray& rhs)const {
			return compare(rhs, std::make_index_sequence<N>());
		}
		template<size_t... I>
		constexpr bool compare(const ctArray& rhs,IdSeq<I...>)const{
			return foldl(logical_and{}, data[I] == rhs[I]...);
		}
		template<size_t M>
		constexpr bool operator!=(const ctArray<T, M>& rhs)const {
			return !(operator==(rhs));
		}
		constexpr size_t size()const noexcept { return N; }
		
	};

	template<class A,class T>
	constexpr size_t find(const A& array,T&& val,size_t cur=0) {
		return (cur==array.size()?
					no_index:
				(array[cur]==forward_m(val)?
					cur:
					find(array,forward_m(val),cur+1)));
	}

	template<class T,class...Ts>
	constexpr auto makeArray(Ts&&...args) {
		return ctArray<T, sizeof...(args)>{{forward_m(args)...}};
	}

	template<class T,size_t L,size_t M>
	constexpr auto operator+(const ctArray<T,L>& lhs,const ctArray<T,M>& rhs){
		return ctArray<T, M + L>(lhs, rhs, std::make_index_sequence<L>(), std::make_index_sequence<M>());
	}
	template<class T,size_t L>
	constexpr auto operator+(const ctArray<T,L>& lhs,const T& rhs){
		return ctArray<T, L+1>(lhs, ctArray<T,1>{{rhs}}, std::make_index_sequence<L>(), IdSeq<0>());
	}
	template<class T,size_t N,size_t L,size_t... I>
	constexpr auto slice(const ctArray<T,N>& lhs,Tagi<L>,IdSeq<I...>){
		return ctArray<T, L>{{lhs[I]...}};
	}
	template<size_t M,class T,size_t N>
	constexpr auto slice(const ctArray<T,N>& lhs){
		return slice(lhs,Tagi<M>(),std::make_index_sequence<M>());
	}

	struct ctStringBase {
		static constexpr size_t npos = no_index;
	};

	template<size_t L>
	class ctString:public ctStringBase{
	public:
		using size_type = std::size_t;
		ctArray<char, L> data;
	private:
		template<size_t N,size_t...Index>
		constexpr ctString(const char (&str)[N], IdSeq<Index...>&&)noexcept
		:data{{str[Index]...}} {}
		constexpr ctString(const ctArray<char,L>& rhs)noexcept:data(rhs){}
	public:	
		constexpr ctString(const ctString<L>&args) = default;

		template<size_t M,size_t N>
		constexpr ctString(const ctString<M>& lhs,const ctString<N>& rhs)
		:data(slice<M-1>(lhs.data) + rhs.data){}
	
		template<size_t N>
		constexpr ctString(const char(&str)[N])
		:ctString(str,std::make_index_sequence<N>()){
			static_assert(N <= L, "");
		}
	
		constexpr decltype(auto) operator[](size_type index)const  {
			return data[index];
		}

		constexpr bool operator==(const ctString& rhs)const noexcept {
			return data == rhs.data;
		}
		template<size_t M>
		constexpr bool operator==(const ctString<M>& rhs)const noexcept {
			return false;
		}
		template<size_t M>
		constexpr bool operator!=(const ctString<M>& rhs)const noexcept {
			return !(*this == rhs);
		}
		constexpr auto size()const noexcept { return L-1; }
		constexpr auto length()const noexcept { return L-1; }


		constexpr size_t find(char obj,size_t cur=0)const {
			return cur==size()?
						npos:
						(operator[](cur) == obj ? 
							cur : 
							find(obj, cur + 1));
		}

		template<size_t N>
		constexpr size_t search(const ctString<N>& str, size_t cur = 0)const {
			return str.size()==1?
					find(str[0],cur): 
					find1(str, find(str[0], cur));
		}
		template<size_t N>
		constexpr size_t search(const char(&str)[N], size_t cur = 0)const {
			return search(ctString<N>(str), cur);
		}
	private:
		template<size_t N>
		constexpr size_t find1(const ctString<N>& str, size_t cur)const {
			//precond: *this[cur]==str[0]
			return cur==npos||cur+str.size() > size() ?
						npos: 
						find2(str, cur, find(str[1], cur + 1), 1);
		}

		template<size_t N>
		constexpr size_t find2(const ctString<N>& str,size_t prev_pos,size_t cur_pos,size_t cur_id)const {
			return cur_pos==npos?
				npos:
				( prev_pos+1!=cur_pos? 
						find1(str,find(str[0],cur_pos-cur_id)):
						(cur_id==str.size()-1?
							cur_pos+1-str.size():
							find2(str, cur_pos, find(str[cur_id+1],cur_pos+1), cur_id+1)) 
				);
		}
		//MSVC bug
		//template<size_t M>
		//constexpr auto append(const ctString<M>& rhs)const {
		//	return *this + rhs;
		//}
	};

	template<size_t L,size_t L2>
	constexpr auto operator+(const ctString<L>& lhs, const ctString<L2>& rhs)noexcept {
		return ctString<L + L2 - 1>(lhs, rhs);
	}
	template<size_t L,size_t L2>
	constexpr auto operator+(const ctString<L>& lhs, const char (&rhs)[L2])noexcept {
		return ctString<L + L2 - 1>(lhs, ctString<L2>(rhs));
	}
	template<size_t L,size_t L2>
	constexpr auto operator+(const char (&lhs)[L],const ctString<L2>& rhs)noexcept {
		return ctString<L + L2 - 1>(ctString<L>(lhs), rhs);
	}
	template<size_t N>
	constexpr auto makeCtString(const char(&str)[N]) {
		return ctString<N>(str);
	}


}//namespace Talg

#include "undef_macro.h"
