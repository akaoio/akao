# Swarm Workflow Protocol
**Goal:** Ship akao MVP in 1 month (by ~May 10, 2026)

# Swarm Workflow Protocol
**Goal:** Ship akao MVP in 1 month (by ~May 10, 2026)

## Team Roles

### Humans
- **Sơn** (hpoqou) - Project lead, architecture & theory
- **Huy** (@huycyphernguyen) - Frontend developer, UI/UX specialist
- **Đức** (@duke0503) - Core libraries & infrastructure wiring

### Bots
- **son-pc** (@sonakao_bot) - TBD (needs role assignment)
- **son-pi** (@sonakaopi_bot) - PM Bot - project coordination, workflow management

## Distributed Async Workflow

### Daily Protocol
1. **Morning sync** (agents only)
   - Read `node swarm.js history`
   - Read `swarm/notes.md`
   - Report status if working on tasks

2. **Task lifecycle**
   ```bash
   # Starting work
   node swarm.js send "start | task: <description> | branch: <branch>"
   
   # Progress update (if blocked or milestone)
   node swarm.js send "progress | task: <description> | status: <update>"
   
   # Completion
   node swarm.js send "done | task: <description> | branch: <branch> | PR: <link>"
   
   # Blocked/need help
   node swarm.js send "blocked | task: <description> | need: <requirement>"
   ```

3. **Important insights** → `node swarm.js notes "<insight>"`

### Branch Strategy
- **main** - production ready
- **feature/<name>** - new features
- **fix/<name>** - bug fixes
- **refactor/<name>** - code improvements

### Communication Guidelines
- **Sync discussions** - Use Telegram for immediate needs
- **Async updates** - Use swarm messages for status reports
- **Decisions** - Document in swarm/notes.md
- **Code review** - Tag humans (@huycyphernguyen, @duke0503, @hpoqou) for PR reviews

### Task Prioritization
1. Critical blockers (breaks main)
2. MVP features (required for launch)
3. Performance/optimization
4. Nice-to-have features

## Current Sprint (Week 1)
**Focus:** Foundation & swap pool

### Active Tasks
- [ ] Swap pool implementation (Huy) - in progress
- [ ] Role assignments (all bots) - pending
- [ ] Workflow documentation (son-pi) - completed ✓

### Blockers
None currently

## Notes
- Swarm is non-realtime: expect delays in responses
- Always check history + notes before starting work
- Update status frequently to avoid duplicate work
- Humans have final say on all decisions
