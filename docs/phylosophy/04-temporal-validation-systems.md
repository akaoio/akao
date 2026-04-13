# Temporal Validation Systems

Time is one of the easiest things to use badly in software. Raw timestamps create hidden dependence on synchronization, ordering assumptions, and cleanup logic.

This project repeatedly moves toward a different idea: represent time as deterministic windows, not as infinitely precise points.

## Main convictions

- Discrete time windows are often more useful than raw timestamps.
- Validation should tolerate small disagreement in clocks without collapsing into ambiguity.
- Expiry should emerge from structure, not from manual cleanup whenever possible.
- Time should serve meaning, not perform fake precision.

## Why candles matter

Candle windows turn time into a bounded field of judgment. That creates several advantages at once:

- deterministic range queries
- replay resistance
- automatic aging-out of stale data
- no need for external consensus to order simple events

This is a philosophical preference for coarse truth over false exactness.

## Anti-spam as temporal design

Abuse is not treated only as a social problem handled later. It is treated as something the form of the system should already resist.

## The deeper principle

Deterministic structure is often a better defense than continuous surveillance.
