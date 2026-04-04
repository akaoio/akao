export class Logic {
    static options(locales) {
        return locales.map((locale) => ({ value: locale.code, label: locale.name }))
    }

    static find(code, locales) {
        return locales.find((l) => l.code === code)
    }
}

export default Logic
