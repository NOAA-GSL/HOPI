/// @file system.cpp
/*
 * Project:         HOPI
 * File:            system.cpp
 * Date:            Sep 30, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Oct 11, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */


#include "boost/mpi.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

#include "hopi.hpp"


#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <random>
#include <utility>
#include <unordered_set>

#include <valarray>



template<typename T>
class Matrix {


    class View {
        View(std::valarray<double>& data, std::size_t offset, std::size_t size, std::size_t stride) : data_(data), offset_(offset), size_(size), stride_(stride) {
        }

        T& operator[](const std::size_t index)
        {
            assert(index < this->size());
            return data_[offset_ + index*stride_];
        }

        std::size_t size() const {
            return size_;
        }

        private:
            std::valarray<T>& data_;
            std::size_t       offset_;
            std::size_t       size_;
            std::size_t       stride_;
    };


    Matrix(std::size_t R, std::size_t C) : data_(R*C), nrow_(R), ncol_(C) {
    }

    View operator[](const std::size_t r)
    {
       return this->row(r);
    }

    View row(const std::size_t r)
    {
       return View(data_, r*ncol_, ncol_, 1);
    }

    View column(const std::size_t c)
    {
       return View(data_, c, nrow_, ncol_);
    }

    std::valarray<T> data_;
    std::size_t      nrow_;
    std::size_t      ncol_;
};



template<typename T, std::size_t N>
void display(const std::array<T, N>& a) {
    namespace mpi = boost::mpi;
    mpi::environment env;
	mpi::communicator world;
	auto my_rank   = world.rank();
    if( my_rank == 0 ) {
    for(auto e : a){
        std::cout << e << " ";
    }
    std::cout << std::endl;
    }
}


template <typename T, typename IndexedType>
void fill_random(IndexedType& xyz, const std::pair<T, T> xyz_range = std::make_pair<T,T>(0,1)) {
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution<T> unif(std::get<0>(xyz_range), std::get<1>(xyz_range));
    const auto num_xyz = std::size(xyz);
    const auto num_dim = std::size(xyz[0]);
    for(std::ptrdiff_t i = 0; i < num_xyz; ++i){
        for(std::ptrdiff_t j = 0; j < num_dim; ++j){
            xyz[i][j] = unif(re);
        }
    }
}

template <typename IndexedType>
void duplicate_vector(const std::size_t n, IndexedType& xyz) {
    const auto num_xyz = std::size(xyz);
    const auto num_dim = std::size(xyz[0]);
    xyz.resize((n+1)*num_xyz);
    for(std::ptrdiff_t r = 1; r < (n+1); ++r){
        for(std::ptrdiff_t i = 0; i < num_xyz; ++i){
            for(std::ptrdiff_t d = 0; d < num_dim; ++d){
                xyz[r*num_xyz+i][d] = xyz[i][d];
            }
        }
    }
}

struct ArrayHasher {
    template<typename T, std::size_t N>
    std::size_t operator()(const std::array<T, N>& a) const {
        std::size_t h = 0;
        for (auto e : a) {
            h ^= std::hash<T>{}(e) + 0x9e3779b9 + (h << 6) + (h >> 2); 
        }
        return h;
    } 
};


template <std::size_t N, typename T, typename A>
void fill_variables(const std::vector<std::array<T, N>, A>& xyz, std::vector<T, A>& var) {
    var.resize(xyz.size());
    for (std::size_t i = 0; i < xyz.size(); ++i) {
        var[i] = i + 3.1415;
        for (std::size_t j = 0; j < N; j += 2) {
            var[i] += 0.13 * xyz[j];
        }
        for (std::size_t j = 1; j < N; j += 2) {
            var[i] -= 0.10 * xyz[j];
        }
    }
}


namespace detail_extractor {
template<typename PairType>
struct pair_extractor {
	using bound_type = typename PairType::first_type;
	using key_type   = typename PairType::second_type;

