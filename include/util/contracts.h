#ifndef HG_COLT_CONTRACTS
#define HG_COLT_CONTRACTS

#include <cassert>

/// @brief Creates a precondition for a function body
#define COLT_PRE(condition) { assert(condition);
/// @brief Creates a postcondition for a function body
#define COLT_POST(condition) assert(condition); }
/// @brief Begins a postcondition if a function does not have a precondition
#define COLT_BEGIN() {
/// @brief Ends a precondition if a function does not have a postcondition
#define COLT_END() }

#endif //!HG_COLT_CONTRACTS