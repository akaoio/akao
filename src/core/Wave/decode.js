export async function decode(params = {}, options = {}) {
	return await this.request({ method: "decode", params, transfer: options.transfer, timeout: options.timeout })
}
