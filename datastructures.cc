// Datastructures.cc
// TESTIPALAUTUS


#include "datastructures.hh"

#include <random>
#include <algorithm>
#include <deque>
#include <stack>
#include <queue>

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
        int brightness = 3 * color.r + 6 * color.g + color.b;
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
    name_map_.clear();
    brightness_map_.clear();
}

std::vector<BeaconID> Datastructures::all_beacons()
{
    //Initialize result vector
    std::vector<BeaconID> result = {};

    for (const auto& [id, beacon] : beacon_map_ ) {
        result.push_back(id);
    }
    //result now contains every beacon in beacon_map_
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
    if (brightness_map_.empty()) {
        return NO_BEACON;
    }
    //->second=Beacon tied to this key.
    return brightness_map_.begin()->second->id;
}

BeaconID Datastructures::max_brightness()
{
    if (brightness_map_.empty()) {
        return NO_BEACON;
    }
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


bool Datastructures::add_lightbeam(BeaconID sourceid, BeaconID targetid)
{
    //Check that both beacons exist. Check that source beacon isn't already
    //beaming someone else.
    if (!beacon_map_.contains(sourceid) || !beacon_map_.contains(targetid)
        || beacon_map_.at(sourceid)->outbeam != nullptr) {
        return false;
    }

    std::shared_ptr<Beacon> source_beacon = beacon_map_.at(sourceid);
    std::shared_ptr<Beacon> target_beacon = beacon_map_.at(targetid);

    //Save that source beacon sends it beam to target beacon.
    source_beacon->outbeam = target_beacon;
    target_beacon->inbeams.insert(source_beacon->id);

    //This lightbeam will affect everyone in the outbeam of target_beacon.
    //Go through the new outbeam linked list and make the same changes for everyone.

    //Beacons only send out their total color.
    //Total color is the average of (incoming beams + og_color).
    while (target_beacon != nullptr) {
        Color source_total_color = get_total_color(sourceid);
        target_beacon->total_color_sum.r += source_total_color.r;
        target_beacon->total_color_sum.g += source_total_color.g;
        target_beacon->total_color_sum.b += source_total_color.b;
        //iterate
        source_beacon = target_beacon;
        target_beacon = target_beacon->outbeam;
    }



    return true;
}

std::vector<BeaconID> Datastructures::get_lightsources(BeaconID id)
{
    std::vector<BeaconID> result = {};

    if (!beacon_map_.contains(id)) {
        result.push_back(NO_BEACON);
        return result;
    }
    const auto& inbeams = beacon_map_.at(id)->inbeams;

    //The set inbeams is already sorted in ascending order by ids
    for (const auto& beacon_id: inbeams) {
        result.push_back(beacon_id);
    }
    return result;
}

std::vector<BeaconID> Datastructures::path_outbeam(BeaconID id)
{
    //shii
    std::vector<BeaconID> result = {};
    if (!beacon_map_.contains(id)) {
        result.push_back(NO_BEACON);
        return result;
    }
    //Add id to results
    result.push_back(id);

    //Create beacon_ptr to id.
    std::shared_ptr<Beacon> beacon_ptr = beacon_map_.at(id);

    //Go through the linked list of outbeams and ids to result.
    while (beacon_ptr->outbeam != nullptr) {
        beacon_ptr = beacon_ptr->outbeam;
        result.push_back(beacon_ptr->id);
    }

    return result;
}

std::vector<BeaconID> Datastructures::path_inbeam_longest(BeaconID id)
{
    std::vector<BeaconID> longest_inbeam_route = {};
    if (!beacon_map_.contains(id)) {
        longest_inbeam_route.push_back(NO_BEACON);
        return longest_inbeam_route;
    }

    longest_inbeam_route = get_longest_inbeam_route(id);
    return longest_inbeam_route;
}

Color Datastructures::total_color(BeaconID id)
{
    if (!beacon_map_.contains(id)) {
        return NO_COLOR;
    }
    Color tc = get_total_color(id);
    return tc;
}

bool Datastructures::add_fibre(Coord xpoint1, Coord xpoint2, Cost cost)
{
    if (xpoint1 == xpoint2) {
        return false;
    }
    //Try emplacing the coords to fibers.
    //If one already exists -> nothing happens.
    fibres_.try_emplace(xpoint1, std::make_shared<Fibre_node>(xpoint1));
    fibres_.try_emplace(xpoint2, std::make_shared<Fibre_node>(xpoint2));
    std::shared_ptr<Fibre_node> node1 = fibres_.at(xpoint1);
    std::shared_ptr<Fibre_node> node2 = fibres_.at(xpoint2);

    //Check if the fiber nodes are connected.
    //If yes do nothing and return false.
    //If not connect them and return true.
    if (node1->edges.contains(xpoint2)) {
        return false;
    }
    node1->edges.emplace(xpoint2, cost);
    node2->edges.emplace(xpoint1, cost);

    //Add fibre to unique_fibres.
    //Make sure pair.first < pair.second.
    if (xpoint1 < xpoint2) {
        unique_fibres_.emplace(xpoint1, xpoint2);
    } else {
        unique_fibres_.emplace(xpoint2, xpoint1);
    }

    return true;
}

std::vector<Coord> Datastructures::all_xpoints()
{
    std::vector<Coord> result = {};
    //Iterate through all fibers.
    for (const auto& [coord, ptr] : fibres_) {
        result.push_back(coord);
    }
    //Sort coordinates.
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::pair<Coord, Cost> > Datastructures::get_fibres_from(Coord xpoint)
{
    std::vector<std::pair<Coord, Cost>> fibres_from = {};
    if (!fibres_.contains(xpoint)) {
        return fibres_from;
    }

    //Iterate through the map edges. push back edges in to vector.
    //Edges are already sorted by coordinate.
    for (const auto& [goes_to, with_cost] : fibres_.at(xpoint)->edges) {
        fibres_from.push_back(std::make_pair(goes_to, with_cost));
    }
    return fibres_from;
}

std::vector<std::pair<Coord, Coord> > Datastructures::all_fibres()
{
    std::vector<std::pair<Coord, Coord>> result = {};
    for (const auto& fibre: unique_fibres_) {
        result.push_back(fibre);
    }
    return result;
}

bool Datastructures::remove_fibre(Coord xpoint1, Coord xpoint2)
{
    std::pair<Coord, Coord> fibre = std::make_pair(NO_COORD, NO_COORD);
    //Check which coord is smaller.
    if (xpoint1 < xpoint2) {
        fibre = {xpoint1, xpoint2};
    }
    else {
        fibre = {xpoint2, xpoint1};
    }

    //Check if fibre exists.
    if (!unique_fibres_.contains(fibre)) {
        return false;
    }
    unique_fibres_.erase(fibre);

    std::shared_ptr<Fibre_node> f1 = fibres_.at(xpoint1);
    std::shared_ptr<Fibre_node> f2 = fibres_.at(xpoint2);
    f1->edges.erase(xpoint2);
    f2->edges.erase(xpoint1);

    if (f1->edges.empty()) {
        fibres_.erase(xpoint1);
    }
    if (f2->edges.empty()) {
        fibres_.erase(xpoint2);
    }
    return true;
}

void Datastructures::clear_fibres()
{
    fibres_.clear();
    unique_fibres_.clear();
}

std::vector<std::pair<Coord, Cost> > Datastructures::route_any(Coord fromxpoint, Coord toxpoint)
{
    //Breadth first search
    //result will be stored in this vector
    std::vector<std::pair<Coord, Cost>> route = {};

    //If no coordinate no route.
    if (!fibres_.contains(fromxpoint) || !fibres_.contains(toxpoint)) {
        return route;
    }

    //Initialize all nodes to be WHITE, have d=inf, pi=NIL.
    reset_fibre_graph_state();

    //Q works as a checklist for the BFS
    std::deque<std::shared_ptr<Fibre_node>> Q = {};

    std::shared_ptr<Fibre_node> s = fibres_.at(fromxpoint);
    s->color = GRAY;
    s->d = 0;
    Q.push_back(s);

    //v is used on this block level
    std::shared_ptr<Fibre_node> v = nullptr;

    while (!Q.empty()) {
        std::shared_ptr<Fibre_node> u = Q.front();
        Q.pop_front();
        for (const auto& [coord, cost] : u->edges) {
            v = fibres_.at(coord);
            if (v->color == WHITE) {
                v->color = GRAY;
                v->d = u->d+cost;
                v->path_back = u;
                Q.push_back(v);
            }
            if (v->location == toxpoint) {
                Q.clear();
                break;
            }
        }
        u->color = BLACK;
    }
    if (v->location == toxpoint) {
        while (v->path_back != nullptr) {
            route.push_back(std::make_pair(v->location, v->d));
            v = v->path_back;
        }
        route.push_back(std::make_pair(fromxpoint, s->d));
        std::reverse(route.begin(), route.end());
    }
    return route;
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_least_xpoints(Coord fromxpoint, Coord toxpoint)
{
    return route_any(fromxpoint, toxpoint);
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_fastest(Coord fromxpoint, Coord toxpoint)
{
    //Dijkstras algorithm
    std::vector<std::pair<Coord, Cost>> fastest_route = {};

    if (!fibres_.contains(fromxpoint) || !fibres_.contains(toxpoint)) {
        return fastest_route;
    }
    //Change all nodes to white, d to inf, path_back to NIL.
    reset_fibre_graph_state();

    //Priority queue ordered by d.
    std::priority_queue<std::shared_ptr<Fibre_node>,
                        std::vector<std::shared_ptr<Fibre_node>>, Cmp_fibre_ptrs> Q = {};

    std::shared_ptr<Fibre_node> s = fibres_.at(fromxpoint);
    s->color = GRAY;
    s->d = 0;
    Q.push(s);

    //Dijkstras algorithm.
    while (!Q.empty()) {
        std::shared_ptr<Fibre_node> u = Q.top();
        Q.pop();
        for (const auto& [coord, edges] : u->edges) {
            std::shared_ptr<Fibre_node> v = fibres_.at(coord);
            if (relax(u, v)) {
                if (v->color == WHITE) {
                    v->color = GRAY;
                }
                Q.push(v);
            }
        }
        u->color = BLACK;
    }
    s = fibres_.at(toxpoint);

    //S wasn't reached it it's still white
    if (s->color == WHITE) {
        return fastest_route;
    }

    //Traverse through the route backwards.
    while (s != nullptr) {
        fastest_route.emplace_back(s->location, s->d);
        s = s->path_back;
    }
    std::reverse(fastest_route.begin(), fastest_route.end());
    return fastest_route;
}

std::vector<Coord> Datastructures::route_fibre_cycle(Coord startxpoint)
{
    std::vector<Coord> loop = {};
    //Check point exists
    if (!fibres_.contains(startxpoint)) {
        return loop;
    }

    reset_fibre_graph_state();

    std::stack<std::shared_ptr<Fibre_node>> S = {};

    std::shared_ptr<Fibre_node> s = fibres_.at(startxpoint);
    //Initialize u, v here. They are needed after the while loop.
    std::shared_ptr<Fibre_node> u = nullptr;
    std::shared_ptr<Fibre_node> v = nullptr;
    bool loop_found = false;
    S.push(s);

    //Depth first search
    while (!S.empty()) {
        u = S.top();
        S.pop();
        if (u->color == WHITE) {
            u->color = GRAY;
            S.push(u);
            for (const auto& [coord, cost] : u->edges) {
                v = fibres_.at(coord);
                if (u->path_back == v) {
                    continue;
                }
                if (v->color == WHITE) {
                    v->path_back = u;
                    S.push(v);
                }
                //loop found
                else if (v->color == GRAY) {
                    loop_found = true;
                    //Fullfill outer while loops condition.
                    while (!S.empty()) {
                        S.pop();
                    }
                    //Break inner for loop.
                    break;
                }
            }

        } else {
            u->color = BLACK;
        }
    }
    if (!loop_found) {
        return loop;
    }
    loop.push_back(v->location);
    while (u != nullptr) {
        loop.push_back(u->location);
        u = u->path_back;
    }
    std::reverse(loop.begin(), loop.end());
    return loop;
}

std::vector<BeaconID> Datastructures::get_longest_inbeam_route(BeaconID id) const
{
    std::vector<BeaconID> longest_route = {};

    //Easier to read like this
    std::shared_ptr<Beacon> this_beacon = beacon_map_.at(id);

    //Iterate through the ids corresponding inbeams.
    for (const auto& inbeam_id: this_beacon->inbeams) {
        //Get the longest inbeam route leading to the beacon with the id inbeam_id
        std::vector<BeaconID> new_route = get_longest_inbeam_route(inbeam_id);

        //Compare lengths
        if (longest_route.size() < new_route.size()) {
            //Huh size does matter
            longest_route = new_route;
        }
    }

    //If the beacon had no inbeams, the for loop will be skipped
    //and a vector with only this id will be returned. Otherwise
    //append this id to the end of the longest known route and
    //return it.
    longest_route.push_back(id);
    return longest_route;
}

Color Datastructures::get_total_color(BeaconID id) const
{
    //W(n), B(1).
    std::shared_ptr<Beacon> bcn = beacon_map_.at(id);

    //W/A/B(1)
    int inbeam_c = bcn->inbeams.size();
    inbeam_c += 1;

    Color tc = Color(bcn->total_color_sum.r/inbeam_c, bcn->total_color_sum.g/inbeam_c,
                     bcn->total_color_sum.b/inbeam_c);
    return tc;
}

void Datastructures::reset_fibre_graph_state()
{
    //Change nodes to have color=WHITE, d=infity, pi=NIL.
    for (const auto& [xy, fibre_ptr] : fibres_) {
        fibre_ptr->color = WHITE;
        fibre_ptr->d = -1;
        fibre_ptr->path_back = nullptr;
    }
}

bool Datastructures::relax(std::shared_ptr<Fibre_node> u, std::shared_ptr<Fibre_node> v)
{
    int costUV = u->edges.at(v->location);
    if (v->d > u->d+costUV || v->d == -1) {
        v->d = u->d+costUV;
        v->path_back = u;
        return true;
    }
    return false;
}

/*std::shared_ptr<Datastructures::Fibre_node> Datastructures::dfs_recursive(std::shared_ptr<Fibre_node> s)
{
    s->color = GRAY;
    for (const auto& [coord, cost] : s->edges) {
        std::shared_ptr<Fibre_node> v = fibres_.at(coord);
        if (v->color == WHITE) {
            v->path_back = s;
            v = dfs_recursive(v);
            if (v->color == BLACK) {
                return v;
            }
        }
        if (s->path_back != v && v->color == GRAY) {
            v->color = BLACK;
            return s;
        }
    }
    return nullptr;
}*/




