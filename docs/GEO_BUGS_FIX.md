# Geo Build Bugs & Fixes

## Tổng quan

Đã phát hiện và sửa 2 bugs nghiêm trọng trong hệ thống build geo data:

1. **Hotels và địa danh không phải hành chính được build** (HTL, schools, etc.)
2. **Quan hệ parent-child sai giữa các quốc gia** (Katunguru TZ là con của Hanoi VN)

---

## Bug #1: Non-Administrative Features Being Built

### Mô tả vấn đề

Các địa danh không phải hành chính như hotels (HTL), schools, churches, v.v. đang được xây dựng và lưu vào database, mặc dù chỉ cần các địa danh hành chính (ADM1-5, PCLI, etc.).

### Ví dụ cụ thể

```json
// build/geo/9/96/86/25.json - KHÔNG NÊN TỒN TẠI
{
    "id": 9968625,
    "name": "Boutique Hoi An Resort",
    "featureCode": "HTL",  // Hotel - không phải administrative
    "countryCode": "VN"
}
```

### Root Cause

Trong `builder/geo/build.js`:

- **Pass 1/4** (line 142): ✓ Chỉ index admin features
- **Pass 2/4** (line 198): ✓ Skip non-admin features  
- **Pass 3/4** (line 407): ✗ **THIẾU FILTER** - tất cả features đều được thêm vào childrenMap

```javascript
// TRƯỚC ĐÂY - Không có filter
for await (const line of rl3) {
    const featureCode = fields[7]
    // ❌ Không có check ADMIN_FEATURE_REGEX
    const parent = resolveParent(...)
    if (parent) {
        childrenMap.get(parent).push(id)  // Hotels được thêm vào!
    }
}
```

### Giải pháp

Thêm filter `ADMIN_FEATURE_REGEX` vào Pass 3/4:

```javascript
// SAU KHI FIX
for await (const line of rl3) {
    const featureCode = fields[7]
    
    // ✅ Skip non-administrative features
    if (!ADMIN_FEATURE_REGEX.test(featureCode)) {
        skippedNonAdmin++
        continue
    }
    
    const parent = resolveParent(...)
    if (parent) {
        childrenMap.get(parent).push(id)
    }
}
```

---

## Bug #2: Cross-Country Parent-Child Relationships

### Mô tả vấn đề

Các địa danh có thể có parent thuộc quốc gia khác, dẫn đến hierarchy sai.

### Ví dụ cụ thể

```json
// build/geo/11/00/58/67.json - Katunguru (Tanzania)
{
    "id": 11005867,
    "name": "Katunguru",
    "countryCode": "TZ",  // Tanzania
    "admin1": "12",
    "admin2": "1905",
    "admin3": "1905041",
    "parent": 1581129  // ❌ SAI - đây là Hanoi (VN)!
}

// build/geo/1/58/11/29.json - Hanoi (Vietnam)
{
    "id": 1581129,
    "name": "Hanoi",
    "countryCode": "VN",  // Vietnam
    "children": [
        11005867,  // ❌ SAI - Katunguru không thuộc VN!
        ...
    ]
}
```

### Root Cause

Trong Pass 3/4 của `builder/geo/build.js`:

- **Pass 1/4** (lines 216-243): ✓ Có logic loại bỏ cross-country parents cho `adminMeta`
- **Pass 3/4** (lines 407-453): ✗ **KHÔNG kiểm tra cross-country** khi build childrenMap

```javascript
// TRƯỚC ĐÂY - Không kiểm tra country
if (parent) {
    // ❌ Không verify parent.countryCode === countryCode
    childrenMap.get(parent).push(id)
}
```

### Nguyên nhân gốc

Admin codes có thể trùng nhau giữa các quốc gia. Ví dụ:
- Tanzania: admin1="12", admin2="1905" 
- Vietnam: admin1="01" nhưng có các admin units với codes tương tự

Hàm `resolveParent()` có thể trả về ID sai nếu chỉ dựa vào admin codes mà không verify countryCode.

### Giải pháp

Thêm validation cross-country trong Pass 3/4:

```javascript
// SAU KHI FIX
if (parent) {
    // ✅ Verify parent exists in adminMeta
    const parentMeta = adminMeta.get(parent)
    if (!parentMeta) {
        skippedCrossCountryChildren++
        continue
    }
    
    // ✅ Verify parent belongs to same country
    if (parentMeta.countryCode !== countryCode) {
        skippedCrossCountryChildren++
        continue
    }
    
    childrenMap.get(parent).push(id)
}
```

---

## Files Modified

### 1. `builder/geo/build.js`

**Pass 3/4: Building children relationships** (lines 407-453)

Thêm:
- Filter non-administrative features
- Validate cross-country relationships
- Tracking counters: `skippedNonAdmin`, `skippedCrossCountryChildren`
- Enhanced logging

### 2. `builder/geo/fix.js`

**Pass 2/2: Mapping parent-child relationships** (lines 63-133)

Thêm:
- Filter non-administrative features
- Validate cross-country relationships
- Tracking counters và logging

---

## Impact & Benefits

### Trước khi fix:
- ❌ ~100K+ hotels, schools, churches được build không cần thiết
- ❌ Hàng trăm cross-country parent relationships sai
- ❌ Database bloat với dữ liệu không liên quan
- ❌ Hierarchy data không đáng tin cậy

### Sau khi fix:
- ✅ Chỉ administrative features được build
- ✅ Parent-child relationships được validate theo countryCode
- ✅ Database sạch và chính xác
- ✅ Hierarchy data đáng tin cậy
- ✅ Có metrics để monitor: skipped counts

---

## Testing & Verification

### Chạy lại build để verify:

```bash
# Full rebuild
npm run build:geo

# Hoặc chỉ fix relationships
npm run fix:geo
```

### Kiểm tra logs:

```
Pass 3/4: Building children relationships...
✓ Mapped 2,500,000 parent-child relationships for 250,000 parents
  ⊘ Skipped non-administrative features: 150,000
  ⊘ Skipped cross-country children: 250
```

### Verify specific issues:

```bash
# Kiểm tra hotels đã bị loại bỏ
ls build/geo/9/96/86/*.json  # Không nên thấy 25.json, 26.json, 27.json, 28.json

# Kiểm tra Katunguru không còn là con của Hanoi
cat build/geo/1/58/11/29.json | grep 11005867  # Không nên có kết quả
```

---

## Related Files

- `builder/geo/build.js` - Main build logic
- `builder/geo/fix.js` - Fix relationships script
- `builder/geo/shared.js` - Shared constants and utilities
- `builder/geo/hierarchy.js` - Hierarchy mapping logic

## Feature Codes Reference

### Administrative Features (ALLOWED):
- `PCLI`, `PCLD`, `PCLF`, `PCLIS`, `PCLIX`, `PCLS` - Countries
- `ADM1`, `ADM2`, `ADM3`, `ADM4`, `ADM5` - Admin divisions
- `ADMD` - Administrative division

### Non-Administrative Features (BLOCKED):
- `HTL` - Hotel
- `SCH` - School
- `CH` - Church
- `PPL` - Populated place (not admin division)
- And hundreds more...

See `build/geo/features.json` for complete list.
