# Getting Started

Welcome to the **ratr0-engine** documentation. 

## Initialization

Before doing anything else, you must initialize the global engine state. 

```{doxygenfunction} ratr0_engine_startup
:project: ratr0-engine
```

## Configuration Options

The engine setup is controlled by the following structure:

```{doxygenstruct} Ratr0MemoryConfig
:project: ratr0-engine
:members:
:no-link:
```

```{tip}
Always check the return value of the init function. If it returns a non-zero value, check your graphics drivers.
```
