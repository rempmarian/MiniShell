#include "kobject.h"

/* Main program */
int main() {
    /* Create a bus */
    struct Bus bus;
    bus_init(&bus, "BusA");

    /* Create devices */
    struct Device device1, device2;
    
    //TODO: Initialize devices with appropriate name and parent
    device_init(&device1, "Device_a", &bus.kset.kobj);
    device_init(&device2, "Device_b", &bus.kset.kobj);

    /* Create a bus */
    struct Bus bus2;
    bus_init(&bus2, "BusB");

    struct Bridge bridge1;
    // BusA is parent of BusA2BusB
    bridge_init(&bridge1, "BusA2BusB", &bus); 
    // add bridge to BusA kset
    kset_add(&bus.kset, &bridge1.kobject);
    // making Bridge the parent of BusB by using kobject_init
    kobject_init(&bus2.kset.kobj, bus2.kset.name, &bridge1.kobject);

    // TODO: Initialize device with appropriate name and parent
    struct Device device3;
    device_init(&device3, "Device_c", &bus2.kset.kobj);

    // TODO: How do the bridge and buses fit into the hierarchy
    // add Device_a and Device_b to parent BusA
    bus_add_device(&bus, &device1); 
    bus_add_device(&bus, &device2);

    // TODO: Set up appropriate hierarchy for all (refer above for the rest of hierarchy set-up)
    // add Device_c to parent BusB
    bus_add_device(&bus2, &device3);

    /* Print the hierarchy */
    bus_print_hierarchy(&bus);
    bus_print_hierarchy(&bus2);

    printf("Please make sure that the print functions and device, bus or bridge names are not hardcoded.\n");
    return 0;
}