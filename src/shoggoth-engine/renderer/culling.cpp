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


#include "shoggoth-engine/renderer/culling.hpp"

#include <cfloat>
#include <bullet/btBulletCollisionCommon.h>
#include "shoggoth-engine/kernel/transform.hpp"
#include "shoggoth-engine/kernel/entity.hpp"
#include "shoggoth-engine/renderer/renderablemesh.hpp"
#include "shoggoth-engine/resources/model.hpp"

using namespace std;

/*
 * The following code is based on the Bullet Physics DbvtCullingDemo
 * It has been adapted and is probably not optimized enough
 */

btDispatcher* Culling::m_collisionDispatcher = 0;
btDbvtBroadphase* Culling::m_broadphase = 0;
btCollisionConfiguration* Culling::m_collisionConfiguration = 0;
btCollisionWorld* Culling::m_collisionWorld = 0;
Culling::collision_object_map_t Culling::m_collisionObjects = collision_object_map_t();
Culling::renderable_mesh_map_t Culling::m_renderableMeshes = renderable_mesh_map_t();



struct DbvtBroadphaseFrustumCulling : btDbvt::ICollide {
    btAlignedObjectArray<btCollisionObject*>* m_pCollisionObjectArray;
    short int m_collisionFilterMask;

    DbvtBroadphaseFrustumCulling(btAlignedObjectArray<btCollisionObject*>* _pArray = 0):
        m_pCollisionObjectArray(_pArray),
        m_collisionFilterMask(btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger)
    {}
    DbvtBroadphaseFrustumCulling(const DbvtBroadphaseFrustumCulling& rhs):
        m_pCollisionObjectArray(rhs.m_pCollisionObjectArray),
        m_collisionFilterMask(rhs.m_collisionFilterMask)
    {
        cerr << "Error: DbvtBroadphaseFrustumCulling copy constructor should not be called!" << endl;
    }
    DbvtBroadphaseFrustumCulling& operator=(const DbvtBroadphaseFrustumCulling&) {
        cerr << "Error: DbvtBroadphaseFrustumCulling assignment operator should not be called!" << endl;
        return *this;
    }

    void Process(const btDbvtNode* node, btScalar) {
        Process(node);
    }

    void Process(const btDbvtNode* leaf) {
        btBroadphaseProxy* proxy = static_cast<btBroadphaseProxy*>(leaf->data);
        btCollisionObject* co = static_cast<btCollisionObject*>(proxy->m_clientObject);

        if ((proxy->m_collisionFilterGroup & m_collisionFilterMask) != 0)
            m_pCollisionObjectArray->push_back(co);
    }
} g_DBFC;



void Culling::initialize() {
    cout << "Creating dbvtBroadphase collision world for rendering culling" << endl;
    m_broadphase = new btDbvtBroadphase;
    m_collisionConfiguration = new btDefaultCollisionConfiguration;
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_collisionWorld = new btCollisionWorld(m_collisionDispatcher, m_broadphase, m_collisionConfiguration);
}

void Culling::shutdown() {
    cout << "Destroying all collision objects for culling" << endl;
    collision_object_map_t::const_iterator it;
    for (it = m_collisionObjects.begin(); it != m_collisionObjects.end(); ++it) {
        btCollisionObject* object = it->second;
        m_collisionWorld->removeCollisionObject(object);
        delete object->getCollisionShape();
        delete object;
    }

    cout << "Destroying dbvtBroadphase collision world" << endl;
    delete m_collisionWorld;
    delete m_collisionDispatcher;
    delete m_collisionConfiguration;
    delete m_broadphase;
}

void Culling::registerForCulling(RenderableMesh* const renderablemesh) {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float lengthX, lengthY, lengthZ;

    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = FLT_MIN;

    // find bounding box
    const Model* model = renderablemesh->getModel();
    for (size_t m = 0; m < model->getTotalMeshes(); ++m) {
        const Mesh* mesh = model->getMesh(m);

        for (size_t i = 0; i < mesh->getVerticesSize(); i += 3) {
            minX = min(minX, mesh->getVertex(i));
            maxX = max(maxX, mesh->getVertex(i));
        }

        for (size_t i = 1; i < mesh->getVerticesSize(); i += 3) {
            minY = min(minY, mesh->getVertex(i));
            maxY = max(maxY, mesh->getVertex(i));
        }

        for (size_t i = 2; i < mesh->getVerticesSize(); i += 3) {
            minZ = min(minZ, mesh->getVertex(i));
            maxZ = max(maxZ, mesh->getVertex(i));
        }
    }
    lengthX = maxX - minX;
    lengthY = maxY - minY;
    lengthZ = maxZ - minZ;

    // register into the collision world
    btCollisionObject* object = new btCollisionObject();
    btCollisionShape* boundingBox = new btBoxShape(btVector3(lengthX * 0.5f, lengthY * 0.5f, lengthZ * 0.5f));
    object->setCollisionShape(boundingBox);
    m_collisionWorld->addCollisionObject(object);

    m_collisionObjects.insert(pair<RenderableMesh*, btCollisionObject*>(renderablemesh, object));
    m_renderableMeshes.insert(pair<btCollisionObject*, RenderableMesh*>(object, renderablemesh));
}

