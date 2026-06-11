//MAMAN03
//Implementation of mount namespace management in xv6.
//The fixed function mount_nsleave now release correctly mount list objects preventing kernel panics. 
//Name: Sacha HADJADJ
//T.Z: 329604441

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"
#include "namespace.h"
#include "mnt_ns.h"

struct {
  struct spinlock lock;
  struct mount_ns mount_ns[NNAMESPACE];
} mountnstable;

struct mount_list {
  struct mount mnt;
  struct mount_list *next;
};

void mount_nsinit()
{
  initlock(&mountnstable.lock, "mountns");
  for (int i = 0; i < NNAMESPACE; i++) {
    initlock(&mountnstable.mount_ns[i].lock, "mount_ns");
  }
}

struct mount_ns* mount_nsdup(struct mount_ns* mount_ns)
{
  acquire(&mountnstable.lock);
  mount_ns->ref++;
  release(&mountnstable.lock);

  return mount_ns;
}

//Only for information. Commented because decraration here is not needed
/*struct mount_list {
  struct mount mnt;
  struct mount_list *next;
};*/
void
umountlist(struct mount_list* mounts)
{
  while (mounts != 0) {
    struct mount_list* next = mounts->next;
    if (mounts->mnt.parent == 0) {
      // No need to unmount root - 
      mounts->mnt.ref = 0;
    } else if (umount(&mounts->mnt) != 0) {
      panic("failed to umount upon namespace close");
    }
    mounts = next;
  }
}

//MAMAN03
//Implementation of mount namespace management in xv6.
//The fixed function now release correctly mount list objects preventing kernel panics. 
//Name: Sacha HADJADJ
//T.Z: 329604441

void mount_nsleave(struct mount_ns* mount_ns)
{
  acquire(&mountnstable.lock);
  
  mount_ns->ref--;
  
  //If this is the last process using namespace
  if (mount_ns->ref == 0) {
    umountlist(mount_ns->list_mounts);
    mount_ns->list_mounts = 0; //Clear list so allocmount_ns can reuse this slot
  }
  
  release(&mountnstable.lock);
}

static struct mount_ns* allocmount_ns()
{
  acquire(&mountnstable.lock);
  for (int i = 0; i < NNAMESPACE; i++) {
    if (mountnstable.mount_ns[i].ref == 0 && mountnstable.mount_ns[i].list_mounts == 0)
     {
      struct mount_ns* mount_ns = &mountnstable.mount_ns[i];
      mount_ns->ref = 1;
      release(&mountnstable.lock);
      return mount_ns;
     }
  }
  release(&mountnstable.lock);

  panic("out of mount_ns objects");
}

struct mount_ns* copymount_ns()
{
  struct mount_ns* mount_ns = allocmount_ns();
  mount_ns->list_mounts = copyactivemounts();
  mount_ns->root = getroot(mount_ns->list_mounts);
  return mount_ns;
}

struct mount_ns* newmount_ns()
{
  struct mount_ns* mount_ns = allocmount_ns();
  return mount_ns;
}
