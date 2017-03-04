#pragma once
#include <doctest/doctest.h>
#include <string>


struct Check {
	inline void operator()(bool res,const std::string& expr=" ",int line=0,const std::string& file=" ") {
		do {                                                                   
			doctest::detail::ResultBuilder rb(doctest::detail::assertType::DT_CHECK, 
				file.c_str(),line,expr.c_str());                                  
			try {                                 
				rb.setResult(doctest::detail::ExpressionDecomposer() << res);
			} catch(...) { rb.m_threw = true; }
			if(rb.log())                                
				DOCTEST_BREAK_INTO_DEBUGGER();          
			rb.react();
		} while (doctest::detail::always_false());
	}
};






