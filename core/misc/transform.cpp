#include "transform.h"

void rt::transform::setTranslate(const vector3& v)
{
	m_translate = v;
	m_hasTranslate = true;
}

void rt::transform::setRotate(const vector3& v)
{
	m_rotate = v;
	m_hasRotate = true;
}

void rt::transform::setScale(const vector3& v)
{
	m_scale = v;
	m_hasScale = true;
}

vector3 rt::transform::getTranslate() const
{
	return m_translate;
}

vector3 rt::transform::getRotate() const
{
	return m_rotate;
}

vector3 rt::transform::getScale() const
{
	return m_scale;
}

bool rt::transform::hasTranslate() const
{
	return m_hasTranslate;
}

bool rt::transform::hasRotate() const
{
	return m_hasRotate;
}

bool rt::transform::hasScale() const
{
	return m_hasScale;
}