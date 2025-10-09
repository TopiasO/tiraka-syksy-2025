// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <source_location>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <memory>

// Type for beacon IDs
using BeaconID = std::string;
using Name = std::string;

// Return value for cases where required beacon was not found
BeaconID const NO_BEACON= "--NO_BEACON--";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "-- NO_NAME --";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for color (RGB)
struct Color
{
    int r = NO_VALUE;
    int g = NO_VALUE;
    int b = NO_VALUE;
};

// Equality and non-equality comparisons for Colors
inline bool operator==(Color c1, Color c2) { return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b; }
inline bool operator!=(Color c1, Color c2) { return !(c1==c2); }

// Return value for cases where color was not found
Color const NO_COLOR = {NO_VALUE, NO_VALUE, NO_VALUE};

// Type for light transmission cost (used only in the second assignment)
using Cost = int;

// Return value for cases where cost is unknown
Cost const NO_COST = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    explicit NotImplemented(std::string const msg = "",
                            const std::source_location location = std::source_location::current())
        : msg_{}
    {
        std::string funcname = location.function_name();
        if (auto namestart = funcname.find_last_of(':'); namestart != std::string::npos)
        { funcname.erase(0, namestart+1); }
        if (auto nameend = funcname.find_first_of('('); nameend != std::string::npos)
        { funcname.erase(nameend, std::string::npos); }
        msg_ = (!msg.empty() ? msg+" in " : "")+funcname+"()";
    }
    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

// This is the class you are supposed to implement


/* Struct that represents a single beacon.
 * First 4 fields should be self explanatory.
 * total_color_sum stores the sum of all incoming colors.
 * true total color value is then calculated by adding og_color to total_color_sum
 * and dividing each color value by the total_color_denominator, which is the amount of
 * beacons contributing to total_color_sum.
 * inbeams is a vector of all the beacons that are directly sending their beam
 * to this Beacon, empty if there are no such beams.
 * outbeam is nullptr if this Beacon isn't sending a beam to any other Beacon, otherwise
 * it is a pointer to a Beacon.
*/
struct Beacon {
    BeaconID id;
    Name name;
    Coord xy;
    Color og_color;
    Color total_color_sum = Color(0,0,0);
    std::vector<std::shared_ptr<Beacon>> inbeams = {};
    std::shared_ptr<Beacon> outbeam = nullptr;
    int total_color_denominator = 1;
};




//type for the main datastructure used for storing all the beacons.
using Beacon_uo_map = std::unordered_map<BeaconID, std::shared_ptr<Beacon>>;

//Used for getting the beacons sorted alphabetically.
using Beacon_name_map = std::multimap<Name, std::shared_ptr<Beacon>>;

