#pragma once
#define CWEASSERT(a) if(!(a)) throw std::runtime_error(__FILE__ + ":" + std::string(__LINE__) + " " + a);