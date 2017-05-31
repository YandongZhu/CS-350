#include <types.h>
#include <lib.h>
#include <synchprobs.h>
#include <synch.h>
#include <opt-A1.h>

/* 
 * This simple default synchronization mechanism allows only vehicle at a time
 * into the intersection.   The intersectionSem is used as a a lock.
 * We use a semaphore rather than a lock so that this code will work even
 * before locks are implemented.
 */

/* 
 * Replace this default synchronization mechanism with your own (better) mechanism
 * needed for your solution.   Your mechanism may use any of the available synchronzation
 * primitives, e.g., semaphores, locks, condition variables.   You are also free to 
 * declare other global variables if your solution requires them.
 */

/*
 * replace this with declarations of any synchronization and other variables you need here
 */
//static struct semaphore *intersectionSem;
//static struct lock [4];
static struct lock* control;

static const int increase = 1;
static const int decrease = -1;
static volatile int arr[12];
static struct cv* dirarr[12];



/* 
 * The simulation driver will call this function once before starting
 * the simulation
 *
 * You can use it to initialize synchronization and other variables.
 * 
 */

enum Positions
  { 
    northsouth = 0,
    northeast = 1,
    eastwest = 2,
    eastsouth = 3,
    southnorth = 4,
    southwest = 5,
    westeast = 6,
    westnorth = 7,
    northwest = 8,
    westsouth = 9,
    southeast = 10,
    eastnorth = 11,
    error = 12
  };

typedef enum Positions Position;

static Position transite(Direction origin, Direction destination){
  if (origin == north && destination == west){
    return northwest;
  }
  else if (origin == north && destination == south){
    return northsouth;
  }
  else if (origin == north && destination == east){
    return northeast;
  }
  else if (origin == west && destination == north){
    return westnorth;
  }
  else if (origin == west && destination == east){
    return westeast;
  }
  else if (origin == west && destination == south){
    return westsouth;
  }
  else if (origin == south && destination == west){
    return southwest;
  }
  else if (origin == south && destination == north){
    return southnorth;
  }
  else if (origin == south && destination == east){
    return southeast;
  }
  else if (origin == east && destination == north){
    return eastnorth;
  }
  else if (origin == east && destination == west){
    return eastwest;
  }
  else if (origin == east && destination == south){
    return eastsouth;
  }
  else{
    panic("cant find the direction");
    return error;
  }
}

static void change(Position pos, int num){
  if (pos == northsouth)
  {
    arr[eastwest] += num;
    arr[eastsouth] += num;
    arr[southwest] += num;
    arr[westeast] += num;
    arr[westnorth] += num;
    arr[westsouth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[westeast], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[westsouth], control);
    }
  }
  else if (pos == northeast)
  {
    arr[eastwest] += num;
    arr[eastsouth] += num;
    arr[southnorth] += num;
    arr[southwest] += num;
    arr[westeast] += num;
    arr[westnorth] += num;
    arr[southeast] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[westeast], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[southeast], control);
    }
  }
  else if (pos == eastwest)
  {
    arr[northsouth] += num;
    arr[northeast] += num;
    arr[southnorth] += num;
    arr[southwest] += num;
    arr[westnorth] += num;
    arr[northwest] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[northwest], control);
    }
  }
  else if (pos == eastsouth)
  {
    arr[northsouth] += num;
    arr[northeast] += num;
    arr[southnorth] += num;
    arr[southwest] += num;
    arr[westeast] += num;
    arr[westnorth] += num;
    arr[westsouth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[westeast], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[westsouth], control);
    }
  }
  else if (pos == southnorth)
  {
    arr[northeast] += num;
    arr[eastwest] += num;
    arr[eastsouth] += num;
    arr[westeast] += num;
    arr[westnorth] += num;
    arr[eastnorth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[westeast], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[eastnorth], control);
    }
  }
  else if (pos == southwest)
  {
    arr[northsouth] += num;
    arr[northeast] += num;
    arr[eastwest] += num;
    arr[eastsouth] += num;
    arr[westeast] += num;
    arr[westnorth] += num;
    arr[northwest] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[westeast], control);
      cv_broadcast(dirarr[westnorth], control);
      cv_broadcast(dirarr[northwest], control);
    }
  }
  else if (pos == westeast)
  {
    arr[northsouth] += num;
    arr[northeast] += num;
    arr[eastsouth] += num;
    arr[southnorth] += num;
    arr[southwest] += num;
    arr[southeast] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[southeast], control);
    }
  }
  else if (pos == westnorth)
  {
    arr[northsouth] += num;
    arr[northeast] += num;
    arr[eastwest] += num;
    arr[eastsouth] += num;
    arr[southnorth] += num;
    arr[southwest] += num;
    arr[eastnorth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[eastsouth], control);
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[southwest], control);
      cv_broadcast(dirarr[eastnorth], control);
    }
  }
  else if (pos == northwest)
  {
    arr[eastwest] += num;
    arr[southwest] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[eastwest], control);
      cv_broadcast(dirarr[southwest], control);
    }
  }
  else if (pos == westsouth)
  {
    arr[northsouth] += num;
    arr[eastsouth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northsouth], control);
      cv_broadcast(dirarr[eastsouth], control);
    }
  }
  else if (pos == southeast)
  {
    arr[northeast] += num;
    arr[westeast] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[northeast], control);
      cv_broadcast(dirarr[westeast], control);
    }
  }
  else if (pos == eastnorth)
  {
    arr[southnorth] += num;
    arr[westnorth] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[southnorth], control);
      cv_broadcast(dirarr[westnorth], control);
    }
  }
  else {
    panic("cant get the correct position");
  }
}


