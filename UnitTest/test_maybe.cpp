#include <doctest/doctest.h>
#include <Talg/algorithm.h>
#include <memory>
#include <Talg/maybe.h>
using namespace Talg;
namespace {

	class Error :std::exception {};

	
	enum Command{
		ConstructNonullCompleteObj=0,
		EmitStatelessException,
		EmitStatefulException,
		PreventInvalidAcess,
		ConstructRealEmptyObject,
		ReportErrorCode,
		CommandCount
	};
	struct StateException{
		Command info=CommandCount;
		constexpr StateException(Command v)noexcept:info(v){}
	};
	Maybe<std::unique_ptr<int>> f(int c,int* err_code=nullptr) {
		using Res = Maybe<std::unique_ptr<int>>;

		switch (c)
		{
		case ConstructNonullCompleteObj:
			return Res{std::make_unique<int>(0)};
		case EmitStatelessException:
			return Res::from_exception<Error>();
		case ReportErrorCode:
			assert(err_code != nullptr);
			return Res::from_function([err_code]{
				*err_code = -1; 
			});
		case EmitStatefulException:
			return Res::from_exception(StateException{ EmitStatefulException });
		case PreventInvalidAcess:
			return Res{};
		case ConstructRealEmptyObject:
			return Res(std::unique_ptr<int>(nullptr));
		default:
			throw std::logic_error("");
		}
	}
	//todo 提供更便利的方式来给test case设置class标签,而不是像这里这样硬编码.
	TEST_CASE("The noexcept-check style usage about Maybe<std::unique_ptr<int>>") { 
		CHECK(f(ConstructNonullCompleteObj));
		auto res = f(ConstructRealEmptyObject); //this kind of usage is very rare but should be legal.
		bool cmp = (res && res() == nullptr);
		CHECK(cmp);
		for (int i = ConstructNonullCompleteObj+1; i != ConstructRealEmptyObject; ++i) {
			auto res = f(i);
			CHECK(!res);
		}
	}


	TEST_CASE("The automatic-throw style usage about Maybe<std::unique_ptr<int>>") {
		using Val = std::unique_ptr<int>;
		using Res = Maybe<Val>;
		CHECK_THROWS_AS(f(PreventInvalidAcess)(), EmptyValueError<Res>);
		CHECK(f(ConstructRealEmptyObject)()==nullptr);
		try {
			f(EmitStatefulException);
		}
		catch (StateException& er) {
			CHECK(er.info == EmitStatefulException);
		}

		CHECK_THROWS_AS(f(EmitStatelessException)(), Error);
	}
	TEST_CASE("The error-code style usage about Maybe<std::unique_ptr<int>>") {
		int error_code = 0;
		auto res=f(ReportErrorCode, &error_code)();
		bool cmp = (res == nullptr&&error_code == -1); //zz doctest!
		CHECK(cmp);
	}


}