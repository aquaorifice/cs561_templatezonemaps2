#include "zonemaps.h"
#include <algorithm>

template<typename T>
zonemap<T>::zonemap(std::vector<T> _elements, uint _num_elements_per_zone)
{
    // constructor 
    // Your code starts here ...
    elements = _elements;
    num_elements_per_zone = _num_elements_per_zone;
    //for ceil we have to convert to float which seems unnecessary
    if (elements.size() % num_elements_per_zone == 0) {
        num_zones = elements.size() / num_elements_per_zone;
    } else {
        num_zones = elements.size() / num_elements_per_zone + 1;
    }   

}

template<typename T>
void zonemap<T>::build(){
    // iterate over all elements present
    for (size_t i = 0; i < elements.size(); i = i + num_elements_per_zone){
        //determine the end of the zone
        size_t end_idx;
        // if iterating from the current index = i to the num_elements in each range exceeds size then end is just the input size
        // otherwise the end index is determined by adding number of elements in each zone tp starting index
        if (i + num_elements_per_zone < elements.size()) {
            end_idx = i + num_elements_per_zone;
        } else {
            end_idx = elements.size();
        }
        // initializing a zone here
        zone<T> z;
        // at every interval = num_elements_per_zone, we should create a new zone 
        // we need to fill this zone with elements using the end_index we determined before
        for (size_t j = i; j < end_idx; j++) {
            z.elements.push_back(elements[j]);
        }
        // since we dont sort, the min. max could be anywhere
        // zone has min, max and size as parameters
        // normally the size should be constant except for a zone with fewer than num_elements_per_zone

        //these functions return iterator - so used * to dereference values
        z.min = *std::min_element(z.elements.begin(), z.elements.end()); //find min
        z.max = *std::max_element(z.elements.begin(), z.elements.end());// find max
        z.size = z.elements.size();
        zones.push_back(z);
        //zones now has all the individual zones z inside it
    }
}

// this is the second query function that returns the elements that match query
// probably for a range query
template<typename T>
std::vector<T> zonemap<T>::query(T low, T high){
    // results is a vector that will return all elements matching the query
    std::vector<T> results;
    //iterator for zones vector
    for (const auto& zone : zones)
    {
        if (high < zone.min || low > zone.max) {
            continue;
        }
        //if the range matches then we go through each element in the zone
        //this will be a linear search
        for (const auto& element : zone.elements)
        {
            if (element >= low && element <= high) {
                results.push_back(element);
            }
        }
    }

    return results;
}

//this is the first query function that just returns a count
template<typename T>
size_t zonemap<T>::query(T key)
{
    size_t ctr = 0; //ctr that can be returned as the result
    for (const auto& zone : zones) // iterator to go through all zones
    {
        if (key < zone.min || key > zone.max) { //same as the other one where we check if the zone is in range
            continue;
        }
        // try to find the key if the zone is in range by going through each element in the zone
        // zone.elements has all the elements
        // this is linear search
        for (const auto& element : zone.elements)
        {
            if (element == key) {
                ctr += 1;
            }
        }
    }
    return ctr;
}

