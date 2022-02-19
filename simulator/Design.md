# Intro

This keeps evolving, but at the present state.

## Key points

### Memory Management

- Circuit creates and deletes gates, junctions and wires
- Gate never creates or deletes memory
- Junction never creates or deletes memory
- Network alone creates and deletes Network instances.
- Wire never creates or deletes memory.

When the circuit deletes a junction or wire, it lets the Networks
know. (They may update records immediately or later - not exposed to interface)
