#ifndef SPARK_COLLISION_SHAPE_HPP
#define SPARK_COLLISION_SHAPE_HPP

#include <core/pch.hpp>
#include <core/math/math.hpp>

namespace Spark {

	enum class ShapeType { Sphere, OBB, Capsule };

	class CollisionShape {
	  public:
		virtual ~CollisionShape()                                = default;

		virtual ShapeType GetType() const                        = 0;
		virtual _MATH Vec3 GetCenter() const                      = 0;
		virtual _MATH Mat3 CalculateInertiaTensor(f32 mass) const = 0;
	};

	class OBBShape : public CollisionShape {
	  public:
		OBBShape(const _MATH Vec3& half_extents)
			: m_half_extents(half_extents)
			, m_orientation(_MATH Quat(1.0f, 0.0f, 0.0f, 0.0f)) {}

		ShapeType GetType() const override { return ShapeType::OBB; }
		_MATH Vec3 GetCenter() const override { return _MATH Vec3(0.0f); }

		_MATH Mat3 CalculateInertiaTensor(f32 mass) const override {
			f32       x2 = m_half_extents.x * m_half_extents.x;
			f32       y2 = m_half_extents.y * m_half_extents.y;
			f32       z2 = m_half_extents.z * m_half_extents.z;
			_MATH Mat3 inertia_tensor((1.0f / 12.0f) * mass * (y2 + z2), 0.0f, 0.0f, 0.0f, (1.0f / 12.0f) * mass * (x2 + z2), 0.0f, 0.0f, 0.0f, (1.0f / 12.0f) * mass * (x2 + y2));
			return _MATH Mat3Cast(m_orientation) * inertia_tensor * _MATH Mat3Cast(_MATH Conjugate(m_orientation));
		}

		const _MATH Vec3& GetHalfExtents() const { return m_half_extents; }
		const _MATH Quat& GetOrientation() const { return m_orientation; }
		void             SetOrientation(const _MATH Quat& orientation) { m_orientation = orientation; }

		// Get the OBB's local axes
		_MATH Vec3 GetLocalXAxis() const { return _MATH Rotate(m_orientation, _MATH Vec3(1.0f, 0.0f, 0.0f)); }
		_MATH Vec3 GetLocalYAxis() const { return _MATH Rotate(m_orientation, _MATH Vec3(0.0f, 1.0f, 0.0f)); }
		_MATH Vec3 GetLocalZAxis() const { return _MATH Rotate(m_orientation, _MATH Vec3(0.0f, 0.0f, 1.0f)); }

		// Project the OBB onto an axis
		void ProjectOntoAxis(const _MATH Vec3& axis, f32& min, f32& max) const {
			f32 center_projection = _MATH Dot(GetCenter(), axis);
			f32 radius            = _MATH Abs(_MATH Dot(GetLocalXAxis() * m_half_extents.x, axis)) + _MATH Abs(_MATH Dot(GetLocalYAxis() * m_half_extents.y, axis)) +
			             _MATH Abs(_MATH Dot(GetLocalZAxis() * m_half_extents.z, axis));
			min = center_projection - radius;
			max = center_projection + radius;
		}

	  private:
		_MATH Vec3 m_half_extents;
		_MATH Quat m_orientation;
	};

	class SphereShape : public CollisionShape {
	  public:
		SphereShape(f32 radius)
			: m_radius(radius) {}

		ShapeType GetType() const override { return ShapeType::Sphere; }
		_MATH Vec3 GetCenter() const override { return _MATH Vec3(0.0f); }
		_MATH Mat3 CalculateInertiaTensor(f32 mass) const override {
			f32 i = 2.0f * mass * m_radius * m_radius / 5.0f;
			return _MATH Mat3(i);
		}

		f32 GetRadius() const { return m_radius; }

	  private:
		f32 m_radius;
	};

	class CapsuleShape : public CollisionShape {
	  public:
		CapsuleShape(f32 radius, f32 height)
			: m_radius(radius)
			, m_height(height) {}

		ShapeType GetType() const override { return ShapeType::Capsule; }
		_MATH Vec3 GetCenter() const override { return _MATH Vec3(0.0f, m_height * 0.5f, 0.0f); }
		_MATH Mat3 CalculateInertiaTensor(f32 mass) const override {
			// Approximation: Treat as cylinder for simplicity
			f32 cylinder_height = m_height - 2.0f * m_radius;
			f32 cylinder_mass   = mass * cylinder_height / (cylinder_height + 4.0f * m_radius / 3.0f);
			f32 sphere_mass     = (mass - cylinder_mass) / 2.0f;

			f32 cylinder_ix     = cylinder_mass * (3.0f * m_radius * m_radius + cylinder_height * cylinder_height) / 12.0f;
			f32 cylinder_iy     = cylinder_mass * m_radius * m_radius / 2.0f;

			f32 sphere_i        = 2.0f * sphere_mass * m_radius * m_radius / 5.0f;
			f32 sphere_offset   = (m_height - m_radius) / 2.0f;

			f32 ix              = cylinder_ix + 2.0f * (sphere_i + sphere_mass * sphere_offset * sphere_offset);
			f32 iy              = cylinder_iy + 2.0f * sphere_i;

			return _MATH Mat3(ix, 0.0f, 0.0f, 0.0f, iy, 0.0f, 0.0f, 0.0f, ix);
		}

		f32 GetRadius() const { return m_radius; }
		f32 GetHeight() const { return m_height; }

	  private:
		f32 m_radius;
		f32 m_height;
	};

} // namespace Spark

#endif // SPARK_COLLISION_SHAPE_HPP