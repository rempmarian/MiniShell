//Name: Marian Rempola
//PSU Email: mhr5220@psu.edu
//PSU ID: 993254039

#include "polymorphic_ints.h"

// Function to add the values of a list of TaggedUnion
int addTaggedUnions(TaggedUnion *unions, size_t count) {
    // Edge case 1: handle empty array of TaggedUnion objs
    if (unions == NULL || count == 0 ) {
        return 0; // nothing to sum
    }

    // Edge case 2: ensure the function works for large arrays with mixed types
    __int64_t sum = 0; // using larger number of bytes for large arrays

    // parse thru the unions array
    for (size_t i = 0; i < count; i++) {
        if (unions[i].tag == INT) {
            // add integer value
            sum += unions[i].data.asInt;
        } else if ( unions[i].tag == CHAR4) {
            // add the sum of four characters
            sum += unions[i].data.asChar[0];
            sum += unions[i].data.asChar[1];
            sum += unions[i].data.asChar[2];
            sum += unions[i].data.asChar[3];
        }
    }
    // return total sum
    return sum;
}
