
#define forward_m(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)


/*!
	\brief	免于写noexcept(noexcept(...))
	\note	不直接定义为 noexcept(noexcept(...)){ return ...},是为了便于调试
*/
#define except_when_m(...) noexcept(noexcept(__VA_ARGS__))

#define omit_t_m(...) typename __VA_ARGS__::type

#define declare_iterator_tpedef_m(T)	\
using value_type		= typename T::value_type;		\
using pointer			= typename T::pointer;			\
using reference			= typename T::reference;		\
using difference_type	= typename T::difference_type;	\
using iterator			= typename T::iterator;			\
using const_iterator	= typename T::const_iterator;	


