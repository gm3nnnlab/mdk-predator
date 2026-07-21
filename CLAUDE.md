# Claude Code Guide for MDK-Predator

A comprehensive guide for AI assistants working on the MDK-Predator codebase. This document explains the project structure, key conventions, development workflows, and common tasks.

## Project Overview

**MDK-Predator** is a comprehensive security research suite for the Mayhem-MDK module (an addon board for the HackRF Portapack H4M with Mayhem firmware). It provides tools for automotive and wireless security research.

**Key Facts:**
- Written in C with C++ wrappers for the PortaPack UI
- Targets ARM Cortex-M4 (embedded) and x86/x64 (desktop testing)
- Production-ready code with comprehensive test coverage
- Designed for authorized security research only
- Licensed under MIT

**Important:** This is security research software. Always respect authorization boundaries and legal compliance.

---

## Project Structure

```text
mdk-predator/
├── src/                          # Source code (C)
│   ├── automotive/               # Automotive security modules
│   │   ├── key_fob_analyzer.c   # Key fob signal analysis (315/433.92/868/915 MHz)
│   │   └── rolling_code_tester.c # Rolling code implementations (KeeLoq, HCS301)
│   ├── wireless/                 # Wireless security modules
│   │   ├── wifi_analyzer.c       # WiFi network scanning & security assessment
│   │   ├── bluetooth_analyzer.c  # Bluetooth device discovery & GATT enumeration
│   │   └── subghz_analyzer.c     # SubGHz RF analysis (300-928 MHz)
│   ├── crypto/                   # Cryptographic analysis
│   │   └── crypto_analyzer.c     # Algorithm ID, entropy analysis
│   └── mdk_predator.c            # Main integration module (entry point)
├── include/                      # Header files (C)
│   ├── mdk_predator.h           # Main public API
│   ├── input_validation.h       # Input sanitization utilities
│   ├── automotive/              # Automotive headers
│   ├── wireless/                # Wireless headers
│   └── crypto/                  # Crypto headers
├── hal/                          # Hardware Abstraction Layer
│   ├── hal.h                    # HAL interface
│   └── hal.c                    # HAL implementation
├── app/                          # PortaPack application (C++)
│   ├── main.cpp                 # External app entry point
│   ├── mdk_predator_app.cpp     # Application logic
│   ├── mdk_predator_app.hpp     # UI and app state
│   └── manifest.json            # App metadata for PortaPack
├── tests/                        # Test suite (C)
│   ├── automotive/              # Automotive module tests
│   ├── wireless/                # Wireless module tests
│   ├── crypto/                  # Crypto module tests
│   └── integration/             # Integration & validation tests
├── docs/                         # Technical documentation
│   ├── ARCHITECTURE.md          # System design & data flow
│   ├── API.md                   # API reference
│   ├── TESTING.md               # Testing guide
│   └── DEPLOYMENT.md            # Deployment instructions
├── scripts/                      # Build scripts (Bash, PowerShell, Batch)
├── Makefile                      # Build system (library & tests)
├── Dockerfile                    # Docker build environment
└── README.md                     # Project README

```

### Core Module Organization

Each security module (automotive, wireless, crypto) follows a consistent pattern:

1. **Header** (`include/module/analyzer.h`): Public API definitions
2. **Implementation** (`src/module/analyzer.c`): Core logic with validation
3. **Tests** (`tests/module/test_analyzer.c`): Comprehensive test coverage

---

## Key Concepts & Architecture

### Module Model
Each security module is independent but follows a unified pattern:
- **Initialization**: `module_init()` - prepare resources
- **Operation**: `module_analyze()` / `module_scan()` - perform security research
- **Reporting**: Functions return structured results (status, data, findings)
- **Cleanup**: `module_cleanup()` - release resources

### Hardware Abstraction Layer (HAL)
The HAL (`hal/hal.c`) abstracts hardware operations:
- Sample rate, bandwidth, and gain configuration
- RF frontend control
- Frequency/bandwidth validation

