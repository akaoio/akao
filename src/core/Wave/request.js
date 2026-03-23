export async function request() {
    if (!navigator?.mediaDevices?.getUserMedia) {
        console.warn("Microphone is not supported in this browser")
        return false
    }

    try {
        const stream = await navigator.mediaDevices.getUserMedia({
            audio: {
                echoCancellation: false,
                noiseSuppression: false,
                autoGainControl: false,
                channelCount: 1
            }
        })
        stream.getTracks().forEach(track => track.stop())
        return true
    } catch {
        return false
    }
}