void
intersection_sync_init(void)
{
  /* replace this default implementation with your own implementation */

  //intersectionSem = sem_create("intersectionSem",1);
  //if (intersectionSem == NULL) {
  //  panic("could not create intersection semaphore");
  //

  control = lock_create("control");
  if(control == NULL) {
    panic("lock cant be create");
  }
  dirarr[northsouth] = cv_create("northsouth");
  dirarr[northeast] = cv_create("northeast");
  dirarr[eastwest] = cv_create("eastwest");
  dirarr[eastsouth] = cv_create("eastsouth");
  dirarr[southnorth] = cv_create("southnorth");
  dirarr[southwest] = cv_create("southwest");
  dirarr[westeast] = cv_create("westeast");
  dirarr[westnorth] = cv_create("westnorth");
  dirarr[northwest] = cv_create("northwest");
  dirarr[westsouth] = cv_create("westsouth");
  dirarr[southeast] = cv_create("southeast");
  dirarr[eastnorth] = cv_create("eastnorth");

  for (int i = 0; i < 8; ++i)
  {
    if (dirarr[i] == NULL)
    {
      panic("cv cant be create");
    }
  }
  for(int i = 0; i < 12; ++i){
    arr[i] = 0;
  }
  return;
}

/* 
 * The simulation driver will call this function once after
 * the simulation has finished
 *
 * You can use it to clean up any synchronization and other variables.
 *
 */
void
intersection_sync_cleanup(void)
{
  /* replace this default implementation with your own implementation */
  //KASSERT(intersectionSem != NULL);
  KASSERT(control != NULL);
  for (int i = 0; i < 12; ++i)
  {
    KASSERT(dirarr[i] != NULL);
  }

  //sem_destroy(intersectionSem);
  lock_destroy(control);
  for (int i = 0; i < 12; ++i)
  {
    cv_destroy(dirarr[i]);
  }
}


/*
 * The simulation driver will call this function each time a vehicle
 * tries to enter the intersection, before it enters.
 * This function should cause the calling simulation thread 
 * to block until it is OK for the vehicle to enter the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle is arriving
 *    * destination: the Direction in which the vehicle is trying to go
 *
 * return value: none
 */



void
intersection_before_entry(Direction origin, Direction destination) 
{
  /* replace this default implementation with your own implementation */
  //(void)origin;  /* avoid compiler complaint about unused parameter */
  //(void)destination; /* avoid compiler complaint about unused parameter */
  KASSERT(control != NULL);

  lock_acquire(control);
  Position cur = transite(origin, destination);
  //bool right = check_right_turn(cur);
  //if (right){
  //}
  while (arr[cur]){
      cv_wait(dirarr[cur], control);
    }
  change(cur, increase);
  lock_release(control);
  //P(intersectionSem);
}


/*
 * The simulation driver will call this function each time a vehicle
 * leaves the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle arrived
 *    * destination: the Direction in which the vehicle is going
 *
 * return value: none
 */

void
intersection_after_exit(Direction origin, Direction destination) 
{
  /* replace this default implementation with your own implementation */
  //(void)origin;  /* avoid compiler complaint about unused parameter */
  //(void)destination; /* avoid compiler complaint about unused parameter */
  KASSERT(control != NULL);

  lock_acquire(control);
  Position cur = transite(origin, destination);
  change(cur, decrease);
  lock_release(control);

  //V(intersectionSem);
}
