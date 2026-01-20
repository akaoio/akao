/**
 * Parse CSV/TSV text into array of objects or arrays
 * @param {string} text - CSV/TSV text to parse
 * @param {Object} options - Parsing options
 * @param {string} options.delimiter - Field delimiter (default: ",")
 * @param {string} options.quote - Quote character (default: '"')
 * @param {boolean|string[]} options.headers - Use first row as headers or provide custom headers
 * @param {number} options.skipLines - Number of comment/header lines to skip (0 = auto-detect #)
 * @param {string} options.commentPrefix - Comment line prefix to auto-skip (default: "#")
 * @returns {Array<Object>|Array<Array>} Parsed data as array of objects or arrays
 */
export function parse(text, { delimiter = ",", quote = '"', headers = true, skipLines = 0, commentPrefix = "#" } = {}) {
    if (typeof text !== "string") return []
    
    // Split by newlines, handling both \n and \r\n
    const lines = text.split(/\r?\n/)
    if (lines.length === 0) return []
    
    let headerRow = null
    let dataStartIndex = 0
    
    // Find header row and data start
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i]
        const trimmed = line.trim()
        
        if (!trimmed) continue // Skip empty lines
        
        // Check if this is a header row (starts with comment prefix)
        if (commentPrefix && trimmed.startsWith(commentPrefix)) {
            if (headers === true && !headerRow) {
                // Parse this potential header row
                const parsed = parseRow(line, delimiter, quote)
                if (parsed && parsed.length > 3) { // Must have at least 4 fields to be a valid header
                    // Remove comment prefix from first field
                    parsed[0] = parsed[0].replace(new RegExp(`^${commentPrefix}+\\s*`), "")
                    // Check if all fields are non-empty (valid header characteristics)
                    const nonEmptyFields = parsed.filter(f => f.trim()).length
                    if (nonEmptyFields >= parsed.length * 0.7) { // At least 70% fields are non-empty
                        headerRow = parsed
                        dataStartIndex = i + 1
                    }
                }
            }
            continue // Skip comment lines
        }
        
        // If we found a header, this is where data starts
        if (headerRow) {
            dataStartIndex = i
            break
        }
        
        // If headers is false or we haven't found a header yet, this is data start
        if (!headerRow) {
            dataStartIndex = i
            break
        }
    }
    
    // Parse data rows
    const rows = []
    for (let i = dataStartIndex; i < lines.length; i++) {
        const line = lines[i]
        const trimmed = line.trim()
        
        if (!trimmed) continue // Skip empty lines
        if (commentPrefix && trimmed.startsWith(commentPrefix)) continue // Skip comment lines
        
        const row = parseRow(line, delimiter, quote)
        if (row && row.length > 0) {
            rows.push(row)
        }
    }
    
    if (rows.length === 0) return []
    
    // Handle headers
    if (headers === false) {
        return rows
    }
    
    if (Array.isArray(headers)) {
        // Custom headers provided
        headerRow = headers
    } else if (!headerRow) {
        // No header found in comments, use first data row
        headerRow = rows.shift()
    }
    
    if (!headerRow || headerRow.length === 0) return []
    
    // Convert to objects
    return rows.map((row) => {
        const obj = {}
        for (let i = 0; i < headerRow.length; i++) {
            const key = headerRow[i].trim()
            if (key) { // Only add non-empty keys
                obj[key] = row[i] !== undefined ? row[i] : ""
            }
        }
        return obj
    })
}

/**
 * Parse a single CSV/TSV row into array of fields
 * Handles quoted fields, escaped quotes, and multi-line fields
 * @param {string} line - Line to parse
 * @param {string} delimiter - Field delimiter
 * @param {string} quote - Quote character
 * @returns {string[]} Array of field values
 */
function parseRow(line, delimiter, quote) {
    if (!line) return null
    
    const fields = []
    let field = ""
    let inQuotes = false
    let i = 0
    
    while (i < line.length) {
        const char = line[i]
        const next = line[i + 1]
        
        if (char === quote) {
            if (inQuotes && next === quote) {
                // Escaped quote (double quote)
                field += quote
                i += 2
            } else {
                // Toggle quote mode
                inQuotes = !inQuotes
                i++
            }
        } else if (char === delimiter && !inQuotes) {
            // End of field
            fields.push(field.trim())
            field = ""
            i++
        } else if ((char === "\r" || char === "\n") && !inQuotes) {
            // End of line outside quotes
            break
        } else {
            // Regular character
            field += char
            i++
        }
    }
    
    // Add last field
    fields.push(field.trim())
    
    return fields
}
