#ifndef HERMES_H_
#define HERMES_H_

#include <cstdint>
#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "glog/logging.h"

#include "hermes_types.h"
#include "buffer_pool.h"
#include "rpc.h"
#include "id.h"

namespace hermes {
namespace api {

class Hermes {
 public:
  std::set<std::string> bucket_list_;
  std::set<std::string> vbucket_list_;

  // TODO(chogan): Temporarily public to facilitate iterative development.
  hermes::SharedMemoryContext context_;
  hermes::CommunicationContext comm_;
  hermes::RpcContext rpc_;
  hermes::Arena trans_arena_;
  std::string shmem_name_;
  std::string rpc_server_name_;

  /** if true will do more checks, warnings, expect slower code */
  const bool debug_mode_ = true;

  Hermes() {}

  explicit Hermes(SharedMemoryContext context) : context_(context) {}

  void Display_bucket() {
    for (auto it = bucket_list_.begin(); it != bucket_list_.end(); ++it)
      std::cout << *it << '\t';
    std::cout << '\n';
  }

  void Display_vbucket() {
    for (auto it = vbucket_list_.begin(); it != vbucket_list_.end(); ++it)
      std::cout << *it << '\t';
    std::cout << '\n';
  }

  bool IsApplicationCore();
  bool IsFirstRankOnNode();
  void AppBarrier();
  int GetProcessRank();
  int GetNodeId();
  int GetNumProcesses();
  void *GetAppCommunicator();
  void Finalize(bool force_rpc_shutdown = false);

  // MPI comms.
  // proxy/reference to Hermes core
};

class VBucket;

class Bucket;

typedef std::vector<unsigned char> Blob;

enum class PlacementPolicy {
  kRandom,
  kRoundRobin,
  kMinimizeIoTime,
};

struct Context {
  static int default_buffer_organizer_retries;

  PlacementPolicy policy;
  int buffer_organizer_retries;

  Context() : policy(PlacementPolicy::kRoundRobin),
              buffer_organizer_retries(default_buffer_organizer_retries) {}
};

struct TraitTag{};
typedef ID<TraitTag, int64_t, -1> THnd;

struct BucketTag{};
typedef ID<BucketTag, int64_t, -1> BHnd;

/** rename a bucket referred to by name only */
Status RenameBucket(const std::string &old_name,
                    const std::string &new_name,
                    Context &ctx);

/** transfer a blob between buckets */
Status TransferBlob(const Bucket &src_bkt,
                    const std::string &src_blob_name,
                    Bucket &dst_bkt,
                    const std::string &dst_blob_name,
                    Context &ctx);

std::shared_ptr<api::Hermes> InitHermes(const char *config_file = NULL,
                                        bool is_daemon = false,
                                        bool is_adapter = false);

}  // namespace api

std::shared_ptr<api::Hermes> InitHermes(Config *config, bool is_daemon = false,
                                        bool is_adapter = false);
std::shared_ptr<api::Hermes> InitHermesDaemon(char *config_file = NULL);
std::shared_ptr<api::Hermes> InitHermesDaemon(Config *config);
std::shared_ptr<api::Hermes> InitHermesClient(const char *config_file = NULL);

}  // namespace hermes

#endif  // HERMES_H_
