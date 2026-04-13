# Local-First Storage Architecture

Modern browsers are no longer thin terminals. They contain multiple storage models, query models, and execution contexts. This project treats those as infrastructure.

## Main convictions

- Local persistence is not a fallback; it is part of the primary architecture.
- Not all truths deserve the same container.
- Query power matters: key-value, relational, binary, and sync all solve different problems.
- Main-thread responsiveness is a moral constraint, not only a performance optimization.

## The layered view

The main idea is not "use many databases." It is that different forms of memory answer different questions. Some memory is for reaction. Some is for analysis. Some is for durability. Some is for exchange. Confusion begins when one container is forced to pretend it can honor every kind of truth equally well.

## Why workers matter

If a system makes the person wait because its own internal organization is clumsy, then the architecture has failed ethically before it has failed technically.

## The deeper principle

Bring data closer to the user, and make the browser worthy of that trust.
