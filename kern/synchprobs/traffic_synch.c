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
    arr[2] += num;
    arr[3] += num;
    arr[5] += num;
    arr[6] += num;
    arr[7] += num;
    arr[9] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[6], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[9], control);
    }
  }
  else if (pos == northeast)
  {
    arr[2] += num;
    arr[3] += num;
    arr[4] += num;
    arr[5] += num;
    arr[6] += num;
    arr[7] += num;
    arr[10] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[6], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[10], control);
    }
  }
  else if (pos == eastwest)
  {
    arr[0] += num;
    arr[1] += num;
    arr[4] += num;
    arr[5] += num;
    arr[7] += num;
    arr[8] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[8], control);
    }
  }
  else if (pos == eastsouth)
  {
    arr[0] += num;
    arr[1] += num;
    arr[4] += num;
    arr[5] += num;
    arr[6] += num;
    arr[7] += num;
    arr[9] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[6], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[9], control);
    }
  }
  else if (pos == southnorth)
  {
    arr[1] += num;
    arr[2] += num;
    arr[3] += num;
    arr[6] += num;
    arr[7] += num;
    arr[11] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[6], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[11], control);
    }
  }
  else if (pos == southwest)
  {
    arr[0] += num;
    arr[1] += num;
    arr[2] += num;
    arr[3] += num;
    arr[6] += num;
    arr[7] += num;
    arr[8] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[6], control);
      cv_broadcast(dirarr[7], control);
      cv_broadcast(dirarr[8], control);
    }
  }
  else if (pos == westeast)
  {
    arr[0] += num;
    arr[1] += num;
    arr[3] += num;
    arr[4] += num;
    arr[5] += num;
    arr[10] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[10], control);
    }
  }
  else if (pos == westnorth)
  {
    arr[0] += num;
    arr[1] += num;
    arr[2] += num;
    arr[3] += num;
    arr[4] += num;
    arr[5] += num;
    arr[11] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[3], control);
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[5], control);
      cv_broadcast(dirarr[11], control);
    }
  }
  else if (pos == northwest)
  {
    arr[2] += num;
    arr[5] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[2], control);
      cv_broadcast(dirarr[5], control);
    }
  }
  else if (pos == westsouth)
  {
    arr[0] += num;
    arr[3] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[0], control);
      cv_broadcast(dirarr[3], control);
    }
  }
  else if (pos == southeast)
  {
    arr[1] += num;
    arr[6] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[1], control);
      cv_broadcast(dirarr[6], control);
    }
  }
  else if (pos == eastnorth)
  {
    arr[4] += num;
    arr[7] += num;
    if (num == -1)
    {
      cv_broadcast(dirarr[4], control);
      cv_broadcast(dirarr[7], control);
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
  dirarr[0] = cv_create("northsouth");
  dirarr[1] = cv_create("northeast");
  dirarr[2] = cv_create("eastwest");
  dirarr[3] = cv_create("eastsouth");
  dirarr[4] = cv_create("southnorth");
  dirarr[5] = cv_create("southwest");
  dirarr[6] = cv_create("westeast");
  dirarr[7] = cv_create("westnorth");
  dirarr[8] = cv_create("northwest");
  dirarr[9] = cv_create("westsouth");
  dirarr[10] = cv_create("southeast");
  dirarr[11] = cv_create("eastnorth");

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
