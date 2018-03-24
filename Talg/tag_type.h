#pragma once

namespace Talg {

	/*
		\brief	占位用的tag type,表明该参数需要被推导.
		\see	\@makeit
	*/
	struct deduce_t{ };
	
	struct default_t{};

	struct no_head_t{}; //Head_s
	struct no_tail_t{}; //Tail_s
	/*!
		\brief	用于表示弱异常约束.也即,某函数在一定条件下会抛出异常,
				但是当前可以约定这条件不成立,于是可以将该函数视为noexcept,
				此时就使用weak_except_t标记.
		\note	实现方有权决定是否提供检查.
	*/
	struct weak_except_t{}; 

	//要求实现方一定提供检查.
	struct with_check_t{};

	//用于形容某种特化的一般情况
	//用于需要一个tag而不管它是什么,
	//反正不同于别的就可
	struct trival_case_t{};
}

