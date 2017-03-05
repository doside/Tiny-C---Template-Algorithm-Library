#ifndef STRIPPE_QUALIFIER_H_INCLUDED
#define STRIPPE_QUALIFIER_H_INCLUDED
#include <type_traits>

#include "core.h"




namespace Talg {

//原本rm_cvrImp就是rm_cvr,我们决定多加一层,是为了防止找不到特化版本时type无限递归
//尽管我们认为特化版本已经覆盖到了所有可能的边缘情况.
template<class T>
struct rm_cvr {
	struct helper
	{
		T a;
	};
	//todo: 我们仍未知道一个指向不存在的成员函数的成员指针是否合法故而强行使用了这个helper
	using type = typename rm_cvr< T(helper::*)>::type;
	using seq_type = typename rm_cvr< T(helper::*)>::seq_type;
};


#if 0
template<class R, class...Ts>
struct rm_cvr<R(Ts...)> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) volatile> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const volatile> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) &> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) &&> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const &> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) volatile&> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const volatile&> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const &&> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) volatile&&> {
	using type = R(Ts...);
	
};
template<class R, class...Ts>
struct rm_cvr<R(Ts...) const volatile&&> {
	using type = R(Ts...);
	
};
#endif


template<class R, class ObjT>
struct rm_cvr<R(ObjT::*)> {
	using type = R();
	using seq_type = Seq<R>;
};

template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...)> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) volatile> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const volatile> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) &> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) && > {
	using type = R(Ts...);

};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const &> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) volatile&> {
	using type = R(Ts...);

};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const volatile&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const &&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) volatile&&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class ObjT, class...Ts>
struct rm_cvr<R(ObjT::*)(Ts...) const volatile&&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};




template<class T>
using rm_top = std::remove_cv_t< std::remove_pointer_t< std::remove_reference_t<  std::remove_cv_t<T> > > >;

template<class F>
using RemoveCvrp = typename rm_cvr< rm_top<F> >::type;



}//namespace Talg



#endif // STRIPPE_QUALIFIER_H_INCLUDED












#if 0
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ > {
	using type = __VA_ARGS__;

};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ volatile> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const volatile> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ &> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ &&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const &> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ volatile&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const volatile&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const &&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ volatile&&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};
template<class R, class...Ts>
struct rm_cvr<__VA_ARGS__ const volatile&&> {
	using type = __VA_ARGS__;
	using ft = R(Ts...);
};



template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) volatile> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const volatile> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) &> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) && > {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const &> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) volatile&> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const volatile&> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const &&> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) volatile&&> {
	using type = R(Ts...);

};
template<class R, class...Ts>
struct rm_cvr<R(*)(Ts...) const volatile&&> {
	using type = R(Ts...);

};



#endif

