#include "Collider.h"

#include <PGE/Math/Plane.h>

using namespace PGE;

Collider::Collider(float radius, float height)
    : radius(radius), height(height) { }

void Collider::setCollisionMeshCollection(CollisionMeshCollection* c) {
    cmc = c;
}

Vector3f Collider::tryMove(const Vector3f& position, const Vector3f& goal) const {
    Vector3f pos = position;
    int iterations = 0;
    Line3f line(pos, goal);
    Vector3f targetDir = (line.pointB - line.pointA).normalize();
    Vector3f currDir = targetDir;
    while (true) {
        Collision coll = cmc->checkCollision(line, height, radius);
        if (coll.hit) {
            Vector3f resultPos = line.pointA + (line.pointB - line.pointA) * (coll.coveredAmount * 0.995f);
            if (resultPos.distanceSquared(pos) < 0.0001f) {
                resultPos = pos;
                coll.coveredAmount = 0.f;
            }
            pos = resultPos;
            if (iterations >= 5) { break; }
            iterations++;
            float remainingDist = (line.pointB - pos).length();
            if (coll.normal.y <= 0.71f) {
                //surface is too steep to climb up or pushes you down
                coll.normal.y = 0.f;
                coll.normal = coll.normal.normalize();
            }
            Vector3f reflectedDir = currDir.reflect(coll.normal) * -remainingDist;
            Plane p(coll.normal, pos);
            Vector3f tempPos = pos;
            tempPos = tempPos + reflectedDir;
            tempPos = tempPos - p.normal * ((tempPos - pos).dotProduct(p.normal) * 0.995f);
            currDir = (tempPos - pos).normalize();
            if (currDir.dotProduct(targetDir) < 0.f && iterations > 1) { break; }
            line = Line3f(pos, tempPos);
        } else {
            pos = line.pointB;
            break;
        }
    }
    return pos;
}
