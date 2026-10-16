# Brick Breaker — Checkpoint 2 interface and stub starter

**Status: scaffolding only. Nothing here implements game behaviour, drawing or tests.**

Based on your Checkpoint 1 game spec and Checkpoint 2 requirements. Built for the
single-player core game; two-player enhancement and optional power-ups are *not*
implemented/scaffolded here.

## Before copying into your repository

- **Do not overwrite your existing `ball.h` or `ball.c`!** Compare and merge the
  prototypes/fields, preserving the work you've already done.
- `events.h` in this folder is only a placeholder. Check it against the
  instructor-provided `events.h` when you can access the skeleton.
- `MAX_BRICKS 128` is a provisional maximum capacity, **not** specified by the
  assignment; change it when deciding actual layouts.
- Brick color names are scoring tiers from your spec. The Atari monochrome
  raster graphics are still just on/off pixels; do not treat those names as
  hardware colors.
- Your spec lists the left/top/bottom ball-edge events but omits a
  right-wall collision event despite the ball needing to bounce there. The
  scaffold adds `handle_ball_right_edge` as an inferred necessary handler.
- Ball `dx/dy` are signed velocities, a compact representation of velocity and
  direction. Verify this matches how you want to implement the spec.
- The renderer uses a *contiguous* 8x16 font. If you use the Atari TOS font
  directly, its rows are strided by 256 bytes instead; adapt that contract.
- `rast_asm.s` is **not** in this zip; create it when you actually write
  assembly. The checkpoint mentions it for assembly routines, if any.
- Comments with TODO are reminders and not working functionality.
- Source file headers and function comments still need your team/author ID
  and full descriptive comments to satisfy submission requirements.

## Suggested work order

1. Tonight: merge these `.h` files and placeholder `.c` functions into your repo.
2. Confirm you can compile every `.c` into an object file (no full game required).
3. Friday: implement object model functions + asynchronous, synchronous and
   conditional event handling; write a few targeted tests **immediately**.
4. Saturday: implement and test all 14 raster functions. Only then connect
   the renderer.
5. Sunday: finish renderer and broader test coverage.

## Quick compile-only check on a PC (not Atari compatibility testing)

From this directory:

```sh
gcc -std=c89 -Wall -Wextra -pedantic -c ball.c paddle.c brick.c score.c lives.c model.c asynch.c synch.c cond.c raster.c render.c
```

The above checks syntax of stubs on a local host only; it does not test the
implementation or prove that the Atari cross-compiler accepts every construct.
The three test_*.c files currently have placeholder `main()` bodies, not tests.
