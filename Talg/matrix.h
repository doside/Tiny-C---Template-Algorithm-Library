#pragma once
#include "vec_op.h"
namespace Talg {
	//对称矩阵
	template<class T>
	struct SymMatirx {
		size_t n;
		std::vector<T> data;
	
		SymMatirx(size_t N,const T& val={}):data(N,val){}
		template<class U>
		explicit SymMatirx(const U& container):data(container.begin(),container.end()){}
		explicit SymMatirx(std::initializer_list<T> list):data(list.begin(),list.end()){}

		size_t row()const {
			return n;
		}
		size_t col()const {
			return n;
		}
		decltype(auto) fast_at(size_t row, size_t col)const {
			return data[row*(row + 1) / 2 + col];
		}
		decltype(auto) fast_at(size_t row, size_t col){
			return data[row*(row + 1) / 2 + col];
		}
		decltype(auto) operator()(size_t row, size_t col)const{
			return row < col ? fast_at(col, row) : fast_at(row, col);
		}
		decltype(auto) operator()(size_t row, size_t col){
			return row < col ? fast_at(col, row) : fast_at(row, col);
		}
		auto begin()const {
			return data.begin();
		}
		auto end()const {
			return data.end();
		}
		SymMatirx& operator+=(const SymMatirx& rhs) {
			auto riter = rhs.begin();
			for (auto iter = begin(); iter != end(); ++iter) {
				*iter += *riter++;
			}
			return *this;
		}
		SymMatirx& operator-=(const SymMatirx& rhs) {
			auto riter = rhs.begin();
			for (auto iter = begin(); iter != end(); ++iter) {
				*iter -= *riter++;
			}
			return *this;
		}
		SymMatirx& operator*=(const SymMatirx& rhs) {
			auto riter = rhs.begin();
			for (auto iter = begin(); iter != end(); ++iter) {
				*iter *= *riter++;
			}
			return *this;
		}
		SymMatirx& operator/=(const SymMatirx& rhs) {
			auto riter = rhs.begin();
			for (auto iter = begin(); iter != end(); ++iter) {
				*iter /= *riter++;
			}
			return *this;
		}
		template<class U>
		SymMatirx& operator+=(U&& rhs) {
			for (auto& elem : data) {
				elem += std::forward<U>(rhs);
			}
			return *this;
		}
		template<class U>
		SymMatirx& operator-=(U&& rhs) {
			for (auto& elem : data) {
				elem -= std::forward<U>(rhs);
			}
			return *this;
		}
		template<class U>
		SymMatirx& operator*=(U&& rhs) {
			for (auto& elem : data) {
				elem *= std::forward<U>(rhs);
			}
			return *this;
		}
		template<class U>
		SymMatirx& operator/=(U&& rhs) {
			for (auto& elem : data) {
				elem /= std::forward<U>(rhs);
			}
			return *this;
		}
	};
}





