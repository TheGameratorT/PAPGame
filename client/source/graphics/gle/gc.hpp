/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 gc.hpp
 */

#pragma once

#include "../../Types.hpp"

#define GLE_HEADER	<GL/glew.h>
#define GLE_BEGIN	namespace GLE {
#define GLE_END		}

#define GLE_EXT_SUPPORTED(x) GLEW_##x

//Merges bilinear and trilinear filtering modes
#define GLE_TEXTURE_MERGE_FILTERS 1

//#define GLE_DISABLE_DEBUG
#define GLE_PASS_UNLINKED_SHADERS 1


#define GLE_MATRIX_HEADER	<glm/glm.hpp>
#define GLE_DEFINE_MATRIX	typedef glm::vec2 GLEVec2; \
							typedef glm::vec3 GLEVec3; \
							typedef glm::vec4 GLEVec4; \
							typedef glm::mat2 GLEMat2; \
							typedef glm::mat3 GLEMat3; \
							typedef glm::mat4 GLEMat4;

#define GLE_VECTOR_VALUE_PTR(v) (&v.x)
#define GLE_VECTOR_ARRAY_PTR(v) (&v->x)
#define GLE_MATRIX_VALUE_PTR(m) (&m[0].x)
#define GLE_MATRIX_ARRAY_PTR(m) (&(*m)[0].x)

//User includes

#include <string>

#define gle_assert(cond, msg, ...) //assert(cond, msg, __VA_ARGS__)
#define gle_force_assert(msg, ...) //assert(false, msg, __VA_ARGS__)


GLE_BEGIN

namespace __Detail {
	constexpr const char* logSystemName = "GL Engine";
}

template<typename... Args>
inline void debug(const std::string& msg, Args&&... args) noexcept {
	//Log::debug(__Detail::logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(const std::string& msg, Args&&... args) noexcept {
	//Log::info(__Detail::logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(const std::string& msg, Args&&... args) noexcept {
	//Log::warn(__Detail::logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(const std::string& msg, Args&&... args) noexcept {
	//Log::error(__Detail::logSystemName, msg, std::forward<Args>(args)...);
}


constexpr static inline u32 maxVertexAttributes = 16;
constexpr static inline u32 maxVertexAttributeBytes = (maxVertexAttributes + 7) / 8;

constexpr static inline u32 invalidID = -1;
constexpr static inline u32 invalidBoundID = -2;

GLE_END
