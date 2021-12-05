#pragma once

#include <Ranae/Math/Vector.h>
#include <Ranae/Math/Quaternion.h>

namespace ranae {

  struct Transform {
    Vector<float, 3> position{ 0.0f };
    Quaternion orientation{}; // my orientation is gay
    Vector<float, 3> scale{ 1.0f };
  };

  // World = Parent * Local
  inline Transform operator*(const Transform& parent, const Transform& local) {
    return Transform {
      .position    = parent.position + parent.orientation * (parent.scale * local.position),
      .orientation = parent.orientation * local.orientation,
      .scale       = parent.scale * (parent.orientation * local.scale),
    };
  }

  inline Transform& operator*=(Transform& parent, const Transform& local) {
    parent = parent * local;
    return parent;
  }

  // Local = World / Parent
  inline Transform operator/(const Transform& world, const Transform& parent) {
    const Quaternion parentConjugate{ conjugate(parent.orientation) };

    return Transform {
      .position    = (parentConjugate * (world.position - parent.position)) / parent.scale;
      .orientation = (parentConjugate * world.orientation),
      .scale       = (parentConjugate * (world.scale / parent.scale)),
    };
  }

  inline Transform& operator/=(Transform& world, const Transform& parent) {
    world = world / parent;
    return world;
  }

  inline Vector<float, 3> transformPoint(const Transform& transform, const Vector<float, 3>& point) {
    return (conjugate(transform.orientation) * (transform.position - point)) / transform.scale;
  }

  inline Transform inverse(const Transform& t) {
    const Quaternion invOrientation{ conjugate(t.orientation) };

    return Transform {
      .position    = (invOrientation * -t.position) / t.scale,
      .orientation = (invOrientation),
      .scale       = (invOrientation * (Vector<float, 3>{ 1.0f } / t.scale)),
    };
  }

  // Returns true if a Transform does bugger all.
  bool identity(const Transform& t) {
    return t.position == Vector<float, 3>{0.0f} &&
           t.orientation == Quaternion{} &&
           t.scale == Vector<float, 3>{1.0f};
  }

  // TODO:
  /*Matrix4 transformMatrix4(const Transform& t) {
    return Math::translate(t.position) *
          quaternionToMatrix4(t.orientation) *
          Math::scale(t.scale);
  }*/

}
