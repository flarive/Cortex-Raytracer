#pragma once

#include "../utilities/types.h"

namespace rt
{
	class transform
	{
	public:
		void setTranslate(const vector3& v);
		void setRotate(const vector3& v);
		void setScale(const vector3& v);

		vector3 getTranslate() const;
		vector3 getRotate() const;
		vector3 getScale() const;

		bool hasTranslate() const;
		bool hasRotate() const;
		bool hasScale() const;

	private:
		bool m_hasTranslate = false;
		bool m_hasRotate = false;
		bool m_hasScale = false;

		vector3 m_translate{};
		vector3 m_rotate{};
		vector3 m_scale{};
	};
}