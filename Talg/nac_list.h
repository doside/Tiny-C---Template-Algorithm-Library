#pragma once
#include <iterator>
#include <memory>
#include <forward_list>
namespace NAC {

	template<class T>
	class forward_list {
		struct node;
		struct node {
			node* next;
			value_type val;
		};
	public:
		using value_type = T;
		using reference = value_type&;
		using pointer = value_type*;


		struct iterator :public iterator_traits<T*>{
		public:
			using iterator_category = std::forward_iterator_tag;
			iterator next()const {
				return cur->next;
			}
			T& operator*() {
				return cur->val;
			}
			T& operator*()const{
				return cur->val;
			}
			T* operator->() {
				return std::addressof(cur->val);
			}
			T* operator->()const {
				return std::addressof(cur->val);
			}
		private:
			node* cur;
			iterator(node*);
		};
		using const_iterator = const iterator;
	private:
		
		iterator before_beg_;
	public:
		iterator before_begin()noexcept{ 
			return before_beg_; 
		}
		const_iterator before_begin()const noexcept{ 
			return before_beg_;
		}

		iterator begin()noexcept{ 
			return before_begin().next(); 
		}
		const_iterator begin()const noexcept{
			return before_begin().next(); 
		}

		iterator end()noexcept{ 
			return iterator();
		}
		const_iterator end()const noexcept{ 
			return iterator();
		}

		auto cbefore_begin()const noexcept{ 
			return before_begin(); 
		}
		auto cbegin()const noexcept{
			return begin();
		}
		auto cend()const noexcept{
			return end();
		}

		template<class U,class Alloc>
		static iterator insert_after(const_iterator pos, U&& value,Alloc& alloc=std::allocator<node>{}) {
			using allocator_traits = std::allocator_traits<Alloc>;
			node* ptr = alloc.allocate(1);
			auto next = pos->next;
			allocator_traits::construct(alloc, ptr,std::forward<U>(value),next);
			pos->next = ptr;
		}

		forward_list();
		~forward_list(){}
	};









}//namespace NAC




