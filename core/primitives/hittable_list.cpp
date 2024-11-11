#include "hittable_list.h"

#include "../misc/singleton.h"

hittable_list::hittable_list(std::string _name)
{
    m_name = _name;
}

hittable_list::hittable_list(std::shared_ptr<hittable> object, std::string _name)
{
    m_name = _name;

    add(object);
}

void hittable_list::clear()
{
    objects.clear();
}

std::shared_ptr<hittable>& hittable_list::back()
{
    return objects.back();
}

void hittable_list::add(std::shared_ptr<hittable> object)
{
    objects.push_back(object);
    m_bbox = aabb(m_bbox, object->bounding_box());
}

std::shared_ptr<hittable>& hittable_list::get(const std::string& name)
{
    for (auto& object : objects)
    {
        if (object->getName() == name)
        {
            return object;
        }
    }
    static std::shared_ptr<hittable> null_hittable = nullptr;
    return null_hittable;
}

bool hittable_list::remove(std::shared_ptr<hittable> object)
{
    auto it = std::remove_if(objects.begin(), objects.end(),
        [&object](const std::shared_ptr<hittable>& obj) {
            return obj == object;
        });

    if (it != objects.end()) {
        objects.erase(it, objects.end());
        return true;
    }
    return false;
}

bool hittable_list::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects)
    {
        if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec, depth, rnd))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

aabb hittable_list::bounding_box() const
{
    return m_bbox;
}

double hittable_list::pdf_value(const point3& o, const vector3& v, randomizer& rnd) const
{
    auto weight = 1.0 / objects.size();
    auto sum = 0.0;

    for (const auto& object : objects)
    {
        sum += weight * object->pdf_value(o, v, rnd);
    }

    return sum;
}

/// <summary>
/// Random special implementation for hittable list (override base)
/// </summary>
/// <param name="origin"></param>
/// <returns></returns>
vector3 hittable_list::random(const vector3& o, randomizer& rnd) const
{
    auto int_size = static_cast<int>(objects.size());
    if (int_size > 0)
    {
        return objects[rnd.get_int(0, int_size - 1)]->random(o, rnd);
    }
    
    return vector3();
}



//// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void hittable_list::updateBoundingBox()
{
    // to implement
}