void Culling::unregisterForCulling(RenderableMesh* const renderablemesh) {
    collision_object_map_t::const_iterator it;
    it = m_collisionObjects.find(renderablemesh);
    if (it != m_collisionObjects.end()) {
        btCollisionObject* object = it->second;
        m_collisionWorld->removeCollisionObject(object);
        delete object->getCollisionShape();
        delete object;
        m_renderableMeshes.erase(object);
        m_collisionObjects.erase(renderablemesh);
    }
}

void Culling::performFrustumCulling(const float* projectionMatrix,
                                    const Entity* camera,
                                    set<RenderableMesh*>& modelsInFrustum) {
//     renderable_mesh_map_t::const_iterator itTemp;
//     for (itTemp = m_renderableMeshes.begin(); itTemp != m_renderableMeshes.end(); ++itTemp) {
//         RenderableMesh* renderable = itTemp->second;
//         modelsInFrustum.insert(renderable);
//     }
//     return;

    // get modelview matrix
    float modelviewMatrix[16];
    trans(camera->getOrientationAbs(), camera->getPositionAbs()).inverse().getOpenGLMatrix(modelviewMatrix);

    // storage
    btVector3 planeNormals[5];
    btScalar planeOffsets[5];
    btScalar mmp[16]; // modelview * projection
    openGLMatrixMult(modelviewMatrix, projectionMatrix, mmp);

    // extract the frustum planes
    planeNormals[0] = btVector3(mmp[ 3] - mmp[ 0], mmp[ 7] - mmp[ 4], mmp[11] - mmp[ 8]); // right
    planeNormals[1] = btVector3(mmp[ 3] + mmp[ 0], mmp[ 7] + mmp[ 4], mmp[11] + mmp[ 8]); // left
    planeNormals[2] = btVector3(mmp[ 3] - mmp[ 1], mmp[ 7] - mmp[ 5], mmp[11] - mmp[ 9]); // top
    planeNormals[3] = btVector3(mmp[ 3] + mmp[ 1], mmp[ 7] + mmp[ 5], mmp[11] + mmp[ 9]); // bottom
    planeNormals[4] = btVector3(mmp[ 3] - mmp[ 2], mmp[ 7] - mmp[ 6], mmp[11] - mmp[10]); // far

    planeOffsets[0] = mmp[15] - mmp[12]; // right
    planeOffsets[1] = mmp[15] + mmp[12]; // left
    planeOffsets[2] = mmp[15] - mmp[13]; // top
    planeOffsets[3] = mmp[15] + mmp[13]; // bottom
    planeOffsets[4] = mmp[15] - mmp[14]; // far

    // update collision objects' transforms
    renderable_mesh_map_t::const_iterator it;
    for (it = m_renderableMeshes.begin(); it != m_renderableMeshes.end(); ++it) {
        btCollisionObject* object = it->first;
        RenderableMesh* renderable = it->second;
        const Entity* e = renderable->getEntity();
        object->setWorldTransform(trans(e->getOrientationAbs(), e->getPositionAbs()));
    }
    m_collisionWorld->updateAabbs();

    // check for the dbvt collisions
    btAlignedObjectArray<btCollisionObject*> objectsInFrustum;
    g_DBFC.m_pCollisionObjectArray = &objectsInFrustum;
    btDbvt::collideKDOP(m_broadphase->m_sets[1].m_root, planeNormals, planeOffsets, 5, g_DBFC);
    btDbvt::collideKDOP(m_broadphase->m_sets[0].m_root, planeNormals, planeOffsets, 5, g_DBFC);

    // fill the models in frustum to be drawn
//     static size_t pastObjects = 0;
//     size_t objects = 0;
    for (int i = 0; i < objectsInFrustum.size(); ++i) {
        it = m_renderableMeshes.find(objectsInFrustum[i]);
        if (it != m_renderableMeshes.end()) {
            modelsInFrustum.insert(it->second);
//             ++objects;
        }
    }
//     if (pastObjects != objects) {
//         cout << objects << " objects in frustum" << endl;
//         pastObjects = objects;
//     }
}

void Culling::openGLMatrixMult(const float* a, const float* b, float* const res) {
    res[ 0] = a[ 0] * b[ 0] + a[ 1] * b[ 4] + a[ 2] * b[ 8] + a[ 3] * b[12];
    res[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
    res[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
    res[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

    res[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
    res[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
    res[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
    res[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

    res[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
    res[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
    res[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
    res[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

    res[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
    res[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
    res[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
    res[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
}