//Used for sort_brightness, min_ and max_brightness.
using Beacon_brightness_map = std::multimap<int, std::shared_ptr<Beacon>>;

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // A operations

    //Worst case = W, Average case = A, Best case = B.
    //If a line code inside an operation isn't mentioned in an estimate
    //it means that I assumed that its complexity is constant.

    // Estimate of performance:
    //W(n) in the size of beacon_map_.
    //A/B(log n) in the size of beacon_map_.
    // Short rationale for estimate:
    //beacon_map_.emplace is constant on average and linear in the size of the
    //container in the worst case. -> W = n , A/B case= 1.
    //name_map_/brightness_map_.emplace() are both logarithmic in the size of the
    //container. -> W = n + log n, A/B = 1 + log n.
    //We get W = n and A/B = log n.
    bool add_beacon(BeaconID id, Name const& name, Coord xy, Color color);

    // Estimate of performance: W/A/B(1)
    // Short rationale for estimate:
    //unordered_map.size() is constant.
    int beacon_count();

    // Estimate of performance: W/A/B(n) in the size of beacon_map_.
    // Short rationale for estimate:
    //unordered_map.clear() is linear in the size of the container
    //and map.clear() is linear in the size of the container.
    void clear_beacons();

    // Estimate of performance: W/A/B(n) in the size of beacon_map_.
    // Short rationale for estimate:
    //A for loop through all the elements in beacon_map_ is linear in its size.
    std::vector<BeaconID> all_beacons();

    // Estimate of performance: W(n) in the size of beacon_map_. A/B(1).
    // Short rationale for estimate:
    //unordered_map.contains() is constant on average, worst case linear in the
    //size of the container. -> W = n, A/B = 1.
    //unordered_map.at() is constant on average, worst case linear in size of the
    //container. -> W = n + n, A/B = 1 + 1.
    //We get W = n and A/B = 1.
    Name get_name(BeaconID id);

    // Estimate of performance: W(n) in the size of beacon_map_. A/B(1).
    // Short rationale for estimate: Same as in get_name.
    Coord get_coordinates(BeaconID id);

    // Estimate of performance: W(n) in the size of beacon_map_. A/B(1).
    // Short rationale for estimate: Same as in get_name.
    Color get_color(BeaconID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: W/A/B(n) in the size of name_map_.
    // Short rationale for estimate: A for loop through all the elements
    //in name_map_ is linear in its size.
    std::vector<BeaconID> beacons_alphabetically();

    // Estimate of performance: W/A/B(n) in the size of brightness_map_.
    // Short rationale for estimate: Same as in beacons_alphabetically.
    std::vector<BeaconID> beacons_brightness_increasing();

    // Estimate of performance: W/A/B(log n) in the size of brightness_map_.
    // Short rationale for estimate:
    BeaconID min_brightness();

    // Estimate of performance:
    // Short rationale for estimate:
    BeaconID max_brightness();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<BeaconID> find_beacons(Name const& name);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_beacon_name(BeaconID id, Name const& newname);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_lightbeam(BeaconID sourceid, BeaconID targetid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<BeaconID> get_lightsources(BeaconID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<BeaconID> path_outbeam(BeaconID id);

    // B operations

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<BeaconID> path_inbeam_longest(BeaconID id);

    // Estimate of performance:
    // Short rationale for estimate:
    Color total_color(BeaconID id);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_fibre(Coord xpoint1, Coord xpoint2, Cost cost);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<Coord> all_xpoints();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Coord, Cost>> get_fibres_from(Coord xpoint);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Coord, Coord>> all_fibres();

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_fibre(Coord xpoint1, Coord xpoint2);

    // Estimate of performance:
    // Short rationale for estimate:
    void clear_fibres();

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Coord, Cost>> route_any(Coord fromxpoint, Coord toxpoint);

    // C operations

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Coord, Cost>> route_least_xpoints(Coord fromxpoint, Coord toxpoint);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::pair<Coord, Cost>> route_fastest(Coord fromxpoint, Coord toxpoint);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<Coord> route_fibre_cycle(Coord startxpoint);

private:
    // Explain below your rationale for choosing the data structures you use in this class.

    // Add stuff needed for your class implementation below

    /*Main datastructure for the program is a std::unordered_map<BeaconID, beacon>.
     * This was chosen because the 4 most used operations all require fetching beacons
     * by ID and unordered_map allows this to be done in stadard time on average.
    */
    Beacon_uo_map beacon_map_;

    //Ordered map where names of beacons and their pointers are stored.
    //Without this data structure. Getting the beacons in an alphabetical
    //order would require beacon names and beacon pointers to be extracted into
    //a new data structure that would be sorted by name and then moved to a
    //vector that is returned in the methods that need the beacons to be sorted alphabetically.
    Beacon_name_map name_map_;

    //Ordered map where the brightness of the og_color of a beacon and a pointer
    //to said beacon is stored. The reasons for using this data structure are the same as above.
    //Additionally max/min_brightness methods are fast by using map.end()/begin().
    Beacon_brightness_map brightness_map_;
};

#endif // DATASTRUCTURES_HH
