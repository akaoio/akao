export async function check() {
    if (!navigator?.mediaDevices?.getUserMedia) {
        console.warn("Microphone is not supported in this browser")
        return false
    }

    try {
        if (navigator?.permissions?.query) {
            const permission = await navigator.permissions.query({ name: "microphone" })
            if (permission.state === "granted") return true
            if (permission.state === "denied") return false
            return false
        }
    } catch {}

    return false
}