# frzlib
Simple freeze frame library for embedded applications

## Example Usage
### Step 1: Configuration
- Allocate memory for configuration structure:
```c
frzlib_config_t cfg;
```
- Define the number of 32-bit variables:
```c
cfg.num32 = 2U;
```
- Allocate memory for variables' address table for variable type:
```c
uint32_t * addr32[cfg.num32];
cfg.pp_addr32 = addr32;
```
- Define the number of freeze frames to capture:
```c
cfg.size = 100U;
```
- Allocate memory for 2-D data table:
```c
uint32_t data32[cfg.size][cfg.num32];
cfg.pp_data32 = data32;
```
- Set trigger signal and signal type:
```c
cfg.p_trigger    = &trigger_signal;
cfg.trigger_type = FRZLIB_TRIGGER_RISING_EDGE;
```
- Set decimation factor:
```c
cfg.decimation_factor = 1U;
```
- Initialize the library:
```c
frzlib_initialize(&cfg);
```
- Add variables for capture:
```c
frzlib_add32(&signal1);
frzlib_add32(&signal2);
```
### Step 2: Activation
- Activate when ready to start data collection:
```c
frzlib_activate();
```
### Step 3: Update
- Call to collect data
```c
frzlib_update();
```
### Step 4 (optional): Deactivation
- Deactivate to stop data collection:
```c
frzlib_deactivate();
```
