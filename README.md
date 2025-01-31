# spyglass

A lightweight C99 multi-level logging library to spot troubles on the horizon.

# Usage

###### Simple Spotting:

```
MARK("This is a Mark level Spot");
SIGHTING("This is a Sight level Spot");
MAYDAY("This is a Mayday level Spot");
```

###### Variable Arguments

```
MARK("Program started with value: %d", value);
SIGHTING("Data might be incomplete: %d", value);
MAYDAY("Failed to process item: %d: %s", value, "invalid input");

```

## Configuration

###### Spotting:

```makefile
SPYGLASS_MAYDAY           # enable/disable Mayday Spot Level
SPYGLASS_SIGHT            # enable/disable Sighting Spot Level
SPYGLASS_MARK             # enable/disable Mark Spot Level
```

###### Lens:

```makefile
SPYGLASS_LENS_COLOR       # For outputs: [stderr, stdout] use color based on spot type
SPYGLASS_LENS_TIME        # Log will include: '[YYYY-MM-dd HH:mm:ss]'
SPYGLASS_LENS_FILE        # Log will include: '(<filename>):'
SPYGLASS_LENS_FUNC        # Log will include: '(<function_name>):'
SPYGLASS_LENS_LINE        # Log will include: '(<line_number>):'

```

###### Configuration:

```makefile
SPYGLASS_CONFIG_LENS      # Lens configuration. Default: SPYGLASS_LENS_TIME | SPYGLASS_LENS_FILE | SPYGLASS_LENS_FUNC | SPYGLASS_LENS_LINE
SPYGLASS_CONFIG_SPOT      # Spot configuration. Default: SPYGLASS_MAYDAY
```

###### Functionality:

```makefile
SPYGLASS_USE_DEPLOY       # When enabled only Spotting within `spyglass_deploy()` and `spyglass_stow()` will be logged.
```

###### Logging:

```makefile
SPYGLASS_AIM              # Output path(s) for logs. Default: [stderr, .log/log.spyglass]
```

## Pre-made Configuration:

```makefile
SPYGLASS_AIM = "[stderr, \".log/log.spyglass\"]"

DEBUG_SPOTTING = "(SPYGLASS_MAYDAY|SPYGLASS_SIGHT|SPYGLASS_MARK)"
DEBUG_LENS = "(SPYGLASS_LENS_COLOR \
		|SPYGLASS_LENS_TIME \
		|SPYGLASS_LENS_FILE \
		|SPYGLASS_LENS_FUNC \
		|SPYGLASS_LENS_LINE)"

TEST_SPOTTING = "(SPYGLASS_MAYDAY|SPYGLASS_SIGHT)"
TEST_LENS = "(SPYGLASS_LENS_TIME \
		|SPYGLASS_LENS_FILE \
		|SPYGLASS_LENS_FUNC \
		|SPYGLASS_LENS_LINE)"

debug: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(DEBUG_LENS) \
		 -D SPYGLASS_CONFIG_SPOT=$(DEBUG_SPOTTING)
test: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(TEST_LENS) \
		-D SPYGLASS_CONFIG_SPOT=$(TEST_SPOTTING)

debug-scoped: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(DEBUG_LENS) \
			-D SPYGLASS_CONFIG_SPOT=$(DEBUG_SPOTTING) \
			-D SPYGLASS_USE_DEPLOY
test-scoped: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(TEST_LENS) \
		       -D SPYGLASS_CONFIG_SPOT=$(TEST_SPOTTING) \
		       -D SPYGLASS_USE_DEPLOY
```
