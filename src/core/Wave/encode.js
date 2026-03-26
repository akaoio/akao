export async function encode(params = {}, options = {}) {
	return await this.request({ method: "encode", params, timeout: options.timeout })
}