	const bound_type& operator()(PairType const& pair) const {
		return pair.first;
	}
};
template<typename TupleType>
struct tuple_extractor {
	using bound_type = typename std::tuple_element<0,TupleType>::type;
	const bound_type& operator()(TupleType const& tup) const {
		return std::get<0>(tup);
	}
};
}




class UniqueMap {

public:

    template<typename T, typename A>
    void setup(const std::vector<T,A>& vals) {
        unique_idx_.clear();
        non_unique_map_.clear();
        std::unordered_map<T, std::size_t, ArrayHasher> unique_id_map;
        for(auto i = 0; i < vals.size(); ++i){
            auto kv_pair = unique_id_map.find(vals[i]);
            if( kv_pair == std::end(unique_id_map) ){ // Unique
                unique_id_map.emplace(vals[i], i);
                unique_idx_.push_back(i);
            }
            else {                                    // Not Unique
                non_unique_map_.push_back(std::make_pair(i, kv_pair->second));
            }
        }
    }

    template<typename T, typename A1, typename A2>
    void reduce_to_unique(const std::vector<T,A2>& vin, std::vector<T,A1>& vout) const {
        const auto num_unique = unique_idx_.size();
        vout.resize(num_unique);
        for(auto i = 0; i < num_unique; ++i){
            vout[i] = vin[unique_idx_[i]];
        }
    }

    template<typename T, typename A1, typename A2>
    void expand_to_non_unique(const std::vector<T,A2>& vin, std::vector<T,A1>& vout) const {
        vout.resize(unique_idx_.size() + non_unique_map_.size());
        for(std::size_t i = 0; i < unique_idx_.size(); ++i){
            vout[unique_idx_[i]] = vin[i];
        }
        for(std::size_t i = 0; i < non_unique_map_.size(); ++i){
            vout[non_unique_map_[i].first] = vout[non_unique_map_[i].second];
        }
    }

    std::size_t num_total() const {
        return unique_idx_.size() + non_unique_map_.size();
    }

    std::size_t num_unique() const {
        return unique_idx_.size();
    }

private:
    std::vector<std::size_t>                        unique_idx_;     // [list of unique ID's]
    std::vector<std::pair<std::size_t,std::size_t>> non_unique_map_; // [{orig_idx, unique_idx}, {}, ... {}]
};



