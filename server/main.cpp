

#include <cache/executor.h>
#include <server/server.h>

#include <string>

int main(int argc, char** argv) {
  size_t cache_size = 1000;
  size_t queue_max_size = 100;
  if (argc > 1) {
    cache_size = std::stoul(argv[1]);
  }
  if (argc > 2) {
    queue_max_size = std::stoul(argv[2]);
  }

  server::cache::TaskQueue task_queue(queue_max_size);
  server::cache::ResultQueue result_queue(queue_max_size);
  server::cache::Cache cache(cache_size);
  server::cache::TaskExecutor executor(task_queue, result_queue, cache);

  std::thread executor_thread(&server::cache::TaskExecutor::Start, &executor);

  server::Server server(8080, task_queue, result_queue);
  std::thread result_thread(&server::Server::Dispatch, &server);

  server.Serve();

  executor_thread.join();
}