**Key Point:** All hardware operations go through HAL, never direct hardware calls.

### Main Integration (`src/mdk_predator.c`)
Central entry point that:
- Initializes HAL and hardware
- Manages module lifecycle
- Provides unified configuration
- Runs diagnostics

### PortaPack Application (`app/`)
UI wrapper for the PortaPack Mayhem firmware:
- C++ code that wraps the C library
- Provides interactive UI and menu system
- Handles user input and display
- Communicates with HAL through the C API

---

## Development Conventions

### Code Style & Safety

1. **Input Validation**
   - Always validate user/external input using `input_validation.h`
   - Check for null pointers, bounds, and valid ranges
   - Example:
     ```c
     if (!validate_not_null(config)) return false;
     if (validate_frequency(freq) != VALIDATION_OK) return false;
     ```

2. **Memory Safety**
   - Use `safe_memcpy()` for buffer operations
   - Avoid unsafe functions (gets, strcpy, sprintf without bounds)
   - Initialize all local variables
   - Check return codes from all functions

3. **Error Handling**
   - Functions return `bool` for success/failure
   - Complex operations return status codes via enum (e.g., `VALIDATION_OK`)
   - No exceptions (C code)
   - Always log errors in debug builds

4. **Comments & Documentation**
   - Only add comments for **why**, not what (code should be self-explanatory)
   - Use function headers for public APIs
   - Document non-obvious constraints or side effects
   - No need for end-of-line comments on obvious code

