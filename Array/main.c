#include <stdio.h>

#define LISTMAXSIZE 100
#define STRMAXSIZE 200

/* BlockDeviceFormat */
enum bd_field { target, source };
int blockdevice_format_basename = 0;
static enum bd_field blockdevice_format = target;

#define sfree(ptr)                                                             \
  do {                                                                         \
    free(ptr);                                                                 \
    (ptr) = NULL;                                                              \
  } while (0)

typedef char* c_string;

typedef struct{
   struct block_device* next;
   c_string uuid;   /* UUID of VM */
   c_string* path_list;  /* name of block device */
   int nr_path;
} block_device;

static struct block_device *block_devices = NULL;

/* InterfaceFormat. */
enum if_field { if_address, if_name, if_number };

/* BlockDeviceFormatBasename */
static enum if_field interface_format = if_address;


typedef struct
{
  char *path;       /* name of interface device */
  char *address;    /* mac address of interface device */
  char *number;     /* interface device number */
} interface_flag;
//typedef struct interface_flag* interface_flag_ptr;
/* Actual list of network interfaces found on last refresh. */
typedef struct interface_device {
  struct interface_device *next;
  c_string uuid;    /* UUID of VM */
  interface_flag **interface_flags;
  int nr_interface_flags;

} interface_device;

static struct interface_device *interface_devices = NULL;

static void free_interface_devices(void);
static int add_interface_device(const char *uuid, const char *path,
                                const char *address, unsigned int number);
static int get_vm_display_name_list(const char *uuid, c_string *display_name_list, int nr_interface);

static void free_block_devices(void) {
  block_device* block_device_entry = NULL;

  while (block_devices != NULL) {
    block_device_entry = block_devices;
    if (block_device_entry->uuid != NULL) {
      sfree(block_device_entry->uuid);
    }
    for(int i = 0; i < block_device_entry->nr_path; i++) {
      sfree(block_device_entry->path_list[i]);
    }
    if (block_device_entry->path_list != NULL) {
      sfree(block_device_entry->path_list);
    }
    block_devices = block_device_entry->next;
    sfree(block_device_entry);
  }
}

static int add_block_device(const char *uuid, const char *path) {
  if (uuid == NULL || path == NULL) {
    return -1;
  }
  int uuid_is_exist = 0;
  block_device* block_device_entry;
  static block_device* pre_block_device_entry;
  block_device_entry = block_devices;
  while (block_device_entry != NULL) {
    if (strcmp(block_device_entry->uuid, uuid) == 0) {
      uuid_is_exist = 1;
      break;
    }
    else {
      uuid_is_exist = 0;
    }
    block_device_entry = block_device_entry->next;
  }

  if (!uuid_is_exist) {
    block_device_entry = calloc(1, sizeof(block_device));
    if (block_device_entry == NULL) {
      printf("out of memory.\n");
      return -1;
    }
    char* copy_uuid = strdup(uuid);
    if (copy_uuid == NULL)
      return -1;
    block_device_entry->uuid = copy_uuid;
    block_device_entry->path_list = NULL;
    block_device_entry->nr_path = 0;
    block_device_entry->next = NULL;

    if (block_devices == NULL) {
      block_devices = block_device_entry;
    }
    else {
      pre_block_device_entry->next = block_device_entry;
    }

    pre_block_device_entry = block_device_entry;
  }

  int index = block_device_entry->nr_path;
  if (index == 0) {
    block_device_entry->path_list = malloc(sizeof(c_string));
  }
  else {
    c_string* t_ptr = (c_string*)realloc(block_device_entry->path_list,
                                         sizeof(c_string)*(index + 1));
    if (t_ptr != NULL)
      block_device_entry->path_list = t_ptr;
  }
  char *path_copy = strdup(path);
  if (path_copy == NULL)
    return -1;
  block_device_entry->path_list[index] = path_copy;
  index++;
  block_device_entry->nr_path = index;
  return block_device_entry->nr_path;
}

static int get_vm_type_instance_list(const char *uuid,
                                     c_string* type_instance_list, int nr_disk)
{
  block_device* block_device_entry = NULL;
  block_device_entry = block_devices;
  if (uuid == NULL || block_device_entry == NULL || type_instance_list ==NULL) {
    return -1;
  }
  while (block_device_entry != NULL) {
    if(strcmp(block_device_entry->uuid, uuid) == 0) {
      break;
    }
    block_device_entry = block_device_entry->next;
  }
  if (block_device_entry == NULL) {
    return -1;
  }

  for (int i = 0 ; i < block_device_entry->nr_path && i < nr_disk; i++) {
    c_string type_instance = NULL;
    if (blockdevice_format_basename && blockdevice_format == source)
      type_instance = basename(block_device_entry->path_list[i]);
    else
      type_instance = block_device_entry->path_list[i];
    type_instance_list[i] = strdup(type_instance);
  }
  return block_device_entry->nr_path;
}