int
main(int argc, char* argv[])
{
	namespace mpi = boost::mpi;
    mpi::environment env;
	mpi::communicator world;
	auto my_rank   = world.rank();
	auto num_ranks = world.size();

    using value_type         = double;
    constexpr std::size_t ND = 3;    // # of Dimensions
    constexpr std::size_t Nt = 1000; // # of Target Points
    constexpr std::size_t Ns = 1000; // # of Source Points
    constexpr std::size_t Nc = 50;   // # of Points in Cloud
    constexpr std::size_t Ntdup = 3; // # of Target Duplicates
    constexpr std::size_t Nsdup = 5; // # of Source Duplicates

    // Init Data
    std::vector<std::array<value_type, ND>> target_xyz(Nt);
    std::vector<std::array<value_type, ND>> source_xyz(Ns);

    // Fill Random Targets & Sources
    fill_random<value_type>(target_xyz, {-100, 100});
    fill_random<value_type>(source_xyz, {-100, 100});

    // Duplicate
    duplicate_vector(Ntdup, target_xyz);
    duplicate_vector(Nsdup, source_xyz);

    // ----------------------------------------------------------
    // Remove Duplicates
    // ----------------------------------------------------------

    UniqueMap target_unique_map;
    target_unique_map.setup(target_xyz);
    std::vector<std::array<value_type, ND>> target_unique_xyz;
    target_unique_map.reduce_to_unique(target_xyz, target_unique_xyz);
    if( my_rank == 0 ) {
        std::cout << "Total Targets  = " << target_unique_map.num_total()  << std::endl;
        std::cout << "Unique Targets = " << target_unique_map.num_unique() << std::endl;
        std::cout << "Unique T XYZ   = " << target_unique_xyz.size()       << std::endl;
    }

    UniqueMap source_unique_map;
    source_unique_map.setup(source_xyz);
    std::vector<std::array<value_type, ND>> source_unique_xyz;
    target_unique_map.reduce_to_unique(source_xyz, source_unique_xyz);
    if( my_rank == 0 ) {
        std::cout << "Total Sources  = " << source_unique_map.num_total()  << std::endl;
        std::cout << "Unique Sources = " << source_unique_map.num_unique() << std::endl;
        std::cout << "Unique S XYZ   = " << source_unique_xyz.size()       << std::endl;
    }


    // ----------------------------------------------------------
    // Create Search Data Structure for Targets
    // ----------------------------------------------------------

    using target_bound_type     = hopi::spatial::bound::Box<value_type,ND>;
    using target_array_type     = typename target_bound_type::array_type;
    using target_value_type     = std::pair<target_bound_type, std::size_t>;   // (bound, key)
    using target_extractor_type = detail_extractor::pair_extractor<target_value_type>;
    using target_sds_type       = hopi::spatial::shared::index::RTree<target_value_type, target_extractor_type>;

    target_array_type txyz;
    target_sds_type target_sds;
    for(auto i = 0; i < target_unique_xyz.size(); ++i){
        txyz = target_unique_xyz[i];
        auto target_bound = target_bound_type(txyz, txyz);
        auto target_value = target_value_type(target_bound, i);
        target_sds.insert(target_value);
    }

    // ----------------------------------------------------------
    // Create Search Data Structure for Sources
    // ----------------------------------------------------------
    
    using source_bound_type     = hopi::spatial::bound::Box<value_type,ND>;
    using source_array_type     = typename source_bound_type::array_type;
    using source_value_type     = std::pair<source_bound_type, std::size_t>;   // (bound, key)
    using source_extractor_type = detail_extractor::pair_extractor<source_value_type>;
    using source_sds_type       = hopi::spatial::shared::index::RTree<source_value_type, source_extractor_type>;

    source_array_type sxyz;
    source_sds_type source_sds;
    for(auto i = 0; i < source_unique_xyz.size(); ++i){
        sxyz = source_unique_xyz[i];
        auto source_bound = source_bound_type(sxyz, sxyz);
        auto source_value = source_value_type(source_bound, i);
        source_sds.insert(source_value);
    }

    // ----------------------------------------------------------
    // AllGather all Target Bounds on each Rank
    // ----------------------------------------------------------
    
    std::vector<target_bound_type> target_bounds_by_rank;
	const auto my_target_bnd = target_sds.bounds();
	mpi::all_gather(world, my_target_bnd, target_bounds_by_rank);

    std::vector<source_bound_type> source_bounds_by_rank;
	const auto my_source_bnd = source_sds.bounds();
	mpi::all_gather(world, my_source_bnd, source_bounds_by_rank);

    // ----------------------------------------------------------
    // Exchange Target locations
    // ----------------------------------------------------------
    //
    // Assuming we have non-overlapping target regions resulting 
    // from some algorithm (ie. RCB).  Send the targets from each 
    // processor that overlap with the rendevous rank.  Eliminate 
    // duplicates and set up send/recv buffers for future comms.  
    // 

    // ----------------------------------------------------------
    // Exchange Source locations
    // ----------------------------------------------------------
    //
    // Working off the assumption of performing the interpolation 
    // on the rank the target rendezvous at we need to send all
    // sources to the target ranks which we may overlap with.
    //
    
    using rank_type = int;
	std::map<rank_type, mpi::request> send_requests;
	std::map<rank_type, mpi::request> recv_requests;
	std::map<rank_type, std::vector<source_value_type>> send_to_ranks;   // {recv_rank, [{bbox, local ID to send}, ...] }
	std::map<rank_type, std::vector<source_value_type>> recv_from_ranks; // {send_rank, [{bbox, remote ID received}, ...]}
    constexpr value_type bbox_scale = 1.10; // scale box 10% larger

    // Loop over all source bounds posting recieves if expanded "my_target_bnd" overlaps
    auto expanded_my_target_bound = my_target_bnd;
    expanded_my_target_bound.scale(1.10);
    for(rank_type rank = 0; rank < num_ranks; ++rank){

        // Expanded source bound of rank
        auto expanded_source_bound = source_bounds_by_rank[rank];
        //expanded_source_bound.scale(bbox_scale);  // We can't ensure we send sources operate on points (not bounds)

        // Expect a send if we intersect
        if(hopi::spatial::bound::Intersects(expanded_my_target_bound,expanded_source_bound)) {
            recv_requests[rank] = world.irecv(rank, 0, recv_from_ranks[rank]);
        }
    }

    // Loop over all target ranks sending points that intersect
    for(rank_type rank = 0; rank < num_ranks; ++rank){

        // Expand each ranks target bound slightly to capture nearby sources
        auto expanded_target_bound = target_bounds_by_rank[rank];
        expanded_target_bound.scale(bbox_scale);

        // Get local sources which intersect ranks expanded target region
        std::vector<source_value_type> search_results;
		source_sds.query( hopi::spatial::shared::predicate::Intersects(expanded_target_bound), std::back_inserter(search_results) );

	    send_to_ranks.emplace(rank, search_results);
		send_requests[rank] = world.isend(rank, 0, send_to_ranks[rank]); 
    }
    
    // ----------------------------------------------------------
    // Recieve all source locations and place into Search Data Structure
    // ----------------------------------------------------------

    // Make mega list filtering out repeat values before searching
    
    using global_source_bound_type     = hopi::spatial::bound::Box<value_type,ND>;
    using global_source_array_type     = typename global_source_bound_type::array_type;
    using global_source_value_type     = std::tuple<global_source_bound_type, std::size_t, std::size_t>;   // (bound, rank, remote ID)
    using global_source_extractor_type = detail_extractor::tuple_extractor<global_source_value_type>;
    using global_source_sds_type       = hopi::spatial::shared::index::RTree<global_source_value_type, global_source_extractor_type>;

    global_source_sds_type global_source_sds;

    for(auto& [rank, req]: recv_requests){
        req.wait();                                           // Wait to receive data
        auto& pairs_from_rank = recv_from_ranks[rank];        // Get handle to data

        // Loop over each remote pair<bound, id> and create new search data structure
		for(auto& [remote_bnd, remote_id]: pairs_from_rank){
            global_source_sds.insert(global_source_value_type(remote_bnd,rank,remote_id));
        }
    }
    for(auto& [rank, req]: send_requests){
        req.wait();        
    }

    // ----------------------------------------------------------
    // Get nearest points for each Target
    // ----------------------------------------------------------
    // A lot of this could be re-used from before (OR extracted from target_sds)
    const auto num_unique_targets = target_unique_xyz.size();
    std::vector<std::vector<global_source_value_type>> target_nearest_points(num_unique_targets);
    for(auto i = 0; i < num_unique_targets; ++i){
        txyz = target_unique_xyz[i];
        auto target_bound = target_bound_type(txyz, txyz);

        // Search for Nc nearest source neighbors to target location
        global_source_sds.query(hopi::spatial::shared::predicate::Nearest(target_bound, Nc), std::back_inserter(target_nearest_points[i]));
    }

    // ----------------------------------------------------------
    // Build unique list from each processor we need
    // ----------------------------------------------------------

    // Use a set to sort and eliminate duplicates
    std::set<std::pair<std::size_t, std::size_t>> remote_id_by_rank;  // {{rank, remote_id}, {}, ..., {}}
    for(auto i = 0; i < num_unique_targets; ++i){
        const auto& point_cloud = target_nearest_points[i];
        for(auto j = 0; j < point_cloud.size(); ++j){
            auto rank_id = std::make_pair(std::get<1>(point_cloud[j]), std::get<2>(point_cloud[j]));
            remote_id_by_rank.insert(rank_id);
        }
    }
    // if( my_rank == 0 ) {
    //     std::cout << "Total Sources = " << remote_id_by_rank.size() << std::endl;
    //     for(auto& s : remote_id_by_rank) {
    //         std::cout << s.first << " " << s.second << std::endl;
    //     }
    // }

    // std::map<std::size_t, std::vector<std::size_t>> id_needed_from_rank;
    // std::for_each(std::begin(remote_id_by_rank), std::end(remote_id_by_rank), [&](const auto& id_by_rank){
    //     id_needed_from_rank[id_by_rank.first].push_back(id_by_rank.second);
    // });

    // // ----------------------------------------------------------
    // // Exchange ID we need to send
    // // ----------------------------------------------------------

	// std::map<rank_type, std::vector<std::size_t>> local_id_to_send;   // {send_rank: [local IDs to send, ...], ... }

    // // For each rank I sent coordinates to I should recieve a list back
    // for(auto& [rank, req]: send_requests){
    //     recv_requests[rank] = world.irecv(rank, 0, local_id_to_send[rank]);
    //     send_requests[rank] = world.isend(rank, 0, id_needed_from_rank[rank]);
    // }

    // // Process results into local data structures
    // std::vector<std::size_t> rank_i_send_to;
    // std::vector<std::size_t> count_i_send_to_each_rank;
    // std::vector<std::size_t> buffer_to_local_id_mapping;
    // for(auto& [rank, req]: recv_requests){
    //     req.wait();                                           // Wait to receive data
    //     auto& local_id_to_rank = local_id_to_send[rank];      // Get handle to data

    //     if(local_id_to_rank.size() > 0){
    //         rank_i_send_to.push_back(rank);
    //         count_i_send_to_each_rank.push_back(local_id_to_rank.size());
    //         buffer_to_local_id_mapping.insert(std::end(buffer_to_local_id_mapping), std::begin(local_id_to_rank), std::end(local_id_to_rank));
    //     }
    // }
    // for(auto& [rank, req]: send_requests){
    //     req.wait();        
    // }

    // // At this point each rank has
    // // ------------------------------
    // // rank_i_send_to[]  = [rank, ...]                  List of ranks I send sources to
    // // count_i_send_to[] = [#0, #1, ...]                Number of sources I send to each Rank
    // // buffer_to_local_id_mapping[i] = [Local ID, ...]  Local ID to place into send buffer at positions i
    // //

    // // Build Remote ID's to return to Source Ranks
    // // Process results into local data structures
    // std::vector<std::size_t> rank_i_recv_from;
    // std::vector<std::size_t> count_i_recv_from_each_rank;
    // //std::vector<std::size_t> buffer_to_rendez_id_mapping;

    // std::set<std::size_t> unique_rank_set;
    // std::for_each(std::begin(remote_id_by_rank), std::end(remote_id_by_rank), [&](const auto& id_by_rank){
    //     unique_rank_set.insert(id_by_rank.first);
    // });
    // std::copy(std::begin(unique_rank_set), std::end(unique_rank_set), std::back_inserter(rank_i_recv_from));
    // for(auto i = 0; i < rank_i_recv_from.size(); ++i){
    //     auto rank = rank_i_recv_from[i];
    //     count_i_recv_from_each_rank.push_back(id_needed_from_rank[rank].size());
    // }
    
    // // At this point each rank has
    // // ------------------------------
    // // rank_i_recv_to[]  = [rank, ...]                   List of ranks I recieve sources from
    // // count_i_send_to[] = [#0, #1, ...]                 Number of sources I recv from each Rank
    // // buffer_to_rendez_id_mapping[i] = [Local ID, ...]  Rendezvous ID to place into send buffer at positions i
    // //

    return EXIT_SUCCESS;
}