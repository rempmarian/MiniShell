#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simulating kobject with reference counting */
struct kobject {
    char name[50];
    struct kobject *parent;
    int refcount;
    void (*release)(struct kobject *kobj);
};

/* Simulating kset */
struct kset {
    char name[50];
    struct kobject kobj;
    struct kobject **members;
    size_t member_count;
};

/* Device structure */
struct Device {
    // connect with a kobject
    struct kobject kobject;
};

/* Bus structure */
struct Bus {
    // connect with a kobject
    struct kobject kobject;
    struct kset kset; // to support bus_add_device()
};

/* Bridge strucutre */
struct Bridge {
    // connect with a kobject
    struct kobject kobject;
};
/* */

/* Initialize kobject */
void kobject_init(struct kobject *kobj, const char *name, struct kobject *parent);

/* Initialize kset */
void kset_init(struct kset *kset, const char *name, struct kobject *parent);

/* Add kobject to kset */
void kset_add(struct kset *kset, struct kobject *kobj);

/* Initialize Device */
void device_init(struct Device *device, const char *name, struct kobject *parent);

/* Initialize Bus */
void bus_init(struct Bus *bus, const char *name);

/* Add Device to Bus */
void bus_add_device(struct Bus *bus, struct Device *device);

/* Print hierarchy recursively */
void print_hierarchy(const struct kobject *kobj, int level);

/* Print Bus hierarchy */
void bus_print_hierarchy(const struct Bus *bus);

void bridge_init(struct Bridge *bridge, const char *name, struct Bus *parent_bus);


