#include "spyglass.h"

int main()
{
    const int value = 42;
    SPYGLASS_LOG_INFO("Program started with value %d", value);
    SPYGLASS_LOG_WARN("This is a warning #%d", 1);
    SPYGLASS_LOG_ERROR("Failed to process item %d: %s", value, "invalid input");
    return 0;
}