static void free_interface_devices(void) {
  interface_device* interface_device_entry = NULL;

  while (interface_devices != NULL) {
    interface_device_entry = interface_devices;
    if (interface_device_entry->uuid != NULL) {
      sfree(interface_device_entry->uuid);
    }
    for (int i = 0; i < interface_device_entry->nr_interface_flags; i++) {
      interface_flag* t_ptr = interface_device_entry->interface_flags[i];
      if (t_ptr != NULL) {
        if (t_ptr->path != NULL) {
          sfree(t_ptr->path);
        }
        if (t_ptr->address != NULL) {
          sfree(t_ptr->address);
        }
        if (t_ptr->number != NULL) {
          sfree(t_ptr->number);
        }
        sfree(t_ptr);
      }
    }
    if (interface_device_entry->interface_flags != NULL) {
      sfree(interface_device_entry->interface_flags);
    }
    interface_devices = interface_device_entry->next;
    sfree(interface_device_entry);
  }
}

static int add_interface_device(const char *uuid, const char *path,
                                const char *address, unsigned int number) {
  char *path_copy, *address_copy, *number_string;
  if ((path == NULL) || (address == NULL))
    return -1;

  path_copy = strdup(path);
  if (!path_copy)
    return -1;

  address_copy = strdup(address);
  if (!address_copy) {
    sfree(path_copy);
    return -1;
  }

  /*
   * The len of number_string is 15;
   */
  number_string = malloc(15 * sizeof(char));
  snprintf(number_string, 15, "interface-%u", number);

  int uuid_is_exist = 0;
  interface_device* interface_device_entry;
  static interface_device* pre_interface_device_entry;
  interface_device_entry = interface_devices;
  while (interface_device_entry != NULL) {
    if (strcmp(interface_device_entry->uuid, uuid) == 0) {
      uuid_is_exist = 1;
      break;
    }
    else {
      uuid_is_exist = 0;
    }
    interface_device_entry = interface_device_entry->next;
  }

  if (!uuid_is_exist) {
    interface_device_entry = calloc(1, sizeof(interface_device));
    if (interface_device_entry == NULL) {
      printf("out of memory.\n");
      return -1;
    }
    char* copy_uuid = strdup(uuid);
    if (copy_uuid == NULL)
      return -1;
    interface_device_entry->uuid = copy_uuid;
    interface_device_entry->nr_interface_flags = 0;
    interface_device_entry->next = NULL;

    if (interface_devices== NULL) {
      interface_devices = interface_device_entry;
    }
    else {
      pre_interface_device_entry->next = interface_device_entry;
    }

    pre_interface_device_entry = interface_device_entry;
  }

  int index = interface_device_entry->nr_interface_flags;
  if (index == 0) {
    interface_device_entry->interface_flags = malloc(sizeof(interface_flag*));
  }
  else {
    interface_flag **t_ptr = (interface_flag**)realloc(interface_device_entry->interface_flags,
                                               sizeof(interface_flag*)*(index + 1));
    if (t_ptr != NULL)
      interface_device_entry->interface_flags = t_ptr;
  }

  interface_flag *interface_flags_entry = malloc(sizeof(interface_flag));
  interface_flags_entry->address = address_copy;
  interface_flags_entry->path = path_copy;
  interface_flags_entry->number = number_string;

  interface_device_entry->interface_flags[index] = interface_flags_entry;

  index++;
  interface_device_entry->nr_interface_flags = index;
  return  interface_device_entry->nr_interface_flags;
}

static int get_vm_display_name_list(const char *uuid, c_string *display_name_list, int nr_interface)
{
  interface_device* interface_device_entry = NULL;
  interface_device_entry = interface_devices;
  if (uuid == NULL || interface_device_entry == NULL || display_name_list ==NULL) {
    return -1;
  }
  while (interface_device_entry != NULL) {
    if(strcmp(interface_device_entry->uuid, uuid) == 0) {
      break;
    }
    interface_device_entry = interface_device_entry->next;
  }
  if (interface_device_entry == NULL) {
    return -1;
  }

  for (int i = 0 ; i < interface_device_entry->nr_interface_flags && i < nr_interface; i++) {
    interface_flag* n_ptr = interface_device_entry->interface_flags[i];
    char *display_name = NULL;
    switch (interface_format) {
      case if_address:
        display_name = n_ptr->address;
        break;
      case if_number:
        display_name = n_ptr->number;
        break;
      case if_name:
      default:
      display_name = n_ptr->path;
    }
    display_name_list[i] = strdup(display_name);
  }
  return interface_device_entry->nr_interface_flags;
}

int main()
{
    for(int j = 0; j < 100000000; j++) {

    for(int i = 0 ; i < 10; i++) {
        add_block_device("World!", "Hello");
    }
    c_string* type_instance_list;
    type_instance_list = malloc(sizeof(c_string) * 10);
    int status = get_vm_type_instance_list("World!", type_instance_list, 10);
    for(int i = 0 ; i < status; i++) {
        printf("%d   %s\n",i, type_instance_list[i]);
    }
    free_block_devices();
    for(int i = 0 ; i < 10; i++) {
        sfree(type_instance_list[i]);
    }
    sfree(type_instance_list);
  }

  for(int j = 0; j < 0; j++) {
    for(int i = 0; i < 10; i++) {
        add_interface_device("Hello", "World!", "127.0.0.1", i);
    }
    c_string *display_name_list = malloc(sizeof(c_string) * 10);
    int status = get_vm_display_name_list("Hello", display_name_list, 10);
    for(int i = 0; i < status; i++) {
        printf("%d   %s\n", i, display_name_list[i]);
    }
    free_interface_devices();
    for(int i = 0; i < 10; i++) {
        sfree(display_name_list[i]);
    }
    sfree(display_name_list);
   }
}
