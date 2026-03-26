/**
 * Convert array of objects or arrays to CSV/TSV text
 * @param {Array<Object>|Array<Array>} data - Data to convert
 * @param {Object} options - Formatting options
 * @param {string} options.delimiter - Field delimiter (default: ",")
 * @param {string} options.quote - Quote character (default: '"')
 * @param {boolean|string[]} options.headers - Include headers or use custom headers
 * @returns {string} CSV/TSV text
 */
export function stringify(data, { delimiter = ",", quote = '"', headers = true } = {}) {
    if (!Array.isArray(data) || data.length === 0) return ""
    
    const lines = []
    const isObjects = typeof data[0] === "object" && !Array.isArray(data[0])
    
    // Determine headers
    let headerRow = null
    if (headers === true && isObjects) 
        // Auto-generate from object keys
        headerRow = Object.keys(data[0])
     else if (Array.isArray(headers)) 
        // Custom headers provided
        headerRow = headers
    
    
    // Add header line
    if (headerRow) 
        lines.push(stringifyRow(headerRow, delimiter, quote))
    
    
    // Add data rows
    for (const item of data) {
        let row
        if (isObjects) 
            // Convert object to array using header order
            row = headerRow ? headerRow.map((key) => formatValue(item[key])) : Object.values(item).map(formatValue)
         else 
            // Already an array
            row = item.map(formatValue)
        
        lines.push(stringifyRow(row, delimiter, quote))
    }
    
    return lines.join("\n")
}

/**
 * Convert a row array to CSV/TSV string
 * @param {Array} row - Row data
 * @param {string} delimiter - Field delimiter
 * @param {string} quote - Quote character
 * @returns {string} Formatted row
 */
function stringifyRow(row, delimiter, quote) {
    return row.map((field) => escapeField(String(field), delimiter, quote)).join(delimiter)
}

/**
 * Escape a field value if needed
 * @param {string} field - Field value
 * @param {string} delimiter - Field delimiter
 * @param {string} quote - Quote character
 * @returns {string} Escaped field
 */
function escapeField(field, delimiter, quote) {
    // Check if field needs quoting
    const needsQuoting = field.includes(delimiter) || field.includes(quote) || field.includes("\n") || field.includes("\r")
    
    if (!needsQuoting) return field
    
    // Escape quotes by doubling them (RFC 4180)
    const escaped = field.replace(new RegExp(quote, "g"), quote + quote)
    
    return quote + escaped + quote
}

/**
 * Format a value for CSV output
 * @param {*} value - Value to format
 * @returns {string} Formatted value
 */
function formatValue(value) {
    if (value === null || value === undefined) return ""
    return String(value)
}
