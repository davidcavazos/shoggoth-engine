/*
 *    Copyright (c) 2012 David Cavazos <davido262@gmail.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef CULLING_HPP
#define CULLING_HPP

#include <set>
#include <map>
#include <boost/unordered_map.hpp>

class Quaternion;
class Vector3;
class Entity;
class RenderableMesh;
class btDispatcher;
class btDbvtBroadphase;
class btCollisionConfiguration;
class btCollisionWorld;
class btCollisionShape;
class btCollisionObject;

class Culling {
public:
    static void initialize();
    static void shutdown();
    static void registerForCulling(RenderableMesh* const renderablemesh);
    static void unregisterForCulling(RenderableMesh* const renderablemesh);
    static void performFrustumCulling(const float* projectionMatrix,
                                      const Entity* camera,
                                      std::set<RenderableMesh*>& modelsInFrustum);

private:
    typedef std::map<RenderableMesh*, btCollisionObject*> collision_object_map_t;
    typedef std::map<btCollisionObject*, RenderableMesh*> renderable_mesh_map_t;

    static btDispatcher* m_collisionDispatcher;
    static btDbvtBroadphase* m_broadphase;
    static btCollisionConfiguration* m_collisionConfiguration;
    static btCollisionWorld* m_collisionWorld;
    static collision_object_map_t m_collisionObjects;
    static renderable_mesh_map_t m_renderableMeshes;

    static void openGLMatrixMult(const float* a, const float* b, float* const res);
};

#endif // CULLING_HPP
