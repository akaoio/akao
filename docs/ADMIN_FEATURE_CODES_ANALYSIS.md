# Comprehensive Analysis of Administrative Feature Codes

## 1. COMPLETE LIST OF ALL A.* FEATURE CODES

There are **25 administrative feature codes** in total:

### Administrative Division Codes (ADM)
| Code | Type | Description | Current? | Historical? |
|------|------|-------------|----------|------------|
| ADM1 | ✓ Admin | First-order administrative division | ✓ YES | ✗ NO |
| ADM1H | ✓ Admin | Historical first-order administrative division | ✗ NO | ✓ YES |
| ADM2 | ✓ Admin | Second-order administrative division | ✓ YES | ✗ NO |
| ADM2H | ✓ Admin | Historical second-order administrative division | ✗ NO | ✓ YES |
| ADM3 | ✓ Admin | Third-order administrative division | ✓ YES | ✗ NO |
| ADM3H | ✓ Admin | Historical third-order administrative division | ✗ NO | ✓ YES |
| ADM4 | ✓ Admin | Fourth-order administrative division | ✓ YES | ✗ NO |
| ADM4H | ✓ Admin | Historical fourth-order administrative division | ✗ NO | ✓ YES |
| ADM5 | ✓ Admin | Fifth-order administrative division | ✓ YES | ✗ NO |
| ADM5H | ✓ Admin | Historical fifth-order administrative division | ✗ NO | ✓ YES |
| ADMD | ✓ Admin | Administrative division (undifferentiated level) | ✓ YES | ✗ NO |
| ADMDH | ✓ Admin | Historical administrative division | ✗ NO | ✓ YES |

### Political Entity Codes (PCL)
| Code | Type | Description | Current? | Historical? |
|------|------|-------------|----------|------------|
| PCL | - | Political entity (generic) | ? | ? |
| PCLD | ✓ Admin | Dependent political entity | ✓ YES | ✗ NO |
| PCLF | ✓ Admin | Freely associated state | ✓ YES | ✗ NO |
| PCLH | ✓ Admin | Historical political entity | ✗ NO | ✓ YES |
| PCLI | ✓ Admin | Independent political entity (country) | ✓ YES | ✗ NO |
| PCLIX | ✓ Admin | Section of independent political entity | ✓ YES | ✗ NO |
| PCLS | ✓ Admin | Semi-independent political entity | ✓ YES | ✗ NO |

### Other Administrative/Territorial Codes
| Code | Type | Description | Current? | Historical? |
|------|------|-------------|----------|------------|
| ADMS | ✓ Admin | School district | ✓ YES | ✗ NO |
| LTER | ✓ Admin | Leased area | ✓ YES | ✗ NO |
| PRSH | ✓ Admin | Parish (ecclesiastical district) | ✓ YES | ✗ NO |
| TERR | ✓ Admin | Territory | ✓ YES | ✗ NO |
| ZN | ✓ Admin | Zone | ✓ YES | ✗ NO |
| ZNB | ✓ Admin | Buffer zone | ✓ YES | ✗ NO |

---

## 2. ADMINISTRATIVE vs HISTORICAL CLASSIFICATION

### ✓ SHOULD BE INCLUDED (Current/Non-Historical Administrative)
These are active administrative entities and should be included when filtering for administrative data:

**ADM levels (6 total):**
- ADM1, ADM2, ADM3, ADM4, ADM5, ADMD

**Political entities (4 total):**
- PCLI (independent countries)
- PCLD (dependent territories)
- PCLF (freely associated states)
- PCLS (semi-independent political entities)

**Other administrative features (6 total):**
- ADMS (school districts)
- LTER (leased areas)
- PCLIX (sections of independent political entities)
- PRSH (parishes)
- TERR (territories)
- ZN (zones)
- ZNB (buffer zones)

**Total to include: 17 codes**

### ✗ SHOULD BE EXCLUDED (Historical)
These are historical/former entities and should typically be excluded:

- ADM1H, ADM2H, ADM3H, ADM4H, ADM5H (historical admin divisions)
- ADMDH (historical admin division)
- PCLH (historical political entity)

**Total to exclude: 7 codes**

---

## 3. CURRENT FILTER LOGIC IN geo.js

### Current Implementation (Line 223)
```javascript
const isAdmin = featureCode === "PCLI" || featureCode.startsWith("PCL") || /^ADM[1-5]$/.test(featureCode)
```

### What This Filter INCLUDES
1. **PCLI** - Independent political entities (explicitly matched)
2. **All codes starting with "PCL"** - This includes:
   - PCLI (independent)
   - PCLD (dependent)
   - PCLF (freely associated)
   - PCLH (historical) ← ⚠️ UNWANTED
   - PCLIX (sections)
   - PCLS (semi-independent)
3. **ADM1-ADM5 only** - This explicitly includes:
   - ADM1, ADM2, ADM3, ADM4, ADM5
   - Does NOT include: ADM1H-ADM5H (good), ADMD, ADMDH

### What This Filter EXCLUDES
- **Historical admin divisions** - ADM1H, ADM2H, ADM3H, ADM4H, ADM5H ✓ (correct)
- **ADMD** - Undifferentiated admin level ✗ (potentially missing)
- **ADMDH** - Historical undifferentiated admin ✓ (correct)
- **ADMS** - School districts
- **LTER** - Leased areas
- **PRSH** - Parishes
- **TERR** - Territories
- **ZN, ZNB** - Zones

