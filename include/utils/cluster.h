#ifndef INCLUDE_UTILS_CLUSTER_H_
#define INCLUDE_UTILS_CLUSTER_H_
#include <glog/logging.h>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "proto/cluster.pb.h"
#include "utils/cluster_rt.h"

using std::shared_ptr;
using std::string;
using std::vector;

namespace singa {

/**
 * Cluster is a singleton object, which provides cluster configuations,
 * e.g., the topology of the cluster.
 * All IDs start from 0.
 */
class Cluster {
 public:
  static shared_ptr<Cluster> Get();
  static shared_ptr<Cluster> Get(const ClusterProto& cluster, int procs_id);

  const int nserver_groups()const{ return cluster_.nserver_groups(); }
  const int nworker_groups()const { return cluster_.nworker_groups(); }
  int nworkers_per_group()const {return cluster_.nworkers_per_group();}
  int nservers_per_group()const {return cluster_.nservers_per_group();}
  int nworkers_per_procs()const{return cluster_.nworkers_per_procs();}
  int nservers_per_procs()const{return cluster_.nservers_per_procs();}
  int nworker_groups_per_server_group() const {
    return cluster_.nworker_groups()/cluster_.nserver_groups();
  }

  /**
   * @return true if the calling procs has server threads, otherwise false
   */
  bool has_server()const {
    if(server_worker_separate()){
      CHECK_LT(procs_id_, nprocs());
      return procs_id_>=nworker_procs();
    }else
      return procs_id_<nserver_procs();
  }
  /**
   * @return true if the calling procs has worker threads.
   */
  bool has_worker()const {
    if(server_worker_separate()){
      return procs_id_<nworker_procs();
    }else
      return procs_id_<nprocs();
  }
  /**
   * @return global procs id, which starts from 0.
   */
  int procs_id()const {return procs_id_;}
  bool server_worker_separate() const {
    return cluster_.server_worker_separate();
  }
  int nworker_procs() const {
    return nworker_groups()*nworkers_per_group()/nworkers_per_procs();
  }
  int nserver_procs() const {
    return nserver_groups()*nservers_per_group()/nservers_per_procs();
  }
  int nprocs() const {
    return cluster_.nprocs();
  }

  const string endpoint() const {
    return endpoint(procs_id());
  }
  /**
   * @return endpoint of the router of a procs with the specified id
   */
  const string endpoint(int procs_id) const {
    CHECK_LT(procs_id, nprocs());
    CHECK_GE(procs_id, 0);
    return endpoints_.at(procs_id);
  }
  const string workspace() {return cluster_.workspace();}
  const string vis_folder(){
    return cluster_.workspace()+"/visualization";
  }
  const string log_folder(){
    if(cluster_.has_log_dir()){
      return cluster_.workspace()+"log";
    }else
      return "";
  }

  const int stub_timeout() const {
    return cluster_.stub_timeout();
  }
  const int worker_timeout() const {
    return cluster_.worker_timeout();
  }
  const int server_timeout() const {
    return cluster_.server_timeout();
  }

  const bool server_update() const {
    return cluster_.server_update();
  }

  const bool share_memory() const {
    return cluster_.share_memory();
  }

  /**
   * bandwidth MB/s
  float bandwidth() const {
    return cluster_.bandwidth();
  }
   */

  shared_ptr<ClusterRuntime> runtime() const {
    return cluster_rt_;
  }

 private:
  Cluster(const ClusterProto &cluster, int procs_id) ;
  void SetupFolders(const ClusterProto &cluster);

 private:
  int procs_id_;
  std::vector<std::string> endpoints_;
  // cluster config proto
  ClusterProto cluster_;
  shared_ptr<ClusterRuntime> cluster_rt_;
  // make this class a singlton
  static shared_ptr<Cluster> instance_;
};

}  // namespace singa

#endif  // INCLUDE_UTILS_CLUSTER_H_