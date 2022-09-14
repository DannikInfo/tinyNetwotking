#ifndef TINYNETWORKING_THREADMANAGETYPE_H
#define TINYNETWORKING_THREADMANAGETYPE_H
namespace tcp {
    enum class threadManageType : bool {
        single_thread = false,
        thread_pool = true
    };
}
#endif //TINYNETWORKING_THREADMANAGETYPE_H
