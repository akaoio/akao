# TDD as Specification

The deepest idea in test-driven development is not "write tests first." It is that truth should be stated before implementation tries to satisfy it.

In this view, code does not define correctness. Tests define correctness. Code is only one possible answer.

## Main convictions

- A test is a claim about reality.
- A failing test is not a nuisance; it is proof that the claim has not yet been satisfied.
- Red before green is a discipline against self-deception.
- Verification should stay close to the reality it claims to protect.

## Why failure comes first

If success appears before failure was observed, there is no proof that the test can detect absence. A test that never demonstrated its ability to fail is not yet trustworthy.

The temptation in software is always to let proof become symbolic. We simulate too much, abstract too much, and then slowly stop testing the thing that will actually exist. The deeper demand of this mindset is correspondence: the tested world should resemble the lived world as much as possible.

## Tests as mathematical constraints

A strong suite behaves like a lightweight proof system. It constrains:

- valid inputs
- invalid inputs
- invariant behavior
- failure modes
- state transitions

That turns tests into specification rather than comfort.

## The deeper principle

Testing is epistemology. It answers: how do we know this is true?
