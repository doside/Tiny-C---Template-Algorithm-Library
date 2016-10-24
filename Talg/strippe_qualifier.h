#ifndef STRIPPE_QUALIFIER_H_INCLUDED
#define STRIPPE_QUALIFIER_H_INCLUDED
#include <type_traits>

#include "core.h"






//ԭ��rm_cvrImp����rm_cvr,���Ǿ������һ��,��Ϊ�˷�ֹ�Ҳ����ػ��汾ʱtype���޵ݹ�
//����������Ϊ�ػ��汾�Ѿ����ǵ������п��ܵı�Ե���.
template<class T>
struct rm_cvr {
	struct helper
	{
		T a;
	};
	//todo: ������δ֪��һ��ָ�򲻴��ڵĳ�Ա�����ĳ�Աָ���Ƿ�Ϸ��ʶ�ǿ��ʹ�������helper
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


template<class R, class P>
struct rm_cvr<R(P::*)> {
	using type = R();
	using seq_type = Seq<R>;
};

template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...)> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) volatile> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const volatile> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) &> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) && > {
	using type = R(Ts...);

};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const &> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) volatile&> {
	using type = R(Ts...);

};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const volatile&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const &&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) volatile&&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};
template<class R, class P, class...Ts>
struct rm_cvr<R(P::*)(Ts...) const volatile&&> {
	using type = R(Ts...);
	using seq_type = Seq<R, Ts...>;
};




template<class T>
using rm_top = std::remove_cv_t< std::remove_pointer_t< std::remove_reference_t<  std::remove_cv_t<T> > > >;

template<class F>
using RemoveCvrp = typename rm_cvr< rm_top<F> >::type;







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

