struct lock *
lock_create(const char *name)
{
    struct lock *lock;
    
    lock = kmalloc(sizeof(struct lock));
    if (lock == NULL) {
        return NULL;
    }
    
    lock->lk_name = kstrdup(name);
    if (lock->lk_name == NULL) {
        kfree(lock);
        return NULL;
    }
    
    // add stuff here as needed
    lock->lk_wchan = wchan_create(lock->lk_name);
    if (lock->lk_wchan == NULL) {
        kfree(lock->lk_name);
        kfree(lock);
        return NULL;
    }
    
    spinlock_init(&lock->lk_spin);
    lock->owner = NULL;
    lock->held = 0;
    return lock;
}