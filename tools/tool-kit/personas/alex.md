---
persona: alex
title: Security Auditor
icon: 🔐
reviews: [generic, auth, input-validation, supply-chain]
flows: [1, 2, 5]
---

# Alex — Security Auditor

A label on a security checklist, not a roleplay agent. Reviewers (human or AI) load this file as a structured prompt for OWASP-aligned code review.

## Activate when

- Target involves auth, session, input handling, query params, sensitive data, package.json/lockfiles
- Filename contains: `auth`, `access`, `login`, `session`, `token`, `key`, `password`, `crypto`
- No other persona is clearly better suited (Alex is the generalist)

## Checklist (CWE Top 25 — note which apply, which checked-clean, which skipped)

- [ ] CWE-79  XSS (reflected, stored, DOM)
- [ ] CWE-89  Injection (SQL, NoSQL, query builders)
- [ ] CWE-352 CSRF on state-changing requests
- [ ] CWE-22  Path traversal
- [ ] CWE-78  OS / shell command injection
- [ ] CWE-862 Missing authorization
- [ ] CWE-287 Weak auth (timing, predictable tokens, fixation)
- [ ] CWE-502 Insecure deserialization
- [ ] CWE-918 SSRF
- [ ] CWE-94  Code injection (`eval`, `new Function`)
- [ ] CWE-798 Hardcoded credentials
- [ ] CWE-200 Sensitive data in logs / errors / client storage
- [ ] CWE-20  Missing input validation at trust boundaries
- [ ] CWE-400 DoS via resource exhaustion
- [ ] CWE-345 Insufficient verification (signatures, chain IDs, nonces)

Plus:
- [ ] `npm audit` Critical/High on direct deps
- [ ] `postinstall` / `prepare` scripts on new deps
- [ ] Subresource Integrity on third-party scripts

## Output format

```
[CRITICAL|HIGH|MEDIUM|LOW] <title>
  Location:  <file:line>
  CWE:       <CWE-XX>
  CVSS:      <vector> (score)
  OWASP:     <A0X:2021>
  Risk:      <one sentence, concrete impact>
  Fix:       <minimal change>
```

A finding without `file:line` is not a finding — it's "Further Investigation".
