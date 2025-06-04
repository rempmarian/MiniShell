//Name: Marian Rempola
//PSU Email: mhr5220@psu.edu
//PSU ID: 993254039

#include "kobject.h"

/* Initialize kobject */
void kobject_init(struct kobject *kobj, const char *name, struct kobject *parent) {
    strncpy(kobj->name, name, sizeof(kobj->name));
    kobj->parent = parent;
    kobj->refcount = 1; // Initial reference count
    kobj->release = NULL;
    if (parent) {
        parent->refcount++;
    }
}

/* Initialize kset */
void kset_init(struct kset *kset, const char *name, struct kobject *parent) {
    strncpy(kset->name, name, sizeof(kset->name));
    kobject_init(&kset->kobj, name, parent);
    kset->members = NULL;
    kset->member_count = 0;
}

/* Add kobject to kset */
void kset_add(struct kset *kset, struct kobject *kobj) {
    kset->members = realloc(kset->members, sizeof(struct kobject *) * (kset->member_count + 1));
    if (!kset->members) {
        perror("Failed to allocate memory");
        return;
    }
    kset->members[kset->member_count] = kobj;
    kset->member_count++;
    printf("Added kobject '%s' to kset '%s'.\n", kobj->name, kset->name);
}

/* Initialize Device */
void device_init(struct Device *device, const char *name, struct kobject *parent) {
    // init a Device obj w a name and its parent
    kobject_init(&device->kobject, name, parent);

    printf("Initialized Device '%s'.\n", name);
}

/* Initialize Bus */
void bus_init(struct Bus *bus, const char *name) {
    // init Bus kset 
    kset_init(&bus->kset, name, NULL);

    printf("Initialized Bus '%s'.\n", name);
}

/* Add Device to Bus */
void bus_add_device(struct Bus *bus, struct Device *device) {
    // check if bus or device  passed is valid
    if(!bus || !device) {
        perror("bus and/or device passed is not valid");
        return;
    }   

    // using kset_add() to add Device to a Bus
    kset_add(&bus->kset, &device->kobject);
}

/* Print hierarchy recursively */
void print_hierarchy(const struct kobject *kobj, int level) {
    // check if inputs are valid
    if (!kobj || level < 0) {
        perror("kobject or level passed is not valid");
        return;
    }

    // print the hierarchy of kobjects, starting from the root
    for (int i = 0; i < level; i++) {
        // child objects are indented to represent parent-child relationships
        printf("  ");
    }
    printf("kobject: %s\n", kobj->name);

    // if the object passed has a parent, print again with its parent being the next level
    if(kobj->parent) {
        print_hierarchy(kobj->parent, level += 1);
    }
}

/* Print Bus hierarchy */
void bus_print_hierarchy(const struct Bus *bus) {
    // check if bus passed is valid
    if (!bus) {
        perror("Bus passed is not initialized");
        return;
    }
    
    // print the hierarchy of a specific bus, starting from its root kobject
    printf("Hierarchy of Bus '%s':\n", bus->kset.kobj.name);
    print_hierarchy(&bus->kset.kobj, 0);

    // if there're child objects, print recursively
    if (bus->kset.member_count > 0 && bus->kset.members) {
        for (size_t i = 0; i < bus->kset.member_count; i++) {
            //print hierarchy recursively
            print_hierarchy(bus->kset.members[i], 1);
        }
    }
}

void bridge_init(struct Bridge *bridge, const char *name, struct Bus *parent_bus) {
    // check if bridge, name, or parent_bus passed are valid
    if (!bridge || !name || !parent_bus) {
        perror("bridge, name, and/or parent bus passed are not valid");
        return;
    }

    // init a Bridge obj w a name and parent to the parent bus object
    kobject_init(&bridge->kobject, name, &parent_bus->kset.kobj);

    // print out initialized bridge (as Device)
    printf("Initialized Device '%s'.\n", name);
}