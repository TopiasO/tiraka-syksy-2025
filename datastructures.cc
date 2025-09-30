// Datastructures.cc
// TESTIPALAUTUS


#include "datastructures.hh"

#include <random>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here

}

Datastructures::~Datastructures()
{
    // Write any cleanup you need here

}

bool Datastructures::add_beacon(BeaconID id, const Name& name, Coord xy, Color color)
{
    //make a shared_ptr to the new Beacon object.
    std::shared_ptr<Beacon> beaconptr = std::make_shared<Beacon>(id, name, xy, color, color);

    //emplace.second returns true if the key (id) wasn't a duplicate
    if (beacon_map_.emplace(id, beaconptr).second) {
        //maintain auxiliary data structures that help with getting
        //sorted data.
        name_map_.emplace(name, beaconptr);
        int brightness = 3*color.r+6*color.g+color.b;
        brightness_map_.emplace(brightness, beaconptr);
        return true;
    }
    return false;
}

int Datastructures::beacon_count()
{
    return beacon_map_.size();
}

void Datastructures::clear_beacons()
{
    beacon_map_.clear();
}

std::vector<BeaconID> Datastructures::all_beacons()
{
    //Initialize result vector
    std::vector<BeaconID> result = {};

    for (const auto& [id, beacon] : beacon_map_ ) {
        result.push_back(id);
    }
    return result;
}

Name Datastructures::get_name(BeaconID id)
{
    if (beacon_map_.contains(id)) {
        return beacon_map_.at(id)->name;
    }
    else {
        return NO_NAME;
    }
}

Coord Datastructures::get_coordinates(BeaconID id)
{
    if (beacon_map_.contains(id)) {
        return beacon_map_.at(id)->xy;
    }
    else {
        return NO_COORD;
    }
}

Color Datastructures::get_color(BeaconID id)
{
    if (beacon_map_.contains(id)) {
        return beacon_map_.at(id)->og_color;
    }
    else {
        return NO_COLOR;
    }
}

std::vector<BeaconID> Datastructures::beacons_alphabetically()
{
    std::vector<BeaconID> result = {};
    for (const auto& [name, beaconptr] : name_map_ ) {
        result.push_back(beaconptr->id);
    }
    return result;
}

std::vector<BeaconID> Datastructures::beacons_brightness_increasing()
{
    std::vector<BeaconID> result = {};
    for (const auto& [brightness, beaconptr] : brightness_map_ ) {
        result.push_back(beaconptr->id);
    }
    return result;
}

BeaconID Datastructures::min_brightness()
{
    //->second=Beacon tied to this key.
    return brightness_map_.begin()->second->id;
}

BeaconID Datastructures::max_brightness()
{
    //iterator marking the space after the last element of
    //the map.
    auto max_b_it = brightness_map_.end();
    //decrement. now it points to the last element.
    max_b_it--;
    return max_b_it->second->id;
}

std::vector<BeaconID> Datastructures::find_beacons(Name const& name)
{
    std::vector<BeaconID> result = {};
    auto range = name_map_.equal_range(name);
    for (auto& i = range.first; i != range.second; ++i) {
        result.push_back(i->second->id);
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool Datastructures::change_beacon_name(BeaconID id, const Name& newname)
{
    if (beacon_map_.contains(id)) {
        auto range = name_map_.equal_range(beacon_map_.at(id)->name);
        auto& i = range.first;
        for (i; i != range.second; ++i) {
            if (i->second->id == id) {
                break;
            }
        }
        name_map_.emplace(newname, i->second);
        name_map_.erase(i);
        beacon_map_.at(id)->name = newname;
        return true;
    }
    else {
        return false;
    }
}

bool Datastructures::add_lightbeam(BeaconID /*sourceid*/, BeaconID /*targetid*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<BeaconID> Datastructures::get_lightsources(BeaconID /*id*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<BeaconID> Datastructures::path_outbeam(BeaconID /*id*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<BeaconID> Datastructures::path_inbeam_longest(BeaconID /*id*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

Color Datastructures::total_color(BeaconID /*id*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

bool Datastructures::add_fibre(Coord /*xpoint1*/, Coord /*xpoint2*/, Cost /*cost*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<Coord> Datastructures::all_xpoints()
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<std::pair<Coord, Cost> > Datastructures::get_fibres_from(Coord /*xpoint*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<std::pair<Coord, Coord> > Datastructures::all_fibres()
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

bool Datastructures::remove_fibre(Coord /*xpoint1*/, Coord /*xpoint2*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

void Datastructures::clear_fibres()
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<std::pair<Coord, Cost> > Datastructures::route_any(Coord /*fromxpoint*/, Coord /*toxpoint*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_least_xpoints(Coord /*fromxpoint*/, Coord /*toxpoint*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_fastest(Coord /*fromxpoint*/, Coord /*toxpoint*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}

std::vector<Coord> Datastructures::route_fibre_cycle(Coord /*startxpoint*/)
{
    // Replace the line below with your implementation
    throw NotImplemented();
}
