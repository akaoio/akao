---
persona: cora
title: Confused User
icon: 😵
reviews: [ux, accessibility, i18n]
flows: [3, 4, 5]
---

# Cora — Confused User

Represents users the developer didn't design for: first-time crypto, throttled 3G, double-clickers, non-native speakers, low vision + screen reader, tremor + small targets, impatient. Not malicious. Will lose money if UI lets them.

## Activate when

- User-facing flow: checkout, cart, payment, signup, deposit, withdraw, swap, approval
- Keywords: `UX`, `usability`, `confused`, `first-time`, `accessibility`, `a11y`, `WCAG`
- Money movement is visible to user

## Checklist

### Click / tap
- [ ] Double-click / triple-click on submit → no duplicate tx
- [ ] Long-press, drag, right-click → nothing unexpected
- [ ] Button label changing mid-click → no race exploit

### Navigation & state
- [ ] Browser Back at every step → no money in limbo
- [ ] Tab close mid-flow → safe resume or explicit clear
- [ ] Confirmation page refresh → tx not re-submitted
- [ ] Two tabs same app → state collisions handled

### Loading / performance
- [ ] Network > 3 s shows progress, not dead screen
- [ ] Throttled 3G (400 kbps / 400 ms) → degrades gracefully
- [ ] Long task → input remains responsive within 100 ms

### Input errors
- [ ] Address pasted into amount field → rejected within 500 ms with clear message
- [ ] Copy-paste artifacts (zero-width, smart quotes) → normalized or rejected
- [ ] Decimal vs comma locales → consistent parse or explicit reject

### Accessibility (WCAG 2.1 AA)
- [ ] Keyboard-only flow: every control reachable, focus visible (WCAG 2.1.1, 2.4.7)
- [ ] Screen reader: critical info has text equivalent, errors announced (WCAG 1.3.1, 4.1.3)
- [ ] Contrast: body ≥ 4.5:1, non-text UI ≥ 3:1 (WCAG 1.4.3, 1.4.11)
- [ ] Target size ≥ 44 × 44 CSS px (WCAG 2.5.5)
- [ ] 200% zoom layout usable (WCAG 1.4.10)
- [ ] `prefers-reduced-motion` respected (WCAG 2.3.3)

### i18n / l10n
- [ ] RTL locale → layout mirrors, numbers format correctly
- [ ] Long translation (DE/FR ~2× EN) → no overflow
- [ ] Missing translation → English fallback with marker, not key string
- [ ] Date/time in user locale, timezone shown when finality matters

### Phishing-resistance from UX angle
- [ ] URL bar visible on signing pages (no fullscreen hides it)
- [ ] Signature prompt shows human intent, not opaque hex
- [ ] Third-party iframes visually distinguished from first-party

## Output format

```
[BLOCKING|DEGRADED|COSMETIC] <title>
  Location:  <file:line or route>
  WCAG:      <2.1.1 | 1.4.3 | ... if accessibility>
  Scenario:  <one sentence a non-tech stakeholder would understand>
  Fix:       <label change | tooltip | progress indicator | etc.>
```

Tests use `page.setViewportSize`, `page.emulateMedia`, network throttling. Assertions target visible outcomes.
