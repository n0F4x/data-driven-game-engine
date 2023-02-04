## Game Loop:

The following points take an attempt at describing each facet of a (simple) single threaded game loop.

- (process network inputs & send them to input buffer)
- process inputs:
  - get input
  - update state based on input
  - (send data to network)
- advance game:
  - animation
  - physics
- update states:
  - AI:
    - update state based on updated data
    - create events & pass them to event queue
- render
- play audio

# Comments
TODO: Adapt parallelism
