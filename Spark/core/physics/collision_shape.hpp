#ifndef SPARK_COLLISION_SHAPE_HPP
#define SPARK_COLLISION_SHAPE_HPP

#include <core/pch.hpp>
#include <include/glm/glm.hpp>
#include <include/glm/gtx/quaternion.hpp>

namespace Spark {

enum class ShapeType {
	Sphere,
	OBB,
	Capsule
};

class CollisionShape {
  public:
	virtual ~CollisionShape()                                = default;

	virtual ShapeType GetType() const                        = 0;
	virtual glm::vec3 GetCenter() const                      = 0;
	virtual glm::mat3 CalculateInertiaTensor(f32 mass) const = 0;
};

class OBBShape : public CollisionShape {
  public:
	OBBShape(const glm::vec3& half_extents)
		: m_half_extents(half_extents)
		, m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {}

	ShapeType GetType() const override { return ShapeType::OBB; }
	glm::vec3 GetCenter() const override { return glm::vec3(0.0f); }

	glm::mat3 CalculateInertiaTensor(f32 mass) const override {
		f32 x2 = m_half_extents.x * m_half_extents.x;
		f32 y2 = m_half_extents.y * m_half_extents.y;
		f32 z2 = m_half_extents.z * m_half_extents.z;
		glm::mat3 inertia_tensor(
			(1.0f / 12.0f) * mass * (y2 + z2),
			0.0f,
			0.0f,
			0.0f,
			(1.0f / 12.0f) * mass * (x2 + z2),
			0.0f,
			0.0f,
			0.0f,
			(1.0f / 12.0f) * mass * (x2 + y2));
		return glm::mat3_cast(m_orientation) * inertia_tensor * glm::mat3_cast(glm::conjugate(m_orientation));
	}

	const glm::vec3& GetHalfExtents() const { return m_half_extents; }
	const glm::quat& GetOrientation() const { return m_orientation; }
	void SetOrientation(const glm::quat& orientation) { m_orientation = orientation; }

	// Get the OBB's local axes
	glm::vec3 GetLocalXAxis() const { return glm::rotate(m_orientation, glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 GetLocalYAxis() const { return glm::rotate(m_orientation, glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::vec3 GetLocalZAxis() const { return glm::rotate(m_orientation, glm::vec3(0.0f, 0.0f, 1.0f)); }

	// Project the OBB onto an axis
	void ProjectOntoAxis(const glm::vec3& axis, f32& min, f32& max) const {
		f32 center_projection = glm::dot(GetCenter(), axis);
		f32 radius =
			std::abs(glm::dot(GetLocalXAxis() * m_half_extents.x, axis)) +
			std::abs(glm::dot(GetLocalYAxis() * m_half_extents.y, axis)) +
			std::abs(glm::dot(GetLocalZAxis() * m_half_extents.z, axis));
		min = center_projection - radius;
		max = center_projection + radius;
	}

  private:
	glm::vec3 m_half_extents;
	glm::quat m_orientation;
};

class SphereShape : public CollisionShape {
  public:
	SphereShape(f32 radius)
		: m_radius(radius) {}

	ShapeType GetType() const override { return ShapeType::Sphere; }
	glm::vec3 GetCenter() const override { return glm::vec3(0.0f); }
	glm::mat3 CalculateInertiaTensor(f32 mass) const override {
		f32 i = 2.0f * mass * m_radius * m_radius / 5.0f;
		return glm::mat3(i);
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
	glm::vec3 GetCenter() const override { return glm::vec3(0.0f, m_height * 0.5f, 0.0f); }
	glm::mat3 CalculateInertiaTensor(f32 mass) const override {
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

		return glm::mat3(
			ix,
			0.0f,
			0.0f,
			0.0f,
			iy,
			0.0f,
			0.0f,
			0.0f,
			ix);
	}

	f32 GetRadius() const { return m_radius; }
	f32 GetHeight() const { return m_height; }

  private:
	f32 m_radius;
	f32 m_height;
};

} // namespace Spark

#endif // SPARK_COLLISION_SHAPE_HPP