#pragma once
#include "algorithm.h"
#include <stdexcept>
namespace Talg {

	template<class T,size_t N>
	class ArraySizeDetector {
	protected:
		T data[N];
	};
	template<class T>
	class ArraySizeDetector<T,0> {
	protected:
		T data[1];
	};

	template<class T,size_t N>
	class Array:ArraySizeDetector<T,N> {
		using Base = ArraySizeDetector<T, N>;
		using Base::data;
	public:
		using	value_type				=T;
		using	size_type				=std::size_t;
		using	difference_type			=std::ptrdiff_t;
		using	reference				=value_type&;
		using	const_reference			=const value_type&;
		using	pointer					=value_type*;
		using	const_pointer			=const value_type*;
		using	iterator				=T*;
		using	const_iterator			=const T*;
		using	reverse_iterator		=std::reverse_iterator<iterator>;
		using	const_reverse_iterator	=std::reverse_iterator<const_iterator>;
		
		constexpr Array() = default;
		constexpr Array(std::initializer_list<T> list):Base{list}{}
		template<class Iter>
		Array(Iter beg, Iter end) {
			std::copy(beg, end, std::begin(data));
		}
		Array(const T& val) {
			fill(val);
		}
		T& at(size_t id) {
			if (((N == 0 && id != 0) || (id >= N)))
				throw std::out_of_range("Array");
			return data[id];
		}
		constexpr const T& at(size_t id) const{
			return ((N==0&&id!=0)||(id>=N)) ?
				throw std::out_of_range("Array"):
				data[id];
		}
		T& operator[](size_t index)noexcept {
			return data[index];
		}
		constexpr T& operator[](size_t index)const noexcept {
			return data[index];
		}
		T& front()noexcept{
			return data[0];
		}
		const T& front()const noexcept{
			return data[0];
		}
		T& back()noexcept{
			return data[N==0?0:N-1];
		}
		const T& back()const noexcept{
			return data[N==0?0:N-1];
		}
		void reverse() {
			std::reverse(begin(), end());
		}
		void fill(const T& value) {
			for (auto& elem : data) {
				elem = value;
			}
		}
		template<class Iter>
		void assign(iterator iter, Iter first, Iter last) {
			auto end_ = end();
			for (; iter != end_&&first != last; ++iter, ++first) {
				*iter = *first;
			}
		}
		void assign(size_t count, const T& value) {
			for (size_t i = 0; i != count; ++i)
				data[i] = value;
		}
		template<class InputIt>
		void assign(InputIt first, InputIt last) {
			assign(begin(), first, last);
		}
		void assign(std::initializer_list<T> list) {
			return assign(list.begin(), list.end());
		}
		void swap(T& rhs){
			std::swap(data, rhs.data);
		}
		void sort() {
			std::sort(begin(), end());
		}
		constexpr bool empty() const noexcept{
			return N == 0;
		}
		constexpr size_type size() const noexcept {
			return N;
		}
		constexpr size_type max_size() const noexcept{
			return N;
		}
		constexpr auto cbegin()const noexcept{
			return &data[0];
		}
		constexpr auto begin()const noexcept{
			return &data[0];
		}
		auto begin()noexcept{
			return &data[0];
		}
		constexpr auto end()const noexcept{
			return data + N;
		}
		constexpr auto cend()const noexcept{
			return data + N;
		}
		auto end()noexcept{
			return data + N;
		}
		reverse_iterator rbegin()noexcept {
			return end();
		}
		constexpr const_reverse_iterator rbegin() const noexcept {
			return end();
		}
		constexpr const_reverse_iterator crbegin() const noexcept {
			return end();
		}
		reverse_iterator rend()noexcept {
			return begin();
		}
		constexpr const_reverse_iterator rend() const noexcept {
			return begin();
		}
		constexpr const_reverse_iterator crend() const noexcept {
			return begin();
		}
	};


}