---

## 4. ANALYSIS: GAPS IN CURRENT IMPLEMENTATION

### Critical Issues

#### Issue 1: Includes Historical Political Entities (PCLH)
The filter `featureCode.startsWith("PCL")` matches all PCL* codes including **PCLH** (historical political entities), which should be excluded.

**Impact:** The code will include historical/former countries and political entities that no longer exist.

**Current regex matches:**
- PCL, PCLD, PCLF, PCLH ✗, PCLI, PCLIX, PCLS

#### Issue 2: Missing ADMD (Undifferentiated Admin Level)
The filter only matches `ADM[1-5]`, missing **ADMD** which represents administrative divisions at an undifferentiated level.

**Impact:** Some administrative data won't be indexed if it only has ADMD classification (no specific level).

#### Issue 3: Missing Optional Administrative Features
The current filter excludes:
- **ADMS** (school districts) - currently active, not historical
- **LTER** (leased areas) - currently active, not historical
- **PRSH** (parishes) - ecclesiastical districts
- **TERR** (territories) - geographic territories
- **ZN, ZNB** (zones/buffer zones) - geographic zones

**Question:** Should these be included? They are administrative in nature but represent different categories than traditional administrative divisions.

---

## 5. RECOMMENDED FILTER CORRECTIONS

### Option A: Strict Administrative Divisions Only
Include only the core administrative hierarchy and countries:

```javascript
const isAdmin = /^(ADM[1-5]|ADMD|PCLI|PCLD|PCLF|PCLIS|PCLIX)$/.test(featureCode)
```

**What this includes:**
- ADM1-5, ADMD (administrative hierarchy)
- PCLI, PCLD, PCLF, PCLIS, PCLIX (current political entities, no historical)

**Total: 11 codes**

### Option B: Extended Administrative + Territorial
Include all administrative features that aren't historical:

```javascript
const isAdmin = /^(?!.*H$)(A\.)?[A-Z]+/.test(featureCode) && 
               (featureCode.match(/^(ADM\d|ADMD|ADMS|LTER|PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS|PRSH|TERR|ZN|ZNB)$/))
```

Or more simply:

```javascript
const isAdmin = featureCode.match(/^(ADM[1-5]|ADMD|ADMS|LTER|PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS|PRSH|TERR|ZN|ZNB)$/)
```

**What this includes:**
- All 17 current (non-historical) administrative codes
- Excludes: ADM*H, ADMDH, PCLH

**Total: 17 codes**

### Option C: Strict Fix for Current Approach
Minimal changes to the current implementation to fix the PCLH issue:

```javascript
const isAdmin = featureCode === "PCLI" || (featureCode.startsWith("PCL") && !featureCode.endsWith("H")) || /^ADM[1-5]$/.test(featureCode)
```

**What this includes:**
- PCLI
- PCL* codes EXCEPT PCLH
- ADM1-5 only

**Impact:** Fixes PCLH inclusion but misses ADMD and other codes

**Total: 14 codes** (PCLD, PCLF, PCLIS, PCLIX, PCLS + ADM1-5 + PCLI)

---

## 6. RECOMMENDED ACTION

### Best Practice Recommendation: Use Option B

The current approach uses `featureCode.startsWith("PCL")` which is overly broad and includes unwanted historical codes. The best fix would be:

```javascript
const isAdmin = /^(ADM[1-5]|ADMD|PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS)$/.test(featureCode)
```

**Rationale:**
1. **Explicit whitelist** - No accidental inclusions
2. **Excludes all historical codes** - ADM*H, ADMDH, PCLH are all excluded
3. **Includes undifferentiated admin** - ADMD is included
4. **Clear and maintainable** - Easy to add new codes or understand what's included
5. **Performance** - Single regex is efficient

**Current gaps fixed:**
- ✓ Removes PCLH (historical)
- ✓ Adds ADMD (missing)
- ✓ Keeps all essential codes
- ✓ Remains performant

### Alternative if you need broader coverage:

Include all territorial/administrative features but still exclude historical:

```javascript
const isAdmin = /^(ADM[1-5]|ADMD|ADMS|LTER|PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS|PRSH|TERR|ZN|ZNB)$/.test(featureCode)
```

---

## 7. SUMMARY TABLE

| Issue | Current Filter | Problem | Recommendation |
|-------|---|---------|-----------------|
| PCLH inclusion | startsWith("PCL") | Includes historical entities | Exclude codes ending with H |
| ADMD missing | /^ADM[1-5]$/ | Misses undifferentiated level | Add ADMD to regex |
| PCLI match | === "PCLI" | Redundant with startsWith | Consolidate into regex |
| Clarity | 3 separate conditions | Hard to maintain | Single explicit whitelist |

**Current filter matches: 14 codes**
- PCLI, PCLD, PCLF, PCLH ✗, PCLIS, PCLIX, PCLS, ADM1-5

**Recommended filter matches: 16+ codes**
- Remove: PCLH
- Add: ADMD, (optionally: ADMS, LTER, PRSH, TERR, ZN, ZNB)
