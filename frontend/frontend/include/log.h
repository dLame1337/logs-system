#pragma once
#ifdef _DEBUG
#include <iostream>
#define LOGD(...) std::cout  << __FUNCTION__ << ":" << __LINE__  << " [D] " << __VA_ARGS__ << std::endl;
#define LOGI(...) std::cout  << __FUNCTION__ << ":" << __LINE__  << " [I] " << __VA_ARGS__ << std::endl;
#define LOGW(...) std::cout  << __FUNCTION__ << ":" << __LINE__  << " [W] " << __VA_ARGS__ << std::endl;
#define LOGE(...) std::cout  << __FUNCTION__ << ":" << __LINE__  << " [E] " << __VA_ARGS__ << std::endl;
#else
#define LOGD(...)
#define LOGI(...) std::cout << "[I] " << __VA_ARGS__ << std::endl;
#define LOGW(...)
#define LOGE(...)
#endif
