# miniRT_final Development Guidelines

Auto-generated from all feature plans. Last updated: 2025-12-15

## Active Technologies
- C (C99 standard), Makefile, YAML (GitHub Actions), Shell scripts (Bash) (001-cicd-pipeline-github-workflow)
- Git repository, GitHub Actions artifacts for build logs and releases (001-cicd-pipeline-github-workflow)
- Bash 4.0+, C (C99 standard for miniRT), GitHub Actions YAML (001-workflow-improvements)
- File system for logs (logs/), build artifacts (build/), git for version control (001-workflow-improvements)
- C (C99 standard), compiled with cc/gcc + MiniLibX (minilibx-linux on Linux, minilibx-macos on macOS), X11/Xext (Linux), OpenGL/AppKit (macOS) (001-scene-info-hud)
- N/A (in-memory scene data from RT file parsing) (001-scene-info-hud)
- C (C99/C11 standard, gcc/clang compatible) (001-rendering-performance-optimization)
- Scene files (.rt format) loaded at startup, in-memory representation only (001-rendering-performance-optimization)
- C (C99 standard), compiled with gcc/clang + MiniLibX (X11 for Linux, OpenGL/AppKit for macOS), libm (math library) (006-cross-platform-keyboard)
- N/A (no persistent storage for keyboard handling) (006-cross-platform-keyboard)
- C (C99 standard), 42 School norminette compliance + MiniLibX (42 School graphics library), libm (math library), libft (custom C library) (007-hud-improvements)
- N/A (no persistent storage required for this feature) (007-hud-improvements)
- C (C99 standard with 42 School norminette compliance) + MiniLibX (graphics), libft (custom C library), math.h (standard math functions) (008-bvh-performance-diagnostics)
- N/A (all data in-memory during runtime) (008-bvh-performance-diagnostics)
- C (C99), compiled with gcc/clang with -Wall -Wextra -Werror + MiniLibX (school graphics library), libft (custom C library), math.h (009-keyboard-bvh-controls)
- N/A (scene data loaded from .rt files, no persistent storage) (009-keyboard-bvh-controls)
- C (C99 standard), compiled with gcc/clang with flags `-Wall -Wextra -Werror` + MiniLibX (42 School graphics library), libm (math library), libft (custom C library) (011-code-refactoring)
- N/A (reads scene files, no persistent storage) (011-code-refactoring)
- C (C99 standard), compiled with cc/gcc with flags -Wall -Wextra -Werror + MiniLibX (graphics library), libm (math library), libft (custom C library) (011-code-refactoring)
- N/A (scene files parsed at runtime, no persistent storage) (011-code-refactoring)
- C (C99 standard) + MiniLibX (42 School graphics library), math.h, libft (custom C library) (001-performance-optimization)
- In-memory scene data structures (no persistent storage) (001-performance-optimization)
- C (C99/C11 compatible with 42 norminette standards) + MiniLibX (graphics library), libm (math library), libft (custom C library) (012-code-cleanup-refactor)
- Scene files (.rt format), N/A for runtime (012-code-cleanup-refactor)
- C (C99 standard per 42 School requirements) + MiniLibX (graphics), libft (custom C library), math library (-lm) (013-norminette-compliance)
- N/A (no persistent storage, scene files loaded at startup) (013-norminette-compliance)
- C (C99 standard), compiled with gcc/clang with -Wall -Wextra -Werror + Standard C library (stdio.h, stdlib.h), math.h (-lm), MiniLibX graphics library, libft (custom C library), get_next_line (014-bvh-tree-visualization)
- N/A (terminal output only, no persistence required) (014-bvh-tree-visualization)
- C (C99), compiled with cc -Wall -Wextra -Werror + MiniLibX (mlx_*), libm (math.h), standard C library (gettimeofday, snprintf, memcpy) (015-bvh-viz-improvements)
- N/A (in-memory scene data, stack-allocated timer pool and input queue) (015-bvh-viz-improvements)
- C (C99), compiled with cc -Wall -Wextra -Werror + MiniLibX (mlx_*), libm (math.h), standard C library (gettimeofday, snprintf, memcpy) (015-bvh-viz-improvements)
- C99 (42 School project - strict standard compliance) + MiniLibX (42 graphics library), libft (custom C library), math.h (015-bvh-viz-improvements)

- C (C99 or later with -std=c99 or compatible) + MinilibX (X11/XQuartz graphics library), libm (math library), optional libft (001-raytracer)

## Project Structure

```text
src/
tests/
```

## Commands

# Add commands for C (C99 or later with -std=c99 or compatible)

## Code Style

C (C99 or later with -std=c99 or compatible): Follow standard conventions

## Recent Changes
- 015-bvh-viz-improvements: Added C99 (42 School project - strict standard compliance) + MiniLibX (42 graphics library), libft (custom C library), math.h
- 015-bvh-viz-improvements: Added C (C99), compiled with cc -Wall -Wextra -Werror + MiniLibX (mlx_*), libm (math.h), standard C library (gettimeofday, snprintf, memcpy)
- 015-bvh-viz-improvements: Added C (C99), compiled with cc -Wall -Wextra -Werror + MiniLibX (mlx_*), libm (math.h), standard C library (gettimeofday, snprintf, memcpy)


<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
