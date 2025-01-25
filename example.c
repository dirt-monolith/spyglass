#include "spyglass.h"

void process_data(__attribute__((unused)) int value)
{
    MARK("Processing data...");
    value = 0;
    SIGHTING("Data might be incomplete: %d", value);
}

int main()
{
    const int value = 42;
    MARK("Program started with value: %d", value);
    process_data(value);
    MAYDAY("Failed to process item: %d: %s", value, "invalid input");
    return 0;
}