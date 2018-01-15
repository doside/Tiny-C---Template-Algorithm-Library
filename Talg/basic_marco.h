#pragma once

#define forward_m(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
#define except_when(...) noexcept(noexcept(__VA_ARGS__))
#define OMIT_T(...) typename __VA_ARGS__::type