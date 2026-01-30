export const ADMIN_FEATURE_REGEX = /^(ADM[1-5]|ADMD|PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS)$/
export const COUNTRY_FEATURE_REGEX = /^(PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS)$/

export function parseAdminField(field) {
    if (!field || field === "00") return null
    const num = parseInt(field, 10)
    return Number.isInteger(num) && num > 999 ? num : field
}

export function normalizeAdminCodes(fields) {
    return {
        admin1: fields[10] && fields[10] !== "00" ? fields[10] : "",
        admin2: fields[11] && fields[11] !== "00" ? fields[11] : "",
        admin3: fields[12] && fields[12] !== "00" ? fields[12] : "",
        admin4: fields[13] && fields[13] !== "00" ? fields[13] : ""
    }
}

export function resolveParent({ featureCode, countryCode, admin1, admin2, admin3, admin4, adminIndex, hierarchyMap, countryIds }) {
    const admin1Val = parseAdminField(admin1)
    const admin2Val = parseAdminField(admin2)
    const admin3Val = parseAdminField(admin3)
    const admin4Val = parseAdminField(admin4)

    // If hierarchy.txt provides direct parent mapping, use it first
    const childId = parseAdminField(featureCode === "ADM1" ? admin1 : featureCode === "ADM2" ? admin2 : featureCode === "ADM3" ? admin3 : featureCode === "ADM4" ? admin4 : null)
    if (hierarchyMap && typeof childId === "number" && hierarchyMap.has(childId)) {
        return hierarchyMap.get(childId)
    }

    const getCountryParent = () => {
        // First, try to use countryIds map if available (fastest)
        if (countryIds && countryIds.has(countryCode)) {
            return countryIds.get(countryCode)
        }
        
        // Fallback: search adminIndex for country record
        const key = `${countryCode}|||||PCLI`
        let parent = adminIndex.get(key)
        if (!parent) {
            for (const [k, v] of adminIndex.entries()) {
                if (k.startsWith(`${countryCode}|||||PCL`) && !k.endsWith("|PCLH")) {
                    parent = v
                    break
                }
            }
        }
        return parent || null
    }

    // Country-level features don't have parents
    if (COUNTRY_FEATURE_REGEX.test(featureCode)) {
        return null
    }

    // ADM1 units always have the country as parent
    if (featureCode === "ADM1") {
        return getCountryParent()
    }

    // Check if this is a level-1 administrative unit (no admin1 = directly under country)
    const isLevel1 = !admin1
    if (isLevel1) {
        return getCountryParent()
    }

    if (featureCode === "ADM2") {
        if (typeof admin1Val === "number") return admin1Val
        return adminIndex.get(`${countryCode}|${admin1}||||ADM1`) || null
    }

    if (featureCode === "ADM3") {
        if (typeof admin2Val === "number") return admin2Val
        return adminIndex.get(`${countryCode}|${admin1}|${admin2}|||ADM2`) || null
    }

    if (featureCode === "ADM4") {
        if (typeof admin3Val === "number") return admin3Val
        return adminIndex.get(`${countryCode}|${admin1}|${admin2}|${admin3}||ADM3`) || null
    }

    if (featureCode === "ADM5") {
        if (typeof admin4Val === "number") return admin4Val
        return adminIndex.get(`${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|ADM4`) || null
    }

    if (featureCode === "ADMD" || /^ADM[1-5]$/.test(featureCode)) {
        // Try numeric parents first
        if (typeof admin4Val === "number") return admin4Val
        if (typeof admin3Val === "number") return admin3Val
        if (typeof admin2Val === "number") return admin2Val
        if (typeof admin1Val === "number") return admin1Val

        // Try code-based lookup from deepest to shallowest
        const order = [
            { a1: admin1, a2: admin2, a3: admin3, a4: admin4, fc: "ADM4" },
            { a1: admin1, a2: admin2, a3: admin3, fc: "ADM3" },
            { a1: admin1, a2: admin2, fc: "ADM2" },
            { a1: admin1, fc: "ADM1" }
        ]

        for (const entry of order) {
            const key = `${countryCode}|${entry.a1 || ""}|${entry.a2 || ""}|${entry.a3 || ""}|${entry.a4 || ""}|${entry.fc}`
            const parent = adminIndex.get(key)
            if (parent) return parent
        }

        return getCountryParent()
    }

    return null
}
