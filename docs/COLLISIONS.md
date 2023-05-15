# Collision system

## AABB

The RATR0 collision detection system is designed aound aAxis aligned bounding boxes
(AABB), which has are both simple and efficient to implement.

## Quad Trees

If we would check each possible collision object against each other, we would end
up with O(n^2) collision checks which is very wasteful.

To reduce the number of collision checks, we manage our collidable objects within
a quad tree so that only objects that are in a region close to each other are
checked.

We insert objects based on their bounding boxes, but check collisions based on
collision boxes. That is because we use the quadtree also to determine which
moving objects need to be redrawn because they were overlapped.

### Strategy

There is one quad tree for all static and moving objects in the scene.
Static objects are never removed from the tree unless they represent destructible
objects. By this we can limit updating to only the moving objects.