5. **Naming Conventions**
   - Functions: `snake_case`
   - Types: `snake_case_t` (for typedef'd structs)
   - Enums: `UPPER_CASE`
   - File-scoped statics: `g_` or `s_` prefix
   - Constants: `UPPER_CASE`

### Compilation Targets

The codebase targets **two distinct architectures**:

**1. ARM Cortex-M4 (Embedded)**
- Target: HackRF PortaPack H4M (Mayhem firmware)
- Compiler: `arm-none-eabi-gcc`
- Flags: `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16`
- Use: PortaPack application (external app)
- **File:** `Makefile` (main library target)

**2. x86/x64 (Desktop)**
- Target: Linux, macOS, Windows
- Compiler: `gcc` / `clang`
- Flags: `-Wall -Wextra -O2 -std=c11`
- Use: Testing, development, validation
- **File:** `Makefile` (test targets)

**Key Rule:** When adding code, ensure it compiles for both targets. Use `#ifdef` guards for platform-specific code only when necessary.

### Testing Requirements

All significant changes must include tests:

1. **Unit Tests**: Test individual functions in isolation
2. **Integration Tests**: Test module interactions
3. **Validation Tests**: Test input sanitization and edge cases

Tests use a simple assertion framework (`assert()`) and return exit code 0 on success.

**Running Tests:**
```bash
make test                  # Run all tests
make test-automotive      # Run automotive tests only
make test-wireless        # Run wireless tests only
make test-crypto          # Run crypto tests only
make test-integration     # Run integration tests only
```

---

## Build System

### Library Build (Makefile - ARM Target)

```bash
make              # Build library (default target)
make clean        # Clean build artifacts
make info         # Show build configuration
```

**Output:** `build/lib/libmdk_predator.a` (static library for ARM)

**Compiler:** `arm-none-eabi-gcc` (required)

### Test Build (Makefile - x86 Target)

```bash
make test         # Build and run all tests
make test-build   # Build test binaries only
make test-run     # Run already-built tests
```

**Compiler:** `gcc` (native)

### Build Scripts

Platform-specific automation scripts:

**Linux/macOS:**
```bash
./scripts/build.sh [--install-deps] [--clean] [--test]
./scripts/build_portapack_app.sh [--install-deps] [--download-firmware] [--mayhem /path]
```

**Windows (PowerShell):**
```powershell
.\scripts\build.ps1 [-InstallDeps] [-Clean] [-Test]
.\scripts\build_portapack_app.ps1 [-InstallDeps] [-DownloadFirmware] [-MayhemPath "C:\path"]
```

**Docker Build (Recommended):**
```bash
./docker-build.sh          # Linux/macOS
.\docker-build.ps1         # Windows PowerShell
```

Docker provides a pre-configured environment with all ARM toolchain dependencies. No local installation needed.

### PortaPack Build

Building the external app (`.ppma` file):

1. Requires: Mayhem firmware source
2. Integrates with: `external.cmake` in firmware
3. Output: `mdk_predator.ppma` (deployable app file)
4. **Recommended:** Use `scripts/build_portapack_app.sh` (or `.ps1` for Windows)

The build script automatically:
- Downloads Mayhem firmware (if needed)
- Copies files to correct location
- Registers app in external.cmake
- Builds external app only (fast)

---

## Common Development Tasks

### Adding a New Function to a Module

1. **Add declaration** to `include/module/analyzer.h`
2. **Add implementation** to `src/module/analyzer.c`
3. **Add tests** to `tests/module/test_analyzer.c`
4. **Validate input** at function entry using `input_validation.h`
5. **Test compilation** for both ARM and x86:
   ```bash
   make clean && make              # ARM library
   make test-build                 # x86 tests
   ```

### Fixing a Bug

1. **Write a test** that reproduces the bug (in appropriate test file)
2. **Make test fail** to confirm it catches the issue
3. **Fix the bug** in the implementation
4. **Make test pass**
5. **Check related modules** for similar issues
6. **Run full test suite** to ensure no regressions:
   ```bash
   make test
   ```

### Updating the HAL

The HAL is the lowest-level interface to hardware. Changes here affect all modules:

1. Update `hal/hal.h` (interface)
2. Update `hal/hal.c` (implementation)
3. Update any modules that call the changed functions
4. Update HAL tests if they exist
5. Test with `make test`
6. Verify with `make` (ARM build)

### Adding Input Validation

Use the centralized validation library in `include/input_validation.h`:

```c
#include "input_validation.h"

// Function entry point
bool my_function(uint32_t frequency) {
    if (validate_frequency(frequency) != VALIDATION_OK) {
        return false;
    }
    // ... rest of function
}
```

**Available validators:**
- `validate_not_null(ptr)` - NULL check
- `validate_frequency(freq)` - Frequency range validation
- `validate_buffer_length(length, max_length)` - Buffer bounds check
- `safe_memcpy(dst, dst_size, src, src_size)` - Safe memory copy

---

## Important Workflows

### Development -> Testing -> Deployment

1. **Development**
   - Edit source files in `src/` or `include/`
   - Follow code style conventions
   - Add tests alongside features

2. **Local Testing**
   - Compile for desktop (x86): `make test-build`
   - Run tests: `make test-run`
   - Check ARM compilation: `make clean && make`

3. **Review**
   - All tests passing: ✅
   - No compiler warnings: ✅
   - Input validation on all external data: ✅
   - Documentation updated: ✅

4. **Deployment to PortaPack**
   - Build PortaPack app: `./scripts/build_portapack_app.sh`
   - Output: `build/portapack/mdk_predator.ppma`
   - Copy to SD card: `/APPS/mdk_predator.ppma`
   - Test on hardware

### Working with Module-Specific Code

Each module is in its own directory. When modifying a module:

1. Tests live in `tests/module_name/test_*.c`
2. Headers in `include/module_name/*.h`
3. Implementation in `src/module_name/*.c`

Example: Adding to WiFi module:
- Edit: `src/wireless/wifi_analyzer.c`
- Header: `include/wireless/wifi_analyzer.h`
- Test: `tests/wireless/test_wifi_analyzer.c`
- Compile: `make wireless` (module-specific) or `make test` (full suite)

---

## Compilation & Debugging

### Compiler Warnings

The build system compiles with `-Wall -Wextra`. **Zero warnings tolerance.**

If you see a warning:
1. Fix it (don't suppress)
2. Common issues: unused variables, missing return statements, type mismatches
3. Example fix:
   ```c
   // BAD (unused variable warning)
   int result = my_function();
   
   // GOOD
   int result = my_function();
   if (!result) return false;
   ```

### Debug Output

Use `printf()` for debugging. For production:
- Remove debug prints before committing
- Or use conditional compilation with `#ifdef DEBUG` (ensure DEBUG is defined in your build flags if needed)
- Note: The Makefile does not automatically strip debug output in release mode, so explicit removal or guards are required

### Platform-Specific Code

Minimize platform-specific code. When necessary, use:

```c
#ifdef __arm__
    // ARM-specific code
#else
    // Desktop/x86 code
#endif
```

Prefer architecture-agnostic approaches using the HAL.

---

## Configuration & Constants

### Hardware Configuration

Edit `mdk_predator.conf` (if present) or use programmatic config:

```c
mdk_predator_config_t config = {
    .default_module = MODULE_AUTOMOTIVE,
    .enable_logging = true,
    .buffer_size = 1024,
    .hardware = {
        .hackrf_sample_rate = 8000000,   // 8 MHz
        .hackrf_bandwidth = 1750000,     // 1.75 MHz
        .hackrf_lna_gain = 8,
        .hackrf_vga_gain = 8,
        .hackrf_txvga_gain = 0,          // TX disabled by default
    },
    .security = {
        .allow_transmit = false,          // CRITICAL: Keep false
        .require_confirmation = true,
        .log_all_activity = true,
    },
};
```

**Critical:** `allow_transmit` must be `false` unless explicitly authorized.

### Frequency Ranges

Valid RF frequencies (by module):
- **Automotive Key Fob:** 315 MHz, 433.92 MHz, 868 MHz, 915 MHz
- **WiFi:** 2.4 GHz, 5 GHz (protocol-dependent)
- **Bluetooth:** 2.4 GHz (specific channels)
- **SubGHz:** 300-928 MHz (region-dependent)

---

## Security & Safety Considerations

### Authorization & Legal Compliance

⚠️ **Critical**: This is security research software.

**Before using:**
- Verify you have written authorization to test any system
- Confirm compliance with local RF regulations (FCC, CE, etc.)
- Understand legal implications in your jurisdiction
- Never test systems you don't own without permission

### Transmission Safety

- **TX is disabled by default** (`allow_transmit = false`)
- **Never enable transmission** without:
  - Written authorization for testing
  - Understanding of RF regulations
  - Hardware shielding/Faraday cage if needed
- Accidental RF transmission can disrupt critical systems (medical, aviation, etc.)

### Input Validation

All external input must be validated:
- User input from UI
- Configuration files
- Network data
- Command-line arguments

Use `input_validation.h` for all validation. Never trust external data.

---

## Module Reference

### Automotive Security

**Files:**
- `src/automotive/key_fob_analyzer.c`
- `src/automotive/rolling_code_tester.c`
- `include/automotive/*.h`

**Capabilities:**
- Capture and analyze key fob signals (315/433.92/868/915 MHz)
- Detect rolling code implementations (KeeLoq, HCS301)
- Pattern analysis and code sequence display
- Replay protection verification

**Key Functions:**
- `key_fob_analyze()` - Capture and analyze key fob signals
- `rolling_code_test()` - Test rolling code implementations

### Wireless Security

**Files:**
- `src/wireless/wifi_analyzer.c`
- `src/wireless/bluetooth_analyzer.c`
- `src/wireless/subghz_analyzer.c`
- `include/wireless/*.h`

**Capabilities:**
- WiFi network scanning and security assessment
- Bluetooth device discovery and GATT enumeration
- SubGHz RF signal capture (300-928 MHz)
- Protocol decoding (ASK/OOK)
- Spectrum analysis

**Key Functions:**
- `wifi_scan()` - Scan WiFi networks
- `bluetooth_scan()` - Discover Bluetooth devices
- `subghz_scan()` - Scan SubGHz spectrum

### Cryptographic Analysis

**Files:**
- `src/crypto/crypto_analyzer.c`
- `include/crypto/crypto_analyzer.h`

**Capabilities:**
- Algorithm identification
- Key schedule analysis
- Entropy testing
- Statistical quality assessment

**Key Functions:**
- `crypto_identify_algorithm()` - ID block cipher
- `entropy_analyze()` - Analyze randomness quality

### Hardware Abstraction Layer

**Files:**
- `hal/hal.h`
- `hal/hal.c`

**Provides:**
- `hal_init()` - Initialize HAL
- `hal_set_sample_rate()` - Configure sample rate
- `hal_set_bandwidth()` - Set bandwidth
- `hal_set_*_gain()` - Gain configuration
- `hal_cleanup()` - Release resources

---

## Important Files & Their Roles

| File | Purpose |
|------|---------|
| `Makefile` | Build system for library (ARM) and tests (x86) |
| `README.md` | User-facing project documentation |
| `CLAUDE.md` | This file - AI assistant guide |
| `include/mdk_predator.h` | Main public API |
| `include/input_validation.h` | Input sanitization utilities |
| `src/mdk_predator.c` | Main integration & initialization |
| `hal/hal.c` | Hardware abstraction implementation |
| `app/mdk_predator_app.cpp` | PortaPack UI application |
| `docs/ARCHITECTURE.md` | System design documentation |
| `docs/TESTING.md` | Testing methodology |
| `.github/workflows/` | CI/CD pipeline definitions |

---

## Testing Strategy

### Test Structure

Tests use custom assertion macros and a failure counter pattern:

```c
#include <stdio.h>

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("  ✓ %s\n", message); \
        tests_passed++; \
    } else { \
        printf("  ✗ %s\n", message); \
        tests_failed++; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    printf("\n[TEST] %s\n", #test_func); \
    test_func(); \
} while(0)

void test_my_function(void) {
    int result = my_function(42);
    TEST_ASSERT(result == expected_value, "function returns correct value");
}

int main(void) {
    RUN_TEST(test_my_function);
    printf("Passed: %d, Failed: %d\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;  // Exit code 0 = success
}
```

### Test Coverage

Current test coverage:
- **Automotive:** Key fob analysis, rolling code testing
- **Wireless:** WiFi scanning, Bluetooth enumeration, SubGHz analysis
- **Crypto:** Algorithm identification, entropy analysis
- **Integration:** Input validation, system initialization, module coordination

Target: All public functions should have test coverage.

### Running Tests

```bash
make test                    # Build and run all tests
make test-build              # Only compile tests
make test-run                # Only run pre-built tests
make test-automotive         # Automotive tests only
make test-clean              # Remove test artifacts
```

Each test file:
- Exits with code 0 on success
- Exits with code 1 on failure
- Prints human-readable output
- Summary shown by Makefile

---

## Git Workflow

### Branch Naming
- Feature: `feature/description`
- Bugfix: `fix/description`
- Documentation: `docs/description`
- Refactor: `refactor/description`

### Commit Messages
- Clear, concise subject line (under 70 chars)
- Reference issues if applicable: `Fixes #123`
- Explain "why" not "what"
- Example: `Fix null pointer in key_fob_analyzer - add NULL check before dereference`

### Pull Request Process
1. Create feature branch from `main`
2. Implement and test locally
3. All tests must pass: `make test`
4. No compiler warnings: `make clean && make`
5. Update documentation as needed
6. Submit PR with clear description
7. Address review feedback

---

## Troubleshooting

### Build Issues

**ARM compilation fails:**
- Install ARM toolchain: `arm-none-eabi-gcc`
- Use Docker build as alternative (no local dependencies)

**Test compilation fails:**
- Ensure `gcc` is installed
- Check that source files have no syntax errors
- Try: `make test-clean && make test-build`

**Makefile errors:**
- Verify directory structure exists
- Check file paths in Makefile
- Use `make info` to debug configuration

### Test Failures

**Individual test fails:**
1. Run test directly: `./build/tests/bin/module/test_name`
2. Check assertion error message
3. Add debug output to test
4. Verify expected values are correct
5. Check for platform-specific issues (ARM vs x86)

**All tests fail after changes:**
1. Recompile: `make test-clean && make test-build`
2. Check for syntax errors in modified files
3. Verify input validation changes
4. Check for buffer overflows or memory issues

### Compiler Warnings

**Unused variable:**
```c
// FIX: Use the variable or remove it
int x = function();  // If really unused, remove
```

**Implicit function declaration:**
```c
// FIX: Include the correct header
#include "missing_header.h"
```

**Type mismatch:**
```c
// FIX: Cast or change type
int x = (int)some_uint32_t_value;
```

---

## Performance Considerations

### Memory Constraints (Embedded)

The ARM target has limited memory. Be mindful:
- Use `sizeof()` to determine structure sizes
- Avoid large stack allocations (use HAL buffer management)
- Reuse buffers across operations
- Profile memory usage before deployment

### Computational Complexity

Some operations are CPU-intensive:
- Entropy analysis (CPU-bound)
- Signal processing (real-time constraints)
- Large buffer processing (memory-bound)

Optimize where needed, but **don't sacrifice clarity** for micro-optimizations.

---

## Documentation Standards

### Code Documentation

For public functions, include a brief comment block:

```c
/**
 * Analyze automotive key fob signals
 * Captures RF signals in common fob frequencies
 * Returns analysis results and signal count
 */
bool key_fob_analyze(key_fob_result_t *result);
```

For complex algorithms, document the approach:

```c
// KeeLoq algorithm: 64-bit block cipher with 528-bit key schedule
// See: NXP KeeLoq documentation for security properties
```

### Documentation Files

- **README.md:** User-facing overview
- **docs/ARCHITECTURE.md:** System design
- **docs/API.md:** API reference
- **docs/TESTING.md:** Testing approach
- **CLAUDE.md:** This file (AI assistant guide)

---

## Version & Release Management

### Version Tracking

Check version with: `mdk_get_version()` (C API)

Version format: `MAJOR.MINOR.PATCH` (semver)

### Deployment

Current stable build: Check `build/portapack/mdk_predator.ppma` after build

Older versions: Check GitHub releases

---

## Additional Resources

**In Repository:**
- `docs/ARCHITECTURE.md` - Detailed system design
- `docs/API.md` - Complete API reference
- `docs/TESTING.md` - Testing methodology
- `tests/README.md` - Test suite overview
- `app/README.md` - PortaPack application guide
- `CONTRIBUTING.md` - Contribution guidelines
- `CODE_OF_CONDUCT.md` - Community standards

**External:**
- GitHub Issues: [gm3nnnlab/mdk-predator](https://github.com/gm3nnnlab/mdk-predator)
- Original Project: [Predator Security Suite](https://github.com/limbo111111/Predator-Security-Suite)

---

## Checklist for Making Changes

Before committing any code:

- [ ] Code follows style conventions (snake_case, no typos)
- [ ] All input validated using `input_validation.h`
- [ ] Tests written and passing: `make test` ✅
- [ ] No compiler warnings: `make clean && make` ✅
- [ ] Compiles for both ARM and x86 targets
- [ ] Comments added for non-obvious logic
- [ ] Documentation updated (README, docs, comments)
- [ ] No sensitive data committed (keys, tokens, paths)
- [ ] Commit message is clear and references any issues
- [ ] Related tests updated if API changes
- [ ] HAL usage verified (no direct hardware calls)

---

## Summary

This guide provides everything needed to work effectively on MDK-Predator:

1. **Structure:** Clear module organization with consistent patterns
2. **Safety:** Input validation, memory safety, platform abstraction
3. **Quality:** Comprehensive testing, zero warnings, code review process
4. **Deployment:** Clear build pipeline from development to PortaPack
5. **Conventions:** Consistent naming, style, and organization

For questions about specific modules or workflows, refer to the relevant documentation files or examine existing code patterns in the repository.

**Remember:** This is security research software. Respect authorization boundaries, comply with regulations, and always validate input.
