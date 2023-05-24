# Rendering Architecture (Amiga)

## Motivation

The rendering system on the Amiga was designed with the restrictions
of the Amiga hardware in mind. There is a limit on how many blits can
be performed per frame and it is not very high.
Since a game needs to also update its game world, we took measurements
of how much effect dirty rectangle restoration and blit queue would 
take.

On a barebones A500 system, there is surprisingy little time available
for blitting if the game also needs to update other data structures and 
game logic, so it was decided to forego explicit queuing of blits and
just directly blit objects as they are updated.

## Central Concepts

### Double buffering

The render component will strictly use double buffering. This is to ensure smooth
transitions at the cost of moderate additional space consumption

### Render list

On each frame we need to determine

  * the dirty background rectangles
  * the changed objects to redraw
  * the collided objects

We need therefore to have data structures for the following:

  * background tiles
  * BOBs
  * collision boxes

Unfortunately, trees seem to be significantly slower on Amiga than
hashing, so we use hash sets for BOB sets and a hash grids
to process collisions




### Background restoration algorithm (dirty rectangles)

#### Idea

As the screen buffers get modified by game objects, obstructed portions need to be
restored. The restoration method is taking care of that.

#### BOB changes

A change for a BOB happened when

   * the object moved
   * the object frame changed

We need to

   * calculate the restore tiles on the old object
   * add these tiles to the display buffer's dirty list
   * add the BOB to the draw list of the both buffers

##### Dirty rectangle sets

We want to make sure we capture dirty regions, so they can be blitted all in one
go. Also, since multiple objects can obstruct the same space, we want to ensure those
rectangles only get restored once, so some kind of set data structure is needed.

We are splitting the play area into 16x16 pixel tiles, which is a good tradeoff between
size, efficiency and flexibility.

After initial prototyping it became clear that tree or list based data structures
are a bit costly in terms of space and CPU time, O(log(n)) insertion turned out
costlier than expected for something that has to be run repeatedly for every frame.
So the next thing that came to mind was a hash table. It turns out that due to
the regular matrix structure map, we can efficiently map dirty tiles into a bit
set, implemented as arrays of 32 bit integers. Insertion is O(1) and we can
relatively efficiently query the set, since each 32 bit number that is a zero
means that we can skip it.

##### The algorithm

We obviously need to determine every object that has some kind of change before
we render them.

We then add the dirty rectangles to the set of every affected buffer, which means
both the back buffer and possibly the front buffer, since that would be next.

In a first approach, we just add the dirty to both buffers


### Graphics effects

#### Palette interpolation

In a palette based system like the Amiga, palette manipulation is an effective
tool to achieve effects that don't require a lot of computational resources.
We can easily implement fade-in/fade-out effects by interpolating a palette
into another.
